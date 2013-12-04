#ifndef URLHANDLER_H
#define URLHANDLER_H

#include <iostream>

class URLHandler
{
public:
    URLHandler();

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::string loadUrl(const char* url);
};

#endif // URLHANDLER_H
