//
//  SelectXmlScene.cpp
//  XmlViewer
//
//  Created by YongJun on 2016. 10. 25..
//
//

#include "SelectXmlScene.hpp"
#include "FileReader.h"
#include "ui/UIButton.h"


#include "XmlViewerScene.hpp"

using namespace cocos2d;

Scene* SelectXmlScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SelectXmlScene::create();
    
    scene->addChild(layer);
    return scene;
}



bool SelectXmlScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    _screenSize = Director::getInstance()->getWinSize();
    _containerPos = Vec2::ONE;
    
    createGradient();
    
    createScroll();
    
    createSearchBox();
    
    createReturnToPrevSceneBtn();
    
    return true;
}


void SelectXmlScene::onEnter()
{
    Layer::onEnter();
    
    Director::getInstance()->getOpenGLView()->setDesignResolutionSize(_screenSize.width,
                                                                      _screenSize.height,
                                                                      ResolutionPolicy::NO_BORDER);
    
}

void SelectXmlScene::onExit()
{
    Layer::onExit();
}

void SelectXmlScene::createGradient()
{
    LayerGradient* gradient = LayerGradient::create(Color4B(0x1F,0x1C,0x2C,0xFF), Color4B(0x92,0x8D,0xAB,0xFF));
    gradient->setContentSize(_screenSize);
    addChild(gradient);
}

void SelectXmlScene::createScroll()
{
    // arrow
    for(int i = 0; i < 2; i++)
    {
        Sprite* arrow = Sprite::create("res/arrow.png");
        arrow->setPosition(Vec2(500, 460 - (i*200)));
        arrow->setRotation(i * 180);
        addChild(arrow);
    }
    
    Label* text = Label::createWithTTF("위아래로 스크롤", "fonts/SeoulNamsanEB_0.ttf", 15);
    text->setPosition(Vec2(500, 360));
    addChild(text);
    
    // create scroll
    _scroll = ui::ScrollView::create();
    _scroll->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
    _scroll->setContentSize(Size(Director::getInstance()->getWinSize().width / 2,
                                 Director::getInstance()->getWinSize().height));
    _scroll->setAnchorPoint(Vec2::ZERO);
    _scroll->setPosition(Vec2::ZERO);
    addChild(_scroll);
    
    
    
    // xml read
    _xmlList = YJFileReader::create()->getXmlFileList();
    
    makeScrollContents();
    
    if(_containerPos != Vec2::ONE)
    {
        _scroll->setInnerContainerPosition(_containerPos);
    }
    else
    {
        _scrollStartY = _scroll->getInnerContainerPosition().y;
        CCLOG("_scrollStartY : %f", _scrollStartY);
    }

}

void SelectXmlScene::makeScrollContents()
{
    Sprite* sample = Sprite::create("btn.png");
    float width = Director::getInstance()->getWinSize().width;
    float height = sample->getContentSize().height * _xmlList.size();
    for(int i = 0; i < _xmlList.size(); i++)
    {
        ui::Button* btn = ui::Button::create("btn.png");
        btn->setTitleText(_xmlList.at(i));
        btn->setTitleFontSize(22);
        btn->setTitleColor(Color3B::BLACK);
        btn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        btn->setPosition(Vec2(0, height - (i * btn->getContentSize().height)));
        btn->addClickEventListener([=](Ref* s){
            
            _containerPos = _scroll->getInnerContainerPosition();
            std::string xmlPath = UserDefault::getInstance()->getStringForKey("KEY_XML_PATH").append("/").append(btn->getTitleText());
            UserDefault::getInstance()->setStringForKey("CUR_XML_PATH", xmlPath);
            
            auto scene = XmlViewerScene::createScene(xmlPath.c_str());
            Director::getInstance()->pushScene(scene);
            
        });
        _scroll->addChild(btn);
    }
    
    _scroll->setInnerContainerSize(Size(width*2, height));
}

void SelectXmlScene::createReturnToPrevSceneBtn()
{
    auto btn = ui::Button::create("btn.png");
    btn->setTitleText("뒤로");
    btn->setScale(0.5f);
    btn->setTitleFontSize(40);
    btn->setTitleColor(Color3B::RED);
    btn->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    btn->setPosition(Vec2(Director::getInstance()->getWinSize() * 0.98));
    btn->addClickEventListener([=](Ref* pSender){
        auto scene = SelectResoureRootScene::createScene();
        if( scene )
        {
            Director::getInstance()->replaceScene(scene);
        }
    });
    addChild(btn);
}

void SelectXmlScene::createSearchBox()
{
    Label* label = Label::createWithTTF("검색 (대소문자 구분)", "fonts/SeoulNamsanEB_0.ttf", 20);
    label->setPosition(900, 500);
    addChild(label, 100);
    
    ui::Scale9Sprite* nineSprite = ui::Scale9Sprite::create("btn.png");
    ui::EditBox* editBox = ui::EditBox::create(nineSprite->getContentSize(), nineSprite);
    editBox->setPlaceholderFontSize(20);
    editBox->setFontSize(100);
    editBox->setFontColor(Color3B::BLACK);
    editBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    editBox->setPosition(Vec2(900, 400));
    editBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    editBox->setDelegate(this);
    addChild(editBox, 100);
}



void SelectXmlScene::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
    _scroll->stopAutoScroll();
    
    int findIndex = -1;
    for(int i = 0; i < _xmlList.size(); i++)
    {
        if(_xmlList.at(i).find(text) != std::string::npos)
        {
            findIndex = i;
            break;
        }
    }
    
    if(findIndex >= 0 && findIndex < _scroll->getChildrenCount())
    {
        Vec2 findPos = _scroll->getChildren().at(findIndex)->getPosition();
        _scroll->setInnerContainerPosition(Vec2(0, -findPos.y + (_scroll->getContentSize().height)));
    }
    
}

void SelectXmlScene::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
    
}

