// The MIT License (MIT)
//
// Copyright (c) 2014 kamichidu
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef LOLIVIMSON_HPP_
#define LOLIVIMSON_HPP_

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <exception>
#include <cmath>

#ifdef TRACE
#  define log(x) puts(x)
#else
#  define log(x)
#endif

namespace
{
    class parse_exception
    {
        public:
            parse_exception(int const pos, std::string const& mes) : _pos(pos), _mes(mes) {}
            int const position() const { return _pos; }
            std::string const& message() const { return _mes; }
        private:
            int const _pos;
            std::string const _mes;
    };

    class input
    {
        public:
            explicit input(std::string const& in);
            bool const match(char c);
            bool const match(char c, int offset);
            bool const match_any(std::string const& chars);
            bool const match_any(std::string const& chars, int offset);
            char const getc();
            void skip();
            int const position() const;
            size_t const remaining() const;
        private:
            std::string const _in;
            int _i;
    };

    input::input(std::string const& in)
        : _in(in),
          _i(0)
    {
    }

    bool const input::match(char const c)
    {
        return match(c, 0);
    }

    bool const input::match(char const c, int const offset)
    {
        if(!((_i + offset) < _in.length()))
        {
            return false;
        }
        skip();

        return _in.at(_i + offset) == c;
    }

    bool const input::match_any(std::string const& chars)
    {
        return match_any(chars, 0);
    }

    bool const input::match_any(std::string const& chars, int const offset)
    {
        for(auto const& c : chars)
        {
            if(match(c, offset))
            {
                return true;
            }
        }
        return false;
    }

    char const input::getc()
    {
        if(!(_i < _in.length()))
        {
            throw parse_exception(_i, "eof");
        }

        return _in.at(_i++);
    }

    void input::skip()
    {
        while(_i < _in.length())
        {
            switch(_in.at(_i))
            {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    ++_i;
                    break;
                default:
                    return;
            }
        }
    }

    int const input::position() const
    {
        return _i;
    }

    size_t const input::remaining() const
    {
        return _in.length() - (_i + 1);
    }
}

namespace lolivimson
{
    class value
    {
        public:
            typedef int32_t                   number_t;
            typedef std::string               string_t;
            typedef std::vector<value>        list_t;
            typedef std::map<string_t, value> dictionary_t;
            typedef double                    float_t;
        public:
            value();
            explicit value(number_t const& v);
            explicit value(string_t const& v);
            explicit value(list_t const& v);
            explicit value(dictionary_t const& v);
            explicit value(float_t const& v);
            ~value();
            value(value const& o);
            value& operator = (value const& o);
            template<typename T> bool is() const;
            template<typename T> T const& get() const;
            template<typename T> T& get();
            template<typename Iter> void serialize(Iter os) const;
            std::string const serialize() const;
        private:
            enum type_tag
            {
                number_type,
                string_type,
                // won't support
                // funcref_type,
                list_type,
                dictionary_type,
                float_type,
            };
        private:
            type_tag _type;
            union
            {
                number_t*     _number;
                string_t*     _string;
                list_t*       _list;
                dictionary_t* _dictionary;
                float_t*      _float;
            };
    };

    typedef value::number_t     number_t;
    typedef value::string_t     string_t;
    typedef value::list_t       list_t;
    typedef value::dictionary_t dictionary_t;
    typedef value::float_t      float_t;

    value::value() : _type(type_tag::number_type)
    {
        _number= new number_t(0);
    }

    value::value(number_t const& v) : _type(type_tag::number_type)
    {
        _number= new number_t(v);
    }

    value::value(string_t const& v) : _type(type_tag::string_type)
    {
        _string= new string_t(v);
    }

    value::value(list_t const& v) : _type(type_tag::list_type)
    {
        _list= new list_t(v);
    }

    value::value(dictionary_t const& v) : _type(type_tag::dictionary_type)
    {
        _dictionary= new dictionary_t(v);
    }

    value::value(float_t const& v) : _type(type_tag::float_type)
    {
        _float= new float_t(v);
    }

    value::value(value const& o) : _type(o._type)
    {
        switch(_type)
        {
            case number_type:
                _number= new number_t(*(o._number));
                break;
            case string_type:
                _string= new string_t(*(o._string));
                break;
            case list_type:
                _list= new list_t(*(o._list));
                break;
            case dictionary_type:
                _dictionary= new dictionary_t(*(o._dictionary));
                break;
            case float_type:
                _float= new float_t(*(o._float));
                break;
        }
    }

    value& value::operator = (value const& rhs)
    {
        if(this != &rhs)
        {
            this->~value();
            new (this) value(rhs);
        }
        return *this;
    }

