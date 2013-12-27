#ifndef CONNECTIONCONTAINER_H
#define CONNECTIONCONTAINER_H

#include "helper.h"
#include "androidconnection.h"
#include "pcconnection.h"
#include "singleton.h"

//typedef  ConnectionIterator;

class ConnectionContainer
{
public:
    class iterator
    {
    public:
        void next();
        bool hasMore();
        BaseConnection* get();

    /*private:*/
        std::vector<BaseConnection*>::iterator startItr;
        std::vector<BaseConnection*>::iterator a2;
        std::vector<BaseConnection*>::iterator b1;
        std::vector<BaseConnection*>::iterator endItr;
        std::vector<BaseConnection*>::iterator current;
    };

    friend class iterator;

    ConnectionContainer();

    iterator begin();

    void insert(BaseConnection* connection);
    bool get(std::vector<BaseConnection*>& connections);
    PCConnection* getPCConnection(AndroidConnection* con);
    std::vector<AndroidConnection*> getAndroidConnections(PCConnection* con);

private:
    std::vector<AndroidConnection*> containerAndroid;
    std::vector<PCConnection*> containerPC;
};

#define sConnections Singleton<ConnectionContainer>::getInstance()

#endif // CONNECTIONCONTAINER_H
