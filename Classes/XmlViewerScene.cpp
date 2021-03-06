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



Scene* XmlViewerScene::createScene(const char* xmlFile, std::string baseRootPath)
{
    auto scene = Scene::create();
    auto layer = XmlViewerScene::create(xmlFile, baseRootPath);
    
    scene->addChild(layer);
    return scene;
}



XmlViewerScene* XmlViewerScene::create(const char* xmlFile, std::string baseRootPath)
{
    auto ret = new (nothrow) XmlViewerScene(xmlFile, baseRootPath);
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

XmlViewerScene::XmlViewerScene(const char* xmlFile, std::string baseRootPath)
:_labelMouseWorldPosition(nullptr)
,_labelMouseLocalPosition(nullptr)
,_labelMouseWorldAnchor(nullptr)
,_labelMouseLocalAnchor(nullptr)
,_labelImageName(nullptr)
,_labelKeyValue(nullptr)
,_currentTarget(nullptr)
,_localLine(nullptr)
,_optionWindow(nullptr)
,_keySettingWindow(nullptr)
,_baseRootPath(baseRootPath)
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
    
    // selected Image Name
    _labelImageName = Label::createWithTTF(ttfConfig, "");
    _labelImageName->setTextColor(Color4B::WHITE);
    _labelImageName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _labelImageName->enableOutline(Color4B::BLACK);
    _labelImageName->setPosition(0, 60);
    _labelLayer->addChild(_labelImageName);
    
    _labelKeyValue = Label::createWithTTF(ttfConfig, "");
    _labelKeyValue->setTextColor(Color4B::WHITE);
    _labelKeyValue->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _labelKeyValue->setPosition(0, -60);
    _labelKeyValue->enableOutline(Color4B::BLACK);
    _labelLayer->addChild(_labelKeyValue);
    
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
    _optionWindow->setCallbackHide([=](){
        hideSprite();
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
    auto detectSpr = get<1>(detect);
    
    if(event->getMouseButton() == MOUSE_BUTTON_LEFT)
    {
        if(detectSpr)
        {
            showPosition(detect, cursorPos);
        }
        
    }
    else if(event->getMouseButton() == MOUSE_BUTTON_RIGHT)
    {
        if(detectSpr)
        {
            showTarget(detectSpr);
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

_tplSprInfo XmlViewerScene::detectImage(Vec2 cursorPos)
{
    for(auto item : _vSpriteTpl)
    {
        auto sprite = get<1>(item);
        if(sprite->getBoundingBox().containsPoint(cursorPos))
        {
            return item;
        }
    }
    return make_tuple(string(""), nullptr);
}

_tplSprInfo XmlViewerScene::getSpriteTpl(Sprite* spr)
{
    for(auto item : _vSpriteTpl)
    {
        auto sprite = get<1>(item);
        if(sprite == spr)
        {
            return item;
        }
    }
    return make_tuple(string(""), nullptr);
}

void XmlViewerScene::showPosition(std::tuple<std::string,cocos2d::Sprite*> detectItem, Vec2 cursorPos)
{
    
    auto sprite = get<1>(detectItem);
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
    
    // selected Image name
    string tok("UI_Resources");
    string baseStr = sprite->getResourceName();
    auto findPtr = baseStr.find(tok);
    
    if( findPtr != string::npos )
        baseStr.erase(0, findPtr);
    
    _labelImageName->setString(StringUtils::format("Resource Path\n\"%s\"",baseStr.c_str()));
    
    _labelKeyValue->setString(StringUtils::format("XML Key Value\n\"%s\"", get<0>(detectItem).c_str()));
    
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
    _labelImageName->setString("");
    _labelKeyValue->setString("");
    
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
        _screenSize = Size(width, height);
        _uiRootPosition = Vec2(640 - (width * 0.5f), 360 - (height * 0.5f));
        // 오브젝트들
        for(pugi::xml_node node = root_node.first_child(); node; node = node.next_sibling())
        {
            string nodeId = node.attribute("id").as_string();
            string strImg = node.attribute("img").as_string();
            string replaceStrImg = "";
            size_t findOffset = 0;
            if( strImg == "" )
                continue;
            
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
            
            string rootPath = _baseRootPath;
            
            int cutPos = strImg.find_first_of('/');
            if( cutPos == -1 )
                continue;
            
            string imgPath = strImg;
            
            
            if( string(strImg, 0, 12) == "UI_Resources" )
                imgPath = strImg.substr(cutPos, strImg.length());

            string fullPath = StringUtils::format("%s%s",rootPath.c_str(),imgPath.c_str());
            
            Sprite* sprite = Sprite::create(fullPath);
            if(sprite)
            {
                sprite->setAnchorPoint(Vec2::ZERO);
                sprite->setPosition(posX, posY);
                sprite->setName(imgPath);
                base->addChild(sprite);
                _vSpriteTpl.push_back(make_tuple(nodeId, sprite));
            }
        }
    }
    reverse(_vSpriteTpl.begin(), _vSpriteTpl.end());
    
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

void XmlViewerScene::hideSprite()
{
    if( _currentTarget != nullptr )
    {
        _currentTarget->setVisible(false);
        _currentTarget->setPosition(Director::getInstance()->getWinSize());
        showTarget(_currentTarget);
        _optionWindow->setVisible(false);
    }
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
    float h = 220.0f;
    LayerColor* popBG = LayerColor::create(Color4B(0,0,0,175), w, h);
    addChild(popBG);
    
    const int ITEM_COUNT = 4;
    
    string names[ITEM_COUNT] = { "키값 설정하기", "코드 생성하기", "선택화면으로 이동", "이 이미지 숨기기"};
    Color3B colors[ITEM_COUNT] = { Color3B::GREEN, Color3B::BLUE, Color3B::RED, Color3B::BLACK };
    function<void(void)> funcs[ITEM_COUNT] = {_callbackSetting, _callbackGenerateCode, _callbackClose, _callBackHide};
    
    for(int i = 0; i < ITEM_COUNT; i++)
    {
        int index = i;
        ui::Button* btnItem = ui::Button::create("btn.png");
        btnItem->setTitleText(names[index]);
        btnItem->setTitleColor(colors[index]);
        btnItem->setTitleFontSize(40);
        btnItem->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
        btnItem->setScale(0.4f);
        btnItem->setPosition(Vec2(w/2, h - ((h / 5) * (index+1))));
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
    
    
    Label* label = Label::createWithTTF("키값 입력 (키값 == 변수명)", "fonts/SeoulNamsanEB_0.ttf", 30);
    label->setPosition(Vec2(640, 440));
    addChild(label);
    
    ui::Scale9Sprite* nineSprite = ui::Scale9Sprite::create("btn.png");
    ui::EditBox* editBox = ui::EditBox::create(nineSprite->getContentSize(), nineSprite);
    editBox->setPlaceholderFontSize(20);
    editBox->setFontSize(100);
    editBox->setFontColor(Color3B::BLACK);
    editBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    editBox->setPosition(Vec2(640, 360));
    editBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
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
        //_checkType->setSelected(false);
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
    editBoxClassName->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
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
