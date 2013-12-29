#include "connectioncontainer.h"

ConnectionContainer::iterator::iterator(ConnectionVector **_vector)
    : vector(_vector)
{
    current = (*vector)[TYPE_ANDROID].begin();
}

void ConnectionContainer::iterator::next()
{
    if (++current == (*vector)[TYPE_ANDROID].end())
        current = (*vector)[TYPE_PC].begin();
}

bool ConnectionContainer::iterator::hasMore()
{
    return current != (*vector)[TYPE_PC].end();
}

BaseConnection* ConnectionContainer::iterator::get()
{
    return *current;
}

ConnectionContainer::iterator ConnectionContainer::begin()
{
    return iterator((ConnectionVector**)&container);
}

void ConnectionContainer::insert(BaseConnection* connection)
{
    if (connection->getType() != TYPE_COUNT)
        container[connection->getType()].push_back(connection);
}


PCConnection* ConnectionContainer::getPCConnection(AndroidConnection* con)
{
    for (ConnectionVector::iterator itr = container[TYPE_PC].begin(); itr != container[TYPE_PC].end(); ++itr)
        if (con->getUser() == (*itr)->getUser() && con->getPass() == (*itr)->getPass())
            return (PCConnection*)*itr;
}

std::vector<AndroidConnection*> ConnectionContainer::getAndroidConnections(PCConnection* con)
{
    std::vector<AndroidConnection*> vector;
    for (ConnectionVector::iterator itr = container[TYPE_ANDROID].begin(); itr != container[TYPE_ANDROID].end(); ++itr)
        if (con->getUser() == (*itr)->getUser() && con->getPass() == (*itr)->getPass())
            vector.push_back((AndroidConnection*)*itr);

    return vector;
}

void ConnectionContainer::remote(BaseConnection* connection)
{
    ConnectionVector& relatedVector = container[connection->getType()];
    for (ConnectionVector::iterator itr = relatedVector.begin(); itr != relatedVector.end(); )
    {
        if (*itr == connection)
            itr = relatedVector.erase(itr);
        else
            ++itr;
    }
}
