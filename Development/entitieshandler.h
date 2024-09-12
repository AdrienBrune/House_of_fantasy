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

    void registerHero(Hero * hero) { mHero = hero; }
    void registerMap(Map * map) { mMap = map; }

    Hero * getHero() { return mHero; }
    Map * getMap() { return mMap; }

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
