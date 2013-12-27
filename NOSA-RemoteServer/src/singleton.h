#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton
{
    public:
        static T& getInstance()
        {
            static T    instance;
            return instance;
        }
    private:
        Singleton() {}
        Singleton(Singleton const&);
        void operator=(Singleton const&);
};

#endif // SINGLETON_H
