
#ifndef XmlViewerScene_hpp
#define XmlViewerScene_hpp


#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml.hpp"

class CustomOptionWindow;
class KeySettingWindow;
class XmlViewerScene : public cocos2d::Layer, cocos2d::ui::EditBoxDelegate
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
    
    cocos2d::Sprite* detectImage(cocos2d::Vec2 cursorPos);
    void showPosition(cocos2d::Sprite* sprite, cocos2d::Vec2 cursorPos);
    void showTarget(cocos2d::Sprite* sprite);
    
    void loadXml(const char* xmlFile);
    void settingXml();
    
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);
    
    // world position
    cocos2d::Label* _labelMouseWorldPosition;
    cocos2d::Label* _labelMouseWorldAnchor;
    
    // local position
    cocos2d::Label* _labelMouseLocalPosition;
    cocos2d::Label* _labelMouseLocalAnchor;
    
    cocos2d::Layer* _labelLayer;
    cocos2d::Vector<cocos2d::Sprite*> _vSprite;
    
    cocos2d::Sprite* _currentTarget;
    
    cocos2d::DrawNode* _localLine;
    
    cocos2d::Vec2 _uiRootPosition;
    
    CustomOptionWindow* _optionWindow;
    
    cocos2d::ui::EditBox* _editBox;
    
    
};


class CustomOptionWindow : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CC_SYNTHESIZE(std::function<void(void)>, _callbackClose, CallbackClose);
    CC_SYNTHESIZE(std::function<void(void)>, _callbackSetting, CallbackSetiing);
    CREATE_FUNC(CustomOptionWindow);
private:
    
    void createUI();
    bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);
};


class KeySettingWindow : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CREATE_FUNC(KeySettingWindow);
private:
    
    void createUI();
    bool onTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);
};

#endif /* XmlViewerScene_hpp */
