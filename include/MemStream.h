#ifndef base_MemStream_h
#define base_MemStream_h

#include "base.h"

NS_CT_BEGIN

typedef unsigned char byte;

class MemStream
{
public:
    MemStream(byte* buf, int length);
    MemStream(byte* buf, int startIndex, int length);
    ~MemStream();

    int Pos() { return _Pos; };
    int Length() { return _Length; };

    int Write(byte* srcBuf, int startIndex, int length); // unimplement
    int Read(byte* outBuf, int startIndex, int length);

private:
    byte* _pBuf;
    int   _Pos;
    int   _Length;
};

NS_CT_END

#endif //base_MemStream_h
