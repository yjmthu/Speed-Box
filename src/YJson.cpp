﻿#include <iostream>
#include <ostream>
#include <fstream>
#include <deque>
#include "funcbox.h"
#include "YJson.h"
#include "YString.h"
#include "YEncode.h"

#define TYPE_CHAECK()     static_assert(std::is_same<T, std::string::const_iterator>::value ||\
                                        std::is_same<T, std::vector<char>::const_iterator>::value ||\
                                        std::is_same<T, std::deque<char>::const_iterator>::value ||\
                                        std::is_same<T, const char*>::value, "error in type")

std::pair<bool, std::string> YJson::ep = std::pair<bool, std::string>(false, std::string());
YJson::YJson(const char* str) { strict_parse<const char*>(str); }
YJson::YJson(const std::string& str) { strict_parse<std::string::const_iterator>(str.cbegin()); }

YJson::YJson(const std::wstring& str)
{
    std::deque<char> dstr;
    utf16_to_utf8<std::deque<char>&, std::wstring::const_iterator>(dstr, str.cbegin());
    strict_parse(dstr.cbegin());
}

YJson::YJson(const wchar_t* str)
{
    std::deque<char> dstr;
    utf16_to_utf8<std::deque<char>&, const wchar_t*>(dstr, str);
    strict_parse(dstr.cbegin());
}

template<typename T>
bool YJson::strict_parse(T temp)
{
    TYPE_CHAECK();
    temp = StrSkip(temp);
    switch (*temp)
    {
        case '{':
            parse_object(temp);
            break;
        case '[':
            parse_array(temp);
            break;
        default:
            break;
    }
    if (ep.first)
        std::cout << "error:\t" << ep.second << std::endl;
    return ep.first;
}

void YJson::LoadFile(std::ifstream& file)
{
    qout << "从文件加载";
    //std::ifstream file(str, std::ios::in | std::ios::binary);
    _type = YJSON_TYPE::YJSON_OBJECT;
    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        int size = file.tellg();
        if (size < 6)
        {
            file.close();
            return ;
        }
        unsigned char c[3] = {0}; file.seekg(0, std::ios::beg);
        if (!(file.read(reinterpret_cast<char*>(c), sizeof(char) *3)))
        {
            file.close();
            ep.first = true;
            ep.second = "文件不可读";
            return;
        }
        size -= sizeof(char) * 3;
        if (c[0] == 0xef && c[1] == 0xbb && c[2] == 0xbf)
        {
            qout << "utf8编码格式";
            std::vector<char> json_vector;
            json_vector.resize(size + 1);
            json_vector[size] = 0;
            file.read(reinterpret_cast<char*>(&json_vector[0]), size);
            file.close();
            strict_parse(json_vector.cbegin());
        }
        else if (c[0] == 0xff && c[1] == 0xfe)
        {
            qout << "utf16编码格式";
            if ((size + sizeof (char)) % sizeof (wchar_t))
            {
                ep.first = true;
                ep.second = "文件出错";
                file.close(); return;
            }
            else
            {
                std::deque<char> json_str;
                wchar_t* json_str_temp = new wchar_t[size/sizeof(wchar_t) + 1];
                char *ptr = reinterpret_cast<char*>(json_str_temp); *ptr = c[2];
                file.read(++ptr, size);
                utf16_to_utf8<std::deque<char>&, const wchar_t*>(json_str, json_str_temp);
                delete [] json_str_temp; file.close();
                strict_parse(json_str.cbegin());
            }
        }
        else
        {
            file.close();
            ep.first = true;
            ep.second = "文件编码格式不支持。";
        }
    }
    else
    {
        ep.first = true; ep.second = "文件未打开";
    }
    //qout << "文件初始化结束";
}

