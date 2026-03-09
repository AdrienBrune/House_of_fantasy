#ifndef MAPEVENT_H
#define MAPEVENT_H

#include <QGraphicsPixmapItem>
#include <qrandom.h>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "mapitem.h"
#include "itemgenerator.h"
#include "toolfunctions.h"

#include "constants.h"

class MapEvent : public MapItem
{
    Q_OBJECT
public:
    MapEvent();
    virtual ~MapEvent();
public:
    QList<Item*> getItems();
    QList<Item*> takeItems();
    Item * takeItem(Item*);
    void returnItems(QList<Item*>);
    bool eventIsActive();

    virtual void itemsTook()=0;

public:
    inline void toJson(QJsonObject &json) const override
    {
        MapItem::toJson(json);

        QJsonArray jsonArrayItems;
        for (Item* item : mItems)
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            jsonArrayItems.append(jsonItem);
        }
        json["items"] = jsonArrayItems;
    }
    inline virtual void fromJson(const QJsonObject &json) override
    {
        MapItem::fromJson(json);

        qDeleteAll(mItems);
        mItems.clear();

        if (json.contains("items") && json["items"].isArray())
        {
            QJsonArray jsonArrayItems = json["items"].toArray();
            for (QJsonValueRef item : jsonArrayItems)
            {
                QJsonObject jsonItem = item.toObject();
                if (!jsonItem.contains("type") || !jsonItem["type"].isDouble())
                {
                    DEBUG("item type not found, item can't be reconstructed !");
                    assert(false);
                    continue;
                }
                Item* item = Item::Factory(jsonItem["type"].toInt()); 
                if (item)
                {
                    item->fromJson(jsonItem);
                    mItems.append(item);
                }
            }
        }
    }
protected:
    QList<Item*> mItems;
};

class FishingEvent : public MapEvent
{
    Q_OBJECT
public:
    FishingEvent();
    ~FishingEvent();
public slots:
    void animate();
    void startAnimation();
public:
    bool isAnimated();
    void itemsTook();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void initGraphicStuff();
private:
    QList<Fish*> generateRandomFishes();
protected:
    int mNextFrame;
    QTimer * t_animation;
    QTimer * t_startAnimation;
};

class BushEvent : public MapEvent
{
    Q_OBJECT
public:
    BushEvent();
    virtual ~BushEvent();
public slots:
    void animate();
public:
    void startAnimation();
    bool isAnimated();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void restoreGraphicStuff();
protected:
    int mNextFrame;
    QTimer * t_animation;
};

class BushEventCoin : public BushEvent
{
    Q_OBJECT
public:
    BushEventCoin();
    ~BushEventCoin();
public:
    void itemsTook();
private:
    void initGraphicStuff();
};

class BushEventEquipment : public BushEvent
{
    Q_OBJECT
public:
    BushEventEquipment();
    ~BushEventEquipment();
public:
    void itemsTook();
private:
    void initGraphicStuff();
};

class OreSpot : public MapEvent
{
protected:
    enum Ore{
        none,
        stone,
        iron,
        saphir,
        emerald,
        rubis,
        oreNb
    };
public:
    OreSpot();
    virtual ~OreSpot();
public:
    void itemsTook();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void initGraphicStuff();
protected:
    Ore mType;
};

class StoneOreSpot : public OreSpot
{
public:
    StoneOreSpot();
    ~StoneOreSpot();
};

class IronOreSpot : public OreSpot
{
public:
    IronOreSpot();
    ~IronOreSpot();
};

class SaphirOreSpot : public OreSpot
{
public:
    SaphirOreSpot();
    ~SaphirOreSpot();
};

class EmeraldOreSpot : public OreSpot
{
public:
    EmeraldOreSpot();
    ~EmeraldOreSpot();
};

class RubisOreSpot : public OreSpot
{
public:
    RubisOreSpot();
    ~RubisOreSpot();
};

class ChestEvent : public MapEvent
{
    Q_OBJECT
public:
    ChestEvent();
    virtual ~ChestEvent();
signals:
    void sig_clicToOpenChest(ChestEvent*);
public:
    QList<Item*> getItems();
    void revealChest();
    bool isRevealed();
    void openChest(bool);
    bool isOpen();
    void addExtraItems();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void itemsTook();
public:
    inline void toJson(QJsonObject &json) const override
    {
        MapEvent::toJson(json);

        json["opened"] = mIsOpen;
        json["revealed"] = mRevealChest;
    }
    inline virtual void fromJson(const QJsonObject &json) override
    {
        MapEvent::fromJson(json);

        if (json.contains("opened"))
        {
            mIsOpen = json["opened"].toBool();
        }

        if (json.contains("revealed"))
        {
            mRevealChest = json["revealed"].toInt();
            if (mRevealChest == 1)
            {
                setAcceptHoverEvents(true);
            }
            else if (mRevealChest == 2)
            {
                setAcceptHoverEvents(false);
            }
        }

        setShape();
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private:
    void setShape();
protected:
    bool mIsOpen;
    int mRevealChest;
    bool mHover;
};

class ChestBurried : public ChestEvent
{
    Q_OBJECT
public:
    ChestBurried();
    ~ChestBurried();
private:
    void initGraphicStuff();
};

class GoblinChest : public ChestEvent
{
    Q_OBJECT
public:
    GoblinChest();
    ~GoblinChest();
private:
    void initGraphicStuff();
};

#endif // MAPEVENT_H
