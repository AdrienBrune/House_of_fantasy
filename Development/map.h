#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QElapsedTimer>
#include "hero.h"
#include "monster.h"
#include "mapitem.h"
#include "mapevent.h"
#include "w_itemdisplayer.h"
#include "village.h"
#include "village_goblin.h"
#include "win_interface_trading.h"
#include "daynightcycle.h"

class Map : public QObject
{
    Q_OBJECT
public:
    struct SpotEvent{
        bool event_goblinVillage;
    };
public:
    Map(QWidget * parent, QGraphicsView*);
    ~Map();
signals:
    void sig_loadingGameUpdate(quint8);
    void sig_monsterEncountered(Monster*);
    void sig_heroMoved();
    void sig_playSound(int);
    void sig_heroLeftVillage();
    void sig_heroEnterVillage();
    void sig_heroEnterEvent(int);
    void sig_heroLeaveEvent(int);
    void sig_generationMapComplete();
    void sig_showItemInfo(Item*);
    void sig_itemPickedInMap(Item*);
    void sig_openInterface(QGraphicsItem*);
    void sig_showPNJinfo(QGraphicsItem*);
    void sig_replenish(QObject*);
    void sig_displayMonsterData(Monster*);
    void sig_calamitySpawned();
    void sig_mapItemDestoyed(MapItem*);
    void sig_adventurerMapUnlock();
    void sig_timeChanged(qreal);
public slots:
    void startBushAnimation(Bush*);
    void startBushEventAnimation(BushEvent*);
    void putItemThrownInMap(Item*);
    void unfreezeMap() { freeze(false); }
private slots:
    void monstersActionHandler();
    void heroCollisionEventHandler();
    void itemMoved(MapItem*);
    void checkItemMovedPosition(MapItem*);
    void itemInMapClicked(Item*);
    void tryToOpenChest(ChestEvent*);
    void tryToStartPNGInteraction(QGraphicsItem*);
    void deleteMovableMapItem(MapItemMovable*);
    void onLaoShanLungSummoned();
public:
    QList<Monster*> getMonsters();
    QGraphicsScene * getScene();
    Village * getVillage();
    Village_Goblin_Area * getGoblinVillage();
    void setHero(Hero*);
public:
    void freeze(bool enable);
    void reGenerateMap();
    void generateRandomMap();
    void generateMonsters();
    void removeMapElements();
    void removeMonsters();
    void heroThrowItemInMap(Item*);
    void putVillageInMap();
    void generateLaoShanLung();
    bool isNight();
private:
    void removeMapElement(MapItem*);
    bool checkCollisionBeetween(MapItem*, MapItem*);
    bool checkPositionIsEmpty(QRectF, QPointF);
    void generateBushes(QList<MapItem*> bushes);
    void generateBushCoinEvent(QList<MapItem*> bushes);
    void generateBushEquipmentEvent(QList<MapItem*> bushes);
    void generateTrees();
    void generateFallenTrees();
    void generateRocks();
    void generateGround();
    void generatePlanks();
    void generateStones();
    void generateChestBurriedEvent();
    void generateOreSpots();
    void generateItems();
    void putItemsInMap();
    void putGoblinVillageInMap();
    void putLakesInMap();
    void initMonsterConnection(Monster*);
public:
    inline void toJson(QJsonObject &json) const
    {
        QJsonObject jsonDayCycle;
        mDayNightCycle->toJson(jsonDayCycle);
        json["day_cycle"] = jsonDayCycle;

        QJsonObject jsonVillages;

        QJsonObject jsonHeroVillage;
        mVillage->toJson(jsonHeroVillage);
        jsonVillages["hero"] = jsonHeroVillage;
        QJsonObject jsonGoblinVillage;
        mGoblinVillage->toJson(jsonGoblinVillage);
        jsonVillages["goblin"] = jsonGoblinVillage;

        json["villages"] = jsonVillages;

        QJsonObject jsonElements;

        QJsonArray jsonMapItems;
        for (MapItem* item : mElementsInMap)
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            jsonMapItems.append(jsonItem);
        }
        jsonElements["mapitems"] = jsonMapItems;

