#ifndef VILLAGE_H
#define VILLAGE_H

#include <QGraphicsScene>
#include <qrandom.h>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "mapitem.h"
#include "equipment.h"
#include "hero.h"
#include "constants.h"
#include "w_potioncookingslot.h"

#define NUMBER_POTION_SLOT 3
#define NUMBER_OFFERING_SLOT 3

class House : public MapItem
{
    Q_OBJECT
public:
    House();
    virtual ~House();
signals:
    void sig_villageInteraction(QGraphicsItem*);
    void sig_villageShowInfo(QGraphicsItem*);
private slots:
    void showBuildingInfo();
public:
    QString getInformation();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
protected:
    bool mHover;
    QString mInformation;
    QTimer * t_delayHover;
};




class BlacksmithHouse : public House
{
public:
    BlacksmithHouse();
    ~BlacksmithHouse();
private:
    void initGraphicStuff();
};

class Blacksmith : public QObject
{
    Q_OBJECT
public:
    Blacksmith();
    ~Blacksmith();
signals:
    void sig_replenish(QObject*);
private slots:
    void replenish();
public:
    void setPosition(QPointF);
    BlacksmithHouse * getHouse();
    EquipmentToForge * getEquipmentToCraft();
    int getTimeBeforeResplenish();
public:
    BlacksmithHouse * mHouse;
private:
    EquipmentToForge* mEquipmentsToForge;
    QTimer * t_resplenish;
};






class MerchantHouse : public House
{
public:
    MerchantHouse();
    ~MerchantHouse();
private:
    void initGraphicStuff();
};

class Merchant : public QObject
{
    Q_OBJECT
public:
    Merchant();
    ~Merchant();
signals:
    void sig_replenish(QObject*);
    void sig_itemThrown(Item*);
    void sig_adventurerMapUnlock();
private slots:
    void replenish();
public:
    void setPosition(QPointF);
    MerchantHouse * getHouse();
    QList<Item*> getItemsToSell();
public:
    bool itemIsInShop(Item*);
    void addItemInShop(Item*);
    void buyItem(Hero*, Item*);
    void sellItem(Hero*,Item*);
public:
    inline void toJson(QJsonObject &json) const
    {
        QJsonArray itemsArray;
        for (Item* item : mItemsToSell)
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            itemsArray.append(jsonItem);
        }
        json["items"] = itemsArray;
    }
    inline void fromJson(const QJsonObject &json)
    {
        qDeleteAll(mItemsToSell);
        mItemsToSell.clear();

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
                mItemsToSell.append(item);
            }
        }
    }
public:
    MerchantHouse * mHouse;
private:
    QList<Item*> mItemsToSell;
};





class AlchemistHouse : public House
{
public:
    AlchemistHouse();
    ~AlchemistHouse();
private:
    void initGraphicStuff();
};

class Alchemist : public QObject
{
    Q_OBJECT
public:
    Alchemist();
    ~Alchemist();
signals:
    void sig_replenish(QObject*);
    void sig_addPotion(Item*);
private slots:
    void replenish();
    void onCookingRequested(PotionCookingSlot * slot);
    void onCookingDone(PotionCookingSlot * slot);
public:
    void setPosition(QPointF);
    AlchemistHouse * getHouse();
    QList<Item*> getItemsToSell();
    QList<PotionCookingSlot*> getPotionSlots() { return mPotionSlots; }
public:
    bool itemIsInShop(Item*);
    void addItemInShop(Item*);
    void buyItem(Hero*, Item*);
private:
    void addPotionInQueue(PotionCookingSlot * potionSlot) { mCookingQueue.append(potionSlot); potionSlot->setInQueue(true);}
    void removePotionInQueue(PotionCookingSlot * potionSlot) { mCookingQueue.removeOne(potionSlot); potionSlot->setInQueue(false);}
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = mItemsToSell.size();

        stream << numberItems;
        for(Item * item : qAsConst(mItemsToSell))
        {
            stream << item->getIdentifier();
            item->serialize(stream);
        }

