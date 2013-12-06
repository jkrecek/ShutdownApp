#ifndef URLHANDLER_H
#define URLHANDLER_H

#include <iostream>
#include <list>
#include <vector>

enum Quality
{
    OTHER       = 0,
    H264        = 1,
    HD          = 2,
    FULL_HD     = 3
};

struct EpisodeTorrent
{
public:
    EpisodeTorrent(std::string name, std::string magnet);

    bool isValid() { return validity; }
    std::string getTitle() const { return title; }
    std::vector<std::string> getVersion() const { return version; }
    std::string getMagnet() const { return magnet; }
    int getSeason() const { return season; }
    int getEpisode() const { return episode; }
    Quality getQuality();
    bool isSameEpisode(const EpisodeTorrent& episodeTorrent);
    std::string to_string();

private:
    bool validity;
    std::string title;
    std::vector<std::string> version;
    const std::string magnet;
    int season;
    int episode;
};

struct SeriesTitle
{
    //SeriesTitle(const SeriesTitle& st) : title(st.title), order(st.order), version(st.version)
    //{}
};

class URLHandler
{
public:
    URLHandler();

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::string loadUrl(const char* url);

    std::list<EpisodeTorrent> getEztvMagnets(std::string response);
    std::list<EpisodeTorrent> getPirateBayMagnets(std::string response);

};

#endif // URLHANDLER_H