        QJsonArray jsonItems;
        for (Item* item : mItemsInMap)
        {
            QJsonObject jsonItem;
            item->toJson(jsonItem);
            jsonItems.append(jsonItem);
        }
        jsonElements["items"] = jsonItems;

        json["elements"] = jsonElements;

        QJsonArray jsonArrayMonsters;
        for (Monster* monster : mMonsters)
        {
            QJsonObject jsonMonster;
            monster->toJson(jsonMonster);
            jsonArrayMonsters.append(jsonMonster);
        }
        json["monsters"] = jsonArrayMonsters;
    }

    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("day_cycle") && json["day_cycle"].isObject())
        {
            mDayNightCycle->fromJson(json["day_cycle"].toObject());
        }

        if (json.contains("villages") && json["villages"].isObject())
        {
            QJsonObject jsonVillages = json["villages"].toObject();

            if (jsonVillages.contains("hero") && mVillage) {
                mVillage->fromJson(jsonVillages["hero"].toObject());
            }
            
            if (jsonVillages.contains("goblin") && mGoblinVillage) {
                mGoblinVillage->fromJson(jsonVillages["goblin"].toObject());
            }
        }

        if (json.contains("elements") && json["elements"].isObject())
        {
            QJsonObject jsonElements = json["elements"].toObject();
            if (jsonElements.contains("mapitems") && jsonElements["mapitems"].isArray())
            {
                qDeleteAll(mElementsInMap);
                mElementsInMap.clear();

                QJsonArray jsonArrayElements = jsonElements["mapitems"].toArray();
                for (QJsonValueRef item : jsonArrayElements)
                {
                    QJsonObject jsonItem = item.toObject();
                    if (!jsonItem.contains("type") || !jsonItem["type"].isDouble())
                    {
                        DEBUG("item type not found, item can't be reconstructed !");
                        assert(false);
                        continue;
                    }
                    MapItem* item = MapItem::Factory(jsonItem["type"].toInt());
                    mScene->addItem(item);
                    item->fromJson(jsonItem);
                    mElementsInMap.append(item);                
                }
            }
            if (jsonElements.contains("items") && jsonElements["items"].isArray())
            {
                qDeleteAll(mItemsInMap);
                mItemsInMap.clear();

                QJsonArray jsonArrayElements = jsonElements["items"].toArray();
                for (QJsonValueRef item : jsonArrayElements)
                {
                    QJsonObject jsonItem = item.toObject();
                    if (!jsonItem.contains("type") || !jsonItem["type"].isDouble())
                    {
                        DEBUG("item type not found, item can't be reconstructed !");
                        assert(false);
                        continue;
                    }
                    Item* item = Item::Factory(jsonItem["type"].toInt());
                    item->fromJson(jsonItem);
                    mItemsInMap.append(item);
                }
                putItemsInMap();
            }
        }

        if (json.contains("monsters") && json["monsters"].isArray())
        {
            qDeleteAll(mMonsters);
            mMonsters.clear();

            QJsonArray jsonArrayMonsters = json["monsters"].toArray();
            for (QJsonValueRef item : jsonArrayMonsters)
            {
                QJsonObject jsonMonster = item.toObject();
                if (!jsonMonster.contains("name") || !jsonMonster["name"].isString())
                {
                    DEBUG_ERR(QString("monster name not found in json file!"));
                    assert(false);
                    continue;
                }
                Monster* monster = Monster::Factory(jsonMonster["name"].toString(), mView);
                mScene->addItem(monster);
                initMonsterConnection(monster);
                monster->fromJson(jsonMonster);
                mMonsters.append(monster);
            }
        }
    }
private:
    QGraphicsView * mView;
    QGraphicsScene * mScene;

    DayNightCycle * mDayNightCycle;

    Hero * mHero;
    SpotEvent heroEventLocation;
    QTimer * t_collisionHandler;

    Village * mVillage;
    Village_Goblin_Area * mGoblinVillage;
    QList<MapItem*> mElementsInMap;
    QList<Item*> mItemsInMap;

    QList<Monster*> mMonsters;
    QTimer * t_monstersActions;
    int monsterActionIndex;
    QTimer * t_monsterMove;
};

#endif // MAP_H
