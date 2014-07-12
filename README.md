lolivimson
====================================================================================================
A header only library for VIM Script Object Notation (vimson). It can serialize C++ object to a
string, and can de-serialize a vimson string to C++ objects.


Installation
----------------------------------------------------------------------------------------------------
If you wanna use this library, you can copy src/lolivimson.hpp to your include path directory. And
include it.


Using in C++
----------------------------------------------------------------------------------------------------
Its API is similar to [picojson](https://github.com/kazuho/picojson).

```cpp:example
lolivimson::number_t num(0);
lolivimson::float_t flt(0.0);
lolivimson::string_t str("");
lolivimson::list_t list;
lolivimson::dictionary_t dict;

dict["key1"]= lolivimson::value(num);

list.push_back(lolivimson::value(flt));
list.push_back(lolivimson::value(str));

dict["key2"]= lolivimson::value(list);

lolivimson::value value(dict);

std::cout << value.serialize() << std::endl;

lolivimson::value deserialized= lolivimson::parse(value.serialize());

if(deserialized.is<lolivimson::dictionary_t>())
{
    lolivimson::dictionary_t dict2= deserialized.get<lolivimson::dictionary_t>();
}
```


Licence
----------------------------------------------------------------------------------------------------
The MIT License (MIT)

Copyright (c) 2014 kamichidu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