//复制json，当parent为NULL，如果自己有key用自己的key
void YJson::CopyJson(const YJson* json, YJson* parent)
{
    if (ep.first) return;
    _type = json->_type;

    if (!(_parent = parent) || parent->_type != YJSON_TYPE::YJSON_OBJECT)
    {
        delete[] _keystring; _keystring = nullptr;
    }
    else if (!_keystring && json->_keystring)
    {
        _keystring = StrJoin<char>(json->_keystring);
    }

    if (_type == YJSON_TYPE::YJSON_ARRAY || _type == YJSON_TYPE::YJSON_OBJECT)
    {
        YJson *child = nullptr, *childx = nullptr;
        if (child = json->_child)
        {
            childx = _child = new YJson; _child->CopyJson(child, this);
            while (child = child->_next)
            {
                childx->_next = new YJson; childx->_next->CopyJson(child, this);
                childx->_next->_prev = childx; childx = childx->_next;
            }
        }
    }
    else if (_type == YJSON_TYPE::YJSON_STRING)
        _valuestring = StrJoin<char>(json->_valuestring);
    else if (_type == YJSON_TYPE::YJSON_NUMBER)
    {
        _valueint = json->_valueint;
        _valuedouble = json->_valuedouble;
    }
}

//清除_valuestring, _child, 以及ep、valueint、 _valuedouble
void YJson::clearContent()
{
    delete _valuestring, _valuestring = nullptr;
    delete _child, _child = nullptr;
    if (ep.first) ep.first = false; _valueint = 0; _valuedouble = 0;
}

int YJson::getChildNum() const
{
    int j = 0; YJson* child = _child;
    if (_child) { ++j; while (child = child->_next) ++j; }
    return j;
}

YJson::~YJson()
{
    delete[] _keystring;
    delete[] _valuestring;
    delete _child;
    if (_prev) _prev->_next = nullptr;
    if (_next)
    {
        YJson * temp_next  = _next;
        _next = nullptr;
        temp_next->_prev = nullptr;
        delete temp_next;
    }
}

YJson* YJson::find(const char* key) const
{
    if (_type == YJSON_TYPE::YJSON_OBJECT)
    {
        YJson *child = _child;
        if (child && child->_keystring)
        {
            do {
                if (!strcmp<const char*>(child->_keystring, key))
                    return child;
            } while (child = child->_next);
        }
    }
    return nullptr;
}

YJson* YJson::find(int key) const
{
    if (_type == YJSON_TYPE::YJSON_ARRAY)
    {
        YJson *child = _child;
        if (child)
        {
            if (key >= 0)
            {
                do {
                    if (!(key--)) return child;
                } while (child = child->_next);
            }
            else
            {
                int i = 0;
                while (child->_next) ++i, child = child->_next;
                if (-key <= ++i)
                {
                    while (++key) child = child->_prev;
                    return child;
                }
                
            }
        }
    }
    return nullptr;
}

YJson* YJson::findByValue(int value) const
{
    if (_type == YJSON_TYPE::YJSON_ARRAY || _type == YJSON_TYPE::YJSON_OBJECT)
    {
        YJson *child = _child;
        if (child) do 
            if (child->_type==YJSON_TYPE::YJSON_NUMBER && child->_valueint==value && fabs(((double)value)-child->_valuedouble) <= DBL_EPSILON)
                return child;
        while (child = child->_next);
    }
    return nullptr;
}

YJson* YJson::findByValue(double value) const
{
    if (_type == YJSON_TYPE::YJSON_ARRAY || _type == YJSON_TYPE::YJSON_OBJECT)
    {
        YJson *child = _child;
        if (child) do
            if (child->_type == YJSON_TYPE::YJSON_NUMBER && fabs(value-child->_valuedouble) <= DBL_EPSILON)
                return child;
        while (child = child->_next);
        
    }
    return nullptr;
}

YJson* YJson::findByValue(const char* str) const
{
    if (_type == YJSON_TYPE::YJSON_ARRAY || _type == YJSON_TYPE::YJSON_OBJECT)
    {
        YJson *child = _child;
        if (child) do
            if (child->_type == YJSON_TYPE::YJSON_STRING && !strcmp<const char*>(child->_valuestring, str))
                return child;
        while (child = child->_next);
        
    }
    return nullptr;
}

const YJson* YJson::getTop() const
{
    const YJson* top = this;
    while (true)
    {
        if (top->_parent) top = top->_parent;
        else return top;
    }
}

YJson* YJson::append(YJSON_TYPE type)
{
    YJson* child = _child;
    if (child)
    {
        while (child->_next) child = child->_next;
        child->_next = new YJson;
        child->_next->_prev = child; child = child->_next;
    }
    else
    {
        _child = child = new YJson; _child->_parent = this;
    }
    child->_parent = this;
    child->_type = type;
    return child;
}

