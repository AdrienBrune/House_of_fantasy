#ifndef VILLAGE_H
#define VILLAGE_H

#include <QGraphicsScene>
#include <qrandom.h>
#include <QPainter>
#include <QDebug>
#include "mapitem.h"
#include "equipment.h"
#include "hero.h"
#include "constants.h"

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
    bool isObstacle();
    QPainterPath shape() const;
    QRectF boundingRect()const;
protected:
    void setShape();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
protected:
    QRect mBounding;
    QPainterPath mCollisionShape;
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
private slots:
    void replenish();
public:
    void setPosition(QPointF);
    void setPotionPreferencies(QList<int>);
    AlchemistHouse * getHouse();
    QList<Consumable*> getPotionPreferencies();
    QList<Item*> getItemsToSell();
public:
    bool itemIsInShop(Item*);
    void addItemInShop(Item*);
    void buyItem(Hero*, Item*);
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
        numberItems = mPotionPreferencies.size();
        stream << numberItems;
        for(Item * potion : mPotionPreferencies)
        {
            stream << potion->getIdentifier();
            potion->serialize(stream);
        }
        DEBUG("SERIALIZED[in]  : Alchemist");
    }

    void deserialize(QDataStream& stream)
    {
        // Remove attributes
        while(!mItemsToSell.isEmpty())
            delete mItemsToSell.takeLast();
        while(!mPotionPreferencies.isEmpty())
            delete mPotionPreferencies.takeLast();

        quint16 numberItems = 0;
        quint32 identifier = 0;
        Consumable * potion = nullptr;
        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            Item * item = nullptr;

            stream >> identifier;
            item = Item::getInstance(identifier);
            item->deserialize(stream);
            mItemsToSell.append(item);
        }
        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            stream >> identifier;
            potion = dynamic_cast<Consumable*>(Item::getInstance(identifier));
            potion->deserialize(stream);
            mPotionPreferencies.append(potion);
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
public:
    AlchemistHouse * mHouse;
private:
    QList<Item*> mItemsToSell;
    QList<Consumable*> mPotionPreferencies;
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
    AltarBuilding(QList<Offering>* offers = nullptr);
    ~AltarBuilding();
private slots:
    void animate();
private:
    void initGraphicStuff();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    int mAnimation;
    QList<Offering>* mOffers;
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
    QList<Offering> getOfferings();
    QPixmap getOfferingPixmap();
    void setOffering(int idx, Item * item);
    bool isLaoShanLungSummoned();
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = mOfferings.size();
        quint8 isNull;

        stream << numberItems;
        for(Offering offer : mOfferings)
        {
            offer.item ? isNull = 1 : isNull = 0;
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

        // Remove attributes
        while(!mOfferings.isEmpty())
        {
            Offering offering = mOfferings.takeFirst();
            if(offering.item)
                delete offering.item;
        }

        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            quint8 isNull;
            Item * item = nullptr;

            stream >> isNull;
            stream >> identifier;
            if(isNull)
            {
                item = Item::getInstance(identifier);
                item->deserialize(stream);
            }
            Offering offer = {item, identifier};
            mOfferings.append(offer);
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
public:
    AltarBuilding * mBuilding;
private:
    QList<Offering> mOfferings = {
        {nullptr, EARTH_CRISTAL},
        {nullptr, EARTH_CRISTAL},
        {nullptr, EARTH_CRISTAL}
    };
    QPixmap mOfferingPixmap;
};





class MainHouse : public House
{
public:
    MainHouse();
    ~MainHouse();
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
            Item * item = Item::getInstance(identifier);
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
public:
    bool isObstacle();
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
    MainHouse * getHouse();
    HeroChest * getChest();
    void setPosition(QPointF);
public:
    MainHouse * mHouse;
    HeroChest * mChest;
};

class RampartBot :  public House
{
public:
    RampartBot();
    ~RampartBot();
public:
    void setPosition(QPointF);
private:
    void setGraphicStuff();
};

class RampartTop :  public House
{
public:
    RampartTop();
    ~RampartTop();
public:
    void setPosition(QPointF);
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
private:
    QPixmap mImage;
    QPointF mPosition;
    Blacksmith * mBlacksmith;
    Merchant * mMerchant;
    HeroHouse * mHouse;
    Altar * mAltar;
    Alchemist * mAlchemist;
    RampartTop * mRampartTop;
    RampartBot * mRampartBot;
};



#endif // VILLAGE_H
