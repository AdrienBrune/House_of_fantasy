#ifndef VILLAGE_H
#define VILLAGE_H

#include <QGraphicsScene>
#include <qrandom.h>
#include <QPainter>
#include <QDebug>
#include "mapitem.h"
#include "equipment.h"
#include "hero.h"


class House : public MapItem
{
    Q_OBJECT
public:
    House();
    ~House();
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
    QList<EquipmentToForge*> getEquipmentsToForge();
public:
    void buyEquipment(Hero*, EquipmentToForge*);
public:
    BlacksmithHouse * mHouse;
private:
    QList<EquipmentToForge*> mEquipmentsToForge;
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
    Merchant(ItemGenerator*);
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
    ItemGenerator * mGameItems;
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
    Alchemist(ItemGenerator*);
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
    AlchemistHouse * mHouse;
private:
    QList<Item*> mItemsToSell;
    ItemGenerator * mGameItems;
    QList<Consumable*> mPotionPreferencies;
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
        for(Item * item : mItems)
        {
            stream << item->getIdentifier();
            item->serialize(stream);
        }
        qDebug() << "SERIALIZED[in]  : HeroChest";
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
        qDebug() << "SERIALIZED[out] : HeroChest";
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
    Village(ItemGenerator*);
    ~Village();
signals:
    void sig_replenish(QObject*);
    void sig_villageInteraction(QGraphicsItem*);
    void sig_villageShowInfo(QGraphicsItem*);
public:
    void addInScene(QGraphicsScene*);
    void setPosition(QPointF);
    QPointF getPosition();
    Blacksmith * getBlacksmith();
    Merchant * getMerchant();
    HeroHouse * getHeroHouse();
    Alchemist * getAlchemist();
public:
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QPixmap mImage;
    QPointF mPosition;
    Blacksmith * mBlacksmith;
    Merchant * mMerchant;
    HeroHouse * mHouse;
    Alchemist * mAlchemist;
    RampartTop * mRampartTop;
    RampartBot * mRampartBot;
};



#endif // VILLAGE_H
