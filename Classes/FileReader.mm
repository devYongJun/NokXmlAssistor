//
//  FileReader.m
//  XmlViewer
//
//  Created by noknok on 2016. 11. 10..
//
//

#import <Foundation/Foundation.h>
#import "FileReader.h"


@interface IOSFileReader : NSObject

+(IOSFileReader*) getInstance;
-(NSMutableArray*) getFileList:(std::string) xmlPath;

@end

static IOSFileReader* _iosFileReaderInstance = nullptr;

@implementation IOSFileReader

+(IOSFileReader*) getInstance
{
    if(nullptr == _iosFileReaderInstance)
    {
        _iosFileReaderInstance = [[IOSFileReader alloc] init];
    }
    return _iosFileReaderInstance;
}

-(NSMutableArray*) getFileList:(std::string) xmlPath
{
    NSArray* dirs = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithUTF8String:xmlPath.c_str()] error:NULL];

    NSMutableArray *xmlFiles = [[NSMutableArray alloc] init];
    
    [dirs enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop)
    {
        NSString *filename = [NSString stringWithFormat:@"%@",(NSString *)obj];
        
        NSRange subRange;
        subRange = [filename rangeOfString : @"xml"];
        if (subRange.location != NSNotFound)
        {
            [xmlFiles addObject:filename];
        }
    }];
    
    return xmlFiles;
}

@end





YJFileReader* YJFileReader::_instance = nullptr;

YJFileReader* YJFileReader::create()
{
    if(nullptr == _instance)
    {
        _instance = new (std::nothrow) YJFileReader;
    }
    return _instance;
}

std::vector<std::string> YJFileReader::getXmlFileList()
{
    std::string xmlPath = cocos2d::UserDefault::getInstance()->getStringForKey("KEY_XML_PATH");
    NSArray* dirs = [[IOSFileReader getInstance] getFileList:xmlPath];
    
    std::vector<std::string> retList;
    
    for(int i = 0; i < [dirs count]; i++)
    {
        retList.push_back(std::string([dirs[i] UTF8String]));
    }
    
    return retList;
}







