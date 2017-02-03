//
//  SelectResoureRootScene.cpp
//  XmlViewer
//
//  Created by YongJun on 2016. 11. 10..
//
//

#include "SelectResoureRootScene.hpp"
#include "SelectXmlScene.hpp"

using namespace cocos2d;

Scene* SelectResoureRootScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SelectResoureRootScene::create();
    
    scene->addChild(layer);
    return scene;
}



bool SelectResoureRootScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    _screenSize = Director::getInstance()->getWinSize();
    
    createGradient();
    
    createResourceBox();
    
    createButton();
    
    return true;
}

void SelectResoureRootScene::onEnter()
{
    Layer::onEnter();
    
    Director::getInstance()->getOpenGLView()->setDesignResolutionSize(_screenSize.width,
                                                                      _screenSize.height,
                                                                      ResolutionPolicy::NO_BORDER);
}

void SelectResoureRootScene::onExit()
{
    Layer::onExit();
}


void SelectResoureRootScene::createGradient()
{
    LayerGradient* gradient = LayerGradient::create(Color4B(0x36,0x00,0x33,0xFF), Color4B(0x0B,0x87,0x93,0xFF));
    gradient->setContentSize(_screenSize);
    addChild(gradient);
}

void SelectResoureRootScene::createResourceBox()
{
    Sprite* example = Sprite::create("res/ex.png");
    example->setPosition(Vec2(_screenSize.width/2, 550));
    addChild(example);
    
    Label* label = Label::createWithTTF("아래 검정색박스를 더블클릭하고 리소스루트폴더를 드래그하세요(UI_Resources_?경로)",
                                        "fonts/SeoulNamsanEB_0.ttf",
                                        20);
    label->setPosition(_screenSize.width/2, 400);
    addChild(label);
    
    ui::Scale9Sprite* nineSprite = ui::Scale9Sprite::create("res/bar.png");
    ui::EditBox* editBox = ui::EditBox::create(nineSprite->getContentSize(), nineSprite);
    editBox->setTag(Tag::UI_Resource_Root);
    editBox->setFontSize(20);
    editBox->setPlaceholderFontSize(20);
    std::string defaultString = UserDefault::getInstance()->getStringForKey("KEY_RESOURCE_PATH", "");
    editBox->setText(defaultString.c_str());
    editBox->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    editBox->setPosition(Vec2(_screenSize.width/2, 310));
    addChild(editBox);
    
}

void SelectResoureRootScene::createButton()
{
    ui::Button* btnOK = ui::Button::create("btn.png");
    btnOK->setTitleText("경로를 설정했으면 누르세요");
    btnOK->setTitleFontName("fonts/SeoulNamsanEB_0.ttf");
    btnOK->setTitleFontSize(20);
    btnOK->setTitleColor(Color3B::BLACK);
    btnOK->setPosition(Vec2(_screenSize.width/2, 120));
    btnOK->addClickEventListener([=](Ref* s)
    {
         auto imageBox = (ui::EditBox*)getChildByTag(Tag::UI_Resource_Root);
         std::string strImageBox = imageBox->getText();
         UserDefault::getInstance()->setStringForKey("KEY_RESOURCE_PATH", strImageBox.c_str());
         UserDefault::getInstance()->setStringForKey("KEY_XML_PATH", strImageBox.append("/Xml"));
        
         if(strImageBox == "")
         {
             MessageBox("이미지 리소스폴더의 경로를 설정해주세요", "파일경로가 설정되지않음");
         }
         else
         {
             auto scene = SelectXmlScene::createScene();
             Director::getInstance()->replaceScene(scene);
         }
     });
    
    addChild(btnOK);
}

