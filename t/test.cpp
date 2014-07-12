#include <iostream>
#include <lolivimson.hpp>
#include <string>
#include <exception>
#include <fstream>

namespace vimson= lolivimson;

int main(int argc, char const* argv[])
{
    if(!(argc > 1))
    {
        return 1;
    }

    try
    {
        std::string in;
        {
            std::ifstream ifs(argv[1]);

            ifs.seekg(0, std::ios::end);
            size_t const length= ifs.tellg();
            ifs.seekg(0, std::ios::beg);

            char* buf= new char[length];
            ifs.read(buf, length);
            in.append(buf);
            delete [] buf;
        }

        std::cout << in;

        std::cerr << "start parse" << std::endl;
        vimson::value const value= vimson::parse(in);
        std::cerr << "end parse" << std::endl;

        std::cout << "res" << std::endl;
        std::cout << value.serialize() << std::endl;
    }
    catch(std::string const& e)
    {
        std::cerr << e << std::endl;
    }
}
