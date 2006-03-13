/* vim: set sw=4 sts=4 et tw=80 : */

#include "urlencode.hh"
#include <curl/curl.h>

std::string& URLEncode::encode()
{
    iterator it;
    bool first = true;
    std::string delim = "&";

    m_e.clear();

    for(it = begin(); it != end(); ++it) {
        if(!first) m_e += delim;

        m_e += (*it).first + "=" + (*it).second;

        first = false;
    }

    return m_e;
}

std::string URLEncode::escape(const std::string &s)
{
    std::string tmp;
    char *t;

    t = curl_escape(s.c_str(), s.length());

    if(!t) 
        return "";

    tmp = t;
    free(t);
    return tmp;
}

