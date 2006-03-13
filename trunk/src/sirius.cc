#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <openssl/md5.h>

#include <boost/bind.hpp>
#include <boost/regex.hpp>

#include <string>
#include <iostream>
#include <map>

#include <vlc/vlc.h>

#include "urlencode.hh"
#include "curl.hh"
#include "initializer.hh"
#include "sirius.hh"

void Channel::print()
{
    std::cout << num << ":" << token << ":" << text << ":" << desc << std::endl;
}

Sirius::Sirius()
{

}

Sirius::~Sirius()
{

}

bool Sirius::get_token()
{
    boost::regex 
        e("<input type=\\\"hidden\\\" name=\\\"token\\\" value=\\\"(.*?)\\\">",
           boost::match_default|boost::match_partial);
    Curl fetch;
    CURLcode ret;
    bool rv = false;

    ret = fetch.set_url("http://www.sirius.com/servlet/MediaPlayer?activity=selectLoginType");
    if(ret) return false;

    ret = fetch.perform();
    if(ret) return false;

    boost::cmatch what;
    if(!boost::regex_search(fetch.get_chunk(), what, e))
    {
        throw std::runtime_error("Invalid data entered");
    }

    if(what[0].matched)
    {
        m_token = what[1];
        rv = true;
    }

    return rv;
}

bool Sirius::do_login(const std::string &username,
                      const std::string &password)
{
    std::string url = "http://www.sirius.com/servlet/MediaPlayerLogin/subscriber?";
    URLEncode   encoder;
    Curl        fetch;
    unsigned char digest[MD5_DIGEST_LENGTH];
    unsigned char cdigest[MD5_DIGEST_LENGTH*2 + 1];
    static char hexa[] = "0123456789abcdef";

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, password.c_str(), password.length());
    MD5_Final(digest, &ctx);

    for(int i=0, j=0; i<MD5_DIGEST_LENGTH; i++, j+=2) {
        cdigest[j] = hexa[(digest[i]>>4) & 0xf];
        cdigest[j+1] = hexa[digest[i] & 0xf];
    }
    cdigest[MD5_DIGEST_LENGTH*2] = 0;

    encoder["username"] = URLEncode::escape(username);
    encoder["activity"] = URLEncode::escape("login");
    encoder["type"]     = URLEncode::escape("subscriber");
    encoder["password"] = std::string((char*)cdigest);
    encoder["loginForm"]= URLEncode::escape("subscriber");
    encoder["stream"]   = URLEncode::escape("");
    encoder["token"]    = URLEncode::escape(m_token);

    std::string final_url = url + encoder.encode();

    fetch.set_url(final_url.c_str());
    fetch.perform();

    return true;
}

bool Sirius::get_stream_url(const std::string& channel_name, 
                    const std::string& genre_name,
                    const std::string& category_name,
                    std::string &stream_url)
{
    bool ret = false;
    std::string url = "http://www.sirius.com/servlet/MediaPlayer?activity=selectStream&stream=";
    Curl fetch;

    url += channel_name;
    url += "&genre=" + genre_name;
    url += "&category=" + category_name;
    url += "&token=" + m_token;

    fetch.set_url(url);
    fetch.perform();

    boost::regex e("SRC=\\\"(.*?)\\\"",
           boost::match_default|boost::match_partial);

    boost::cmatch what;
    if(!boost::regex_search(fetch.get_chunk(), what, e)) {
        throw std::runtime_error("Invalid data entered2");
    }

    if(what[0].matched) {
        stream_url = what[1];

        fetch.reset();
        fetch.set_url(stream_url);
        fetch.perform();

        boost::regex e2("(mms://.*?)\\\"",
                boost::match_default|boost::match_partial);
        boost::cmatch what2;

        if(!boost::regex_search(fetch.get_chunk(), what2, e2)) {
            throw std::runtime_error("Invalid data entered3");
        }

        if(what2[0].matched) {
            stream_url = what2[1];
        }

        ret = true;
    }

    return ret;
}

bool Sirius::populate_categories()
{
    boost::smatch what;
    boost::regex e("myPlayer.Category\\('(.*?)',",
                boost::match_default|boost::match_partial);
    std::string url = "http://www.sirius.com/mediaplayer/player/common/lineup/category.jsp?category=&genre=&channel=";
    std::string str;
    std::string::const_iterator begin, end;
    Curl fetch;

    fetch.set_url(url);
    fetch.perform();

    str   = fetch.get_chunk();
    begin = str.begin();
    end   = str.end();

    boost::match_flag_type flags = boost::match_default;
    while(boost::regex_search(begin, end, what, e, flags)) {
        std::string result = what[1];

        if(result.size() == 0) {
            begin = what[0].second; 

            // update flags: 
            flags |= boost::match_prev_avail; 
            flags |= boost::match_not_bob; 
            continue;
        }

        m_categories.push_back(result);
        begin = what[0].second; 

        // update flags: 
        flags |= boost::match_prev_avail; 
        flags |= boost::match_not_bob; 
    }

    return true;
}

