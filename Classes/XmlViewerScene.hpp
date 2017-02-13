
#ifndef XmlViewerScene_hpp
#define XmlViewerScene_hpp


#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml.hpp"
#include "ui/UIButton.h"
#include "ui/UICheckBox.h"

class CustomOptionWindow;
class KeySettingWindow;
class CodeConfigWindow;

typedef std::tuple<std::string,cocos2d::Sprite*> _tplSprInfo;
class XmlViewerScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(const char* xmlFile);
    static XmlViewerScene* create(const char* xmlFile);
    virtual bool init();
    
private:
    XmlViewerScene(const char* xmlFile);
    virtual ~XmlViewerScene();
    
    void onEnter();
    
    void onMouseDown(cocos2d::EventMouse* event);
    void onMouseMove(cocos2d::EventMouse* event);
    
    bool usingPopup();
    _tplSprInfo detectImage(cocos2d::Vec2 cursorPos);
    void showPosition(_tplSprInfo detectItem, cocos2d::Vec2 cursorPos);
    void showTarget(cocos2d::Sprite* sprite);
    
    void loadXml(const char* xmlFile);
    void settingXml();
    void settingCode();
    void callbackKey(std::string key, bool isButton);
    void callbackCodeConfig(std::string className);
    
    cocos2d::Size _screenSize;
    
    // world position
    cocos2d::Label* _labelMouseWorldPosition;
    cocos2d::Label* _labelMouseWorldAnchor;
    
    // local position
    cocos2d::Label* _labelMouseLocalPosition;
    cocos2d::Label* _labelMouseLocalAnchor;
    
    // Image Name / key name
    cocos2d::Label* _labelImageName;
    cocos2d::Label* _labelKeyValue;
    
    cocos2d::Layer* _labelLayer;
    std::vector<_tplSprInfo> _vSpriteTpl;
    
    cocos2d::Sprite* _currentTarget;
    
    cocos2d::DrawNode* _localLine;
    
    cocos2d::Vec2 _uiRootPosition;
    
    CustomOptionWindow* _optionWindow;
    KeySettingWindow* _keySettingWindow;
    CodeConfigWindow* _codeConfigWindow;
    
public:
    _tplSprInfo getSpriteTpl(cocos2d::Sprite* spr);
};


class CustomOptionWindow : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CC_SYNTHESIZE(std::function<void(void)>, _callbackSetting, CallbackSetting);
    CC_SYNTHESIZE(std::function<void(void)>, _callbackGenerateCode, CallbackGenerateCode);
    CC_SYNTHESIZE(std::function<void(void)>, _callbackClose, CallbackClose);
    
    CREATE_FUNC(CustomOptionWindow);
    
private:
    
    void onEnter();
    void createUI();
    bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);
};


class KeySettingWindow : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CC_SYNTHESIZE(std::function<void(std::string, bool)>, _callbackKey, CallbackKey);
    CREATE_FUNC(KeySettingWindow);
    
private:
    
    void onEnter();
    void createUI();
    
    cocos2d::ui::CheckBox* _checkType;
    cocos2d::ui::CheckBox* _checkMember;
};

class CodeConfigWindow : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CC_SYNTHESIZE(std::function<void(std::string)>, _callbackConfig, CallbackConfig);
    CREATE_FUNC(CodeConfigWindow);
    
private:
    void onEnter();
    void createUI();
};

#endif /* XmlViewerScene_hpp */
