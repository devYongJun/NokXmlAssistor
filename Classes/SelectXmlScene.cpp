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
    _countLabel = nullptr;
    _sprCaseSen = nullptr;
    
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
    
    _countLabel = Label::createWithTTF("Item Count : 0", "fonts/SeoulNamsanEB_0.ttf", 18);
    _countLabel->setPosition(Vec2(500, 680));
    addChild(_countLabel);
    
    // xml read
    _xmlList = YJFileReader::create()->getXmlFileList();
    _findXmlList = _xmlList;
    
    resetScroll();
    
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
    _scroll->removeAllChildrenWithCleanup(true);
    
    Sprite* sample = Sprite::create("btn.png");
    float width = Director::getInstance()->getWinSize().width;
    float height = sample->getContentSize().height * _findXmlList.size();
    for(int i = 0; i < _findXmlList.size(); i++)
    {
        ui::Button* btn = ui::Button::create("btn.png");
        btn->setTitleText(_findXmlList.at(i));
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
    
    _countLabel->setString(StringUtils::format("Item Count : %d", _findXmlList.size()));
    _scroll->setInnerContainerSize(Size(width*2, height));
}

void SelectXmlScene::resetScroll()
{
    if( _scroll == nullptr )
        return;
    
    makeScrollContents();
    
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
    Label* label = Label::createWithTTF("검색", "fonts/SeoulNamsanEB_0.ttf", 20);
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
    
    _sprCaseSen = Sprite::create("res/check.png");
    _sprCaseSen->setPosition(Vec2(900, 470));
    _sprCaseSen->setVisible(false);
    addChild(_sprCaseSen, 101);
    
    auto btn = ui::Button::create("res/checkbox.png");
    btn->setPosition(_sprCaseSen->getPosition());
    btn->addClickEventListener([=](Ref* pSender) {
        _sprCaseSen->setVisible(!_sprCaseSen->isVisible());
        this->editBoxTextChanged(editBox, editBox->getText());
    });
    addChild(btn, 100);
    
    auto caseLb = Label::createWithTTF("Case Sensitive", "fonts/SeoulNamsanEB_0.ttf", 20);
    caseLb->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    caseLb->setPosition(Vec2(925, 470));
    addChild(caseLb, 100);
}



void SelectXmlScene::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text)
{
    _scroll->stopAutoScroll();
    
    _findXmlList.clear();
    
    for( auto item : _xmlList )
    {
        std::string findingText(text);

        for( auto itemChar : item )
        {
            bool cond;
            
            if( _sprCaseSen->isVisible() )
                cond = (itemChar == findingText[0]);
            else
                cond = (tolower(itemChar) == tolower(findingText[0]));
            
            if( cond )
                findingText.erase(0, 1);
        }
        
        if( findingText.size() == 0 )
            _findXmlList.push_back(item);
    }
    
    resetScroll();
}

void SelectXmlScene::editBoxReturn(cocos2d::ui::EditBox* editBox)
{
    
}