bool Sirius::populate_genre()
{
    Curl fetch;
    std::string url = "http://www.sirius.com/mediaplayer/player/common/lineup/genre.jsp?category=";
    boost::smatch what;
    std::string str;
    std::string::const_iterator begin, end;
    boost::regex e("myPlayer.Genre\\('.*?', '(.*?)'",
                   boost::match_default|boost::match_partial);

    std::vector<std::string>::iterator it;
    for(it = m_categories.begin(); it != m_categories.end(); ++it) {
        std::string final_url = url + *it;

        fetch.set_url(final_url);
        fetch.perform();

        str   = fetch.get_chunk();
        begin = str.begin();
        end   = str.end();

        boost::match_flag_type flags = boost::match_default;
        while(boost::regex_search(begin, end, what, e, flags)) {
            std::string result = what[1];

            if(result.size() == 0) {
                begin = what[0].second; 

                // update flags: 
                flags |= boost::match_prev_avail; 
                flags |= boost::match_not_bob; 
                continue;
            }

            m_categories_genres.insert(std::pair<std::string, std::string>(*it, result));
            std::cout << *it << ":" << result << std::endl;

            begin = what[0].second;

            // update flags: 
            flags |= boost::match_prev_avail; 
            flags |= boost::match_not_bob; 
        }
        fetch.reset();
    }

    return true;
}

bool Sirius::populate_channels()
{
    Channel *channel = NULL;
    std::multimap<std::string, std::string>::iterator it;
    std::string num, text, desc;

    for(it = m_categories_genres.begin();
            it != m_categories_genres.end();
            ++it) {
        Curl fetch;

        std::string url = "http://www.sirius.com/mediaplayer/player/common/lineup/channel.jsp?category=" 
            + (*it).first + "&genre=" + (*it).second;

        fetch.set_url(url);
        fetch.perform();

        boost::smatch what;
        std::string str;
        std::string::const_iterator begin, end;
        boost::regex e("myPlayer.Channel\\('.*?', '.*?', '(.*?)',.*? class=\"(.*?)\">(.*?)</a>",
                boost::match_default|boost::match_partial);

        str   = fetch.get_chunk();
        begin = str.begin();
        end   = str.end();

        boost::match_flag_type flags = boost::match_default;
        while(boost::regex_search(begin, end, what, e, flags)) {
            std::string token  = what[1];
            std::string result = what[2];
            std::string data   = what[3];

            if(token.empty() || result.empty() || data.empty()) {
                continue;
            }

            if(result == "channel") {
                std::string::size_type loc;

                if((loc=data.find(" ")) != std::string::npos) {
                    num = data.erase(loc, data.size());
                }
                else {
                    num = "";
                }
            }
            else if(result == "text") {
                text = data;
            }
            else if(result == "desc") {
                desc = data;

                if(num.empty())
                    break;

                channel = new Channel;
                channel->num = atoi(num.c_str());
                channel->token = token;
                channel->text = text;
                channel->desc = desc;
                channel->genre = (*it).second;
                channel->cat = (*it).first;

                channel->print();

                m_channels[channel->num] = channel;
            }
            else {
                // unhandled type
            }

            begin = what[0].second;

            // update flags: 
            flags |= boost::match_prev_avail; 
            flags |= boost::match_not_bob; 
        }
        fetch.reset();
    }

    return true;
}

int main(int argc, char **argv)
{
    Sirius sirius;
    std::string username, password, url;
    int id, ret;
    struct stat s;
    unsigned int channel_num;
    Channel *channel;

    sirius.populate_categories();
    sirius.populate_genre();
    sirius.populate_channels();

    std::cout << "Enter username:" << std::endl;
    std::cin >> username;

    std::cout << "Enter password:" << std::endl;
    std::cin >> password;

    std::cout << "Enter channel:" << std::endl;
    std::cin >> channel_num;

    if((channel=sirius.get_channel(channel_num)) == NULL) {
        printf("Channel doesn't exist\n");
        return 1;
    }

    Initializer::init();

    id = VLC_Create();

    if(stat("cookie", &s) == 0)
        remove("cookie");

    if(sirius.get_token()) {
        sirius.do_login(username, password);
        if(sirius.get_stream_url(channel->token, channel->genre, channel->cat, url)) {
            std::cout << "URL: " << url << std::endl;
        }
    }

    VLC_Init(id, argc, argv);

    VLC_AddTarget(id, url.c_str(), 0, 0, PLAYLIST_APPEND | PLAYLIST_GO, PLAYLIST_END);

    VLC_Play(id);

    while(1);

    VLC_CleanUp( id );
    VLC_Destroy( id );

    return 0;
}

