#include "bag.h"

Bag::Bag():
    QObject()
{
    mPayload = Payload{150,0};
}

bool Bag::addItem(Item * item)
{
    if(mPayload.current+item->getWeight() > mPayload.max){
        emit sig_bagFull();
        return false;
    }
    mItems.append(item);
    mPayload.current += item->getWeight();
    emit sig_bagEvent();
    return true;
}

bool Bag::itemIsInBag(Item * itemToCheck)
{
    for(Item * item : mItems)
    {
        if(item == itemToCheck){
            return true;
        }
    }
    return false;
}

void Bag::removeItem(Item * item)
{
    for(int i=0;i<mItems.length();i++)
    {
        if(item == mItems[i]){
            Item * toDelete = mItems.takeAt(i);
            mPayload.current -= toDelete->getWeight();
            toDelete->deleteLater();
            emit sig_bagEvent();
            return;
        }
    }
}

Item * Bag::takeItem(Item * item)
{
    for(int i=0;i<mItems.length();i++)
    {
        if(mItems[i] == item){
            mPayload.current -= item->getWeight();
            emit sig_bagEvent();
            return mItems.takeAt(i);
        }
    }
    return nullptr;
}

QList<Item *> Bag::getItems()
{
    return mItems;
}

QList<Consumable *> Bag::getConsumables()
{
    QList<Consumable*> list;
    for(Item * item : mItems)
    {
        Consumable * itemCheck = dynamic_cast<Consumable*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<Weapon *> Bag::getWeapons()
{
    QList<Weapon*> list;
    for(Item * item : mItems)
    {
        Weapon * itemCheck = dynamic_cast<Weapon*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<ArmorPiece *> Bag::getArmorPieces()
{
    QList<ArmorPiece*> list;
    for(Item * item : mItems)
    {
        ArmorPiece * itemCheck = dynamic_cast<ArmorPiece*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<MonsterMaterial *> Bag::getMonsterMaterials()
{
    QList<MonsterMaterial*> list;
    for(Item * item : mItems)
    {
        MonsterMaterial * itemCheck = dynamic_cast<MonsterMaterial*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<Material *> Bag::getMaterials()
{
    QList<Material*> list;
    for(Item * item : mItems)
    {
        Material * itemCheck = dynamic_cast<Material*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<Tool *> Bag::getTools()
{
    QList<Tool*> list;
    for(Item * item : mItems)
    {
        Tool * itemCheck = dynamic_cast<Tool*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

QList<Scroll *> Bag::getScrolls()
{
    QList<Scroll*> list;
    for(Item * item : mItems)
    {
        Scroll * itemCheck = dynamic_cast<Scroll*>(item);
        if(itemCheck){
            list.append(itemCheck);
        }
    }
    return list;
}

Bag::Payload Bag::getPayload()
{
    return mPayload;
}

Item *Bag::getShovel()
{
    for(Item * item : mItems)
    {
        Shovel * shovel = dynamic_cast<Shovel*>(item);
        if(shovel){
            return shovel;
        }
    }
    return nullptr;
}

Item *Bag::getPickaxe()
{
    for(Item * item : mItems)
    {
        Pickaxe * pickaxe = dynamic_cast<Pickaxe*>(item);
        if(pickaxe){
            return pickaxe;
        }
    }
    return nullptr;
}

Item *Bag::getFishingrod()
{
    for(Item * item : mItems)
    {
        FishingRod * fishingrode = dynamic_cast<FishingRod*>(item);
        if(fishingrode){
            return fishingrode;
        }
    }
    return nullptr;
}

#include <QtDebug>
Bag::~Bag()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();
}

void Bag::setMaximumPayload(int max)
{
    mPayload.max = max;
    emit sig_bagEvent();
}
