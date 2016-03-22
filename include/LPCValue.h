#ifndef base_LpcValue_h
#define base_LpcValue_h

#include "base.h"
#include <string>

NS_CT_BEGIN

typedef union _LPCValueUnion
{
    bool     _boolean;
    int       _integer;
    float    _float;
    std::string*  _string;

} LPCValueUnion;

class LPCValue
{
public:
    enum ValueType
    {
        T_UNDEFINED = 0,
        T_BOOL = 1,
        T_INT = 2,
        T_FLOAT = 3,
        T_STRING = 4,
    };

    ValueType Type() { return _type; };

    bool IsBool() { return _type == T_BOOL; };
    bool IsInt() { return _type == T_INT; };
    bool IsFloat() { return _type == T_FLOAT; };
    bool IsString() { return _type == T_STRING; };

    bool    AsBool()                { return _value._boolean; };
    void    AsBool(bool value)      { _type = T_BOOL; _value._boolean = value; };

    int     AsInt()                 { return _value._integer; };
    void    AsInt(int value)        { _type = T_INT; _value._integer = value; };

    float   AsFloat()               { return _value._float; };
    void    AsFloat(float value)    { _type = T_FLOAT; _value._float = value; };

    std::string  AsString()              { return *(_value._string); };
    void         AsString(std::string value)  { _type = T_STRING; _value._string = new std::string(value); };

    static LPCValue* Create(bool val);
    static LPCValue* Create(int val);
    static LPCValue* Create(float val);
    static LPCValue* Create(std::string val);

private:
    LPCValue();
public:
    ~LPCValue();

private:
    // data type, default is int
    ValueType _type;
    LPCValueUnion _value;
};

NS_CT_END

#endif //base_LpcValue_h
