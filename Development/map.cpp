#include "map.h"
#include "toolfunctions.h"

#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QApplication>

extern quint8 loadingStep;

Map::Map(QWidget * parent, QGraphicsView * view):
    QObject(),
    mView(view),
    mDayNightCycle(nullptr),
    mHero(nullptr),
    heroEventLocation(SpotEvent{false}),
    t_collisionHandler(nullptr),
    mVillage(nullptr),
    mGoblinVillage(nullptr),
    mElementsInMap(QList<MapItem*>()),
    mItemsInMap(QList<Item*>()),
    mMonsters(QList<Monster*>()),
    t_monstersActions(nullptr),
    monsterActionIndex(0),
    t_monsterMove(nullptr)
{
    connect(this, SIGNAL(sig_loadingGameUpdate(quint8)), parent, SIGNAL(sig_loadingGameUpdate(quint8)));
    connect(this, SIGNAL(sig_loadingGameStep(const QString&)), parent, SIGNAL(sig_loadingGameStep(const QString&)));

    MapConfig::setRatio(1.5f);

    mScene = new GrassScene(this);
    mScene->setSceneRect(QRect(0, 0, MAP_WIDTH, MAP_HEIGHT));

    mDayNightCycle = new DayNightCycle(mScene, this);
    connect(mDayNightCycle, &DayNightCycle::sig_timeChanged, this, &Map::sig_timeChanged);

    t_monstersActions = new QTimer(this);
    connect(t_monstersActions, SIGNAL(timeout()), this, SLOT(monstersActionHandler()));

    t_monsterMove = new QTimer(this);
    connect(t_monsterMove, SIGNAL(timeout()), mScene, SLOT(advance()));

    t_collisionHandler = new QTimer(this);
    connect(t_collisionHandler, SIGNAL(timeout()), this, SLOT(heroCollisionEventHandler()));

    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    generateRandomMap();

    generateMonsters();

    t_monsterMove->start(TIMER_MONSTERS_MOVE);
    t_collisionHandler->start(TIMER_COLLISION);
}

void Map::monstersActionHandler()
{
    if(monsterActionIndex >= mMonsters.length()){
        monsterActionIndex = 0;
    }
    mMonsters[monsterActionIndex]->nextAction(mHero, mDayNightCycle);
    monsterActionIndex++;
}

void Map::heroCollisionEventHandler()
{
    QList<QGraphicsItem *> list = mScene->collidingItems(mHero) ;

    // Check village interaction
    bool found = false;
    for(QGraphicsItem * item : qAsConst(list))
    {
        Village * village = dynamic_cast<Village*>(item);
        if(village){
            found = true;
        }
    }
    // Hero just left the village
    if(mHero->isInVillage() && !found){
        mHero->setIsInVillage(false);
        emit sig_heroLeftVillage();
    // Hero just enter the village
    }else if(found && !mHero->isInVillage()){
        mHero->setIsInVillage(true);
        emit sig_heroEnterVillage();
    }

    // Check event interaction
    found = false;
    for(QGraphicsItem * item : qAsConst(list))
    {
        Village_Goblin_Area * goblinVillage = dynamic_cast<Village_Goblin_Area*>(item);
        if(goblinVillage){
            found = true;
        }
    }
    // Hero just left the event location
    if(heroEventLocation.event_goblinVillage && !found){
        heroEventLocation.event_goblinVillage = false;
        emit sig_heroLeaveEvent(MUSICEVENT_ENTER_GOBLIN_VILLAGE);
    // Hero just enter the event location
    }else if(found && !heroEventLocation.event_goblinVillage){
        heroEventLocation.event_goblinVillage = true;
        emit sig_heroEnterEvent(MUSICEVENT_ENTER_GOBLIN_VILLAGE);
    }

    // Check for Monster fight event
    for(QGraphicsItem * item : qAsConst(list))
    {
        Monster * monster = dynamic_cast<Monster *>(item);
        if(monster && !monster->isDead()){
            emit sig_monsterEncountered(monster);
            continue;
        }
    }
}

void Map::itemMoved(MapItem * item)
{
    Plank * plank = dynamic_cast<Plank*>(item);
    if(plank){
        emit sig_playSound(SOUND_MOVE_PLANK);
    }
    Stone * stone = dynamic_cast<Stone*>(item);
    if(stone){
        emit sig_playSound(SOUND_MOVE_ROCK);
    }
}

void Map::checkItemMovedPosition(MapItem * item)
{
    MapItemMovable * itemMovable = dynamic_cast<MapItemMovable*>(item);
    if(item->collidesWithItem(mHero)){
        connect(item, SIGNAL(sig_deleteItem(MapItemMovable*)), this, SLOT(deleteMovableMapItem(MapItemMovable*)));
        itemMovable->setReadyToDelete();
    }else{
        QList<QGraphicsItem*> list = mScene->collidingItems(item);
        for(QGraphicsItem * collisionItem : list){
            Monster * monster = dynamic_cast<Monster*>(collisionItem);
            if(monster){
                connect(item, SIGNAL(sig_deleteItem(MapItemMovable*)), this, SLOT(deleteMovableMapItem(MapItemMovable*)));
                itemMovable->setReadyToDelete();
                return;
            }
        }
    }
}

