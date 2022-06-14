#include "save.h"

Save::Save():
    mHero(nullptr),
    mChest(nullptr),
    mName(QString())
{

}

Save::~Save()
{
    if(mHero)
        delete mHero;
    if(mChest)
        delete mChest;
}

Hero * Save::getHero()
{
    return mHero;
}

HeroChest * Save::getHeroChest()
{
    return mChest;
}

const QString &Save::getName() const
{
    return mName;
}

void Save::setHero(Hero * hero)
{
    mHero = hero;
}

void Save::setChest(HeroChest * chest)
{
    mChest = chest;
}

void Save::setName(QString name)
{
    mName = name;
}