    value::~value()
    {
        switch(_type)
        {
            case number_type:
                delete _number;
                break;
            case string_type:
                delete _string;
                break;
            case list_type:
                delete _list;
                break;
            case dictionary_type:
                delete _dictionary;
                break;
            case float_type:
                delete _float;
                break;
        }
    }

    template<> inline bool value::is<number_t>()     const { return _type == type_tag::number_type; }
    template<> inline bool value::is<string_t>()     const { return _type == type_tag::string_type; }
    template<> inline bool value::is<list_t>()       const { return _type == type_tag::list_type; }
    template<> inline bool value::is<dictionary_t>() const { return _type == type_tag::dictionary_type; }
    template<> inline bool value::is<float_t>()      const { return _type == type_tag::float_type; }

    template<> inline number_t const&     value::get<number_t>()     const { return *_number; }
    template<> inline string_t const&     value::get<string_t>()     const { return *_string; }
    template<> inline list_t const&       value::get<list_t>()       const { return *_list; }
    template<> inline dictionary_t const& value::get<dictionary_t>() const { return *_dictionary; }
    template<> inline float_t const&      value::get<float_t>()      const { return *_float; }

    template<> inline number_t&     value::get<number_t>()     { return *_number; }
    template<> inline string_t&     value::get<string_t>()     { return *_string; }
    template<> inline list_t&       value::get<list_t>()       { return *_list; }
    template<> inline dictionary_t& value::get<dictionary_t>() { return *_dictionary; }
    template<> inline float_t&      value::get<float_t>()      { return *_float; }

    template<typename Iter>
    void serialize_number(number_t const& in, Iter out)
    {
        std::stringstream ss;

        ss << in;

        std::string const s= ss.str();

        for(std::string::const_iterator c= s.begin(); c != s.end(); ++c)
        {
            *out++= *c;
        }
    }

    template<typename Iter>
    void serialize_string(std::string const& s, Iter oi)
    {
        *oi++= '\'';
        for(std::string::const_iterator c= s.begin(); c != s.end(); ++c)
        {
            switch(*c)
            {
                case '\\': *oi++= '\\'; *oi++= '\\'; break;
                case '\'': *oi++= '\''; *oi++= '\''; break;
                case '\n': *oi++= '\\'; *oi++= 'n';  break;
                case '\r': *oi++= '\\'; *oi++= 'r';  break;
                case '\t': *oi++= '\\'; *oi++= 't';  break;
                default:
                    *oi++= *c;
                    break;
            }
        }
        *oi++= '\'';
    }

    template<typename Iter>
    void serialize_list(list_t const& in, Iter out)
    {
        *out++= '[';

        for(auto const& elm : in)
        {
            elm.serialize(out);
            *out++= ',';
        }

        *out++= ']';
    }

    template<typename Iter>
    void serialize_dictionary(dictionary_t const& in, Iter out)
    {
        *out++= '{';

        for(auto const& p : in)
        {
            serialize_string(p.first, out);
            *out++= ':';
            p.second.serialize(out);
            *out++= ',';
        }

        *out++= '}';
    }

    template<typename Iter>
    void serialize_float(float_t const& in, Iter out)
    {
        std::stringstream ss;

        ss << in;

        std::string const s= ss.str();

        for(std::string::const_iterator c= s.begin(); c != s.end(); ++c)
        {
            *out++= *c;
        }
    }

    template<typename Iter>
    void value::serialize(Iter oi) const
    {
        switch(_type)
        {
            case number_type:
                serialize_number(*_number, oi);
                break;
            case string_type:
                serialize_string(*_string, oi);
                break;
            case list_type:
                serialize_list(*_list, oi);
                break;
            case dictionary_type:
                serialize_dictionary(*_dictionary, oi);
                break;
            case float_type:
                serialize_float(*_float, oi);
                break;
        }
    }

    std::string const value::serialize() const
    {
        std::string buf;

        serialize(std::back_inserter(buf));

        return buf;
    }

    lolivimson::number_t const parse_number(input& in)
    {
        log("parse_number()");

        std::stringstream buf;

        if(in.match('-'))
        {
            buf << in.getc();
        }

        do
        {
            buf << in.getc();
        }while(in.match_any("0123456789"));

        log(buf.str().c_str());
        return strtol(buf.str().c_str(), nullptr, 10);
    }

    lolivimson::float_t const parse_float(input& in)
    {
        log("parse_float()");

        std::stringstream buf;

        if(in.match('-'))
        {
            buf << in.getc();
        }

        do
        {
            buf << in.getc();
        }while(in.match_any("0123456789."));

        return strtod(buf.str().c_str(), nullptr);
    }

