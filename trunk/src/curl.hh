#ifndef CURL_HH
#define CURL_HH

#include <string>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

class Curl
{
    public:
        Curl();
        virtual ~Curl();

        void set_verbose(bool verbose);
        CURLcode set_url(const std::string& _url);
        CURLcode perform();

        char*  get_chunk()      { return chunk.memory; }
        size_t get_chunk_size() { return chunk.size; }

        void reset();

    private:
        CURL *m_curl;
        struct MemoryStruct chunk;

        std::string m_url;
};

#endif

