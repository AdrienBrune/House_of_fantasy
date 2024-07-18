#include "map.h"

#include <QElapsedTimer>
#include <QRandomGenerator>

extern quint8 loadingStep;

Map::Map(QWidget * parent, QGraphicsView * view):
    QObject(),
    mView(view),
    mHero(nullptr),
    heroEventLocation(SpotEvent{false}),
    t_collisionHandler(nullptr),
    mBushes(QList<Bush*>()),
    mVillage(nullptr),
    mGoblinVillage(nullptr),
    mElementsInMap_movable(QList<MapItem*>()),
    mElementsInMap(QList<MapItem*>()),
    mMonsters(QList<Monster*>()),
    t_monstersActions(nullptr),
    monsterActionIndex(0),
    t_monsterMove(nullptr)
{
    connect(this, SIGNAL(sig_loadingGameUpdate(quint8)), parent, SIGNAL(sig_loadingGameUpdate(quint8)));

    mScene = new QGraphicsScene(this);
    mScene->setSceneRect(QRect(0,0,MAP_WIDTH,MAP_HEIGHT));
    mScene->setBackgroundBrush(QPixmap(":/map/background.png"));

    t_monstersActions = new QTimer(this);
    connect(t_monstersActions, SIGNAL(timeout()), this, SLOT(monstersActionHandler()));

    t_monsterMove = new QTimer(this);
    connect(t_monsterMove, SIGNAL(timeout()), mScene, SLOT(advance()));

    t_collisionHandler = new QTimer(this);
    connect(t_collisionHandler, SIGNAL(timeout()), this, SLOT(heroCollisionEventHandler()));

    putVillageInMap();
    DEBUG("GENERATED : Village");
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
    mMonsters[monsterActionIndex]->nextAction(mHero);
    monsterActionIndex++;
}

void Map::heroCollisionEventHandler()
{
    QList<QGraphicsItem *> list = mScene->collidingItems(mHero) ;

    // Check village interaction
    bool found = false;
    for(QGraphicsItem * item : list)
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
    for(QGraphicsItem * item : list)
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
    for(QGraphicsItem * item : list)
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
        //removeMapElement(item);
    }else{
        QList<QGraphicsItem*> list = mScene->collidingItems(item);
        for(QGraphicsItem * collisionItem : list){
            Monster * monster = dynamic_cast<Monster*>(collisionItem);
            if(monster){
                connect(item, SIGNAL(sig_deleteItem(MapItemMovable*)), this, SLOT(deleteMovableMapItem(MapItemMovable*)));
                itemMovable->setReadyToDelete();
                //removeMapElement(item);
                return;
            }
        }
    }
}

void Map::itemInMapClicked(Item * item)
{
    if(ToolFunctions::heroDistanceWith(mHero, item) < 150){
        disconnect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
        disconnect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
        mScene->removeItem(item);
        item->setHover(false);
        if(mHero->takeItem(item)){
            emit sig_itemPickedInMap(item);
        }
    }
}

void Map::tryToOpenChest(ChestEvent * chest)
{
    if(ToolFunctions::getDistanceBeetween(mHero, chest) < 100 && chest->isRevealed() && !chest->isOpen())
    {
        emit sig_openInterface(chest);
    }
}

