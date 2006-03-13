#ifndef SIRIUS_HH
#define SIRIUS_HH

#include <string>
#include <map>
#include <vector>

class SatRadio
{
    public:
        SatRadio() {}
        virtual ~SatRadio() {}
    private:

};

typedef struct Channel
{
    unsigned int num;
    std::string  token;
    std::string  text;
    std::string  desc;
    std::string  genre;
    std::string  cat;

    void print();
};

class Sirius : public SatRadio
{
    public:
        Sirius();
        virtual ~Sirius();

        bool get_token();
        bool do_login(const std::string &username, 
                      const std::string &password);
        bool get_stream_url(const std::string& channel_name,
                            const std::string& genre_name,
                            const std::string& category_name,
                            std::string &stream_url);

        bool populate_categories();
        bool populate_genre();
        bool populate_channels();

        Channel* get_channel(unsigned int num) { return m_channels[num]; }

    private:
        std::string m_token;
        std::vector<std::string> m_categories;
        std::multimap<std::string, std::string> m_categories_genres;

        std::map<int, Channel*> m_channels;
};

#endif

