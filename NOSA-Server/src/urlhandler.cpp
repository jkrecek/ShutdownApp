#include "urlhandler.h"
#include <curl/curl.h>
#include <iostream>
#include "helper.h"
#include <vector>
#include <algorithm>
#include <sstream>

URLHandler::URLHandler()
{

}

size_t URLHandler::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string URLHandler::loadUrl(const char* url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    long httpCode;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, URLHandler::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &httpCode);


        if (res != CURLE_OK)
            std::cout <<  "E: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        if (httpCode != 200)
        {
            std::cout << "E: Received wrong HTTP Code: " << httpCode << std::endl;
            return std::string();
        }

        curl_easy_cleanup(curl);
    } else
        std::cout <<  "E: Curl couldn't be created. " << std::endl;

    return readBuffer;
}

std::list<EpisodeTorrent> URLHandler::getEztvMagnets(std::string response)
{
    std::size_t beforeTable = response.find("<div id=\"tooltip\"");
    std::size_t tableStart = response.find("<table ", beforeTable);
    std::size_t tableEnd = response.find("</table>", tableStart);
    std::string table = response.substr(tableStart, tableEnd - tableStart);

    std::size_t tr_start, tr_end, extract_start, extract_end;
    tr_start = tr_end = 0;
    std::string line, name, magnet;
    std::string link_start = "<a href=\"";
    std::string link_start_magnet = link_start + "magnet:";
    std::list<EpisodeTorrent> magnets;
    while ((tr_start = table.find("<tr name=\"hover\"", tr_end)) != std::string::npos)
    {
        tr_end = table.find("</tr>", tr_start);
        line = table.substr(tr_start, tr_end - tr_start);

        extract_start = line.find("<td class");                 // parent
        extract_start = line.find(link_start, extract_start);   // element start
        extract_start = line.find(">", extract_start) + 1;
        extract_end = line.find("</a>", extract_start);
        name = line.substr(extract_start, extract_end - extract_start);


        extract_start = line.find(link_start_magnet) + link_start.length();
        extract_end = line.find('"', extract_start);
        magnet = line.substr(extract_start, extract_end - extract_start);

        magnets.push_back(EpisodeTorrent(name, magnet));
    }

    return magnets;
}

std::list<EpisodeTorrent> URLHandler::getPirateBayMagnets(std::string response)
{
    std::size_t tableStart = response.find("<table id=\"searchResult\">");
    std::size_t tableEnd = response.find("</table>", tableStart);
    tableEnd = response.rfind("<tr>", tableEnd);
    std::string table = response.substr(tableStart, tableEnd - tableStart);

    std::size_t tr_start, tr_end, extract_start, extract_end;
    tr_start = tr_end = 0;
    std::string line, name, magnet;
    std::string link_start = "<a href=\"";
    std::string link_start_magnet = link_start + "magnet:";
    std::list<EpisodeTorrent> magnets;
    while ((tr_start = table.find("<tr>", tr_end)) != std::string::npos)
    {
        tr_end = table.find("</tr>", tr_start);
        line = table.substr(tr_start, tr_end - tr_start);

        extract_start = line.find(link_start + "/torrent");   // element start
        extract_start = line.find(">", extract_start) + 1;
        extract_end = line.find("</a>", extract_start);
        name = line.substr(extract_start, extract_end - extract_start);

        extract_start = line.find(link_start_magnet) + link_start.length();
        extract_end = line.find('"', extract_start);
        magnet = line.substr(extract_start, extract_end - extract_start);

        EpisodeTorrent item(name, magnet);
        if (item.isValid())
            magnets.push_back(item);
    }

    return magnets;
}


EpisodeTorrent::EpisodeTorrent(std::string name, std::string _magnet)
    : season(-1), episode(-1), magnet(_magnet), validity(false)
{
    std::string _name = name;
    std::replace(_name.begin(), _name.end(), '.', ' ');
    std::replace(_name.begin(), _name.end(), '-', ' ');
    std::replace(_name.begin(), _name.end(), '_', ' ');
    std::vector<std::string> parts = Helper::split(_name, ' ');

    if (parts.size() < 2)
        return;

    std::size_t start = parts.at(0).size();
    std::vector<std::string>::iterator itr;
    for (itr = ++parts.begin(); itr != parts.end(); start += (*itr).size() + 1, ++itr)
    {
        if ((*itr).size() == 6 &&
            (*itr).at(0) == 'S' && std::isdigit((*itr).at(1)) && std::isdigit((*itr).at(2)) &&
            (*itr).at(3) == 'E' && std::isdigit((*itr).at(4)) && std::isdigit((*itr).at(5)))
        {

            season = atoi((*itr).substr(1, 2).c_str());
            episode = atoi((*itr).substr(4, 2).c_str());
            break;
        }

        if ((*itr).size() >= 3 && (*itr).size() <= 6)
        {
            std::size_t pos =(*itr).find('x');
            if (pos != 0 && pos != std::string::npos)
            {
                bool isFine = true;
                for (std::size_t i = 0; i < (*itr).size(); ++i)
                {
                    if (i != pos && !std::isdigit((*itr).at(i)))
                    {
                        isFine = false;
                        break;
                    }
                }

                if (isFine)
                {
                    season = episode = 0;
                    break;
                }
            }
        }
    }

    if (itr == parts.end())
        return;

    title = Helper::trim(_name.substr(0, start));
    if (itr != parts.end())
        version.insert(version.end(), ++itr, parts.end());

    validity = true;
}

bool EpisodeTorrent::isSameEpisode(const EpisodeTorrent &episodeTorrent)
{
    return Helper::iequals(title, episodeTorrent.title) &&
            episode == episodeTorrent.episode &&
            season == episodeTorrent.season;
}

Quality EpisodeTorrent::getQuality()
{
    std::vector<std::string>::iterator itr;
    itr = std::find(version.begin(), version.end(), "1080p");
    if (itr != version.end())
        return FULL_HD;

    itr = std::find(version.begin(), version.end(), "720p");
    if (itr != version.end())
        return HD;

    itr = std::find(version.begin(), version.end(), "x264");
    if (itr != version.end())
        return H264;

    return OTHER;
}

std::string EpisodeTorrent::to_string()
{
    std::ostringstream ss;
    ss  << "Title: '" << title << "', "
        << "Season: '" << season << "', "
        << "Episode: '" << episode << "', "
        << "Quality: '" << getQuality() << "'";

    return ss.str();

}
