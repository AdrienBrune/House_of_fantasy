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
    for(Item * item : qAsConst(mItems))
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

Item *Bag::getItem(quint32 identifier)
{
    for(int i=0;i<mItems.length();i++)
    {
        if(mItems[i]->getIdentifier() == identifier){
            return mItems[i];
        }
    }
    return nullptr;
}

QList<Item *> Bag::getItems()
{
    return mItems;
}

Bag::Payload Bag::getPayload()
{
    return mPayload;
}

Item *Bag::getShovel()
{
    for(Item * item : qAsConst(mItems))
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
    for(Item * item : qAsConst(mItems))
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
    for(Item * item : qAsConst(mItems))
    {
        FishingRod * fishingrode = dynamic_cast<FishingRod*>(item);
        if(fishingrode){
            return fishingrode;
        }
    }
    return nullptr;
}

Item *Bag::getKnife()
{
    for(Item * item : qAsConst(mItems))
    {
        Knife * knife = dynamic_cast<Knife*>(item);
        if(knife){
            return knife;
        }
    }
    return nullptr;
}

int Bag::getQuantityOf(Item * itemToFind)
{
    int quantity = 0;
    for(Item * item : qAsConst(mItems))
    {
        if(item->getIdentifier() == itemToFind->getIdentifier())
            quantity++;
    }
    return quantity;
}

int Bag::getQuantityOf(quint32 identifier)
{
    int quantity = 0;
    for(Item * item : qAsConst(mItems))
    {
        if(item->getIdentifier() == identifier)
            quantity++;
    }
    return quantity;
}

Item * Bag::takeItemWithIdentifier(quint32 identifier)
{
    for(int i=0;i<mItems.length();i++)
    {
        if(mItems[i]->getIdentifier() == identifier){
            mPayload.current -= mItems[i]->getWeight();
            emit sig_bagEvent();
            return mItems.takeAt(i);
        }
    }
    return nullptr;
}

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
