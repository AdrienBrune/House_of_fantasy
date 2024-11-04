#ifndef BAG_H
#define BAG_H

#include <QObject>
#include "item.h"
#include "equipment.h"
#include "consumable.h"
#include "material.h"

class Bag : public QObject
{
    Q_OBJECT
public:
    struct Payload{
        int max;
        int current;
    };
public:
    Bag();
    ~Bag();
signals:
    void sig_bagFull();
    void sig_bagEvent();
public:
    void setMaximumPayload(int);
    bool addItem(Item*);
    bool itemIsInBag(Item*);
    void removeItem(Item*);
    Item * takeItem(Item*);
    Item * getItem(quint32);
    QList<Item*> getItems();
    Payload getPayload();
    Item * getShovel();
    Item * getPickaxe();
    Item * getFishingrod();
    Item * getKnife();
    int getQuantityOf(Item*);
    int getQuantityOf(quint32);
    Item * takeItemWithIdentifier(quint32);
    template<typename T>
    QList<T> getItemList() const
    {
        QList<T> filteredItems;
        for(Item* item : mItems)
        {
            T castedItem = dynamic_cast<T>(item);
            if(castedItem)
                filteredItems.append(castedItem);
        }
        return filteredItems;
    }
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = mItems.size();
        stream << mPayload.current << mPayload.max << numberItems;
        for(Item * item : qAsConst(mItems))
        {
            stream << item->getIdentifier();
            item->serialize(stream);
        }
        DEBUG("SERIALIZED[in]  : Bag");
    }
    void deserialize(QDataStream& stream)
    {
        quint16 numberItems = 0;
        stream >> mPayload.current >> mPayload.max >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            stream >> identifier;
            Item * item = Item::getInstance(identifier);
            item->deserialize(stream);
            mItems.append(item);
        }
        DEBUG("SERIALIZED[out] : Bag");
    }
private:
    Payload mPayload;
    QList<Item*> mItems;
};

#endif // BAG_H
