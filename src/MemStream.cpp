#include "../include/MemStream.h"

NS_CT_BEGIN
    
MemStream::MemStream(byte* buf, int length)
: _pBuf(buf)
, _Length(length)
, _Pos(0)
{
}

MemStream::MemStream(byte* buf, int startIndex, int length)
: _pBuf(buf)
, _Length(length)
, _Pos(startIndex)
{
}

MemStream::~MemStream()
{
    if (_pBuf)
    {
        delete _pBuf;
        _pBuf = 0;
    }
}

int MemStream::Write(byte* srcBuf, int startIndex, int length)
{
    return 0;
}

int MemStream::Read(byte* srcBuf, int startIndex, int length)
{
    int ret = 0;

    if (!_pBuf || !srcBuf)
    {
        return ret;
    }

    while (ret < length && _Pos < _Length)
    {
        srcBuf[startIndex + ret] = _pBuf[_Pos];
        _Pos++;
        ret++;
    }
    return ret;
}

NS_CT_END
