//
//  XmlViewerScene.cpp
//  XmlViewer
//
//  Created by YongJun on 2016. 10. 25..
//
//

#include "XmlViewerScene.hpp"
#include "ui/UIButton.h"

using namespace cocos2d;
using namespace std;



Scene* XmlViewerScene::createScene(const char* xmlFile)
{
    auto scene = Scene::create();
    auto layer = XmlViewerScene::create(xmlFile);
    
    scene->addChild(layer);
    return scene;
}



XmlViewerScene* XmlViewerScene::create(const char* xmlFile)
{
    auto ret = new (nothrow) XmlViewerScene(xmlFile);
    if(ret && ret->init())
    {
        ret->autorelease();
    }
    return ret;
}

bool XmlViewerScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    return true;
}

XmlViewerScene::XmlViewerScene(const char* xmlFile)
:_labelMouseWorldPosition(nullptr)
,_labelMouseLocalPosition(nullptr)
,_labelMouseWorldAnchor(nullptr)
,_labelMouseLocalAnchor(nullptr)
,_currentTarget(nullptr)
,_localLine(nullptr)
{
    loadXml(xmlFile);
}

XmlViewerScene::~XmlViewerScene()
{
    
}

void XmlViewerScene::onEnter()
{
    Layer::onEnter();
    
    _labelLayer = Layer::create();
    _labelLayer->setIgnoreAnchorPointForPosition(false);
    _labelLayer->setContentSize(Size::ZERO);
    _labelLayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    addChild(_labelLayer, INT_MAX);
    
    // mouse world position
    TTFConfig ttfConfig("fonts/SeoulNamsanEB_0.ttf", 17, GlyphCollection::DYNAMIC, nullptr, true, 2);
    _labelMouseWorldPosition = Label::createWithTTF(ttfConfig, "");
    _labelMouseWorldPosition->setTextColor(Color4B::GREEN);
    _labelMouseWorldPosition->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _labelMouseWorldPosition->setPosition(-100, 0);
    _labelMouseWorldPosition->enableOutline(Color4B::BLACK);
    _labelLayer->addChild(_labelMouseWorldPosition);
    
    // mouse local position
    _labelMouseLocalPosition = Label::createWithTTF(ttfConfig, "");
    _labelMouseLocalPosition->setTextColor(Color4B::RED);
    _labelMouseLocalPosition->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _labelMouseLocalPosition->setPosition(100, 0);
    _labelMouseLocalPosition->enableOutline(Color4B::BLACK);
    _labelLayer->addChild(_labelMouseLocalPosition);
    
    _localLine = DrawNode::create();
    _localLine->setPosition(_uiRootPosition);
    addChild(_localLine, INT_MAX-1);
    
    _optionWindow = CustomOptionWindow::create();
    _optionWindow->setCallbackClose([=]() {
        Director::getInstance()->popScene();
    });
    _optionWindow->setCallbackSetiing([=]() {
        settingXml();
    });
    _optionWindow->setVisible(false);
    addChild(_optionWindow, INT_MAX);
    
    
    ui::Scale9Sprite* nineSprite = ui::Scale9Sprite::create("btn.png");
    _editBox = ui::EditBox::create(nineSprite->getContentSize(), nineSprite);
    _editBox->setPlaceholderFontSize(20);
    _editBox->setFontSize(100);
    _editBox->setFontColor(Color3B::BLACK);
    _editBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _editBox->setPosition(Vec2(640, 360));
    _editBox->setDelegate(this);
    _editBox->setVisible(false);
    addChild(_editBox, INT_MAX);
    

    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(XmlViewerScene::onMouseDown, this);
    mouseListener->onMouseMove = CC_CALLBACK_1(XmlViewerScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
}

void XmlViewerScene::onMouseDown(cocos2d::EventMouse* event)
{
//    if(_optionWindow->isVisible())
//    {
//        _optionWindow->setVisible(false);
//        return;
//    }
    
    Vec2 cursorPos = Vec2(event->getCursorX() - _uiRootPosition.x,
                          event->getCursorY() - _uiRootPosition.y);
    
    auto detect = detectImage(cursorPos);
    
    if(detect)
    {
        if(event->getMouseButton() == MOUSE_BUTTON_LEFT)
        {
            showPosition(detect, cursorPos);
        }
        else if(event->getMouseButton() == MOUSE_BUTTON_RIGHT)
        {
            showTarget(detect);
            _optionWindow->setPosition(cursorPos);
            _optionWindow->setVisible(true);
        }
    }
}


void XmlViewerScene::onMouseMove(cocos2d::EventMouse* event)
{
    if(_labelLayer)
    {
        _labelLayer->setPosition(event->getCursorX(),
                                 event->getCursorY());
    }
}

cocos2d::Sprite* XmlViewerScene::detectImage(Vec2 cursorPos)
{
    for(auto sprite : _vSprite)
    {
        if(sprite->getBoundingBox().containsPoint(cursorPos))
        {
            return sprite;
        }
    }
    return nullptr;
}


void XmlViewerScene::showPosition(Sprite* sprite, Vec2 cursorPos)
{
    
    // world position text
    _labelMouseWorldPosition->setString(StringUtils::format("world Position\n(%.0f,%.0f)", cursorPos.x, cursorPos.y));
    
    // local position text
    float localX = cursorPos.x - sprite->getPositionX();
    float localY = cursorPos.y - sprite->getPositionY();
    _labelMouseLocalPosition->setString(StringUtils::format("local Position\n(%.0f,%.0f)", localX, localY));
    
    // local position draw
    _localLine->clear();
    _localLine->drawLine(Vec2(cursorPos.x, sprite->getPositionY()),
                         Vec2(cursorPos.x, sprite->getPositionY() + sprite->getContentSize().height),
                         Color4F::RED);
    _localLine->drawLine(Vec2(sprite->getPositionX(), cursorPos.y),
                         Vec2(sprite->getPositionX() + sprite->getContentSize().width, cursorPos.y),
                         Color4F::RED);
    _localLine->drawRect(sprite->getPosition(),
                         sprite->getPosition() + sprite->getContentSize(),
                         Color4F::RED);
    
}


void XmlViewerScene::showTarget(Sprite *sprite)
{
    _currentTarget = sprite;
    
    _labelMouseWorldPosition->setString("");
    _labelMouseLocalPosition->setString("");
    
    // local position draw
    _localLine->clear();
    _localLine->drawRect(sprite->getPosition(),
                         sprite->getPosition() + sprite->getContentSize(),
                         Color4F::GREEN);
}







void XmlViewerScene::loadXml(const char* xmlFile)
{
    string strXmlFile = xmlFile;
    
    if(strXmlFile.empty())
    {
        MessageBox(xmlFile, "잘못된 xmlFile");
        return;
    }
    
    pugi::xml_document doc;
    
    LayerColor* base = LayerColor::create(Color4B::WHITE);
    
    if(doc.load_file(xmlFile))
    {
        // 기본 창크기
        pugi::xml_node root_node = doc.child("scene");
        int width = root_node.attribute("width").as_int();
        int height = root_node.attribute("height").as_int();
        base->setContentSize(Size(width, height));
        
        _uiRootPosition = Vec2(640 - (width * 0.5f), 360 - (height * 0.5f));
        // 오브젝트들
        for(pugi::xml_node node = root_node.first_child(); node; node = node.next_sibling())
        {
            string strImg = node.attribute("img").as_string();
            string replaceStrImg = "";
            size_t findOffset = 0;
            while(true)
            {
                findOffset = strImg.find("\\",findOffset);
                
                if(std::string::npos == findOffset)
                    break;
                strImg.replace(findOffset, 1, "/");
                findOffset++;
            }
            
            float posX = node.attribute("openglX").as_float();
            float posY = node.attribute("openglY").as_float();
            
            string rootPath = UserDefault::getInstance()->getStringForKey("KEY_RESOURCE_PATH");
            
            int cutPos = strImg.find_first_of('/');
            string imgPath = strImg.substr(cutPos, strImg.length());
            string fullPath = StringUtils::format("%s/%s",rootPath.c_str(),imgPath.c_str());
    
            Sprite* sprite = Sprite::create(fullPath);
            if(sprite)
            {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(posX, posY);
                sprite->setName(imgPath);
                base->addChild(sprite);
                _vSprite.pushBack(sprite);
            }
        }
    }
    _vSprite.reverse();
    
    base->setIgnoreAnchorPointForPosition(false);
    base->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    base->setPosition(Director::getInstance()->getWinSize()/2.0f);
    addChild(base);
}

void XmlViewerScene::settingXml()
{
    _optionWindow->setVisible(false);
    
    if(_currentTarget == nullptr)
    {
        return;
    }

    string xmlPath = UserDefault::getInstance()->getStringForKey("CUR_XML_PATH");
    string imgPath = _currentTarget->getName();
    
    CCLOG("xmlPath:%s\nimgPath:%s", xmlPath.c_str(), imgPath.c_str());
    
    _editBox->setVisible(true);
}


void XmlViewerScene::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
    
}

