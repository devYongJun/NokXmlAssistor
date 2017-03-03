//
//  SelectXmlScene.hpp
//  XmlViewer
//
//  Created by YongJun on 2016. 10. 25..
//
//

#ifndef SelectXmlScene_hpp
#define SelectXmlScene_hpp

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/UIScrollView.h"
#include "SelectResoureRootScene.hpp"

class SelectXmlScene : public cocos2d::Layer, cocos2d::ui::EditBoxDelegate
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    void onEnter();
    void onExit();
    
    void createGradient();
    void createScroll();
    void createSearchBox();
    
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);
    
    CREATE_FUNC(SelectXmlScene);
    
    cocos2d::Size _screenSize;
    cocos2d::Label* _countLabel;
    
    std::vector<std::string> _xmlList;
    std::vector<std::string> _findXmlList;
    
    cocos2d::ui::ScrollView* _scroll;
    
    float _scrollStartY;
    cocos2d::Vec2 _containerPos;
    
private:
    void makeScrollContents();
    void resetScroll();
    void createReturnToPrevSceneBtn();
};

#endif /* SelectXmlScene_hpp */