YJson* YJson::append(const YJson& js, const char* key)
{
    if (_type == YJSON_TYPE::YJSON_OBJECT)
        {if (!key && !js._keystring) return nullptr;}
    else
        {if (key || js._keystring) return nullptr;}
    YJson* child = append(js._type); *child = js;
    if (key)
    {
        delete[] child->_keystring;
        child->_keystring = StrJoin<char>(key);
    }
    return child;
}

YJson* YJson::append(YJSON type, const char* key)
{
    YJson* child = append(static_cast<YJSON_TYPE>(type));
    if (key) child->_keystring = StrJoin<char>(key);
    return child;
}

YJson* YJson::append(int value, const char* key)
{
    if (_type == YJSON_TYPE::YJSON_OBJECT) {if (!key) return nullptr;}
    else {if (key) return nullptr;}
    YJson* child = append(YJSON_TYPE::YJSON_NUMBER);
    child->_valuedouble = child->_valueint = value;
    if (key) child->_keystring = StrJoin<char>(key);
    return child;
}

YJson* YJson::append(double value, const char* key)
{
    if (_type == YJSON_TYPE::YJSON_OBJECT)
        {if (!key) return nullptr;}
    else
        {if (key) return nullptr;}
    YJson* child = append(YJSON_TYPE::YJSON_NUMBER);
    child->_valueint = (int)value; child->_valuedouble = value;
    if (key) child->_keystring = StrJoin<char>(key);
    return child;
}

YJson* YJson::append(const char* str, const char* key)
{
    if (_type == YJSON_TYPE::YJSON_OBJECT) {
        if (!key) return nullptr;
    }
    else if (key || _type != YJSON_TYPE::YJSON_ARRAY) return nullptr;
    YJson* child = append(YJSON_TYPE::YJSON_STRING);
    child->_valuestring = StrJoin<char>(str); if (key) child->_keystring = StrJoin<char>(key);
    return child;
}

bool YJson::remove(YJson* item)
{
    if (item)
    {
        if (item->_prev)
        {
            if (item->_prev->_next = item->_next) item->_next->_prev = item->_prev;
        }
        else
        {
            if (_child = item->_next)
            {
                _child->_parent = this;
                _child->_prev = nullptr;
            }
        }
        item->_prev = nullptr; item->_next = nullptr; delete item;
        return true;
    }
    else
        return false;
}

YJson& YJson::operator=(const YJson& s)
{
    if (&s == this)
        return *this;
    else if (s.getTop() == this->getTop())
        return (*this = YJson(s));
    clearContent(); CopyJson(&s, _parent);
    return *this;
}

YJson& YJson::operator=(YJson&& s)
{
    if (&s == this)
        return *this;
    else if (s.getTop() == this->getTop())
        return (*this = YJson(s));
    clearContent(); CopyJson(&s, _parent);
    return *this;
}

bool YJson::join(const YJson & js)
{
    if (&js == this)
    {
        return join(YJson(*this));
    }
    if (ep.first || _type != js._type || (_type != YJSON_TYPE::YJSON_ARRAY && _type != YJSON_TYPE::YJSON_OBJECT))
        return false;
    YJson *child = _child;
    YJson *childx = js._child;
    if (child)
    {
        while (child->_next) child = child->_next;
        if (childx)
        {
            do {
                child->_next = new YJson(*childx);
                child->_next->_keystring = StrJoin<char>(childx->_keystring);
                child->_next->_prev = child;
                child = child->_next;
            } while (childx = childx->_next);
        }
    }
    else if (childx)
    {
        *this = js;
    }
    return true;
}

YJson YJson::join(const YJson & j1, const YJson & j2)
{
    if (ep.first || j1._type != YJSON_TYPE::YJSON_ARRAY || j1._type != YJSON_TYPE::YJSON_OBJECT || j2._type != j1._type)
    {
        return YJson();
    }
    else
    {
        YJson js(j1); js.join(j2);
        return YJson(js);
    }
}

char* YJson::toString(bool fmt)
{
    if (ep.first) return nullptr;
    return fmt?print_value(0):print_value();
}

