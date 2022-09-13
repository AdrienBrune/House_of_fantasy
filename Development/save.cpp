#include "save.h"

Save::Save():
    mHero(nullptr),
    mChest(nullptr),
    mName(QString()),
    mVillage(nullptr)
{

}

Save::~Save()
{
    if(mHero)
        delete mHero;
    if(mChest)
        delete mChest;
    if(mVillage)
        delete mVillage;
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

Village *Save::getVillage()
{
    return mVillage;
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

void Save::setVillage(Village * village)
{
    mVillage = village;
}
