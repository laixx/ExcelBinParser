#include "../include/LPCValue.h"


using namespace std;
NS_CT_BEGIN

LPCValue::LPCValue()
: _type(T_INT)
{
}

LPCValue::~LPCValue()
{
    if (IsString())
    {
        _type = T_UNDEFINED;

        delete _value._string;
        _value._string = 0;
    }
}

LPCValue* LPCValue::Create(bool val)
{
    LPCValue* v = new LPCValue();
    v->AsBool(val);
    return v;
}

LPCValue* LPCValue::Create(int val)
{
    LPCValue* v = new LPCValue();
    v->AsInt(val);
    return v;
}

LPCValue* LPCValue::Create(float val)
{
    LPCValue* v = new LPCValue();
    v->AsFloat(val);
    return v;
}

LPCValue* LPCValue::Create(string val)
{
    LPCValue* v = new LPCValue();
    v->AsString(val);
    return v;
}

NS_CT_END
