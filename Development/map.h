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
public slots:
    void startBushAnimation(Bush*);
    void startBushEventAnimation(BushEvent*);
    void putItemThrownInMap(Item*);
    void unfreezeMap();
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
    void reGenerateMap();
    void generateRandomMap();
    void generateMonsters();
    void removeMapElements();
    void removeMonsters();
    void heroThrowItemInMap(Item*);
    void freezeMap();
    void putVillageInMap(Village * village = nullptr);
    void generateLaoShanLung();
private:
    void removeMapElement(MapItem*);
    bool checkCollisionBeetween(MapItem*, MapItem*);
    bool checkPositionIsEmpty(QRectF, QPointF);
    void generateBushes();
    void generateTrees();
    void generateFallenTrees();
    void generateRocks();
    void generateGround();
    void generatePlanks();
    void generateStones();
    void generateBushCoinEvent();
    void generateBushEquipmentEvent();
    void generateChestBurriedEvent();
    void generateOreSpots();
    void putItemsInMap();
    void putGoblinVillageInMap();
    void putLakesInMap();
    void initMonsterConnection(Monster*);
private:
    QGraphicsView * mView;
    QGraphicsScene * mScene;

    Hero * mHero;
    SpotEvent heroEventLocation;
    QTimer * t_collisionHandler;

    QList<Bush*> mBushes;
    Village * mVillage;
    Village_Goblin_Area * mGoblinVillage;
    QList<MapItem*> mElementsInMap_movable;
    QList<MapItem*> mElementsInMap;

    QList<Monster*> mMonsters;
    QTimer * t_monstersActions;
    int monsterActionIndex;
    QTimer * t_monsterMove;
};

#endif // MAP_H
