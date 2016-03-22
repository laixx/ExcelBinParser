//
//  Util.h
//
//
//  Created by peter on 12-12-19.
//  Copyright (c) 2012 CornTree Tech. All rights reserved.
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include "base.h"
#include <vector>
#include <string>


NS_CT_BEGIN

class IPlatformFunc
{
public:
	virtual void log(const char* content) = 0;
    virtual unsigned char* getFileData(const char* pszFileName, unsigned long * pSize) = 0;
};

class Util
{
public:
    static float distance(float x1, float y1, float x2, float y2);
    static float getRadian(float x1, float y1, float x2, float y2);
    
    static void xorBytes(unsigned char* src, int srcLen, unsigned char xorVal);
    
	static void splitString(const std::string src, const std::string delim, std::vector<std::string>& dest);
    
    static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
    
    static void enableLog(bool enable);
    static void log(const char * pszFormat, ...);
    
    static unsigned char* getFileData(const char* pszFileName, unsigned long * pSize, bool isDecrypt = false);

public:
	static IPlatformFunc* s_PlatformFuncImpl;
    
private:
    static bool     s_bEnableLog;

};

NS_CT_END

#endif //__UTIL_H__