bool YJson::toFile(const std::wstring name, const YJSON_ENCODE& file_encode, bool fmt)
{
    //qout << "开始打印";
    if (ep.first) return false;
    char* buffer = fmt?print_value(0):print_value();
    //qout << "打印成功";
    if (buffer)
    {
        switch (file_encode) {
        case (YJSON_ENCODE::UTF16):
        {
            std::cout << "UTF-16" << u8"保存开始。";
            std::wstring data;
            data.push_back(0xfeff);
            utf8_to_utf16<std::wstring&, const char*>(data, buffer);
            data.back() = L'\n';
            std::ofstream outFile(name, std::ios::out | std::ios::binary);
            if (outFile.is_open())
            {
                outFile.write(reinterpret_cast<const char*>(data.data()), data.length()*sizeof(wchar_t));
                outFile.close();
            }
            break;
        }
        default:
        {
            //cout << "UTF-8" << "保存开始。";
            const unsigned char c[3] = {0xef, 0xbb, 0xbf};
            std::ofstream outFile(name, std::ios::out | std::ios::binary);
            if (outFile.is_open())
            {
                outFile.write(reinterpret_cast<const char*>(c), sizeof(char)*3);
                outFile.write((const char*)(buffer), strlen(buffer)*sizeof(char));
                outFile.write("\n", sizeof(char));
                outFile.close();
            }
            break;
        }
        }
        delete [] buffer;
    }
    return false;
}

char* YJson::joinKeyValue(const char* valuestring, int depth, bool delvalue)
{
    char* buffer = nullptr;
    char* x = StrRepeat(' ', depth*4);
    if (_keystring)
    {
        char *key = print_string(_keystring);
        buffer = StrJoin<char>(x, key, ": ", valuestring);
        delete[] key;
    }
    else
    {
        buffer = StrJoin<char>(x, valuestring);
    }
    delete[] x;
    if (delvalue) delete[] valuestring;
    return buffer;
}

char* YJson::joinKeyValue(const char* valuestring,  bool delvalue)
{
    char* buffer = nullptr;
    if (_keystring)
    {
        char *key = print_string(_keystring);
        buffer = StrJoin<char>(key, ":", valuestring);
        delete[] key;
    }
    else
    {
        buffer = StrJoin<char>(valuestring);
    }
    if (delvalue) delete[] valuestring;
    return buffer;
}

template <typename T>
T YJson::parse_value(T value)
{
    TYPE_CHAECK();
    //qout << "加载数据";
    if (!*value || ep.first || T() == value) return T();
    //qout << "剩余字符串开头：" << *value;
    if (!strncmp(value, "null", 4))     { _type= YJSON_TYPE::YJSON_NULL;  return value+4; }
    if (!strncmp(value, "false", 5))    { _type= YJSON_TYPE::YJSON_FALSE; return value+5; }
    if (!strncmp(value, "true", 4))     { _type= YJSON_TYPE::YJSON_TRUE; _valueint=1;    return value+4; }
    if (*value=='\"')                   { return parse_string(value); }
    if (*value=='-' || (*value>='0' && *value<='9'))    { return parse_number(value); }
    if (*value=='[')                    { return parse_array(value); }
    if (*value=='{')                    { return parse_object(value); }

    //qout << "加载数据出错";
    return T();                              /* failure. */
}

char* YJson::print_value()
{
    switch (_type)
    {
    case YJSON_TYPE::YJSON_NULL:
        return  joinKeyValue("null", false);
    case YJSON_TYPE::YJSON_FALSE:
        return joinKeyValue("false", false);
    case YJSON_TYPE::YJSON_TRUE:
        return joinKeyValue("true", false);
    case YJSON_TYPE::YJSON_NUMBER:
        return joinKeyValue(print_number(), true);
    case YJSON_TYPE::YJSON_STRING:
        return joinKeyValue(print_string(_valuestring), true);;
    case YJSON_TYPE::YJSON_ARRAY:
        return print_array();
    case YJSON_TYPE::YJSON_OBJECT:
        return print_object();
    default:
        return nullptr;
    }
}

