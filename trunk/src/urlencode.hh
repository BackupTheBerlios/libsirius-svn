/* vim: set sw=4 sts=4 et tw=80 : */
#ifndef URLENCODE_HH
#define URLENCODE_HH

#include <string>
#include "container_base.hh"

class URLEncode : public MapBase<std::string, std::string>
{
    public:
        URLEncode()
            : MapBase<std::string, std::string>() { }

        std::string& encode();

        static std::string escape(const std::string &s);

    private:
        std::string m_e;
};

#endif