        numberItems = static_cast<quint16>(NUMBER_POTION_SLOT);
        stream << numberItems;
        for(PotionCookingSlot * slot : mPotionSlots)
            slot->serialize(stream);

        DEBUG("SERIALIZED[in]  : Alchemist");
    }
    void deserialize(QDataStream& stream)
    {
        // Remove attributes
        while(!mItemsToSell.isEmpty())
            delete mItemsToSell.takeLast();

        quint16 numberItems = 0;
        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            Item * item = nullptr;

            stream >> identifier;
            item = Item::Factory(identifier);
            item->deserialize(stream);
            mItemsToSell.append(item);
        }

        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
            mPotionSlots.at(i)->deserialize(stream);

        for(PotionCookingSlot * potionSlot : mPotionSlots)
        {
            if(potionSlot->getInQueue())
                mCookingQueue.append(potionSlot);
        }

        DEBUG("SERIALIZED[out] : Alchemist");
    }
    friend QDataStream& operator<<(QDataStream& stream, const Alchemist& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Alchemist& object)
    {
        object.deserialize(stream);
        return stream;
    }
    inline void toJson(QJsonObject &json) const
    {
        QJsonArray itemsArray;
        for (Item * item : qAsConst(mItemsToSell))
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            itemsArray.append(jsonItem);
        }
        json["items"] = itemsArray;

        QJsonArray cookingSlotsArray;
        for (PotionCookingSlot * slot : qAsConst(mPotionSlots))
        {
            QJsonObject jsonCookSlot;
            slot->toJson(jsonCookSlot);
            cookingSlotsArray.append(jsonCookSlot);
        }
        json["slots"] = cookingSlotsArray;
    }
    inline void fromJson(const QJsonObject &json)
    {
        while(!mItemsToSell.isEmpty())
            delete mItemsToSell.takeLast();

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
                Item* item = Item::Factory(jsonItem["type"].toInt());
                item->fromJson(jsonItem);
                mItemsToSell.append(item);
            }
        }
        if (json.contains("slots") && json["slots"].isArray())
        {
            QJsonArray jsonArraySlots = json["slots"].toArray();
            assert(mPotionSlots.size() == jsonArraySlots.size());
            for (int i = 0; i < jsonArraySlots.size(); ++i)
            {
                QJsonObject jsonCookSlot = jsonArraySlots[i].toObject();
                mPotionSlots[i]->fromJson(jsonCookSlot);
            }
        }

        for(PotionCookingSlot* potionSlot : mPotionSlots)
        {
            if(potionSlot->getInQueue())
            {
                mCookingQueue.append(potionSlot);
            }
        }
    }
public:
    AlchemistHouse * mHouse;
private:
    QList<Item*> mItemsToSell;
    QList<PotionCookingSlot*> mPotionSlots;
    QList<PotionCookingSlot*> mCookingQueue;
};



struct Offering
{
    Item * item;
    quint32 identifier;
};

class AltarBuilding : public House
{
    Q_OBJECT
public:
    AltarBuilding(Offering* offers);
    ~AltarBuilding();
private slots:
    void animate();
private:
    void initGraphicStuff();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    int mAnimation;
    Offering* mOffers;
};


class Altar : public QObject
{
    Q_OBJECT
public:
    Altar();
    ~Altar();
signals:
    void sig_LaoShanLungSummoned();
public:
    void setPosition(QPointF);
    AltarBuilding * getBuilding();
    Offering* getOfferingsTabPtr();
    QPixmap getOfferingPixmap();
    void setOffering(int idx, Item * item);
    bool isLaoShanLungSummoned();
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = NUMBER_OFFERING_SLOT;
        quint8 isNull;

