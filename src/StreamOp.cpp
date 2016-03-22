#include "../include/StreamOp.h"

#include <stdlib.h>
#if (defined(WIN32) && defined(_WINDOWS))
#include <windows.h>
#endif

using namespace std;
NS_CT_BEGIN
    
char* U2G(const char* utf8);
int StreamOp::ReadInt(MemStream* stream)
{
    byte buf[4];
    stream->Read(buf, 0, 4);
    int ret = bytes2Int32(buf, 4);
    return ret;
}

byte StreamOp::ReadInt8(MemStream* stream)
{
    byte buf;
    stream->Read(&buf, 0, 1);
    return buf;
}

short StreamOp::ReadInt16(MemStream* stream)
{
    byte buf[2];
    stream->Read(buf, 0, 2);
    short ret = bytes2Int16(buf, 2);
    return ret;
}

float StreamOp::ReadFloat(MemStream* stream)
{
    float f = 0;
    stream->Read((byte*)&f, 0, sizeof(float));
    return f;
}

bool StreamOp::ReadBool(MemStream* stream)
{
    byte buf;
    stream->Read(&buf, 0, 1);
    if (buf == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

string StreamOp::ReadString(MemStream* stream)
{
    int len = ReadInt(stream);

    std::string ret;

    char* pStr = (char*)malloc(len + 1);
    stream->Read((byte*)pStr, 0, len);
    pStr[len] = '\0';
    
#if (defined(WIN32) && defined(_WINDOWS))
    ret = U2G(pStr);
#else
    ret = pStr;
#endif
    
    free(pStr);

    return ret;
}

int StreamOp::bytes2Int32(byte* bts, int length)
{
    if (!bts || length < 4)
    {
        return -1;
    }

    int mask = 0xff;
    int temp = 0;
    int intVal = 0;
    for (int i = 3; i >= 0; i--) {
        intVal <<= 8;
        temp = bts[i] & mask;
        intVal |= temp;
    }
    return intVal;
}

short StreamOp::bytes2Int16(byte* bts, int length)
{
    if (!bts || length < 2)
    {
        return -1;
    }

    int mask = 0xff;
    int temp = 0;
    int intVal = 0;
    for (int i = 1; i >= 0; i--) {
        intVal <<= 8;
        temp = bts[i] & mask;
        intVal |= temp;
    }
    return intVal;
}

#if (defined(WIN32) && defined(_WINDOWS))
//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    if(wstr) delete[] wstr;
    return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if(wstr) delete[] wstr;
    return str;
}
#endif

NS_CT_END
