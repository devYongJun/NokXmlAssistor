#include "test.h"

using namespace cocos2d;
using namespace std;

test::test()
{
	//initnull
	_bg = nullptr;
	_tab_fur = nullptr;
	_btn_silver = nullptr;
	_btn_gold = nullptr;
	_btn_info_silver = nullptr;
	_btn_info_gold = nullptr;
	_btnTest = nullptr;
	_btn_close = nullptr;
}

test::~test()
{

}

void test::onEnter()
{
	UIBase::onEnter();
	load("")
}

void test::onExit()
{
	UIBase::onExit();
}

void test::createControl(UIResource* ui)
{
	switch(hashStr(ui->id.c_str()))
	{
		//sprite case
		case("bg"):
		{
			_bg = uiutil::SpriteWithResource(ui->toSpriteResources());
			if(_bg != nullptr)
			{
				addChild(_bg);
			}
		}
		break;
		case("tab_fur"):
		{
			_tab_fur = uiutil::SpriteWithResource(ui->toSpriteResources());
			if(_tab_fur != nullptr)
			{
				addChild(_tab_fur);
			}
		}
		break;
		//button case
		case("btn_silver"):
		{
			_btn_silver = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btn_silver != nullptr)
			{
				addChild(_btn_silver);
				_btn_silver->addClickEventListener(CC_CALLBACK_1(test::onBtn_silver, this));
			}		
		break;
		case("btn_gold"):
		{
			_btn_gold = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btn_gold != nullptr)
			{
				addChild(_btn_gold);
				_btn_gold->addClickEventListener(CC_CALLBACK_1(test::onBtn_gold, this));
			}		
		break;
		case("btn_info_silver"):
		{
			_btn_info_silver = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btn_info_silver != nullptr)
			{
				addChild(_btn_info_silver);
				_btn_info_silver->addClickEventListener(CC_CALLBACK_1(test::onBtn_info_silver, this));
			}		
		break;
		case("btn_info_gold"):
		{
			_btn_info_gold = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btn_info_gold != nullptr)
			{
				addChild(_btn_info_gold);
				_btn_info_gold->addClickEventListener(CC_CALLBACK_1(test::onBtn_info_gold, this));
			}		
		break;
		case("btnTest"):
		{
			_btnTest = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btnTest != nullptr)
			{
				addChild(_btnTest);
				_btnTest->addClickEventListener(CC_CALLBACK_1(test::onBtnTest, this));
			}		
		break;
		case("btn_close"):
		{
			_btn_close = uiutil::ButtonWithResource(ui->toSpriteResource());
			if(_btn_close != nullptr)
			{
				addChild(_btn_close);
				_btn_close->addClickEventListener(CC_CALLBACK_1(test::onBtn_close, this));
			}		
		break;
		default:
		break;
	}
}

// function implementation
void test::onBtn_silver(cocos2d::Ref* sender)
{

}


void test::onBtn_gold(cocos2d::Ref* sender)
{

}


void test::onBtn_info_silver(cocos2d::Ref* sender)
{

}


void test::onBtn_info_gold(cocos2d::Ref* sender)
{

}


void test::onBtnTest(cocos2d::Ref* sender)
{

}


void test::onBtn_close(cocos2d::Ref* sender)
{

}