        stream << numberItems;
        for(Offering offer : mOfferings)
        {
            (offer.item == nullptr) ? isNull = 1 : isNull = 0;
            stream << isNull;
            stream << offer.identifier;
            if(offer.item)
                offer.item->serialize(stream);
        }
        DEBUG("SERIALIZED[in]  : Altar");
    }

    void deserialize(QDataStream& stream)
    {
        quint16 numberItems = 0;

        // // Remove attributes
        // while(!mOfferings.isEmpty())
        // {
        //     Offering offering = mOfferings.takeFirst();
        //     if(offering.item)
        //         delete offering.item;
        // }

        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            quint8 isNull;
            Item * item = nullptr;

            stream >> isNull;
            stream >> identifier;
            if(isNull == 0)
            {
                item = Item::Factory(identifier);
                item->deserialize(stream);
            }
            Offering offer = {item, identifier};
            // mOfferings.append(offer);
        }
        DEBUG("SERIALIZED[out] : Altar");
    }
    friend QDataStream& operator<<(QDataStream& stream, const Altar& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Altar& object)
    {
        object.deserialize(stream);
        return stream;
    }
    inline void toJson(QJsonObject &json) const
    {
        QJsonArray itemsArray;
        for (Offering offer : mOfferings)
        {
            if(offer.item)
            {
                QJsonObject jsonItem;
                offer.item->toJson(jsonItem);
                itemsArray.append(jsonItem);
            }
        }
        json["offerings"] = itemsArray;
    }
    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("offerings") && json["offerings"].isArray())
        {
            QJsonArray jsonArrayOfferings = json["offerings"].toArray();
            for (int i = 0; i < jsonArrayOfferings.size(); ++i)
            {
                QJsonObject jsonItem = jsonArrayOfferings[i].toObject();
                if (!jsonItem.contains("type") || !jsonItem["type"].isDouble())
                {
                    DEBUG("item type not found, item can't be reconstructed !");
                    assert(false);
                    continue;
                }
                Item* item = Item::Factory(jsonItem["type"].toInt());
                item->fromJson(jsonItem);
                mOfferings[i].item = item;
                mOfferings[i].identifier = item->getIdentifier();
            }
        }
    }
public:
    AltarBuilding * mBuilding;
private:
    Offering mOfferings[NUMBER_OFFERING_SLOT] = {
        {nullptr, EARTH_CRISTAL},
        {nullptr, EARTH_CRISTAL},
        {nullptr, EARTH_CRISTAL}
    };
    QPixmap mOfferingPixmap;
};





class Taverne : public House
{
public:
    Taverne();
    ~Taverne();
private:
    void setGraphicStuff();
};

class HeroChest : public House
{
public:
    HeroChest();
    ~HeroChest();
public:
    bool itemIsInChest(Item*);
    QList<Item*> getItems();
    void addItem(Item*);
    Item * takeItem(Item*);
    void openChest(bool);
    bool isOpen(); 
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = mItems.size();

        stream << numberItems;
        for(Item * item : qAsConst(mItems))
        {
            stream << item->getIdentifier();
            item->serialize(stream);
        }
        DEBUG("SERIALIZED[in]  : HeroChest");
    }

    void deserialize(QDataStream& stream)
    {
        quint16 numberItems = 0;
        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            stream >> identifier;
            Item * item = Item::Factory(identifier);
            item->deserialize(stream);
            addItem(item);
        }
        DEBUG("SERIALIZED[out] : HeroChest");
    }
    friend QDataStream& operator<<(QDataStream& stream, const HeroChest& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, HeroChest& object)
    {
        object.deserialize(stream);
        return stream;
    }
    inline void toJson(QJsonObject &json) const
    {
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
                Item* item = Item::Factory(jsonItem["type"].toInt());
                item->fromJson(jsonItem);
                mItems.append(item);
            }
        }
    }
private:
    void setGraphicStuff();
private:
    QList<Item*> mItems;
    bool mIsOpen;
};

class HeroHouse
{
public:
    HeroHouse();
    ~HeroHouse();
public:
    Taverne * getHouse();
    HeroChest * getChest();
    void setPosition(QPointF);
public:
    Taverne * mHouse;
    HeroChest * mChest;
};