    lolivimson::value const parse_number_or_float(input& in)
    {
        log("parse_number_or_float()");

        for(int offset= 0; offset < in.remaining(); ++offset)
        {
            if(in.match('.', offset))
            {
                return lolivimson::value(parse_float(in));
            }
            if(!in.match_any("0123456789", offset))
            {
                break;
            }
        }

        return lolivimson::value(parse_number(in));
    }

    lolivimson::string_t const parse_string(input& in)
    {
        log("parse_string()");

        if(in.match('\''))
        {
            in.getc();

            lolivimson::string_t buf;

            while(1)
            {
                if(in.match('\'') && in.match('\'', 1))
                {
                    in.getc();
                    in.getc();
                    buf.push_back('\'');
                }

                if(in.match('\''))
                {
                    break;
                }
                else
                {
                    buf.push_back(in.getc());
                }
            }

            if(in.match('\''))
            {
                in.getc();

                return buf;
            }
            else
            {
                throw parse_exception(in.position(), "expected '");
            }
        }
        if(in.match('"'))
        {
            in.getc();

            lolivimson::string_t buf;

            while(1)
            {
                if(in.match('\\'))
                {
                    in.getc();

                    switch(in.getc())
                    {
                        case '\\': buf.push_back('\\'); break;
                        case '"':  buf.push_back('"'); break;
                        case 't':  buf.push_back('\t'); break;
                        case 'r': buf.push_back('\r'); break;
                        case 'n': buf.push_back('\n'); break;
                        default:
                            throw parse_exception(in.position() - 1, "illegal escape character");
                    }
                }
                else if(in.match('"'))
                {
                    break;
                }
                else
                {
                    buf.push_back(in.getc());
                }
            }

            if(in.match('"'))
            {
                in.getc();

                return buf;
            }
            else
            {
                throw parse_exception(in.position(), "expected \"");
            }
        }

        throw parse_exception(in.position(), "expected ' or \"");
    }

    lolivimson::value const parse_value(input&);

    lolivimson::list_t const parse_list(input& in)
    {
        log("parse_list()");

        if(!in.match('['))
        {
            throw parse_exception(in.position(), "expected [");
        }

        in.getc();

        lolivimson::list_t buf;

        while(!in.match(']'))
        {
            buf.push_back(parse_value(in));

            if(in.match(','))
            {
                in.getc();
            }
        }

        if(in.match(']'))
        {
            in.getc();

            return buf;
        }
        else
        {
            throw parse_exception(in.position(), "expected ]");
        }
    }

    lolivimson::dictionary_t const parse_dictionary(input& in)
    {
        log("parse_dictionary()");

        if(!in.match('{'))
        {
            throw parse_exception(in.position(), "expected {");
        }

        in.getc();

        lolivimson::dictionary_t buf;

        while(!in.match('}'))
        {
            lolivimson::string_t const key= parse_string(in);

            if(in.match(':'))
            {
                in.getc();
            }
            else
            {
                throw parse_exception(in.position(), "expected :");
            }

            buf[key]= parse_value(in);

            if(in.match(','))
            {
                in.getc();
            }
        }

        if(in.match('}'))
        {
            in.getc();

            return buf;
        }
        else
        {
            throw parse_exception(in.position(), "expected }");
        }
    }

    lolivimson::value const parse_value(input& in)
    {
        log("start parse_value()");

        if(in.match('{'))
        {
            return lolivimson::value(parse_dictionary(in));
        }
        if(in.match('['))
        {
            return lolivimson::value(parse_list(in));
        }
        if(in.match('\'') || in.match('"'))
        {
            return lolivimson::value(parse_string(in));
        }
        return parse_number_or_float(in);
    }

    // parsing
    lolivimson::value const parse(std::string const& input)
    {
        try
        {
            ::input in(input);

            return parse_value(in);
        }
        catch(parse_exception const& e)
        {
            std::stringstream mes;

            int const begin_idx= std::min(e.position() - 5, 0);
            int const end_idx= std::min(e.position() + 5, static_cast<int>(input.length() - 1));

            for(int i= begin_idx; i < end_idx; ++i)
            {
                if(input.at(i) != '\r' && input.at(i) != '\n')
                {
                    mes << input.at(i);
                }
                else
                {
                    mes << ' ';
                }
            }
            mes << std::endl;

            int npaddings= e.position() - begin_idx;
            while(npaddings-- > 0)
            {
                mes << ' ';
            }
            mes << "^ " << e.message() << std::endl;

            throw mes.str();
        }
    }
} // namespace lolivimson

#endif // LOLIVIMSON_HPP_