char* YJson::print_value(int depth)
{
    switch (_type)
    {
    case YJSON_TYPE::YJSON_NULL:
        return  joinKeyValue("null", depth, false);
    case YJSON_TYPE::YJSON_FALSE:
        return joinKeyValue("false", depth, false);
    case YJSON_TYPE::YJSON_TRUE:
        return joinKeyValue("true", depth, false);
    case YJSON_TYPE::YJSON_NUMBER:
        return joinKeyValue(print_number(), depth, true);
    case YJSON_TYPE::YJSON_STRING:
        return joinKeyValue(print_string(_valuestring), depth, true);;
    case YJSON_TYPE::YJSON_ARRAY:
        return print_array(depth);
    case YJSON_TYPE::YJSON_OBJECT:
        return print_object(depth);
    default:
        return nullptr;
    }
}

template<typename T>
T YJson::parse_number(T num)
{
    TYPE_CHAECK();
    _valuedouble = 0;
    short sign = 1;
    int scale = 0;
    int signsubscale = 1, subscale = 0;
    if (*num=='-') {
        sign = -1;
        ++num;
    }
    if (*num=='0')
    {
        _valuedouble = _valueint = 0;
        _type = YJSON_TYPE::YJSON_NUMBER;
        return ++num;
    }
    if ('1' <= *num && *num <= '9')
        do _valuedouble += (_valuedouble*=10.0, *num++ - '0');
        while ('0' <= *num && *num <= '9');
    if ('.' == *num && '0' <= num[1] && num[1] <= '9')
    {
        ++num;
        do  _valuedouble += (_valuedouble*=10.0, *num++ - '0'), scale--;
        while ('0' <= *num && *num <= '9');
    }
    if ('e' == *num || 'E' == *num)
    {
        if (*++num == '-')
        {
            signsubscale = -1;
            ++num;
        }
        else if (*num == '+')
        {
            ++num;
        }
        while ('0' <= *num && *num <= '9')
        {
            subscale *= 10;
            subscale += *num++ - '0';
        }
    }
    _valueint = static_cast<int>(_valuedouble *= sign * pow(10, scale + signsubscale * subscale));
    _type = YJSON_TYPE::YJSON_NUMBER;
    return num;
}

char* YJson::print_number()
{
    char* buffer = nullptr;
    if (_valuedouble == 0)
        buffer = StrJoin<char>("0");
    else if (fabs(((double)_valueint) - _valuedouble) <= DBL_EPSILON && _valuedouble <= INT_MAX && _valuedouble >= (double)INT_MIN)
    {
        char temp[21] = { 0 }; sprintf(temp,"%d",_valueint);
        buffer = StrJoin<char>(temp);
    }
    else
    {
        char temp[64] = {0};
        if (fabs(floor(_valuedouble)-_valuedouble)<=DBL_EPSILON && fabs(_valuedouble)<1.0e60)
            sprintf(temp,"%.0f",_valuedouble);
        else if (fabs(_valuedouble)<1.0e-6 || fabs(_valuedouble)>1.0e9)
            sprintf(temp,"%e",_valuedouble);
        else
            sprintf(temp,"%f",_valuedouble);
        buffer = StrJoin<char>(temp);
    }
    return buffer;
}

template<typename T>
inline bool _parse_hex4(T str, uint16_t& h)
{
    TYPE_CHAECK();
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return true;
    return false;
}

template<typename T>
uint16_t parse_hex4(T str)
{
    TYPE_CHAECK();
    uint16_t h = 0;
    if (_parse_hex4(str, h) || _parse_hex4(++str, h = h<<4) || _parse_hex4(++str, h = h<<4) || _parse_hex4(++str, h = h<<4))
        return 0;
    return h;
}

