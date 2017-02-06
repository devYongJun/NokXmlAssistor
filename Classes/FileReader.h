//
//  FileReader.h
//  XmlViewer
//
//  Created by noknok on 2016. 11. 10..
//
//

#ifndef FileReader_h
#define FileReader_h

#include "cocos2d.h"


class YJFileReader
{
public:
    static YJFileReader* create();
    
    std::vector<std::string> getXmlFileList();
    void runCodeGenerateShell(std::string xmlFile, std::string className);
    
private:
    static YJFileReader* _instance;
    
    
};

#endif /* FileReader_h */
