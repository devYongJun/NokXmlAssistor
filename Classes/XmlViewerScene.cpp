//
//  XmlViewerScene.cpp
//  XmlViewer
//
//  Created by YongJun on 2016. 10. 25..
//
//

#include "XmlViewerScene.hpp"

#include "FileReader.h"
#include <fstream>
#include <regex>
#include <algorithm>

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
,_optionWindow(nullptr)
,_keySettingWindow(nullptr)
{
    _screenSize = Director::getInstance()->getWinSize();
    
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
    
    // 설정선택 팝업
    _optionWindow = CustomOptionWindow::create();
    _optionWindow->setCallbackSetting([=]() {
        settingXml();
    });
    _optionWindow->setCallbackGenerateCode([=]() {
        settingCode();
    });
    _optionWindow->setCallbackClose([=]() {
        Director::getInstance()->popScene();
    });
    
    _optionWindow->setVisible(false);
    addChild(_optionWindow, INT_MAX);
    
    
    // 키값설정 팝업
    _keySettingWindow = KeySettingWindow::create();
    _keySettingWindow->setCallbackKey(std::bind(&XmlViewerScene::callbackKey,
                                                this,
                                                placeholders::_1,
                                                placeholders::_2));
    _keySettingWindow->setVisible(false);
    addChild(_keySettingWindow, INT_MAX);
    
    
    
    // 코드설정 팝업
    _codeConfigWindow = CodeConfigWindow::create();
    _codeConfigWindow->setCallbackConfig(std::bind(&XmlViewerScene::callbackCodeConfig,
                                                   this,
                                                   placeholders::_1));
    _codeConfigWindow->setVisible(false);
    addChild(_codeConfigWindow, INT_MAX);
    
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(XmlViewerScene::onMouseDown, this);
    mouseListener->onMouseMove = CC_CALLBACK_1(XmlViewerScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
}

 // 마우스는 swallow설정이 안되서 터치이벤트 내에서 검사..  누가 고쳐주겠지
bool XmlViewerScene::usingPopup()
{
    return _keySettingWindow->isVisible() || _optionWindow->isVisible() || _codeConfigWindow->isVisible();
}


void XmlViewerScene::onMouseDown(cocos2d::EventMouse* event)
{
    
    if(usingPopup())
    {
        return;
    }
    
    Vec2 cursorPos = Vec2(event->getCursorX() - _uiRootPosition.x,
                          event->getCursorY() - _uiRootPosition.y);
    
    auto detect = detectImage(cursorPos);
    
    if(event->getMouseButton() == MOUSE_BUTTON_LEFT)
    {
        if(detect)
        {
            showPosition(detect, cursorPos);
        }
        
    }
    else if(event->getMouseButton() == MOUSE_BUTTON_RIGHT)
    {
        if(detect)
        {
            showTarget(detect);
        }
        _optionWindow->setPosition(cursorPos);
        _optionWindow->setVisible(true);
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
    _labelMouseWorldPosition->setString(StringUtils::format("world Position\n(%.0f,%.0f)\n(%.3f,%.3f)",
                                                            cursorPos.x,
                                                            cursorPos.y,
                                                            cursorPos.x / _screenSize.width,
                                                            cursorPos.y / _screenSize.height));
    
    // local position text
    float localX = cursorPos.x - sprite->getPositionX();
    float localY = cursorPos.y - sprite->getPositionY();
    _labelMouseLocalPosition->setString(StringUtils::format("local Position\n(%.0f,%.0f)\n(%.3f, %.3f)",
                                                            localX,
                                                            localY,
                                                            localX / sprite->getContentSize().width,
                                                            localY / sprite->getContentSize().height));
    
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
    
    _keySettingWindow->setVisible(true);
}

void XmlViewerScene::settingCode()
{
    _codeConfigWindow->setVisible(true);
}

void XmlViewerScene::callbackKey(std::string key, bool isButton)
{
    if(_currentTarget == nullptr)
    {
        return;
    }
    if(key == "")
    {
        return;
    }
    
    string xmlPath = UserDefault::getInstance()->getStringForKey("CUR_XML_PATH");
    string imgPath = _currentTarget->getName();
    CCLOG("xmlPath:%s\nimgPath:%s", xmlPath.c_str(), imgPath.c_str());
    CCLOG("key:%s  isButton:%d", key.c_str(), isButton);
    
    string keyValue = key;
    if(isButton)
    {
        std::transform(key.begin(), key.begin()+1, key.begin(), ::toupper);
        keyValue = "btn";
        keyValue.append(key);
    }
    
    
    vector<string> vXMLTextLines;
    
    ifstream inputStream;
    string thisLine;
    inputStream.open(FileUtils::getInstance()->fullPathForFilename(xmlPath.c_str()));
    
    if(!inputStream.is_open())
    {
        return;
    }
    while( getline(inputStream, thisLine))
    {
        vXMLTextLines.push_back(thisLine);
    }
    
    string strCompare;
    while(imgPath.find("/") != string::npos)
    {
        strCompare = imgPath.replace(imgPath.find("/"),1,"\\");
    }
    
    for(int i = 0; i < vXMLTextLines.size(); i++)
    {
        if(vXMLTextLines.at(i).find(imgPath) != std::string::npos)
        {
            string found = vXMLTextLines.at(i);
            string keyString = StringUtils::format("<object id=\"%s\"", keyValue.c_str());
            std::regex rx("<object id=\"\\w*\"");
            string result = std::regex_replace(found, rx, keyString);
            
            CCLOG("found:%s", found.c_str());
            CCLOG("result:%s", result.c_str());
            vXMLTextLines.at(i) = result;
            break;
        }
    }
    inputStream.close();
    
    ofstream outputStream(xmlPath);
    for(int i = 0; i < vXMLTextLines.size(); i++)
    {
        outputStream << vXMLTextLines.at(i);
        outputStream << "\n";
    }
    outputStream.close();
    
}


void XmlViewerScene::callbackCodeConfig(std::string className)
{
    string xmlFile = UserDefault::getInstance()->getStringForKey("CUR_XML_PATH");
    
    YJFileReader::create()->runCodeGenerateShell(xmlFile, className);
}



#pragma mark - 옵션선택창
bool CustomOptionWindow::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(CustomOptionWindow::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}

void CustomOptionWindow::onEnter()
{
    Layer::onEnter();
    
    createUI();
}


void CustomOptionWindow::createUI()
{
    float w = 200.0f;
    float h = 180.0f;
    LayerColor* popBG = LayerColor::create(Color4B(0,0,0,175), w, h);
    addChild(popBG);
    
    string names[3] = { "키값 설정하기", "코드 생성하기", "선택화면으로 이동" };
    Color3B colors[3] = { Color3B::GREEN, Color3B::BLUE, Color3B::RED };
    function<void(void)> funcs[3] = {_callbackSetting, _callbackGenerateCode, _callbackClose };
    
    for(int i = 0; i < 3; i++)
    {
        int index = i;
        ui::Button* btnItem = ui::Button::create("btn.png");
        btnItem->setTitleText(names[index]);
        btnItem->setTitleColor(colors[index]);
        btnItem->setTitleFontSize(40);
        btnItem->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
        btnItem->setScale(0.4f);
        btnItem->setPosition(Vec2(w/2, h - ((h / 4) * (index+1))));
        btnItem->addClickEventListener([=](Ref* s){
            CCLOG("func:%d", index);
            funcs[index]();
        });
        popBG->addChild(btnItem);
    }

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
bool KeySettingWindow::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    return true;
}

void KeySettingWindow::onEnter()
{
    Layer::onEnter();
    
    createUI();
}

void KeySettingWindow::createUI()
{
    LayerColor* bg = LayerColor::create(Color4B(0,0,0,125), 1280, 720);;
    addChild(bg);
    
    
    Label* label = Label::createWithTTF("키값 입력", "fonts/SeoulNamsanEB_0.ttf", 30);
    label->setPosition(Vec2(640, 440));
    addChild(label);
    
    ui::Scale9Sprite* nineSprite = ui::Scale9Sprite::create("btn.png");
    ui::EditBox* editBox = ui::EditBox::create(nineSprite->getContentSize(), nineSprite);
    editBox->setPlaceholderFontSize(20);
    editBox->setFontSize(100);
    editBox->setFontColor(Color3B::BLACK);
    editBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    editBox->setPosition(Vec2(640, 360));
    addChild(editBox);
    
    Label* prep = Label::createWithTTF("btn + ", "fonts/SeoulNamsanEB_0.ttf", 100);
    prep->setPosition(300, 360);
    prep->setVisible(false);
    addChild(prep);
    
    Label* checkLabelType = Label::createWithTTF("버튼 : 체크O \n스프라이트 : 체크X", "fonts/SeoulNamsanEB_0.ttf", 25);
    checkLabelType->setTextColor(Color4B::WHITE);
    checkLabelType->setPosition(Vec2(1280/2, 250));
    addChild(checkLabelType);
    
    _checkType = ui::CheckBox::create("res/checkbox.png", "res/check.png");
    _checkType->setPosition(Vec2(1280/2, 200));
    _checkType->addEventListener([=](Ref* s, ui::CheckBox::EventType type){
        if(type == ui::CheckBox::EventType::SELECTED)
        {
            prep->setVisible(true);
        }
        else
        {
            prep->setVisible(false);
        }
    });
    addChild(_checkType);
    
    ui::Button* btnGenerate = ui::Button::create("btn.png");
    btnGenerate->setTitleText("생성");
    btnGenerate->setTitleColor(Color3B::BLUE);
    btnGenerate->setTitleFontSize(40);
    btnGenerate->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
    btnGenerate->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    btnGenerate->setPosition(Vec2(500, 100));
    btnGenerate->setScale(0.5f);
    btnGenerate->addClickEventListener([=](Ref* s){
        this->setVisible(false);
        _callbackKey(editBox->getText(), _checkType->getSelectedState());
        _checkType->setSelected(false);
        editBox->setText("");
    });
    addChild(btnGenerate);
    
    
    
    ui::Button* btnClose = ui::Button::create("btn.png");
    btnClose->setTitleText("닫기");
    btnClose->setTitleColor(Color3B::BLACK);
    btnClose->setTitleFontSize(40);
    btnClose->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
    btnClose->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    btnClose->setPosition(Vec2(780, 100));
    btnClose->setScale(0.5f);
    btnClose->addClickEventListener([=](Ref* s){
        this->setVisible(false);
        _checkType->setSelected(false);
        editBox->setText("");
    });
    addChild(btnClose);
}




#pragma mark - 생성코드 설정창
bool CodeConfigWindow::init()
{
    if(!Layer::init())
    {
        return false;
    }
    return true;
}


void CodeConfigWindow::onEnter()
{
    Layer::onEnter();
    
    createUI();
}

void CodeConfigWindow::createUI()
{
    LayerColor* bg = LayerColor::create(Color4B(0,0,0,125), 1280, 720);;
    addChild(bg);
    
    Label* labelClassName = Label::createWithTTF("클래스이름 입력", "fonts/SeoulNamsanEB_0.ttf", 30);
    labelClassName->setPosition(Vec2(1280/2, 440));
    addChild(labelClassName);
    
    ui::EditBox* editBoxClassName = ui::EditBox::create(Size(400, 100), ui::Scale9Sprite::create("res/editbox.png"));
    editBoxClassName->setPlaceholderFontSize(20);
    editBoxClassName->setFontSize(60);
    editBoxClassName->setFontColor(Color3B::BLACK);
    editBoxClassName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    editBoxClassName->setPosition(Vec2(1280/2, 360));
    addChild(editBoxClassName);
    
    ui::Button* btnGenerate = ui::Button::create("btn.png");
    btnGenerate->setTitleText("코드 생성");
    btnGenerate->setTitleColor(Color3B::BLUE);
    btnGenerate->setTitleFontSize(40);
    btnGenerate->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
    btnGenerate->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    btnGenerate->setPosition(Vec2(500, 100));
    btnGenerate->setScale(0.5f);
    btnGenerate->addClickEventListener([=](Ref* s){
        this->setVisible(false);
        _callbackConfig(editBoxClassName->getText());
    });
    addChild(btnGenerate);
    
    
    ui::Button* btnClose = ui::Button::create("btn.png");
    btnClose->setTitleText("닫기");
    btnClose->setTitleColor(Color3B::BLACK);
    btnClose->setTitleFontSize(40);
    btnClose->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
    btnClose->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    btnClose->setPosition(Vec2(780, 100));
    btnClose->setScale(0.5f);
    btnClose->addClickEventListener([=](Ref* s){
        this->setVisible(false);
        editBoxClassName->setText("");
    });
    addChild(btnClose);
    
}