template<typename T>
T YJson::parse_string(T str)
{
    TYPE_CHAECK();
    //cout << "加载字符串：" << str << endl;
    T ptr = str + 1;
    char* ptr2; uint32_t uc, uc2;
    size_t len = 0;
    if (*str != '\"') {
        //cout << "不是字符串！" << endl;
        ep.first = true;
        return T();
    }
    while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;    /* Skip escaped quotes. */
    _valuestring = new char[len + 1];
    ptr = str + 1;
    ptr2 = _valuestring;
    while (*ptr != '\"' && *ptr)
    {
        if (*ptr != '\\') *ptr2++ = *ptr++;
        else
        {
            ptr++;
            switch (*ptr)
            {
            case 'b': *ptr2++ = '\b';    break;
            case 'f': *ptr2++ = '\f';    break;
            case 'n': *ptr2++ = '\n';    break;
            case 'r': *ptr2++ = '\r';    break;
            case 't': *ptr2++ = '\t';    break;
            case 'u': uc=parse_hex4(ptr+1);ptr+=4;                                                   /* get the unicode char. */

                if ((uc>=utf16FirstWcharMark[1] && uc<utf16FirstWcharMark[2]) || uc==0)    break;    /* check for invalid.    */

                if (uc>=utf16FirstWcharMark[0] && uc<utf16FirstWcharMark[1])                         /* UTF16 surrogate pairs.    */
                {
                    if (ptr[1]!='\\' || ptr[2]!='u')    break;                                       /* missing second-half of surrogate.    */
                    uc2=parse_hex4(ptr+3);ptr+=6;
                    if (uc2<utf16FirstWcharMark[1] || uc2>=utf16FirstWcharMark[2])        break;     /* invalid second-half of surrogate.    */
                    uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                }

                len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;

                switch (len) {
                    case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                    case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                    case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                    case 1: *--ptr2 =(uc | utf8FirstCharMark[len]);
                }
                ptr2+=len;
                break;
            default:  *ptr2++ = *ptr; break;
            }
            ptr++;
        }
    }
    *ptr2 = 0; ++str;
    _type= YJSON_TYPE::YJSON_STRING;
    while (*str != '\"' && *str) if (*str++ == '\\') ++str;
    //qout << "所得字符串：" << _valuestring;
    return ++str;
}

char* YJson::print_string(const char* const str)
{
    //qout << "打印字符开始";

    char* buffer = nullptr;
    //cout << "输出字符串" << str << endl;
    const char* ptr; char* ptr2;
    size_t len = 0, flag = 0; unsigned char token;
    
    for (ptr = str; *ptr; ptr++)
        flag |= ((*ptr > 0 && *ptr < 32) || (*ptr == '\"') || (*ptr == '\\')) ? 1 : 0;
    if (!flag)
    {
        len = ptr - str;
        buffer = new char[len + 3];
        ptr2 = buffer; *ptr2++ = '\"';
        memcpy(ptr2, str, len);
        *(ptr2 += len) = '\"'; *++ptr2 = 0;
        return buffer;
    }
    if (!str)
    {
        buffer = StrJoin<char>("\"\"");
        return buffer;
    }
    ptr = str;
    while ((token = (unsigned char)*ptr) && ++len)
    {
        if (strchr("\"\\\b\f\n\r\t", token)) len++;
        else if (token < 32) len += 5; ptr++;
    }
    
    memset(buffer = new char[len + 3], 0, (len + 3) * sizeof(char));

    ptr2 = buffer; ptr = str;
    *ptr2++ = '\"';
    while (*ptr)
    {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\') *ptr2++ = *ptr++;
        else
        {
            *ptr2++='\\';
            switch (token = (unsigned char)*ptr++)
            {
            case '\\':    *ptr2++ = '\\';   break;
            case '\"':    *ptr2++ = '\"';   break;
            case '\b':    *ptr2++ = 'b';    break;
            case '\f':    *ptr2++ = 'f';    break;
            case '\n':    *ptr2++ = 'n';    break;
            case '\r':    *ptr2++ = 'r';    break;
            case '\t':    *ptr2++ = 't';    break;
            default: sprintf(ptr2,"u%04x",token);ptr2+=5;    break;
            }
        }
    }
    *ptr2 = '\"'; *++ptr2 = 0;
    return buffer;
}

template<typename T>
T YJson::parse_array(T value)
{
    TYPE_CHAECK();
    //qout << "加载列表";
    YJson *child = nullptr;
    _type = YJSON_TYPE::YJSON_ARRAY;
    value=StrSkip(++value);
    if (*value==']')
    {
        return value+1;    /* empty array. */
    }
    _child = child = new YJson;
    _child->_parent = this;
    value = StrSkip(child->parse_value(StrSkip(value)));    /* skip any spacing, get the value. */
    if (!*value) return T();

    while (*value==',')
    {
        YJson *new_ = new YJson;
        child->_next = new_;
        new_->_prev = child;
        child = new_; child->_parent = this;
        value = StrSkip(child->parse_value(StrSkip(value + 1)));
        if (!*value) return T();
    }

    if (*value == ']')
    {
        //qout << "加载列表结束";
        return value + 1;
    }
    ep.first = true;
    ep.second = "未匹配到列表结尾‘]’。";
    return T();
}

