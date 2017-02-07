#!/bin/bash
# 클래스이름 입력받는다
#echo -n "class name : "
#read inputclass
#echo $inputclass
inputclass=$1

# 적용할 xml파일 등록
#echo -n "wirte or drag xml filepath : "
#read inputpath
#echo $inputpath
inputpath=$2

# 코드 생성할 위치
#echo -n "write or drag code filefolder : "
#read inputfolder
#echo $inputfolder
inputfolder=$3

if [[ $inputpath =~ [a-zA-Z0-9_]*\/[a-zA-Z0-9_]*\/[a-zA-Z0-9_]*\.xml ]]
then
    xmlpath="${BASH_REMATCH[0]}"
fi

echo "xmlpath : $xmlpath"
inputname="$inputfolder/$inputclass"

# 클래스파일 생성
> "$inputname.h"
> "$inputname.cpp"

now="$(date)"

# .h파일 작성
echo "//"$now"\n
#ifndef "$inputclass"_h
#define "$inputclass"_h\n
#include \"ui/cocosGUI.h\"
#include \"UIControl/UIBase.h\"\n
class "$inputclass" : public UIBase\n{
public:
\t$inputclass();\n\tvirtual ~$inputclass();\n
private:\n
\tvoid onEnter();
\tvoid onExit();
\tvoid createControl(UIResource* ui);
\t// 버 튼\n
\t// 스프라이트\n
\t// 버튼콜백\n}\n
#endif" >> $inputname.h


# .cpp파일 작성
echo "#include \""$inputclass.h"\"\n
using namespace cocos2d;
using namespace std;\n
$inputclass::$inputclass()//initnull\n{\n\t\n}\n
$inputclass::~$inputclass()\n{\n\n}\n
void $inputclass::onEnter()
{\n\tUIBase::onEnter();
\tload("\"$xmlpath\"");\n}\n
void $inputclass::onExit()
{\n\tUIBase::onExit();\n}\n
void $inputclass::createControl(UIResource* ui)
{\n\tswitch(hashStr(ui->id.c_str()))
\t{\n\t\t//sprite case\n\t\t//button case
\t\tdefault:
\t\tbreak;\n\t}\n}\n
// function implementation" >> $inputname.cpp


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

isFirst=true

# xml 파일을 한줄씩 읽는다
while read line
do
    linestr=$line
    cutstr=${linestr/'<object id="'/}
    resultstr="${cutstr%%'"'*}"

    # key값이 없으면 제외
    if [ -z "$resultstr" ];
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

    # 무효 키값
    if [ "$resultStr" = "-" ]
    then
    continue
    fi

    if [ "$resultStr" = "_" ]
    then
    continue
    fi


    # key값 추출해서 함수생성 (btn 으로 시작하는 키는 버튼클래스 생성)
    if [[ $resultstr =~ ^btn[a-zA-Z0-9_]* ]]
        then
            oriname=${BASH_REMATCH[0]}
            convertname=${oriname/b/B}
        if [ $isFirst == true ]
        then
            echo ":_"$oriname"(nullptr)" >> buttonNull.txt
            isFirst=false
        else
            echo ",_"$oriname"(nullptr)" >> buttonNull.txt
        fi
            echo "\tcocos2d::ui::Button* _"$oriname";" >> buttonH.txt
            echo "\tvoid on"$convertname"(Ref* sender);" >> functionH.txt
            echo "void "$inputclass"::on"$convertname"(cocos2d::Ref* sender)\n{\n\n}\n\n" >> functionCPP.txt
            echo "\t\tcase(hashStr("\"$resultstr\"")):
        {\n\t\t\t_"$oriname" = uiutil::ButtonWithResource(ui->toSpriteResource());
            if(_"$oriname" != nullptr)
            {\n\t\t\t\taddChild(_"$oriname");
            \t_"$oriname"->addClickEventListener(CC_CALLBACK_1("$inputclass"::on"$convertname", this));
            }\t\t\n\t\tbreak;" >> buttonCase.txt
        else
            if [ $isFirst == true ]
            then
                echo ":_"$resultstr"(nullptr)" >> spriteNull.txt
                isFirst=false
            else
                echo ",_"$resultstr"(nullptr)" >> spriteNull.txt
            fi
        echo "\tcocos2d::Sprite* _"$resultstr";" >> spriteH.txt
        echo "\t\tcase(hashStr("\"$resultstr\"")):
        {\n\t\t\t_"$resultstr" = uiutil::SpriteWithResource(ui->toSpriteResources());
        \tif(_"$resultstr" != nullptr)
        \t{\n\t\t\t\taddChild(_"$resultstr");
        \t}\n\t\t}\n\t\tbreak;" >> spriteCase.txt
    fi
done < $inputpath

echo "모든파일 통합"
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






