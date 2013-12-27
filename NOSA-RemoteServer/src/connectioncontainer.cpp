#include "connectioncontainer.h"

ConnectionContainer::ConnectionContainer()
{
    for(ConnectionContainer::iterator itr = this->begin(); itr.hasMore(); itr.next())
    {

    }
}

void ConnectionContainer::iterator::next()
{
    if (++current == a2)
        current = b1;
}

bool ConnectionContainer::iterator::hasMore()
{
    return current != endItr;
}

BaseConnection* ConnectionContainer::iterator::get()
{
    return *current;
}

ConnectionContainer::iterator ConnectionContainer::begin()
{
    iterator itr;
    itr.startItr = ((std::vector<BaseConnection*>&)containerAndroid).begin();
    itr.a2 = ((std::vector<BaseConnection*>&)containerAndroid).end();
    itr.b1 = ((std::vector<BaseConnection*>&)containerPC).begin();
    itr.endItr = ((std::vector<BaseConnection*>&)containerPC).end();

    itr.current = itr.startItr;
    return itr;
}

void ConnectionContainer::insert(BaseConnection* connection)
{
    if (connection->getType() == TYPE_ANDROID)
        containerAndroid.push_back((AndroidConnection*)connection);
    else if (connection->getType() == TYPE_PC)
        containerPC.push_back((PCConnection*)connection);
}


PCConnection* ConnectionContainer::getPCConnection(AndroidConnection* con)
{
    for (std::vector<PCConnection*>::iterator itr = containerPC.begin(); itr != containerPC.end(); ++itr)
        if (con->getUser() == (*itr)->getUser() && con->getPass() == (*itr)->getPass())
            return *itr;
}

std::vector<AndroidConnection*> ConnectionContainer::getAndroidConnections(PCConnection* con)
{
    std::vector<AndroidConnection*> vector;
    for (std::vector<AndroidConnection*>::iterator itr = containerAndroid.begin(); itr != containerAndroid.end(); ++itr)
    {
        if (con->getUser() == (*itr)->getUser() && con->getPass() == (*itr)->getPass())
            vector.push_back(*itr);
    }

    return vector;

}
