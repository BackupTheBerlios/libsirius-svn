#include "curl.hh"

void *myrealloc(void *ptr, size_t size)
{
    /* There might be a realloc() out there that doesn't like reallocing
       NULL pointers, so we take care of it here */
    if(ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

Curl::Curl()
{
    m_curl = curl_easy_init();

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; U; PPC Mac OS X Mach-O; en-US; rv:1.7.8) Gecko/20050511 Firefox/1.0.4");
    curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, "cookie");
    curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, "cookie");

    chunk.memory = NULL;
    chunk.size = 0;
}

Curl::~Curl()
{
    reset();
    curl_easy_cleanup(m_curl);
}

void Curl::set_verbose(bool verbose)
{
    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, (verbose)?1:0);
}

CURLcode Curl::set_url(const std::string& _url)
{
    CURLcode rv = curl_easy_setopt(m_curl, CURLOPT_URL, _url.c_str());
    if(rv)
        fprintf(stderr, "%s\n", curl_easy_strerror(rv));

    m_url = _url;

    return rv;
}

CURLcode Curl::perform()
{
    CURLcode rv = curl_easy_perform(m_curl);
    if(rv)
        fprintf(stderr, "%s\n", curl_easy_strerror(rv));
    return rv;
}

void Curl::reset()
{
    if(chunk.memory) {
        free(chunk.memory);
        chunk.memory = NULL;
    }
    chunk.size = 0;
}

