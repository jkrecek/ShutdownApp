#ifndef SHA512_H
#define SHA512_H

#include "helper.h"

#define shr(x,n) ((x & 0xFFFFFFFFFFFFFFFFULL) >> n)
#define rotr(x,n) (shr(x,n) | (x << (64 - n)))

class Sha512
{
public:
    Sha512(const char* text);

    std::string to_string();
private:
    const static ullint K[80];
    const static ullint H[8];

    class Block
    {
        public:
        std::vector<ullint> msg;

        Block();
    };

    static void makeblock(std::vector<ullint>& ret, std::string p_msg);

    void split(std::vector<Block>& blks, std::string& msg);

    std::string mynum(ullint x);

    ullint ch(ullint x, ullint y, ullint z);
    ullint maj(ullint x, ullint y, ullint z);
    ullint fn0(ullint x);
    ullint fn1(ullint x);
    ullint sigma0(ullint x);
    ullint sigma1(ullint x);

    ullint hash[8];
};

#endif // SHA512_H