void XmlViewerScene::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
    _editBox->setVisible(false);
}




#pragma mark - 옵션선택창
bool CustomOptionWindow::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    createUI();
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(CustomOptionWindow::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}

void CustomOptionWindow::createUI()
{
    LayerColor* bg = LayerColor::create(Color4B(0,0,0,175), 200, 100);
    addChild(bg);
    
    ui::Button* btnClose = ui::Button::create("btn.png");
    btnClose->setTitleText("Xml선택화면으로 가기");
    btnClose->setTitleColor(Color3B::RED);
    btnClose->setTitleFontSize(40);
    btnClose->setTitleFontName("res/SeoulNamsanEB_0.ttf");
    btnClose->setScale(0.4f);
    btnClose->setPosition(Vec2(100, 28));
    btnClose->addClickEventListener([=](Ref* s){
        _callbackClose();
    });
    bg->addChild(btnClose);
    
    ui::Button* btnSetting = ui::Button::create("btn.png");
    btnSetting->setTitleText("Xml 키값설정 하기");
    btnSetting->setTitleColor(Color3B::BLACK);
    btnSetting->setTitleFontSize(40);
    btnSetting->setTitleFontName("res/SeoulNamsanEB_0.ttf");
    btnSetting->setScale(0.4f);
    btnSetting->setPosition(Vec2(100, 71));
    btnSetting->addClickEventListener([=](Ref* s){
        _callbackSetting();
    });
    bg->addChild(btnSetting);
}


bool CustomOptionWindow::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    if(isVisible())
    {
        setVisible(false);
        return true;
    }
    return false;
}


#pragma mark - 키값 설정창
void KeySettingWindow::createUI()
{
    ui::Button* btnClose = ui::Button::create("btn.png");
    btnClose->setTitleText("닫기");
    btnClose->setTitleColor(Color3B::BLACK);
    btnClose->setTitleFontSize(40);
    btnClose->setTitleFontName("res/SeoulNamsanEB_0.ttf");
    btnClose->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    btnClose->setPosition(Vec2(1200, 700));
    btnClose->addClickEventListener([=](Ref* s){
        removeFromParent();
    });
    addChild(btnClose);
}

bool KeySettingWindow::onTouchBegan(cocos2d::Touch* t, cocos2d::Event* e)
{
    return true;
}

