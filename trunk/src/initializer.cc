#include "initializer.hh"
#include "curl.hh"

Initializer::Initializer()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void Initializer::init()
{
    get_instance();
}

Initializer& Initializer::get_instance()
{
    static Initializer init;
    return init;
}

