#ifndef ENTITIESHANDLER_H
#define ENTITIESHANDLER_H

#include "character.h"
#include "map.h"

class EntitiesHandler
{
public:
    EntitiesHandler(const EntitiesHandler&) = delete;
    EntitiesHandler& operator=(const EntitiesHandler&) = delete;

    static EntitiesHandler& getInstance() { return mInstance; }

    void registerHero(Hero * hero) { assert(hero); mHero = hero; }
    void registerMap(Map * map) { assert(map); mMap = map; }
    void registerView(QGraphicsView * view) { assert(view); mView = view; }

    Hero * getHero() { assert(mHero); return mHero; }
    Map * getMap() { assert(mMap); return mMap; }
    QGraphicsView * getView() { return mView; }

private:
    EntitiesHandler():
        mHero(nullptr),
        mMap(nullptr),
        mView(nullptr)
    {}
    ~EntitiesHandler() = default;

private:
    Hero * mHero;
    Map * mMap;
    QGraphicsView * mView;

private:
    static EntitiesHandler mInstance;
};

#endif // ENTITIESHANDLER_H