class ChevalDeFrise : public MapItem
{
public:
    ChevalDeFrise(float ratio = 1.0):
        MapItem(),
        mRatio(ratio)
    {}
    virtual ~ChevalDeFrise(){}
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setPosition(QPointF position) { setPos(position.x(),position.y()); }
protected:
    virtual void setGraphicStuff()=0;
protected:
    float mRatio;
};
class ChevalDeFriseFront : public ChevalDeFrise
{
public:
    ChevalDeFriseFront(float ratio = 1.0):
        ChevalDeFrise(ratio)
    {
        setGraphicStuff();
    }
    ~ChevalDeFriseFront(){}
private:
    void setGraphicStuff();
};
class ChevalDeFriseDiag : public ChevalDeFrise
{
public:
    ChevalDeFriseDiag(float ratio = 1.0):
        ChevalDeFrise(ratio)
    {
        setGraphicStuff();
    }
    ~ChevalDeFriseDiag(){}
private:
    void setGraphicStuff();
};





class Village : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Village();
    ~Village();
signals:
    void sig_replenish(QObject*);
    void sig_villageInteraction(QGraphicsItem*);
    void sig_villageShowInfo(QGraphicsItem*);
    void sig_LaoShanLungSummoned();
public:
    void addInScene(QGraphicsScene*);
    void removeFromScene(QGraphicsScene*);
    void setPosition(QPointF);
    QPointF getPosition();
    Blacksmith * getBlacksmith();
    Merchant * getMerchant();
    HeroHouse * getHeroHouse();
    Alchemist * getAlchemist();
    Altar * getAltar();
public:
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void serialize(QDataStream& stream)const
    {
        mAltar->serialize(stream);
        mAlchemist->serialize(stream);
        DEBUG("SERIALIZED[in]  : Village");
    }
    void deserialize(QDataStream& stream)
    {
        mAltar->deserialize(stream);
        mAlchemist->deserialize(stream);
        DEBUG("SERIALIZED[out] : Village");
    }
    friend QDataStream& operator<<(QDataStream& stream, const Village& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Village& object)
    {
        object.deserialize(stream);
        return stream;
    }
    inline void toJson(QJsonObject &json) const
    {
        QJsonObject jsonAltar;
        mAltar->toJson(jsonAltar);
        json["altar"] = jsonAltar;

        QJsonObject jsonAlchemist;
        mAlchemist->toJson(jsonAlchemist);
        json["alchemist"] = jsonAlchemist;

        QJsonObject jsonMerchant;
        mMerchant->toJson(jsonMerchant);
        json["merchant"] = jsonMerchant;

        QJsonObject jsonHeroChest;
        mHouse->mChest->toJson(jsonHeroChest);
        json["hero_chest"] = jsonHeroChest;
    }
    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("altar") && json["altar"].isObject())
        {
            QJsonObject jsonAltar = json["altar"].toObject();
            mAltar->fromJson(jsonAltar);
        }
        if (json.contains("alchemist") && json["alchemist"].isObject())
        {
            QJsonObject jsonAlchemist = json["alchemist"].toObject();
            mAlchemist->fromJson(jsonAlchemist);
        }
        if (json.contains("merchant") && json["merchant"].isObject())
        {
            QJsonObject jsonMerchant = json["merchant"].toObject();
            mMerchant->fromJson(jsonMerchant);
        }
        if (json.contains("hero_chest") && json["hero_chest"].isObject())
        {
            QJsonObject jsonChest = json["hero_chest"].toObject();
            mHouse->mChest->fromJson(jsonChest);
        }
    }
private:
    QPixmap mImage;
    QPointF mPosition;
    Blacksmith * mBlacksmith;
    Merchant * mMerchant;
    HeroHouse * mHouse;
    Altar * mAltar;
    Alchemist * mAlchemist;
    QList<ChevalDeFrise*> mChevalDeFriseList;
};



#endif // VILLAGE_H
