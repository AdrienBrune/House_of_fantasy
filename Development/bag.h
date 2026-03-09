#ifndef BAG_H
#define BAG_H

#include <QObject>
#include "item.h"
#include "equipment.h"
#include "consumable.h"
#include "material.h"
#include "common.h"

class Bag : public QObject
{
    Q_OBJECT
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
    Gauge getPayload();
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
        stream << mPayload.current << mPayload.maximum << numberItems;
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
        stream >> mPayload.current >> mPayload.maximum >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            stream >> identifier;
            Item * item = Item::Factory(identifier);
            item->deserialize(stream);
            mItems.append(item);
        }
        DEBUG("SERIALIZED[out] : Bag");
    }
    inline void toJson(QJsonObject &json) const
    {
        QJsonObject jsonPayload;
        jsonPayload["current"] = mPayload.current;
        jsonPayload["maximum"] = mPayload.maximum;
        json["payload"] = jsonPayload;

        QJsonArray itemsArray;
        for (Item* item : mItems)
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            itemsArray.append(jsonItem);
        }
        json["items"] = itemsArray;
    }
    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("payload") && json["payload"].isObject())
        {
            QJsonObject jsonPayload = json["payload"].toObject();
            if (jsonPayload.contains("current") && jsonPayload["current"].isDouble())
            {
                mPayload.current = jsonPayload["current"].toInt();
            }
            if (jsonPayload.contains("maximum") && jsonPayload["maximum"].isDouble())
            {
                mPayload.maximum = jsonPayload["maximum"].toInt();
            }
        }

        if (json.contains("items") && json["items"].isArray())
        {
            QJsonArray jsonArrayItems = json["items"].toArray();
            for (int i = 0; i < jsonArrayItems.size(); ++i)
            {
                QJsonObject jsonItem = jsonArrayItems[i].toObject();
                if (!jsonItem.contains("type") || !jsonItem["type"].isDouble())
                {
                    DEBUG("item type not found, item can't be reconstructed !");
                    assert(false);
                    continue;
                }
                Item * item = Item::Factory(jsonItem["type"].toInt());
                item->fromJson(jsonItem);
                mItems.append(item);
            }
        }
    }
private:
    Gauge mPayload;
    QList<Item*> mItems;
};

#endif // BAG_H
