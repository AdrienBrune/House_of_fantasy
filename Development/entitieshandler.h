#ifndef ENTITIESHANDLER_H
#define ENTITIESHANDLER_H

#include "character.h"
#include "map.h"

class EntitesHandler
{
public:
    EntitesHandler(const EntitesHandler&) = delete;
    EntitesHandler& operator=(const EntitesHandler&) = delete;

    static EntitesHandler& getInstance() { return mInstance; }

    void registerHero(Hero * hero) { assert(hero); mHero = hero; }
    void registerMap(Map * map) { assert(map); mMap = map; }

    Hero * getHero() { assert(mHero); return mHero; }
    Map * getMap() { assert(mMap); return mMap; }

private:
    EntitesHandler():
        mHero(nullptr),
        mMap(nullptr)
    {}
    ~EntitesHandler() = default;

private:
    Hero * mHero;
    Map * mMap;

private:
    static EntitesHandler mInstance;
};

#endif // ENTITIESHANDLER_H