char* YJson::print_array()
{
    //qout << "打印列表开始";
    std::deque<char*> entries;
    YJson *child = _child;

    if (!child)
    {
        return joinKeyValue("[]", false);
    }
    do {
        entries.push_back(child->print_value());
    } while (child = child->_next);
    return StrJoin<1>(joinKeyValue("[", false), "]", ",", entries);
}

char* YJson::print_array(int depth)
{
    //qout << "打印列表开始";
    std::deque<char*> entries;
    YJson *child = _child;
    char*buffer = nullptr;
    if (!child)
    {
        return joinKeyValue("[]", false);
    }
    do {
        entries.push_back(child->print_value(depth+1));
    } while (child = child->_next);

    char* str_start = joinKeyValue("[\n", depth, false);
    ++(depth *= 4);
    char *str_end = new char[2+depth] {0};
    *str_end = '\n'; str_end[depth] = ']';
    while (--depth) str_end[depth] = ' ';
    //qout << "打印列表结束";
    buffer = StrJoin<2>(str_start, str_end, ",\n", entries);
    delete [] str_end;
    return buffer;
}

template<typename T>
T YJson::parse_object(T value)
{
    TYPE_CHAECK();
    //qout << "加载字典：";
    YJson *child = nullptr;
    _type = YJSON_TYPE::YJSON_OBJECT;
    value = StrSkip(++value);
    if (*value == '}') return value + 1;
    _child = child = new YJson;
    _child->_parent = this;
    value = StrSkip(child->parse_string(StrSkip(value)));
    if (!*value) return T();
    child->_keystring = child->_valuestring;
    child->_valuestring = nullptr;

    if (*value != ':')
    {
        ep.first = true;
        ep.second = "错误：加载字典时，没有找到冒号。";
        return T();
    }
    value = StrSkip(child->parse_value(StrSkip(value + 1)));
    while (*value==',')
    {
        YJson *new_ = new YJson;
        child->_next = new_; new_->_prev = child; child = new_; child->_parent = this;
        value = StrSkip(child->parse_string(StrSkip(value+1)));
        child->_keystring = child->_valuestring; child->_valuestring = nullptr;
        if (*value!=':')
        {
            ep.first = true;
            ep.second = "错误：加载字典时，没有匹配到‘:’。";
            return T();
        }
        value = StrSkip(child->parse_value(StrSkip(value+1)));    /* skip any spacing, get the value. */
        if (!(*value)) return T();
    }
    
    if (*value=='}')
    {
        //qout << "加载字典结束";
        return value+1;
    }
    ep.first = true;
    ep.second = "错误：加载字典时，没有匹配到结尾‘}’。";
    return T();
}

char* YJson::print_object()
{
    //qout << "打印字典开始";

    std::deque<char*> entries;
    YJson *child = _child;
    char* buffer = nullptr;
    if (!child)
    {
        return joinKeyValue("{}", false);;
    }
    do {
        if (!(buffer = child->print_value()))
            return nullptr;
        entries.push_back(buffer);
    } while (child = child->_next);
    return StrJoin<1>(joinKeyValue("{", false), "}", ",", entries);
}

char* YJson::print_object(int depth)
{
    //qout << "打印字典开始";

    std::deque<char*> entries;
    YJson *child = _child;
    char* buffer = nullptr;
    if (!child)
    {
        return joinKeyValue("{}", false);;
    }
    do {
        if (!(buffer = child->print_value(depth+1)))
            return nullptr;
        entries.push_back(buffer);
    } while (child = child->_next);
    //qout << "开始拼接字符串";
    char *str_start = joinKeyValue("{\n", depth, false);
    ++(depth *= 4);
    char *str_end = new char[2+depth] {0};
    *str_end = '\n'; str_end[depth] = '}';
    while (--depth) str_end[depth] = ' ';
    buffer = StrJoin<2>(str_start, str_end, ",\n", entries);
    delete [] str_end;
    return buffer;
}
