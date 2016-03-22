
#include "../include/Util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


using namespace std;
NS_CT_BEGIN

IPlatformFunc* Util::s_PlatformFuncImpl = NULL;

float Util::distance(float x1, float y1, float x2, float y2)
{
    float dx = powf(x2 - x1, 2);
    float dy = powf(y2 - y1, 2);
    return sqrtf(dx + dy);
}

float Util::getRadian(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y1 - y2;
    float xie = (float) sqrtf(powf(x, 2) + powf(y, 2));
    float cosAngle = x / xie;
    float rad = (float) acosf(cosAngle);
    if (y2 < y1)
    {
        rad = -rad;
    }
    return rad;
}

void Util::xorBytes(unsigned char *src, int srcLen, unsigned char xorVal)
{
    for (int i = 0; i < srcLen; i++)
    {
        src[i] = src[i] ^ xorVal;
    }
}


void Util::splitString(const string src, const string delim, vector<string>& dest)
{
	int begin = 0;
	int end = 0;
	string str = src;

	do 
	{
		begin = str.find_first_of(delim);
		if (-1 == begin)
		{
			if(str.length() > 0)
				dest.push_back(str);

			break;
		}
		else
		{
			string tmp = str.substr(0, begin);
			if (tmp.length() > 0)
				dest.push_back(tmp);

			end = str.find_first_of(delim, begin + 1);
			if (-1 == end)
			{
				tmp = str.substr(begin + 1);
				if (tmp.length() > 0)
					dest.push_back(tmp);

				break;
			}
			else
			{
				tmp = str.substr(begin + 1, end - begin - 1);
				if (tmp.length() > 0)
					dest.push_back(tmp);

				str = str.substr(end + 1);
			}
		}

	} while (true);
}

void Util::string_replace( std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
    std::string::size_type pos = 0;
    std::string::size_type srclen = strsrc.size();
    std::string::size_type dstlen = strdst.size();
    
    while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
    {
        strBig.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}


// log {
bool Util::s_bEnableLog = true;

void Util::enableLog(bool enable)
{
    s_bEnableLog = enable;
}

void Util::log(const char *pszFormat, ...)
{
    if (s_bEnableLog && s_PlatformFuncImpl)
    {
		const int kMaxLogLen = 16*1024 + 1;
        char szBuf[kMaxLogLen];
        
        va_list ap;
        va_start(ap, pszFormat);
        vsnprintf(szBuf, kMaxLogLen, pszFormat, ap);
        va_end(ap);
		s_PlatformFuncImpl->log(szBuf);
    }
}
// log }

unsigned char* Util::getFileData(const char* pszFileName, unsigned long * pSize, bool isDecrypt/*true*/)
{
    if (s_PlatformFuncImpl)
    {
       return s_PlatformFuncImpl->getFileData(pszFileName, pSize);
    }
	else
    {
        return NULL;
    }
}

NS_CT_END