void Map::itemInMapClicked(Item * item)
{
    if(ToolFunctions::getDistanceBeetween(mHero, item) < 150){
        disconnect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
        disconnect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
        mScene->removeItem(item);
        mItemsInMap.removeOne(item);
        item->setHover(false);
        if(mHero->takeItem(item)){
            emit sig_itemPickedInMap(item);
        }
    }
}

void Map::tryToOpenChest(ChestEvent * chest)
{
    if(ToolFunctions::getDistanceBeetween(mHero, chest) < 100 && chest->isRevealed())
    {
        emit sig_openInterface(chest);
    }
}

void Map::tryToStartPNGInteraction(QGraphicsItem * png)
{
    if(ToolFunctions::getDistanceBeetween(mHero, png) < 200)
    {
        emit sig_openInterface(png);
    }
}

void Map::deleteMovableMapItem(MapItemMovable * item)
{
    removeMapElement(item);
}

void Map::onLaoShanLungSummoned()
{
    generateLaoShanLung();

    emit sig_calamitySpawned();
    mView->centerOn(mMonsters.last());
}

QList<Monster*> Map::getMonsters()
{
    return mMonsters;
}

QGraphicsScene * Map::getScene()
{
    return mScene;
}

Village * Map::getVillage()
{
    return mVillage;
}

Village_Goblin_Area *Map::getGoblinVillage()
{
    return mGoblinVillage;
}

