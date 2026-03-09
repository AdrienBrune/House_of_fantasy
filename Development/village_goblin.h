#ifndef VILLAGE_GOBLIN_H
#define VILLAGE_GOBLIN_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QVector>
#include <qrandom.h>
#include <QPainter>
#include "mapitem.h"
#include "equipment.h"
#include "monster.h"
#include "toolfunctions.h"

class Hut : public MapItem
{
public:
    Hut();
    ~Hut();
};

class TownHall : public MapItem
{
public:
    TownHall();
    ~TownHall();
};

class WatchTower : public MapItem
{
public:
    WatchTower();
    ~WatchTower();
};

class Village_Goblin : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Village_Goblin();
    ~Village_Goblin();
signals:
    void sig_clicToOpenChest(ChestEvent*);
public:
    void addInScene(QGraphicsScene*);
    void setPosition(QPointF);
    QPointF getPosition();
public:
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    inline void toJson(QJsonObject &json) const
    {
        QJsonObject jsonPosition;
        jsonPosition["x"] = pos().x();
        jsonPosition["y"] = pos().y();
        json["position"] = jsonPosition;

        QJsonArray jsonHuts;
        for (const Hut& hut : mHuts)
        {
            QJsonObject jsonHut;
            jsonHut["x"] = hut.x();
            jsonHut["y"] = hut.y();
            jsonHut["rotation"] = hut.rotation();
            jsonHuts.append(jsonHut);
        }
        json["huts"] = jsonHuts;

        QJsonArray jsonTowers;
        for (const WatchTower& tower : mWatchTowers)
        {
            QJsonObject jsonTower;
            jsonTower["x"] = tower.x();
            jsonTower["y"] = tower.y();
            jsonTowers.append(jsonTower);
        }
        json["towers"] = jsonTowers;
    }
    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("position") && json["position"].isObject())
        {
            QJsonObject jsonPosition = json["position"].toObject();
            QPointF position(jsonPosition["x"].toDouble(), jsonPosition["y"].toDouble());
            setPosition(position);
        }

        if (json.contains("huts") && json["huts"].isArray())
        {
            QJsonArray jsonHuts = json["huts"].toArray();
            for (int i = 0; i < 9 && i < jsonHuts.size(); i++)
            {
                QJsonObject jsonHut = jsonHuts[i].toObject();
                mHuts[i].setPos(jsonHut["x"].toDouble(), jsonHut["y"].toDouble());
                mHuts[i].setRotation(jsonHut["rotation"].toDouble());
            }
        }

        if (json.contains("towers") && json["towers"].isArray())
        {
            QJsonArray jsonTowers = json["towers"].toArray();
            for (int i = 0; i < 2 && i < jsonTowers.size(); i++)
            {
                QJsonObject jsonTower = jsonTowers[i].toObject();
                mWatchTowers[i].setPos(jsonTower["x"].toDouble(), jsonTower["y"].toDouble());
            }
        }
    }
private:
    QPixmap mImage;
    QPointF mPosition;

    int mRoadsWeight;
    Hut mHuts[9];
    WatchTower mWatchTowers[2];
    TownHall mTownHall;
    GoblinChest mChest;

friend class Village_Goblin_Area;
};

class Village_Goblin_Area : public QGraphicsItem
{
public:
    Village_Goblin_Area();
    ~Village_Goblin_Area();
public:
    Village_Goblin& getVillage();
    void addInScene(QGraphicsScene*);
    void setPosition(QPointF);
public:
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    inline void toJson(QJsonObject &json) const
    {
        mVillage.toJson(json);
    }
    inline void fromJson(const QJsonObject &json)
    {
        mVillage.fromJson(json);
        setPos(mVillage.mPosition.x() - boundingRect().width() / 2.0,
               mVillage.mPosition.y() - boundingRect().height() / 2.0);
    }
private:
    Village_Goblin mVillage;
};

#endif // VILLAGE_GOBLIN_H
