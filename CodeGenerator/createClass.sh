#!/bin/bash
# 클래스이름 입력받는다
echo -n "class name : "
read inputname
echo $inputname

# 적용할 xml파일 등록
echo -n "wirte or drag xml filepath : "
read inputpath
echo $inputpath


if [[ $inputpath =~ [a-zA-Z0-9_]*\/[a-zA-Z0-9_]*\/[a-zA-Z0-9_]*\.xml ]]
then
xmlpath="${BASH_REMATCH[0]}"
fi

echo "xmlpath : $xmlpath"

# 클래스파일 생성
> "$inputname.h"
> "$inputname.cpp"

# .h파일 작성
echo -e "#ifndef "$inputname"_h\n#define "$inputname"_h\n" >> $inputname.h
echo -e "#include \"UIControl/UIBase.h\"\n" >> $inputname.h
echo -e "class "$inputname" : public UIBase\n{" >> $inputname.h
echo -e "public:" >> $inputname.h
echo -e "\t$inputname();\n\tvirtual ~$inputname();\n" >> $inputname.h
echo -e "private:" >> $inputname.h
echo -e "\tvirtual void onEnter();\n	virtual void onExit();\n" >> $inputname.h
echo -e "\tvirtual void createControl(UIResource* ui);\n" >> $inputname.h
echo -e "\t// 버 튼\n" >> $inputname.h
echo -e "\t// 스프라이트\n" >> $inputname.h
echo -e "\t// 버튼콜백\n}" >> $inputname.h



# .cpp파일 작성
echo -e "#include \""$inputname.h"\"\n" >> $inputname.cpp
echo -e "$inputname::$inputname()\n{\n\t//initnull\n}\n$inputname::~$inputname()\n{\n\n}\n" >> $inputname.cpp
echo -e "void $inputname::onEnter()\n{\n\tUIBase::onEnter();\n\tNokUtility::addTouchListener(this);\n\tscheduleUpdate();\n\tload("\"$xmlpath\"")\n}\n" >> $inputname.cpp
echo -e "void $inputname::onExit()\n{\n\t_eventDispatcher->removeEventListenersForTarget(this);\n\tunscheduleUpdate();\n\tUIBase::onExit();\n}\n" >> $inputname.cpp
echo -e "void $inputname::createControl(UIResource* ui)\n{\n\tswitch(hashStr(ui->id.c_str()))\n\t{\n\t\t//sprite case\n\t\t//button case\n\t\tdefault:\n\t\tbreak;\n\t}\n}\n" >> $inputname.cpp
echo -e "void $inputname::update(float delta)\n{\n\tfor(auto iter : _vBtnList)\n\t{\n\t\tif(iter->isSelected()) return;\n\t}\n}\n" >> $inputname.cpp
echo -e "// function implementation" >> $inputname.cpp


# 초기화
> "buttonNull.txt"
> "spriteNull.txt"
# 버튼 h
> "buttonH.txt"
# 스프라이트 h
> "spriteH.txt"
# 함수 h
> "functionH.txt"
# 함수 cpp
> "functionCPP.txt"
# 버튼 case
> "buttonCase.txt"
# 스프라이트 case
> "spriteCase.txt"


# xml 파일을 한줄씩 읽는다
while read line
do
linestr=$line
cutstr=${linestr/'<object id="'/}
resultstr="${cutstr%%'"'*}"

# key값이 없으면 제외
if [ -z $resultstr ];
then
continue
fi
# <scene></scene>구문 제외
exceptstr=${resultstr:0:1}
if [ "$exceptstr" = "<" ];
then
continue
fi

# 여기까지왔으면 key값이 존재함
# key값 추출해서 함수생성
# btn으로 시작하는것은 따로 버튼생성 로직으로 예외처리해준다. 나머지는 이미지
if [[ $resultstr =~ ^btn[a-zA-Z0-9_]* ]]
then
oriname=${BASH_REMATCH[0]}
convertname=${oriname/b/B}
echo -e "\t_"$oriname" = nullptr;" >> buttonNull.txt
echo -e "\tcocos2d::ui::Button* _"$oriname";" >> buttonH.txt
echo -e "\tvoid on"$convertname"(Ref* sender);" >> functionH.txt
echo -e "void "$inputname"::on"$convertname"(cocos2d::Ref* sender)\n{\n\n}\n\n" >> functionCPP.txt
echo -e "\t\tcase("\"$resultstr\""):\n\t\t{\n\t\t\t_"$oriname" = uiutil::ButtonWithResource(ui->toSpriteResource()); \n\t\t\taddChild(_"$oriname");\n\t\t\t_"$oriname"->addClickEventListener(CC_CALLBACK_1("$inputname"::on"$convertname", this));\n\t\t}\n\t\tbreak;" >> buttonCase.txt
else
echo -e "\t_"$resultstr" = nullptr;" >> spriteNull.txt
echo -e "\tSprite* _"$resultstr";" >> spriteH.txt
echo -e "\t\tcase("\"$resultstr\""):\n\t\t{\n\t\t\t_"$resultstr" = uiutil::SpriteWithResource(ui->toSpriteResources()); \n\t\t\tif(_"$resultstr" != nullptr)\n\t\t\t{\n\t\t\t\taddChild(_"$resultstr");\n\t\t\t}\n\t\t}\n\t\tbreak;" >> spriteCase.txt
fi



done < $inputpath

# 모든 파일 통합
sed -i '' '/버 튼/r buttonH.txt' $inputname.h
sed -i '' '/스프라이트/r spriteH.txt' $inputname.h
sed -i '' '/버튼콜백/r functionH.txt' $inputname.h
sed -i '' '/initnull/r buttonNull.txt' $inputname.cpp
sed -i '' '/initnull/r spriteNull.txt' $inputname.cpp
sed -i '' '/sprite case/r spriteCase.txt' $inputname.cpp
sed -i '' '/button case/r buttonCase.txt' $inputname.cpp
sed -i '' '/function implementation/r functionCPP.txt' $inputname.cpp

# i는 물어보고 f는 안물어본다
rm -f buttonH.txt
rm -f spriteH.txt
rm -f functionH.txt
rm -f functionCPP.txt
rm -f buttonNull.txt
rm -f spriteNull.txt
rm -f spriteCase.txt
rm -f buttonCase.txt






