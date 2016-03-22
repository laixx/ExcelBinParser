#ifndef base_Stream_Op_h
#define base_Stream_Op_h

#include "MemStream.h"
#include <string>

NS_CT_BEGIN

class StreamOp
{
public:
    static int      ReadInt(MemStream* stream);
    static byte     ReadInt8(MemStream* stream);
    static short    ReadInt16(MemStream* stream);
    static float    ReadFloat(MemStream* stream);
    static bool     ReadBool(MemStream* stream);
    static std::string   ReadString(MemStream* stream);

    static int      bytes2Int32(byte* bts, int length);
    static short    bytes2Int16(byte* bts, int length);
};

NS_CT_END

#endif //base_Stream_Op_h