void Map::generateRandomMap()
{
    QElapsedTimer timer;

    emit sig_loadingGameStep("Génération du village...");
    QApplication::processEvents();
    timer.start();
    putVillageInMap();
    DEBUG("GENERATED : HeroVillage        [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération du village gobelin...");
    QApplication::processEvents();
    timer.start();
    putGoblinVillageInMap();
    DEBUG("GENERATED : GoblinVillage      [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    DEBUG("GENERATING : Map elements");

    emit sig_loadingGameStep("Génération du terrain...");
    QApplication::processEvents();
    timer.start();
    generateGroundVariations();
    DEBUG("GENERATED : Ground             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des lacs...");
    QApplication::processEvents();
    timer.start();
    putLakesInMap();
    DEBUG("GENERATED : Lakes              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des arbres...");
    QApplication::processEvents();
    timer.start();
    generateTrees();
    DEBUG("GENERATED : Trees              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    QList<MapItem*> bushes;
    emit sig_loadingGameStep("Génération des buissons...");
    QApplication::processEvents();
    timer.start();
    generateBushes(bushes);
    DEBUG("GENERATED : Bushes             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();
    timer.start();
    generateBushCoinEvent(bushes);
    DEBUG("GENERATED : BushCoin           [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();
    timer.start();
    generateBushEquipmentEvent(bushes);
    DEBUG("GENERATED : BushEquipment      [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des rochers...");
    QApplication::processEvents();
    timer.start();
    generateRocks();
    DEBUG("GENERATED : Rocks              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des planches et pierres...");
    QApplication::processEvents();
    timer.start();
    generatePlanks();
    DEBUG("GENERATED : Planks             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateStones();
    DEBUG("GENERATED : Stones             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des coffres enterrés...");
    QApplication::processEvents();
    timer.start();
    generateChestBurriedEvent();
    DEBUG("GENERATED : ChestBurried       [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des arbres tombés...");
    QApplication::processEvents();
    timer.start();
    generateFallenTrees();
    DEBUG("GENERATED : FallenTrees        [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Génération des gisements de minerais...");
    QApplication::processEvents();
    timer.start();
    generateOreSpots();
    DEBUG("GENERATED : OreSpots           [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    emit sig_loadingGameStep("Nettoyage de la carte...");
    QApplication::processEvents();
    timer.start();
    // Remove mapItems colliding with village
    for(MapItem * item : qAsConst(mElementsInMap))
    {
        if(item->pos().x()+item->boundingRect().width()>mVillage->getPosition().x() && item->pos().x()<mVillage->getPosition().x()+mVillage->boundingRect().width()
           && item->pos().y()+item->boundingRect().height()>mVillage->getPosition().y() && item->pos().y()<mVillage->getPosition().y()+mVillage->boundingRect().height())
        {
            mElementsInMap.removeOne(item);
            item->deleteLater();
        }
    }
    // Remove mapItems colliding with Goblin village
    for(MapItem * item : qAsConst(mElementsInMap))
    {
        if(item->pos().x()+item->boundingRect().width()>mGoblinVillage->pos().x() && item->pos().x()<mGoblinVillage->pos().x()+mGoblinVillage->boundingRect().width()
           && item->pos().y()+item->boundingRect().height()>mGoblinVillage->pos().y() && item->pos().y()<mGoblinVillage->pos().y()+mGoblinVillage->boundingRect().height())
        {
            mElementsInMap.removeOne(item);
            item->deleteLater();
        }
    }
    DEBUG("GENERATED : Removing items     [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // Hide items in the map
    emit sig_loadingGameStep("Génération des objets cachés...");
    QApplication::processEvents();
    timer.start();
    generateItems();
    emit sig_loadingGameStep("Placement des objets cachés...");
    QApplication::processEvents();
    putItemsInMap();
    DEBUG("GENERATED : Items              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    DEBUG("GENERATED : Elements(" << mElementsInMap.size() << ")");

    initMapElementsConnections();
    DEBUG("Connect all elements");

    mDayNightCycle->resetTime();
}

void Map::removeMapElements()
{
    if(mGoblinVillage)
    {
        delete mGoblinVillage;
    }

    while(!mElementsInMap.isEmpty())
    {
        delete mElementsInMap.takeLast();
    }
    while(!mItemsInMap.isEmpty())
    {
        delete mItemsInMap.takeLast();
    }
    while(!mGroundPatches.isEmpty())
    {
        delete mGroundPatches.takeLast();
    }

    t_monstersActions->stop();
    while(!mMonsters.isEmpty())
    {
        delete mMonsters.takeLast();
    }
}

void Map::freeze(bool enable)
{
    mDayNightCycle->freeze(enable);

    if(enable)
    {
        t_monstersActions->stop();
        t_monsterMove->stop();
        t_collisionHandler->stop();
        for(Monster * monster : qAsConst(mMonsters))
            monster->enableMonsterAnimation(false);
    }
    else
    {
        t_collisionHandler->start(TIMER_COLLISION);
        t_monstersActions->start(TIMER_MONSTERS_ACTION/mMonsters.size());
        t_monsterMove->start(TIMER_MONSTERS_MOVE);
        for(Monster * monster : qAsConst(mMonsters))
            monster->enableMonsterAnimation(true);
    }
}

void Map::removeMapElement(MapItem * item)
{
    mScene->removeItem(item);
    if(mElementsInMap.contains(item)){
        mElementsInMap.removeOne(item);
    }else{
        DEBUG_ERR("ERR : element delete not contain in map");
    }
    item->deleteLater();
}

void Map::generateGroundVariations()
{
    QRandomGenerator rng(QRandomGenerator::global()->generate());

    // dirt patches
    for (int i = 0; i < NUM_GROUND_DIRT; i++) {
        int w    = 160 + static_cast<int>(rng.bounded(180u));
        int h    = w * (55 + static_cast<int>(rng.bounded(35u))) / 100;
        quint32 seed = rng.generate();
        QGraphicsPixmapItem* item = mScene->addPixmap(ToolFunctions::generateDirtPatch(w, h, seed));
        item->setPos(rng.bounded(300u, static_cast<quint32>(MAP_WIDTH  - 300)),
                     rng.bounded(300u, static_cast<quint32>(MAP_HEIGHT - 300)));
        item->setZValue(Z_GROUND);
        mGroundPatches.append(item);
    }

    // gravel zones
    for (int i = 0; i < NUM_GROUND_GRAVEL; i++) {
        int w    = 260 + static_cast<int>(rng.bounded(220u));
        int h    = w * (60 + static_cast<int>(rng.bounded(30u))) / 100;
        quint32 seed = rng.generate();
        QGraphicsPixmapItem* item = mScene->addPixmap(ToolFunctions::generateGravelPatch(w, h, seed));
        item->setPos(rng.bounded(300u, static_cast<quint32>(MAP_WIDTH  - 300)),
                     rng.bounded(300u, static_cast<quint32>(MAP_HEIGHT - 300)));
        item->setZValue(Z_GROUND);
        mGroundPatches.append(item);
    }

    // sand patches
    for (int i = 0; i < NUM_GROUND_SAND; i++) {
        int w    = 520 + static_cast<int>(rng.bounded(440u));
        int h    = w * (60 + static_cast<int>(rng.bounded(30u))) / 100;
        quint32 seed = rng.generate();
        QGraphicsPixmapItem* item = mScene->addPixmap(ToolFunctions::generateSandPatch(w, h, seed));
        item->setPos(rng.bounded(400u, static_cast<quint32>(MAP_WIDTH  - 400)),
                     rng.bounded(400u, static_cast<quint32>(MAP_HEIGHT - 400)));
        item->setZValue(Z_GROUND);
        mGroundPatches.append(item);
    }

    // vegetation chunks (forest floor, mossy rocks, undergrowth)
    auto addVegChunk = [&](auto genFn, int count, int minW, int maxW) {
        for (int i = 0; i < count; i++) {
            int w    = minW + static_cast<int>(rng.bounded(static_cast<quint32>(maxW - minW)));
            int h    = w * (60 + static_cast<int>(rng.bounded(35u))) / 100;
            quint32 seed = rng.generate();
            QGraphicsPixmapItem* item = mScene->addPixmap(genFn(w, h, seed));
            item->setPos(rng.bounded(300u, static_cast<quint32>(MAP_WIDTH  - 300)),
                         rng.bounded(300u, static_cast<quint32>(MAP_HEIGHT - 300)));
            item->setZValue(Z_GROUND);
            mGroundPatches.append(item);
        }
    };
    addVegChunk([](int w, int h, quint32 s){ return ToolFunctions::generateForestFloor(w, h, s); }, NUM_GROUND_FOREST,      200, 420);
    addVegChunk([](int w, int h, quint32 s){ return ToolFunctions::generateMossyRocks(w, h, s);  }, NUM_GROUND_MOSSY,       180, 380);
    addVegChunk([](int w, int h, quint32 s){ return ToolFunctions::generateUndergrowth(w, h, s); }, NUM_GROUND_UNDERGROWTH, 220, 450);

    // flower fields
    for (int i = 0; i < NUM_GROUND_FLOWERS; i++) {
        int w    = 1200 + static_cast<int>(rng.bounded(600u));
        int h    = w * (75 + static_cast<int>(rng.bounded(25u))) / 100;
        quint32 seed = rng.generate();
        QGraphicsPixmapItem* item = mScene->addPixmap(ToolFunctions::generateFlowerField(w, h, seed));
        item->setPos(rng.bounded(500u, static_cast<quint32>(MAP_WIDTH  - 500)),
                     rng.bounded(500u, static_cast<quint32>(MAP_HEIGHT - 500)));
        item->setZValue(Z_GROUND);
        mGroundPatches.append(item);
    }
}

bool Map::tryPlaceMonsterRandomly(Monster* item, int margin, int maxTries)
{
    auto hasBlockingCollision = [](QGraphicsItem* item)
    {
        for (QGraphicsItem* other : item->scene()->collidingItems(item))
        {
            if (other->type() == eQGraphicItemType::village)
            {
                return true;
            }
            if (IsMapitemTypeOrDerived(other))
            {
                MapItem* mapItem = dynamic_cast<MapItem*>(other);
                if (mapItem && mapItem->isObstacle())
                {
                    return true;
                }
            }
        }
        return false;
    };

    for (int i = 0; i < maxTries; i++)
    {
        item->setPosition(QRandomGenerator::global()->bounded(margin, MAP_WIDTH - margin),
                          QRandomGenerator::global()->bounded(margin, MAP_HEIGHT - margin));
        if (!hasBlockingCollision(item))
            return true;
    }

    DEBUG_ERR(QString("tryPlaceMonsterRandomly: impossible to place monster"));
    return false;
}

bool Map::tryPlaceMapItemRandomly(MapItem* item, int margin, int maxTries)
{
    auto hasRelevantCollision = [](QGraphicsItem* item) {
        for (QGraphicsItem* other : item->scene()->collidingItems(item))
        {
            if (other->type() >= QGraphicsItem::UserType)
                return true;
        }
        return false;
    };

    for (int i = 0; i < maxTries; i++)
    {
        item->setPosition(QRandomGenerator::global()->bounded(margin, MAP_WIDTH - margin),
                     QRandomGenerator::global()->bounded(margin, MAP_HEIGHT - margin));
        if (!hasRelevantCollision(item))
            return true;
    }

    MapItem* mapItem = dynamic_cast<MapItem*>(item);
    if(mapItem)
    {
        DEBUG_ERR(QString("impossible to place %1").arg(mapItem->getName()));
    }
    return false;
}

void Map::generateBushes(QList<MapItem*> bushes)
{
    for (int i = 0; i < NUM_BUSHES; i++)
    {
        Bush* bush = new Bush();
        mScene->addItem(bush);
        tryPlaceMapItemRandomly(bush, 100);
        bushes.append(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateBushCoinEvent(QList<MapItem*> bushes)
{
    for (int i = 0; i < NUM_BUSHES_COIN_EVENT; i++)
    {
        BushEventCoin* bush = new BushEventCoin();
        mScene->addItem(bush);
        tryPlaceMapItemRandomly(bush, 100);
        bushes.append(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateBushEquipmentEvent(QList<MapItem*> bushes)
{
    for (int i = 0; i < NUM_BUSHES_EQUIPMENT_EVENT; i++)
    {
        BushEventEquipment* bush = new BushEventEquipment();
        mScene->addItem(bush);
        tryPlaceMapItemRandomly(bush, 100);
        bushes.append(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateTrees()
{
    for (int i = 0; i < NUM_TREES; i++)
    {
        Tree* tree = new Tree();
        mScene->addItem(tree);
        tryPlaceMapItemRandomly(tree, 100);
        mElementsInMap.append(tree);
    }
}

void Map::generateFallenTrees()
{
    for (int i = 0; i < NUM_TREES_FALLEN; i++)
    {
        TreeFallen* treeFallen = new TreeFallen();
        mScene->addItem(treeFallen);
        tryPlaceMapItemRandomly(treeFallen, 100);
        mElementsInMap.append(treeFallen);
    }
}

void Map::generateRocks()
{
    for (int i = 0; i < NUM_ROCKS; i++)
    {
        Rock* rock = new Rock();
        mScene->addItem(rock);
        tryPlaceMapItemRandomly(rock, 100);
        mElementsInMap.append(rock);
    }
}

void Map::generateChestBurriedEvent()
{
    for (int i = 0; i < NUM_CHEST_BURRIED_EVENT; i++)
    {
        ChestBurried* chest = new ChestBurried();
        mScene->addItem(chest);
        tryPlaceMapItemRandomly(chest, 100);
        connect(chest, SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
        mElementsInMap.append(chest);
    }
}

void Map::generateOreSpots()
{
    int rubisSpots  = QRandomGenerator::global()->bounded(NUM_RUBIS_ORE)   + 1;
    int saphirSpots = QRandomGenerator::global()->bounded(NUM_SAPHIR_ORE)  + 1;
    int emeraldSpots= QRandomGenerator::global()->bounded(NUM_EMERALD_ORE) + 1;

    auto placeOre = [&](MapItem* ore) {
        mScene->addItem(ore);
        tryPlaceMapItemRandomly(ore, 100);
        mElementsInMap.append(ore);
    };

    for (int i = 0; i < NUM_IRON_ORE;   i++) placeOre(new IronOreSpot());
    for (int i = 0; i < NUM_STONE_ORE;  i++) placeOre(new StoneOreSpot());
    for (int i = 0; i < saphirSpots;    i++) placeOre(new SaphirOreSpot());
    for (int i = 0; i < emeraldSpots;   i++) placeOre(new EmeraldOreSpot());
    for (int i = 0; i < rubisSpots;     i++) placeOre(new RubisOreSpot());
}

void Map::generateLaoShanLung()
{
    LaoShanLung * monster = new LaoShanLung(mView);
    initMonsterConnection(monster);
    mMonsters.append(monster);
    mScene->addItem(monster);
    tryPlaceMonsterRandomly(monster, 1000);
}

bool Map::isNight()
{
    return mDayNightCycle->isNight();
}

void Map::generatePlanks()
{
    for (int i = 0; i < NUM_PLANK; i++)
    {
        Plank* plank = new Plank();
        mScene->addItem(plank);
        tryPlaceMapItemRandomly(plank, 100);
        plank->setInitialPosition(plank->pos());
        mElementsInMap.append(plank);
    }
}

void Map::generateStones()
{
    for (int i = 0; i < NUM_STONE; i++)
    {
        Stone* stone = new Stone();
        mScene->addItem(stone);
        tryPlaceMapItemRandomly(stone, 100);
        stone->setInitialPosition(stone->pos());
        mElementsInMap.append(stone);
    }
}

void Map::generateItems()
{
    for (MapItem* mapItem : mElementsInMap)
    {
        if (!mapItem->isMovable())
            continue;

        Item* item = nullptr;

        int randomNumber = QRandomGenerator::global()->bounded(100);
        if(randomNumber < 6)
        {
            item = gItemGenerator->generateEquipment();

        }else if(randomNumber < 15)
        {
            item = gItemGenerator->generateRandomConsumable();

        }else if(randomNumber <= 25)
        {
            item = new BagCoin(QRandomGenerator::global()->bounded(1, 5));
        }else if(randomNumber <= 27)
        {
            item = new EarthCristal();
        }

        if(item)
        {
            mItemsInMap.append(item);
        }
    }
}

void Map::putItemsInMap()
{
    int itemIndex = 0;
    for (MapItem* mapItem : mElementsInMap)
    {
        if (!mapItem->isMovable())
            continue;

        if (itemIndex < mItemsInMap.size())
        {
            Item* item = mItemsInMap.at(itemIndex);
            mScene->addItem(item);
            item->setPos(mapItem->x()+mapItem->boundingRect().width()/2-item->boundingRect().width()/2,
                        mapItem->y()+mapItem->boundingRect().height()/2-item->boundingRect().height()/2);

            itemIndex++;
        }
    }
    initItemsInMapConnections();
}

void Map::initItemsInMapConnections()
{
    for (Item* item : mItemsInMap)
    {
        connect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
        connect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
    }
}

void Map::putVillageInMap()
{
    if(!mVillage)
    {
        mVillage = new Village();
        mVillage->addInScene(mScene);
        connect(mVillage, SIGNAL(sig_replenish(QObject*)), this, SIGNAL(sig_replenish(QObject*)));
        connect(mVillage, SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_showPNJinfo(QGraphicsItem*)));
        connect(mVillage, SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SLOT(tryToStartPNGInteraction(QGraphicsItem*)));
        connect(mVillage, SIGNAL(sig_LaoShanLungSummoned()), this, SLOT(onLaoShanLungSummoned()));
        connect(mVillage->getMerchant(), SIGNAL(sig_adventurerMapUnlock()), this, SIGNAL(sig_adventurerMapUnlock()));
    }

    mVillage->setPosition(QPointF(MAP_WIDTH/2 - mVillage->boundingRect().width()/2, MAP_HEIGHT/2 - mVillage->boundingRect().height()/2));
}

void Map::putGoblinVillageInMap()
{
//#define PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
        mGoblinVillage = new Village_Goblin_Area();
        mGoblinVillage->setPosition(QPoint(QRandomGenerator::global()->bounded(MAP_WIDTH-static_cast<int>(2*mGoblinVillage->boundingRect().width())) + static_cast<int>(mGoblinVillage->boundingRect().width()),
                                           QRandomGenerator::global()->bounded(MAP_HEIGHT-static_cast<int>(2*mGoblinVillage->boundingRect().height())) + static_cast<int>(mGoblinVillage->boundingRect().height())));
     if(mVillage != nullptr){
        while(ToolFunctions::getDistanceBeetween(mVillage, mGoblinVillage) < 4000){
            mGoblinVillage->setPosition(QPoint(QRandomGenerator::global()->bounded(MAP_WIDTH-static_cast<int>(2*mGoblinVillage->boundingRect().width())) + static_cast<int>(mGoblinVillage->boundingRect().width()),
                                               QRandomGenerator::global()->bounded(MAP_HEIGHT-static_cast<int>(2*mGoblinVillage->boundingRect().height())) + static_cast<int>(mGoblinVillage->boundingRect().height())));
        }
        mGoblinVillage->addInScene(mScene);
        connect(&mGoblinVillage->getVillage(), SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
    }
#else
    mGoblinVillage = new Village_Goblin_Area();
    connect(mGoblinVillage->getVillage(), SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
    mGoblinVillage->setPosition(QPointF(mVillage->getPosition().x() - 3000, mVillage->getPosition().y()));
    mGoblinVillage->addInScene(mScene);
#endif
}

void Map::putLakesInMap()
{
    for(int i=0;i<NUM_LAKES;i++)
    {
        Lake * lake = new Lake();
        mScene->addItem(lake);
        mScene->addItem(lake->getEvent());
        tryPlaceMapItemRandomly(lake, 500);
        lake->getEvent()->setPosition(lake->pos().x()-60, lake->pos().y()+lake->boundingRect().height()/2);
        mElementsInMap.append(lake);
    }
}

void Map::initMonsterConnection(Monster * monster)
{
    connect(monster, SIGNAL(sig_monsterSound(int)), this, SIGNAL(sig_playSound(int)));
    connect(monster, SIGNAL(sig_movedInBush(Bush*)), this, SLOT(startBushAnimation(Bush*)));
    connect(monster, SIGNAL(sig_movedInBushEvent(BushEvent*)), this, SLOT(startBushEventAnimation(BushEvent*)));
    connect(monster, SIGNAL(sig_showMonsterData(Monster*)), this, SIGNAL(sig_displayMonsterData(Monster*)));
}

void Map::initMapElementsConnections()
{
    for (MapItem* mapItem : mElementsInMap)
    {
        connect(mapItem, SIGNAL(sig_playSound(int)), this, SIGNAL(sig_playSound(int)));

        if (mapItem->type() == eQGraphicItemType::mapitem_movable)
        {
            MapItemMovable* mapItemMovable = dynamic_cast<MapItemMovable*>(mapItem);
            if (mapItemMovable)
            {
                connect(mapItemMovable, SIGNAL(sig_itemMoved(MapItem*)), this, SLOT(itemMoved(MapItem*)));
                connect(mapItemMovable, SIGNAL(sig_itemPositionFixed(MapItem*)), this, SLOT(checkItemMovedPosition(MapItem*)));
            }
        }

        if (mapItem->type() == eQGraphicItemType::mapevent)
        {
            ChestEvent* chest = dynamic_cast<ChestEvent*>(mapItem);
            if (chest)
            {
                connect(chest, SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
            }
        }
    }
}

void Map::generateMonsters()
{
    DEBUG("GENERATING : Monsters");
    int WolfPackNumber = NUM_WOLFPACK, BearNumber = NUM_BEAR, SpiderNumber = NUM_SPIDER, GoblinNumber = NUM_GOBLIN, TrollNumber = NUM_TROLL, OggreNumber = NUM_OGGRE;

    emit sig_loadingGameStep("Génération des meutes de loups...");
    QApplication::processEvents();
    int chunkSize = 500, wolfCount = 0;
    QList<QGraphicsItem*> toAvoid {mVillage, mGoblinVillage};
    for(int n=0;n<WolfPackNumber;n++)
    {
        int nTry = 0;
        QRect wolfPackArea = ToolFunctions::getSpawnChunk(QSize(chunkSize,chunkSize),toAvoid);

        Monster * monster = new WolfAlpha(mView);
        wolfCount++;
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        // Alpha Wolf placement
        monster->setPosition(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
        QList<QGraphicsItem*> list = mScene->collidingItems(monster);
        while(!list.isEmpty())
        {
            monster->setPosition(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
            list = mScene->collidingItems(monster);
            if(nTry++ > 200){ // Echec area already full of obstacles
                DEBUG_ERR("placement du loup ALPHA impossible");
                wolfCount--;
                mMonsters.removeOne(monster);
                delete monster;
                break;
            }
        }

        // Wolfs creations
        for(int n=0;n<QRandomGenerator::global()->bounded(6, 12);n++)
        {
            if(nTry > 200){ // Previous echec, need to start again alpha wolf placement
                break;
            }else{
                nTry = 0;
            }
            Monster * monster = new Wolf(mView);
            wolfCount++;
            initMonsterConnection(monster);
            mMonsters.append(monster);
            mScene->addItem(monster);

            // Wolfs placement
            monster->setPosition(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty() && (nTry++ < 200))
            {
                monster->setPosition(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
                list = mScene->collidingItems(monster);
            }
            if(nTry >= 200){ // Area now full, spawning wolf must cease
                DEBUG_ERR("Wolf placement impossible");
                wolfCount--;
                mMonsters.removeOne(monster);
                delete monster;
                break;
            }
        }
    }
    DEBUG("GENERATED : Wolfs              [" + QString("%1").arg(wolfCount) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des gobelins...");
    QApplication::processEvents();
    for(int m = 0; m < GoblinNumber; m++)
    {
        Monster * monster = new Goblin(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        if(m < GoblinNumber/2)
        {
            tryPlaceMonsterRandomly(monster, 1000);
        }else{
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
            int nTry = 0;
            do {
                monster->setPosition(mGoblinVillage->getVillage().x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage().boundingRect().width())),
                                mGoblinVillage->getVillage().y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage().boundingRect().height())));
            } while(!monster->collidesWithItem(&mGoblinVillage->getVillage()) && ++nTry < 200);
#endif
        }
    }
    DEBUG("GENERATED : Goblins            [" + QString("%1").arg(GoblinNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des ours...");
    QApplication::processEvents();
    for(int m = 0; m < BearNumber; m++)
    {
        Monster * monster = new Bear(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);
        tryPlaceMonsterRandomly(monster, 1000);
    }
    DEBUG("GENERATED : Bears              [" + QString("%1").arg(BearNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des araignées...");
    QApplication::processEvents();
    for(int m = 0; m < SpiderNumber; m++)
    {
        Monster * monster = new Spider(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);
        tryPlaceMonsterRandomly(monster, 1000);
    }
    DEBUG("GENERATED : Spiders              [" + QString("%1").arg(SpiderNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des trolls...");
    QApplication::processEvents();
    for(int m = 0; m < TrollNumber; m++)
    {
        Monster * monster = new Troll(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        if(m < TrollNumber*3/4)
        {
            tryPlaceMonsterRandomly(monster, 1000);
        }
        else
        {
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
            int nTry = 0;
            do {
                monster->setPosition(mGoblinVillage->getVillage().x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage().boundingRect().width())),
                                mGoblinVillage->getVillage().y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage().boundingRect().height())));
            } while(!monster->collidesWithItem(&mGoblinVillage->getVillage()) && ++nTry < 200);
#endif
        }
    }
    DEBUG("GENERATED : Trolls             [" + QString("%1").arg(TrollNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    emit sig_loadingGameStep("Génération des ogres...");
    QApplication::processEvents();
    for(int m = 0; m < OggreNumber; m++)
    {
        Monster * monster = new Oggre(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);
        monster->setPosition(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));

        if(m < 2) // 2 ogres in goblin village
        {
            int nTry = 0;
            while(1)
            {
                if(nTry++ > 200)
                {
                    DEBUG_ERR(QString("tryPlaceMonsterRandomly: impossible to place monster"));
                    break;
                }

                monster->setPosition(mGoblinVillage->x()-QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().width()*3/4)),
                                     mGoblinVillage->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().height()*3/4)));

                bool validPosition = true;
                QList<QGraphicsItem*> items = mScene->collidingItems(monster);
                for (QGraphicsItem* item : items)
                {
                    if (IsMapitemTypeOrDerived(item))
                    {
                        MapItem* mapItem = dynamic_cast<MapItem*>(item);
                        if (mapItem)
                        {
                            if (mapItem->isObstacle())
                            {
                                validPosition = false;
                                break;
                            }
                        }
                    }
                }

                if (validPosition)
                {
                    break;
                }
            }
        }
        else
        {
            tryPlaceMonsterRandomly(monster, 1000);
        }
    }
    DEBUG("GENERATED : Oggres             [" + QString("%1").arg(OggreNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));
    QApplication::processEvents();

    if(mVillage->getAltar()->isLaoShanLungSummoned())
    {
        generateLaoShanLung();
        DEBUG("GENERATED : Lao Shan Lung      [" + QString("%1").arg(1) + "]");
    }

    t_monstersActions->start(TIMER_MONSTERS_ACTION/mMonsters.size());
}

void Map::startBushAnimation(Bush * bush)
{
    if(!bush->isAnimated())
    {
        bush->startAnimation();
        emit sig_playSound(SOUND_BUSH_SHAKED);
    }
}

void Map::startBushEventAnimation(BushEvent * bush)
{
    if(!bush->isAnimated())
    {
        bush->startAnimation();

        BushEventCoin * coin = dynamic_cast<BushEventCoin*>(bush);
        if(coin){
            if(coin->eventIsActive())
                emit sig_playSound(SOUND_BUSH_SHAKED_EVENT_COIN);
            else {
                emit sig_playSound(SOUND_BUSH_SHAKED);
            }
            return;
        }
        BushEventEquipment * equipment = dynamic_cast<BushEventEquipment*>(bush);
        if(equipment){
            if(equipment->eventIsActive())
                emit sig_playSound(SOUND_BUSH_SHAKED_EVENT_EQUIPMENT);
            else {
                emit sig_playSound(SOUND_BUSH_SHAKED);
            }
            return;
        }
    }
}

void Map::setHero(Hero * h)
{
    if(mHero != nullptr){
        disconnect(mHero, SIGNAL(sig_movedInBush(Bush*)), this, SLOT(startBushAnimation(Bush*)));
        disconnect(mHero, SIGNAL(sig_movedInBush(BushEvent*)), this, SLOT(startBushEventAnimation(BushEvent*)));
        disconnect(mHero, SIGNAL(sig_ThrowItem(Item*)), this, SLOT(putItemThrownInMap(Item*)));
    }

    mHero = h;
    mScene->addItem(mHero);

    connect(mHero, SIGNAL(sig_movedInBush(Bush*)), this, SLOT(startBushAnimation(Bush*)));
    connect(mHero, SIGNAL(sig_movedInBushEvent(BushEvent*)), this, SLOT(startBushEventAnimation(BushEvent*)));
    connect(mHero, SIGNAL(sig_ThrowItem(Item*)), this, SLOT(putItemThrownInMap(Item*)));
}

void Map::reGenerateMap()
{
    removeMapElements();

    generateRandomMap();
    generateMonsters();

    emit sig_generationMapComplete();
}

void Map::putItemThrownInMap(Item * item)
{
    connect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
    connect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
    mHero->getBag()->takeItem(item);
    item->setZValue(Z_ITEM_FOREGROUND);
    mScene->addItem(item);
    item->setPos(mHero->pos().x()-mHero->boundingRect().width()/4+QRandomGenerator::global()->bounded(150), mHero->pos().y()+mHero->boundingRect().height()*3/4+QRandomGenerator::global()->bounded(50));
}

void Map::heroThrowItemInMap(Item * item)
{
    connect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
    connect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
    item->setZValue(Z_ITEM_FOREGROUND);
    mScene->addItem(item);
    item->setPos(mHero->pos().x()-mHero->boundingRect().width()/4+QRandomGenerator::global()->bounded(150), mHero->pos().y()+mHero->boundingRect().height()*3/4+QRandomGenerator::global()->bounded(50));
}

bool Map::checkCollisionBeetween(MapItem * item1, MapItem * item2)
{
    qreal dx = item1->x() - item2->x(), dy = item1->y() - item2->y();
    if(dx < 0)
    {
        if(dy < 0)
        {
            if(item1->x() + item1->boundingRect().width() > item2->x()
                    && item1->y() + item1->boundingRect().height() > item2->y())
            {
                return false;
            }
        }else
        {
            if(item1->x() + item1->boundingRect().width() > item2->x()
                    && item2->y() + item2->boundingRect().height() > item1->y())
            {
                return false;
            }
        }
    }else
    {
        if(dy < 0)
        {
            if(item2->x() + item2->boundingRect().width() > item1->x()
                    && item1->y() + item1->boundingRect().height() > item2->y())
            {
                return false;
            }
        }else
        {
            if(item2->x() + item2->boundingRect().width() > item1->x()
                    && item2->y() + item2->boundingRect().height() > item1->y())
            {
                return false;
            }
        }
    }
    return true;
}

bool Map::checkPositionIsEmpty(QRectF rect, QPointF pos)
{
    QGraphicsRectItem * tmp = new QGraphicsRectItem(rect);
    mScene->addItem(tmp);
    tmp->setPos(pos);
    for(MapItem * mapItem : mElementsInMap)
    {
        QGraphicsRectItem * tmpMapItem = new QGraphicsRectItem(QRectF(0,0, mapItem->boundingRect().width(), mapItem->boundingRect().height()));
        mScene->addItem(tmpMapItem);
        tmpMapItem->setPos(mapItem->pos());
        if(tmpMapItem->collidesWithItem(tmp))
        {
            // if Zvalue > zValue continue else false

            mScene->removeItem(tmp);
            mScene->removeItem(tmpMapItem);
            delete tmp;
            delete tmpMapItem;
            return false;
        }
        mScene->removeItem(tmpMapItem);
        delete tmpMapItem;
    }
    mScene->removeItem(tmp);
    delete tmp;
    return true;
}

Map::~Map()
{
   if (mVillage)
       delete mVillage;
    if (mGoblinVillage)
        delete mGoblinVillage;
    if (mDayNightCycle)
        delete mDayNightCycle;
}
