//
//  SelectResoureRootScene.hpp
//  XmlViewer
//
//  Created by YongJun on 2016. 11. 10..
//
//

#ifndef SelectResoureRootScene_hpp
#define SelectResoureRootScene_hpp

#include "cocos2d.h"
#include "cocos-ext.h"

class SelectResoureRootScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
private:
    
    void onEnter();
    void onExit();
    
    void createGradient();
    void createResourceBox();
    void createButton();
    
    enum Tag
    {
        UI_Resource_Root = 100
    };
    
    cocos2d::Size _screenSize;
    
    CREATE_FUNC(SelectResoureRootScene);
    
    
};



#endif /* SelectResoureRootScene_hpp */
