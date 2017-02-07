//Tue Feb 7 15:23:41 KST 2017

#ifndef test_h
#define test_h

#include "ui/cocosGUI.h"
#include "UIControl/UIBase.h"

class test : public UIBase
{
public:
	test();
	virtual ~test();

private:

	void onEnter();
	void onExit();
	void createControl(UIResource* ui);
	// 버튼

	// 스프라이트
	cocos2d::Sprite* _bg;
	cocos2d::Sprite* _tab_fur;

	// 버튼콜백
	void onBtn_silver(Ref* sender);
	void onBtn_gold(Ref* sender);
	void onBtn_info_silver(Ref* sender);
	void onBtn_info_gold(Ref* sender);
	void onBtnTest(Ref* sender);
	void onBtn_close(Ref* sender);
}

#endif