void Map::tryToStartPNGInteraction(QGraphicsItem * png)
{
    if(ToolFunctions::heroDistanceWith(mHero, png) < 200)
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

    timer.start();
    putGoblinVillageInMap();
    DEBUG("GENERATED : GoblinVillage      [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    DEBUG("GENERATING : Map elements");

    timer.start();
    generateGround();
    DEBUG("GENERATED : Ground             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    putLakesInMap();
    DEBUG("GENERATED : Lakes              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateTrees();
    DEBUG("GENERATED : Trees              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateBushes();
    DEBUG("GENERATED : Bushes             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateBushCoinEvent();
    DEBUG("GENERATED : BushCoin           [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateBushEquipmentEvent();
    DEBUG("GENERATED : BushEquipment      [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    while(!mBushes.isEmpty())
        mBushes.removeLast();

    timer.start();
    generateRocks();
    DEBUG("GENERATED : Rocks              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generatePlanks();
    DEBUG("GENERATED : Planks             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateStones();
    DEBUG("GENERATED : Stones             [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateChestBurriedEvent();
    DEBUG("GENERATED : ChestBurried       [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateFallenTrees();
    DEBUG("GENERATED : FallenTrees        [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    generateOreSpots();
    DEBUG("GENERATED : OreSpots           [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    timer.start();
    // Remove mapItems colliding with village
    for(MapItem * item : mElementsInMap)
    {
        if(item->pos().x()+item->boundingRect().width()>mVillage->getPosition().x() && item->pos().x()<mVillage->getPosition().x()+mVillage->boundingRect().width()
           && item->pos().y()+item->boundingRect().height()>mVillage->getPosition().y() && item->pos().y()<mVillage->getPosition().y()+mVillage->boundingRect().height())
        {
            if(mElementsInMap_movable.contains(item)){
                mElementsInMap_movable.removeOne(item);
            }
            mElementsInMap.removeOne(item);
            item->deleteLater();
        }
    }
    // Remove mapItems colliding with Goblin village
    for(MapItem * item : mElementsInMap)
    {
        if(item->pos().x()+item->boundingRect().width()>mGoblinVillage->pos().x() && item->pos().x()<mGoblinVillage->pos().x()+mGoblinVillage->boundingRect().width()
           && item->pos().y()+item->boundingRect().height()>mGoblinVillage->pos().y() && item->pos().y()<mGoblinVillage->pos().y()+mGoblinVillage->boundingRect().height())
        {
            if(mElementsInMap_movable.contains(item)){
                mElementsInMap_movable.removeOne(item);
            }
            mElementsInMap.removeOne(item);
            item->deleteLater();
        }
    }
    DEBUG("GENERATED : Removing items     [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // Hide items in the map
    timer.start();
    putItemsInMap();
    DEBUG("GENERATED : Items              [" << QString("%1").arg(timer.elapsed()) << "]ms");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    DEBUG("GENERATED : Elements(" << mElementsInMap.size() << ")");
}

void Map::removeMapElements()
{
    DEBUG("REMOVING ITEMS : Number before(" << QString("%1").arg(mScene->items().size()) << ")");

    while(!mElementsInMap_movable.isEmpty())
        mElementsInMap_movable.removeLast();
    while(!mElementsInMap.isEmpty()){
        delete mElementsInMap.takeLast();
    }
    QList<QGraphicsItem*> itemsLeftInMap = mScene->items();
    for(QGraphicsItem * element : itemsLeftInMap)
    {
        Item * item = dynamic_cast<Item*>(element);
        if(item){
            delete item;
        }
    }
    DEBUG("REMOVING ITEMS : Number after (" << QString("%1").arg(mScene->items().size()) << ")");
    int n = 0;
    for(QGraphicsItem * item : mScene->items())
    {
        if(dynamic_cast<Monster*>(item)){
            n++;
        }
    }
    DEBUG("REMOVING ITEMS : Number monster(" << n << ")");
}

void Map::removeMonsters()
{
    t_monstersActions->stop();
    while(!mMonsters.isEmpty())
    {
        delete mMonsters.takeLast();
    }
}

void Map::freezeMap()
{
    t_monstersActions->stop();
    t_monsterMove->stop();
    t_collisionHandler->stop();
    for(Monster * monster : mMonsters)
    {
        monster->enableMonsterAnimation(false);
    }
}

void Map::removeMapElement(MapItem * item)
{
    mScene->removeItem(item);
    if(mElementsInMap_movable.contains(item)){
        mElementsInMap_movable.removeOne(item);
    }
    if(mElementsInMap.contains(item)){
        mElementsInMap.removeOne(item);
    }else{
        DEBUG_ERR("ERR : element delete not contain in map");
    }
    item->deleteLater();
}

void Map::unfreezeMap()
{
    t_collisionHandler->start(TIMER_COLLISION);
    t_monstersActions->start(DELAY_BETWEEN_ACTION/mMonsters.size());
    t_monsterMove->start(TIMER_MONSTERS_MOVE);
    for(Monster * monster : mMonsters)
    {
        monster->enableMonsterAnimation(true);
    }
}

void Map::generateGround()
{
    for(int i=0;i<NUM_GROUND;i++){
        Ground * ground = new Ground();
        ground->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
        mScene->addItem(ground);
        mElementsInMap.append(ground);
    }
}

void Map::generateBushes()
{
    bool validateEmplacement;
    for(int i=0;i<NUM_BUSHES;i++){
        Bush * bush = new Bush();
        bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        validateEmplacement = false;
        while(!validateEmplacement && i!=0)
        {
            bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            QList<QGraphicsItem*> list = mScene->collidingItems(bush);
            while(!list.isEmpty())
            {
                bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                list = mScene->collidingItems(bush);
            }
            for(Bush * b : mBushes){
                int dx = static_cast<int>(bush->x() - b->x());
                int dy = static_cast<int>(bush->y() - b->y());
                    if( (dy < 0 && dy > -b->boundingRect().height()) && (abs(dx) < b->boundingRect().width()) ){
                        break;
                    }
                if(mBushes.last() == b){
                    validateEmplacement = true;
                }
            }
        }
        mBushes.append(bush);
        mScene->addItem(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateTrees()
{
    QList<Tree*> trees;
    bool validateEmplacement;
    for(int i=0;i<NUM_TREES;i++){
        Tree * tree = new Tree();
        mScene->addItem(tree);

        validateEmplacement = false;
        while(!validateEmplacement && i!=0)
        {
            tree->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            while(!mScene->collidingItems(tree).isEmpty()){
                tree->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            }

            for(Tree * t : trees){
                int dx = static_cast<int>(tree->x() - t->x());
                int dy = static_cast<int>(tree->y() - t->y());
                    if( (dy < 0 && dy > -t->boundingRect().height()) && (abs(dx) < t->boundingRect().width()) ){
                        break;
                    }
                if(trees.last() == t){
                    validateEmplacement = true;
                }
            }
        }
        trees.append(tree);
        mElementsInMap.append(tree);
        connect(tree, SIGNAL(sig_playSound(int)), this, SIGNAL(sig_playSound(int)));
    }
}

void Map::generateFallenTrees()
{
    for(int i=0;i<NUM_TREES_FALLEN;i++){
        TreeFallen * treeFallen = new TreeFallen();
        mScene->addItem(treeFallen);
        treeFallen->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(treeFallen, item))
                {
                    validatePosition = false;
                    treeFallen->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(treeFallen);
    }
}

void Map::generateRocks()
{
    QList<Rock*> rocks;
    for(int i=0;i<NUM_ROCKS;i++)
    {
        Rock * rock = new Rock();
        mScene->addItem(rock);
        rock->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validateEmplacement = false;
        while(!validateEmplacement && i!=0)
        {
            rock->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            QList<QGraphicsItem*> list = mScene->collidingItems(rock);
            while(!list.isEmpty())
            {
                rock->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                list = mScene->collidingItems(rock);
            }


            for(Rock * r : rocks){
                int dx = static_cast<int>(rock->x() - r->x());
                int dy = static_cast<int>(rock->y() - r->y());
                    if( (dy < 0 && dy > -r->boundingRect().height()) && (abs(dx) < r->boundingRect().width()) ){
                        break;
                    }
                if(rocks.last() == r){
                    validateEmplacement = true;
                }
            }
        }
        rocks.append(rock);
        mElementsInMap.append(rock);
        connect(rock, SIGNAL(sig_playSound(int)), this, SIGNAL(sig_playSound(int)));
    }
}


void Map::generateBushCoinEvent()
{
    bool validateEmplacement;
    for(int i=0;i<NUM_BUSHES_COIN_EVENT;i++){
        BushEventCoin * bush = new BushEventCoin();
        bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        validateEmplacement = false;
        while(!validateEmplacement && i!=0)
        {
            bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            QList<QGraphicsItem*> list = mScene->collidingItems(bush);
            while(!list.isEmpty())
            {
                bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                list = mScene->collidingItems(bush);
            }

            for(Bush * b : mBushes){
                int dx = static_cast<int>(bush->x() - b->x());
                int dy = static_cast<int>(bush->y() - b->y());
                    if( (dy < 0 && dy > -b->boundingRect().height()) && (abs(dx) < b->boundingRect().width()) ){
                        break;
                    }
                if(mBushes.last() == b){
                    validateEmplacement = true;
                }
            }
        }
        mScene->addItem(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateBushEquipmentEvent()
{
    bool validateEmplacement;
    QList<BushEventEquipment*> list;
    for(int i=0;i<NUM_BUSHES_EQUIPMENT_EVENT;i++){
        BushEventEquipment * bush = new BushEventEquipment();
        bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        validateEmplacement = false;
        while(!validateEmplacement && i!=0)
        {
            bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
            QList<QGraphicsItem*> list = mScene->collidingItems(bush);
            while(!list.isEmpty())
            {
                bush->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                list = mScene->collidingItems(bush);
            }

            for(Bush * b : mBushes){
                int dx = static_cast<int>(bush->x() - b->x());
                int dy = static_cast<int>(bush->y() - b->y());
                    if( (dy < 0 && dy > -b->boundingRect().height()) && (abs(dx) < b->boundingRect().width()) ){
                        break;
                    }
                if(mBushes.last() == b){
                    validateEmplacement = true;
                }
            }
        }
        list.append(bush);
        mScene->addItem(bush);
        mElementsInMap.append(bush);
    }
}

void Map::generateChestBurriedEvent()
{
    for(int i=0;i<NUM_CHEST_BURRIED_EVENT;i++){
        ChestBurried * chest = new ChestBurried();
        connect(chest, SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
        mScene->addItem(chest);
        chest->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(chest, item))
                {
                    validatePosition = false;
                    chest->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(chest);
    }
}

void Map::generateOreSpots()
{
    int rubisSpots = QRandomGenerator::global()->bounded(NUM_RUBIS_ORE);
    int saphirSpots = QRandomGenerator::global()->bounded(NUM_SAPHIR_ORE);
    int emeraldSpots = QRandomGenerator::global()->bounded(NUM_EMERALD_ORE);

    for(int i=0;i<NUM_IRON_ORE;i++){
        IronOreSpot * ironOre = new IronOreSpot();
        mScene->addItem(ironOre);
        ironOre->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(ironOre, item))
                {
                    validatePosition = false;
                    ironOre->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(ironOre);
    }

    for(int i=0;i<NUM_STONE_ORE;i++){
        StoneOreSpot * stoneOre = new StoneOreSpot();
        mScene->addItem(stoneOre);
        stoneOre->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(stoneOre, item))
                {
                    validatePosition = false;
                    stoneOre->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(stoneOre);
    }

    for(int i=0;i<saphirSpots;i++){
        SaphirOreSpot * ore = new SaphirOreSpot();
        mScene->addItem(ore);
        ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(ore, item))
                {
                    validatePosition = false;
                    ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(ore);
    }

    for(int i=0;i<emeraldSpots;i++){
        EmeraldOreSpot * ore = new EmeraldOreSpot();
        mScene->addItem(ore);
        ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(ore, item))
                {
                    validatePosition = false;
                    ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(ore);
    }

    for(int i=0;i<rubisSpots;i++){
        RubisOreSpot * ore = new RubisOreSpot();
        mScene->addItem(ore);
        ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(ore, item))
                {
                    validatePosition = false;
                    ore->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap.append(ore);
    }

}

void Map::generateLaoShanLung()
{
    LaoShanLung * monster = new LaoShanLung(mView);
    initMonsterConnection(monster);
    mMonsters.append(monster);
    mScene->addItem(monster);
    QList<QGraphicsItem*> list;
    bool locationValid = true;

    do
    {
        locationValid = true;
        monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH-1000), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT-1000));
        QList<QGraphicsItem*> list = mScene->collidingItems(monster);
        for(QGraphicsItem * item : list)
        {
            MapItem * mapItem = dynamic_cast<MapItem*>(item);
            if(mapItem)
            {
                if(mapItem->isObstacle())
                    locationValid = false;
            }
        }

    }while(!locationValid);
    QPointF p = monster->pos();
}

void Map::generatePlanks()
{
    for(int i=0;i<NUM_PLANK;i++){
        Plank * plank = new Plank();
        connect(plank, SIGNAL(sig_itemMoved(MapItem*)), this, SLOT(itemMoved(MapItem*)));
        connect(plank, SIGNAL(sig_itemPositionFixed(MapItem*)), this, SLOT(checkItemMovedPosition(MapItem*)));
        mScene->addItem(plank);
        plank->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(plank, item))
                {
                    validatePosition = false;
                    plank->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap_movable.append(plank);
        plank->setInitialPosition(plank->pos());
        mElementsInMap.append(plank);
    }
}

void Map::generateStones()
{
    for(int i=0;i<NUM_STONE;i++){
        Stone * stone = new Stone();
        connect(stone, SIGNAL(sig_itemMoved(MapItem*)), this, SLOT(itemMoved(MapItem*)));
        connect(stone, SIGNAL(sig_itemPositionFixed(MapItem*)), this, SLOT(checkItemMovedPosition(MapItem*)));
        mScene->addItem(stone);
        stone->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(stone, item))
                {
                    validatePosition = false;
                    stone->setPos(QRandomGenerator::global()->bounded(100, MAP_WIDTH-100), QRandomGenerator::global()->bounded(100, MAP_HEIGHT-100));
                    break;
                }
            }
        }
        mElementsInMap_movable.append(stone);
        stone->setInitialPosition(stone->pos());
        mElementsInMap.append(stone);
    }
}

void Map::putItemsInMap()
{
    for(MapItem * mapItem : mElementsInMap_movable)
    {
        Item * item = nullptr;
        int randomNumber = QRandomGenerator::global()->bounded(100);
        if(randomNumber < 6)
        {
            item = gItemGenerator->generateEquipment();

            mScene->addItem(item);
            item->setPos(mapItem->x()+mapItem->boundingRect().width()/2-item->boundingRect().width()/2,
                         mapItem->y()+mapItem->boundingRect().height()/2-item->boundingRect().height()/2);

        }else if(randomNumber < 15){
            item = gItemGenerator->generateRandomConsumable();

            mScene->addItem(item);
            item->setPos(mapItem->x()+mapItem->boundingRect().width()/2-item->boundingRect().width()/2,
                         mapItem->y()+mapItem->boundingRect().height()/2-item->boundingRect().height()/2);

        }else if(randomNumber <= 25){
            item = new BagCoin(QRandomGenerator::global()->bounded(1, 5));

            mScene->addItem(item);
            item->setPos(mapItem->x()+mapItem->boundingRect().width()/2-item->boundingRect().width()/2,
                         mapItem->y()+mapItem->boundingRect().height()/2-item->boundingRect().height()/2);
        }else if(randomNumber <= 27){
            item = new EarthCristal();

            mScene->addItem(item);
            item->setPos(mapItem->x()+mapItem->boundingRect().width()/2-item->boundingRect().width()/2,
                         mapItem->y()+mapItem->boundingRect().height()/2-item->boundingRect().height()/2);
        }

        if(item != nullptr){
            connect(item, SIGNAL(sig_itemClicked(Item*)), this, SLOT(itemInMapClicked(Item*)));
            connect(item, SIGNAL(sig_showItemInfo(Item*)), this, SIGNAL(sig_showItemInfo(Item*)));
        }
    }
}

void Map::putVillageInMap(Village * village)
{
    if(!village)
        mVillage = new Village();
    else
    {
        if(mVillage)
            delete mVillage;
        mVillage = village;
    }
    mVillage->setPosition(QPointF(MAP_WIDTH/2, MAP_HEIGHT/2));
    mVillage->addInScene(mScene);
    connect(mVillage, SIGNAL(sig_replenish(QObject*)), this, SIGNAL(sig_replenish(QObject*)));
    connect(mVillage, SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_showPNJinfo(QGraphicsItem*)));
    connect(mVillage, SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SLOT(tryToStartPNGInteraction(QGraphicsItem*)));
    connect(mVillage, SIGNAL(sig_LaoShanLungSummoned()), this, SLOT(onLaoShanLungSummoned()));
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
        connect(mGoblinVillage->getVillage(), SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SLOT(tryToOpenChest(ChestEvent*)));
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
    for(int i=0;i<NUM_LAKES;i++){
        Lake * lake = new Lake(new FishingEvent);
        mScene->addItem(lake);
        mScene->addItem(lake->getEvent());
        lake->setPos(QRandomGenerator::global()->bounded(500, MAP_WIDTH), QRandomGenerator::global()->bounded(500, MAP_HEIGHT));
        lake->getEvent()->setPos(lake->pos().x()-60, lake->pos().y()+lake->boundingRect().height()/2);

        bool validatePosition = false;
        while(!validatePosition)
        {
            validatePosition = true;
            for(MapItem * item : mElementsInMap)
            {
                if(!checkCollisionBeetween(lake, item))
                {
                    validatePosition = false;
                    lake->setPos(QRandomGenerator::global()->bounded(500, MAP_WIDTH), QRandomGenerator::global()->bounded(500, MAP_HEIGHT));
                    lake->getEvent()->setPos(lake->pos().x()-60, lake->pos().y()+lake->boundingRect().height()/2);
                    break;
                }
            }
        }
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

void Map::generateMonsters()
{
    DEBUG("GENERATING : Monsters");
    int WolfPackNumber = NUM_WOLFPACK, BearNumber = NUM_BEAR, GoblinNumber = NUM_GOBLIN, TrollNumber = NUM_TROLL, OggreNumber = NUM_OGGRE;

    int chunkSize = 500, wolfCount = 0, nTry = 0;
    QList<QGraphicsItem*> toAvoid {mVillage, mGoblinVillage};
    for(int n=0;n<WolfPackNumber;n++)
    {
        QRect wolfPackArea = ToolFunctions::getSpawnChunk(QSize(chunkSize,chunkSize),toAvoid);

        Monster * monster = new WolfAlpha(mView);
        wolfCount++;
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        // Alpha Wolf placement
        monster->setPos(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
        QList<QGraphicsItem*> list = mScene->collidingItems(monster);
        while(!list.isEmpty())
        {
            monster->setPos(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
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
            monster->setPos(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty() && (nTry++ < 200))
            {
                monster->setPos(QPointF(wolfPackArea.x()+QRandomGenerator::global()->bounded(wolfPackArea.width()), wolfPackArea.y()+QRandomGenerator::global()->bounded(wolfPackArea.height())));
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

    for(int m=0;m<GoblinNumber;m++)
    {
        Monster * monster = new Goblin(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        if(m < GoblinNumber/2)
        {
            monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty())
            {
                monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
                list = mScene->collidingItems(monster);
            }
        }else{
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
            monster->setPos(mGoblinVillage->getVillage()->x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().width())),
                            mGoblinVillage->getVillage()->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().height())));

            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(1)
            {
                if(monster->collidesWithItem(mGoblinVillage->getVillage()) && list.size()==2)
                {
                    break;
                }

                monster->setPos(mGoblinVillage->getVillage()->x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().width())),
                                mGoblinVillage->getVillage()->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().height())));
                list = mScene->collidingItems(monster);
            }
#endif
        }
    }
    DEBUG("GENERATED : Goblins            [" + QString("%1").arg(GoblinNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    for(int m=0;m<BearNumber;m++)
    {
        Monster * monster = new Bear(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
        QList<QGraphicsItem*> list = mScene->collidingItems(monster);
        while(!list.isEmpty())
        {
            monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
            list = mScene->collidingItems(monster);
        }
    }
    DEBUG("GENERATED : Bears              [" + QString("%1").arg(BearNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    for(int m=0;m<TrollNumber;m++)
    {
        Monster * monster = new Troll(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);

        if(m < TrollNumber*3/4)
        {
            monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty())
            {
                monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
                list = mScene->collidingItems(monster);
            }
        }else{
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
            monster->setPos(mGoblinVillage->getVillage()->x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().width())),
                            mGoblinVillage->getVillage()->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().height())));

            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(1)
            {
                if(monster->collidesWithItem(mGoblinVillage->getVillage()) && list.size() == 2)
                {
                    break;
                }

                monster->setPos(mGoblinVillage->getVillage()->x()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().width())),
                                mGoblinVillage->getVillage()->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->getVillage()->boundingRect().height())));
                list = mScene->collidingItems(monster);
            }
#endif
        }
    }
    DEBUG("GENERATED : Trolls             [" + QString("%1").arg(TrollNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    for(int m=0;m<OggreNumber;m++)
    {
        Monster * monster = new Oggre(mView);
        initMonsterConnection(monster);
        mMonsters.append(monster);
        mScene->addItem(monster);
        monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));

        if(m == 0)
        {
            monster->setPos(mGoblinVillage->x()-QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().width()/2)),
                            mGoblinVillage->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().height()/2)));

            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty())
            {
                monster->setPos(mGoblinVillage->x()-QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().width()/2)),
                                mGoblinVillage->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().height()/2)));
                list = mScene->collidingItems(monster);
            }
        }else if(m == 1)
        {
#ifndef PUT_GOBLIN_VILLAGE_NEXT_TO_VILLAGE
            monster->setPos(mGoblinVillage->x()+mGoblinVillage->boundingRect().width()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().width()/2)),
                            mGoblinVillage->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().height()/2)));

            QList<QGraphicsItem*> list = mScene->collidingItems(monster);
            while(!list.isEmpty())
            {
                monster->setPos(mGoblinVillage->x()+mGoblinVillage->boundingRect().width()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().width()/2)),
                                mGoblinVillage->y()+QRandomGenerator::global()->bounded(static_cast<int>(mGoblinVillage->boundingRect().height()/2)));
                list = mScene->collidingItems(monster);
            }
#endif
        }

        QList<QGraphicsItem*> list = mScene->collidingItems(monster);
        while(!list.isEmpty())
        {
            monster->setPos(QRandomGenerator::global()->bounded(1000, MAP_WIDTH), QRandomGenerator::global()->bounded(1000, MAP_HEIGHT));
            list = mScene->collidingItems(monster);
        }
    }
    DEBUG("GENERATED : Oggres             [" + QString("%1").arg(OggreNumber) + "]");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    if(mVillage->getAltar()->isLaoShanLungSummoned())
    {
        generateLaoShanLung();
        DEBUG("GENERATED : Lao Shan Lung      [" + QString("%1").arg(1) + "]");
    }

    t_monstersActions->start(DELAY_BETWEEN_ACTION/mMonsters.size());
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
    if(mGoblinVillage!=nullptr)
        delete mGoblinVillage;
    removeMapElements();
    removeMonsters();
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
            // if Zvalue > zValue cpntinue else false

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
//    if(mVillage) // Village deleted from save
//        delete mVillage;
    if(mGoblinVillage)
        delete mGoblinVillage;
}
