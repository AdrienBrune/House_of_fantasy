#include "ctrwindow.h"
#include "ui_ctrwindow.h"
#include "win_loadinggamescreen.h"
#include "entitieshandler.h"
#include <QDir>
#include <QCursor>

quint8 loadingStep = 0;
extern Hero * gSelectedHero;

CTRWindow::CTRWindow(QWidget *parent) :
    QMainWindow(parent),
    mMap(nullptr),
    mHero(nullptr),
    mSoundManager(nullptr),
    heroIsSearching(false),
    t_unfreezeMap(nullptr),
    t_PeriodicalEvents(nullptr),
    mCurrentSave(nullptr),
    mLoadingAvancement(0),
    mHeroMovementAllowed(true),
    w_menu(nullptr),
    w_heroStats(nullptr),
    w_explorationLoading(nullptr),
    w_inventory(nullptr),
    w_gear(nullptr),
    w_fight(nullptr),
    w_trading(nullptr),
    w_skill(nullptr),
    w_tool(nullptr),
    w_map(nullptr),
    w_quickItemDrawer(nullptr),
    w_messageLogger(nullptr),
    w_night(nullptr),
    ui(new Ui::CTRWindow)
{
    ui->setupUi(this);

    gItemGenerator = new ItemGenerator();

    QDir directory(QDir::currentPath()+"/"+FILE_SAVE);
    if(!directory.exists())
        directory.mkdir(QDir::currentPath()+"/"+FILE_SAVE);

    w_menu = new Win_Menu();
    connect(w_menu, SIGNAL(sig_startGame(Save*)), this, SLOT(onStartGame(Save*)));
    connect(w_menu, SIGNAL(sig_quitGame()), this, SLOT(onQuitGame()));
    w_menu->show();

    w_loadingScreen = new Win_LoadingGameScreen();
    connect(this, &CTRWindow::sig_loadingGameUpdate, w_loadingScreen, &Win_LoadingGameScreen::updateLoadingProgress);
    w_loadingScreen->hide();

    hide();

    QCursor cursor(QPixmap(":/graphicItems/cursor.png"), 16, 16);
    setCursor(cursor);
    w_menu->setCursor(cursor);
    w_loadingScreen->setCursor(cursor);
}

CTRWindow::~CTRWindow()
{
    if(gItemGenerator)
        delete gItemGenerator;
    if(mMap)
        delete mMap;
    if(w_menu)
        delete w_menu;
    if(w_loadingScreen)
        delete w_loadingScreen;
    if(mCurrentSave)
        delete mCurrentSave;
    delete ui;
}

void CTRWindow::onStartGame(Save * save)
{
    w_menu->hide();

    /* Loading screen */
    loadingStep = 0;
    emit sig_loadingGameUpdate(loadingStep);
    w_loadingScreen->setImage(save->getHero()->getHeroClass());
    w_loadingScreen->showFullScreen();

    if(!mCurrentSave && mCurrentSave != save)
        delete mCurrentSave;
    mCurrentSave = save;

    gSelectedHero = mCurrentSave->getHero();

    QTimer::singleShot(1000, this, &CTRWindow::generateNewGame);
}

void CTRWindow::generateNewGame()
{
    // GAME ITEMS
    DEBUG("GENERATED : ItemGenerator");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // MAP CREATION
    mMap = new Map(this, ui->graphicsView);
    t_unfreezeMap = new QTimer(this);
    t_unfreezeMap->setSingleShot(true);
    connect(mMap, &Map::sig_monsterEncountered, this, &CTRWindow::GoToMonsterFight);
    connect(t_unfreezeMap, &QTimer::timeout, mMap, &Map::unfreezeMap);
    connect(mMap, &Map::sig_replenish, this, &CTRWindow::showReplenish);
    connect(mMap, &Map::sig_showPNJinfo, this, &CTRWindow::showPNJinfo);
    connect(mMap, &Map::sig_openInterface, this, &CTRWindow::openInterface);
    connect(mMap, &Map::sig_showItemInfo, this, &CTRWindow::showItemOnGround);
    connect(mMap, &Map::sig_itemPickedInMap, this, &CTRWindow::showItemPicked);
    connect(mMap, &Map::sig_displayMonsterData, this, &CTRWindow::displayMonsterData);
    connect(mMap, &Map::sig_calamitySpawned, this, &CTRWindow::displayCalamitySpawned);
    connect(mMap, &Map::sig_adventurerMapUnlock, this, [this](){ setButtonsEnable(true); });
    ui->graphicsView->setScene(mMap->getScene());
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mMap->putVillageInMap(mCurrentSave->getVillage());
    if(mMap->getVillage()->getAltar()->isLaoShanLungSummoned())
        mMap->generateLaoShanLung();
    DEBUG("GENERATED : Map");

    // SOUND EFFECTS
    mSoundManager = new SoundManager(this);
    connect(mMap, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    connect(mMap, SIGNAL(sig_heroEnterVillage()), mSoundManager, SLOT(heroEnterVillage()));
    connect(mMap, SIGNAL(sig_heroLeftVillage()), mSoundManager, SLOT(heroLeftVillage()));
    connect(mMap, SIGNAL(sig_heroEnterEvent(int)), mSoundManager, SLOT(startMusicEvent(int)));
    connect(mMap, SIGNAL(sig_heroLeaveEvent(int)), mSoundManager, SLOT(endMusicEvent(int)));
    DEBUG("GENERATED : SoundManager");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // HERO CREATION
    mHero = mCurrentSave->getHero();
    mMap->setHero(mHero);
    mHero->setPos(mHero->getLocation() != QPointF(0,0) ? mHero->getLocation() : QPointF(mMap->getVillage()->pos().x()+500, mMap->getVillage()->pos().y()+800));
    mHero->getAdventurerMap().map.init();
    connect(mHero, SIGNAL(sig_bagFull()), this, SLOT(showBagFull()));
    connect(mHero, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    connect(mHero, SIGNAL(sig_enterMapEvent(Tool*)), this, SLOT(showQuickToolDrawer(Tool*)));
    connect(mHero, SIGNAL(sig_leaveMapEvent()), this, SLOT(hideQuickToolDrawer()));
    connect(mHero, SIGNAL(sig_ThrowItem(Item*)), this, SLOT(unuseToolIfThrown(Item*)));
    connect(mHero, SIGNAL(sig_SellItem(Item*)), this, SLOT(unuseToolIfThrown(Item*)));
    DEBUG("GENERATED : Hero");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // // HERO ITEMS
    // for(MapScroll * mapScroll : mHero->getBag()->getItemList<MapScroll*>())
    //     mapScroll->init();

    // HERO INTERFACE
    w_heroStats = new W_HeroStats(this, mHero);
    w_heroStats->setGeometry(0,0,500,300);
    w_heroStats->show();
    DEBUG("GENERATED : Hero interface");
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    ui->graphicsView->installEventFilter(this);
    mMap->getScene()->installEventFilter(this);

    // SCROLLVIEW
    ui->graphicsView->centerOn(mHero);
    connect(mHero, SIGNAL(sig_heroMoved()), this, SLOT(scrollSceneView()));

    t_PeriodicalEvents = new QTimer(this);
    connect(t_PeriodicalEvents, SIGNAL(timeout()), this, SLOT(onPeriodicalEvents()));
    t_PeriodicalEvents->start(PERIODICAL_EVENTS);
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // HERO CHEST
    for(Item * item : mCurrentSave->getHeroChest()->getItems())
    {
        mMap->getVillage()->getHeroHouse()->getChest()->addItem(item);
    }
    if(mCurrentSave->getHeroChest())
        delete mCurrentSave->getHeroChest();
    mCurrentSave->setChest(nullptr);
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    w_loadingScreen->hide();
    showFullScreen();

    EntitiesHandler::getInstance().registerHero(mHero);
    EntitiesHandler::getInstance().registerMap(mMap);
    EntitiesHandler::getInstance().registerView(ui->graphicsView);

    setButtonsEnable(true);
}

void CTRWindow::closeGame()
{
    if(w_heroStats)
        delete w_heroStats;
    w_heroStats = nullptr;

    if(w_explorationLoading)
        delete w_explorationLoading;
    w_explorationLoading = nullptr;

    if(w_inventory)
        delete w_inventory;
    w_inventory = nullptr;

    if(w_gear)
        delete w_gear;
    w_gear = nullptr;

    if(w_trading)
        delete w_trading;
    w_trading = nullptr;

    if(w_fight)
        delete w_fight;
    w_fight = nullptr;

    if(w_skill)
        delete w_skill;
    w_skill = nullptr;

    if(w_tool)
    {
        w_tool->hideTool();
        delete w_tool;
    }
    w_tool = nullptr;

    if(w_map)
    {
        w_map->hideTool();
        delete w_map;
    }
    w_map = nullptr;

    if(w_quickItemDrawer)
        delete w_quickItemDrawer;
    w_quickItemDrawer = nullptr;

    if(w_messageLogger)
        delete w_messageLogger;
    w_messageLogger = nullptr;

    if(w_night)
        delete w_night;
    w_night = nullptr;

    if(mMap)
        delete mMap;
    mMap = nullptr;
    DEBUG("DELETED : Map");

    if(mSoundManager)
        delete mSoundManager;
    mSoundManager = nullptr;
    DEBUG("DELETED : Sound manager");

    if(w_heroStats)
        delete w_heroStats;
    w_heroStats = nullptr;
    DEBUG("DELETED : Hero stats");

    if(t_unfreezeMap)
        delete t_unfreezeMap;
    if(t_PeriodicalEvents)
        delete t_PeriodicalEvents;
    t_unfreezeMap = nullptr;
    t_PeriodicalEvents = nullptr;
    DEBUG("DELETED : Timers");
}

void CTRWindow::scrollSceneView()
{
    ui->graphicsView->centerOn(mHero);
}

void CTRWindow::onCloseMessageLogger()
{
    if(w_messageLogger)
        delete w_messageLogger;
    w_messageLogger = nullptr;
}

void CTRWindow::ShowPopUpInfo(LogMessage * message)
{
    if(!message)
        return;

    if(!w_messageLogger)
    {
        w_messageLogger = new W_MessageLogger(this);
        w_messageLogger->setGeometry(ui->Inventory->x()-width()/4-30, 0, width()/4, height()-20);
        connect(w_messageLogger, SIGNAL(sig_closeWindow()), this, SLOT(onCloseMessageLogger()));
        w_messageLogger->show();
    }

    w_messageLogger->log(message);
}

void CTRWindow::displayInterfaceTrading(Win_Interface_Trading * interface)
{
    mHero->freeze(true);
    if(w_trading)
        delete w_trading;
    w_trading = interface;
    connect(interface, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    w_trading->show();
}

void CTRWindow::displayMonsterData(Monster * monster)
{
    ShowPopUpInfo(ML_SHOW_MONSTER(monster));
}

void CTRWindow::displayCalamitySpawned()
{
    ShowPopUpInfo(ML_SHOW_CALAMITY());
}

void CTRWindow::GoToMonsterFight(Monster * monster)
{
    if(!w_fight)
    {
        mHero->freeze(true);

        // Close all interfaces
        if(w_trading)
        {
            Win_Chest * chest = dynamic_cast<Win_Chest*>(w_trading);
            if(chest)
                chest->closeChest();
            Win_HeroChest * heroChest = dynamic_cast<Win_HeroChest*>(w_trading);
            if(heroChest)
                heroChest->closeChest();
        }
        if(w_trading){
            delete w_trading;
            w_trading = nullptr;
        }
        hideInventaryGear();
        hideInventary();
        hideSkillWindow();
        if(w_messageLogger)
            w_messageLogger->setVisible(false);

        mSoundManager->setHeroAttack(mHero->getGear()->getWeapon());
        mSoundManager->startMusicFight(monster);
        w_fight = new Win_Fight(this, monster);
        connect(w_fight, SIGNAL(sig_closeWindow()), this, SLOT(hideFightWindow()));
        connect(w_fight, SIGNAL(sig_endFight(Character*)), this, SLOT(fightResult(Character*)));
        connect(w_fight, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
        w_fight->displayInterface();
        mMap->freeze(true);
    }
}

void CTRWindow::fightResult(Character* entityKilled)
{
    mHero->freeze(false);
    mSoundManager->endMusicFight();

    if(entityKilled)
    {
        Hero * hero = dynamic_cast<Hero*>(entityKilled);
        if(hero){
            // TODO : Game Over
            DEBUG("Le Hero a été tué - GAME OVER");
        }
        Monster* monster = dynamic_cast<Monster*>(entityKilled);
        if(monster){
            mSoundManager->playSound(monster->getSoundIndexFor(DIE));
            monster->killMonster();
            mMap->freeze(false);
            if(mHero->addExperience(monster->getExperience()))
            {
                ShowPopUpInfo(ML_SHOW_LEVEL_UP(mHero));
            }
            if(mHero->getLife().current < 30)
                mSoundManager->startMusicEvent(MUSICEVENT_CLOSE_FIGHT);
        }
        mHero->checkMapInteractions();
    }
    else
    {
        t_unfreezeMap->start(2000);
    }

    if(w_messageLogger)
            w_messageLogger->setVisible(true);
}

bool CTRWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->graphicsView && event->type() == QEvent::KeyPress)
    {
        event->ignore();
        return true;
    }
    if(event->type() == QEvent::GraphicsSceneWheel)
    {
        QMainWindow::eventFilter(target, event);
        event->accept();
        return true;
    }

#ifdef ENABLE_MOVEMENT_BY_MOUSE_CLIC
    if(mMap)
    {
        if(target == mMap->getScene())
        {
            if(event->type() == QEvent::GraphicsSceneMousePress)
            {
                const QGraphicsSceneMouseEvent* const me = static_cast<const QGraphicsSceneMouseEvent*>(event);
                if(me->button() == Qt::RightButton)
                {
                    if(heroIsSearching || mHero->isFreeze())
                        return QMainWindow::eventFilter(target, event);

                    const QPointF position(me->scenePos().x()-mHero->boundingRect().width()/2, me->scenePos().y()-mHero->boundingRect().height()/2);
                    if(mHero->contains(QPointF(position.x()+mHero->x(), position.y()-mHero->y())))
                    {
                        return QMainWindow::eventFilter(target, event);
                    }

                    gEnableMovementWithMouseClic = true;

                    qreal dy = position.y()-mHero->y(),
                        dx = position.x()-mHero->x();
                    qreal angle = qRadiansToDegrees(qAtan(dy/dx));
                    if(dy > 0){
                        if(dx < 0){     // BOT-LEFT
                            mHero->mMoveHandler.angle = 90 - abs(angle);
                        }else{          // BOT-RIGHT
                            mHero->mMoveHandler.angle =  270 + angle;
                        }
                    }else{
                        if(dx < 0){     // TOP-LEFT
                            mHero->mMoveHandler.angle =  90 + angle;
                        }else{          // TOP-RIGHT
                            mHero->mMoveHandler.angle =  180 + 90 + angle;
                        }
                    }
                    mHero->mMoveHandler.angle = (static_cast<int>(mHero->mMoveHandler.angle)+180)%360;
                    mHero->startMovingTo(static_cast<int>(position.x()), static_cast<int>(position.y()));
                }
            }
        }
    }
#endif
    return QMainWindow::eventFilter(target, event);
}

void CTRWindow::keyPressEvent(QKeyEvent *event)
{
    if( (event->key() == Qt::Key::Key_F) && !event->isAutoRepeat() && !mHero->isFreeze())
    {
        if(w_explorationLoading)
            delete w_explorationLoading;
        w_explorationLoading = new W_Animation_exploration(this);
        connect(w_explorationLoading, SIGNAL(sig_explorationCompleted()), this, SLOT(explorationCompleted()));
        w_explorationLoading->setGeometry(ui->graphicsView->x()+ui->graphicsView->width()/4, ui->graphicsView->y()+ui->graphicsView->height()-80, ui->graphicsView->width()/2, 50);
        w_explorationLoading->show();
        mHero->stopMoving();
        heroIsSearching = true;

        event->accept();
    }
    else if((event->key() == Qt::Key::Key_Z || event->key() == Qt::Key::Key_S || event->key() == Qt::Key::Key_Q || event->key() == Qt::Key::Key_D) && !mHero->isFreeze())
    {
        gEnableMovementWithMouseClic = false;

        if(event->isAutoRepeat())
        {
            event->ignore();
            return;
        }
        
        switch(event->key())
        {
        case Qt::Key::Key_Z:
            mHero->mMoveHandler.movementMask |= KEY_MASK_Z;
            break;

        case Qt::Key::Key_S:
            mHero->mMoveHandler.movementMask |= KEY_MASK_S;
            break;

        case Qt::Key::Key_Q:
            mHero->mMoveHandler.movementMask |= KEY_MASK_Q;
            break;

        case Qt::Key::Key_D:
            mHero->mMoveHandler.movementMask |= KEY_MASK_D;
            break;
        }

        if(mHero->mMoveHandler.movementMask)
        {
            mHero->mMoveHandler.t_move->start(TIMER_MOVE);
            mHero->t_movement->start();
            mHero->nextMovement();
        }

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void CTRWindow::keyReleaseEvent(QKeyEvent *event)
{
    if( (event->key() == Qt::Key::Key_F) && !event->isAutoRepeat())
    {
        heroIsSearching = false;
        if(w_explorationLoading)
            delete w_explorationLoading;
        w_explorationLoading = nullptr;

        event->accept();

    }else if(event->key() == Qt::Key::Key_Escape)
    {
        if(w_trading)
        {
            Win_Chest * chest = dynamic_cast<Win_Chest*>(w_trading);
            if(chest)
                chest->closeChest();
            Win_HeroChest * heroChest = dynamic_cast<Win_HeroChest*>(w_trading);
            if(heroChest)
                heroChest->closeChest();

            delete w_trading;
            w_trading = nullptr;
            mHero->freeze(false);
        }
        hideInventary();
        hideInventaryGear();
        hideSkillWindow();

        event->accept();
    }
    else if(event->key() == Qt::Key::Key_Z || event->key() == Qt::Key::Key_S || event->key() == Qt::Key::Key_Q || event->key() == Qt::Key::Key_D)
    {
        gEnableMovementWithMouseClic = false;

        if(event->isAutoRepeat())
        {
            event->ignore();
            return;
        }

        switch(event->key())
        {
        case Qt::Key::Key_Z:
            mHero->mMoveHandler.movementMask &= (~KEY_MASK_Z);
            break;

        case Qt::Key::Key_S:
            mHero->mMoveHandler.movementMask &= (~KEY_MASK_S);
            break;

        case Qt::Key::Key_Q:
            mHero->mMoveHandler.movementMask &= (~KEY_MASK_Q);
            break;

        case Qt::Key::Key_D:
            mHero->mMoveHandler.movementMask &= (~KEY_MASK_D);
            break;
        }

        if(!mHero->mMoveHandler.movementMask)
        {
            mHero->stopMoving();
        }

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void CTRWindow::explorationCompleted()
{
    if(w_explorationLoading)
        delete w_explorationLoading;
    w_explorationLoading = nullptr;
    heroIsSearching = false;

    QList<QGraphicsItem*> list = mHero->collidingItems();
    for(QGraphicsItem * item : qAsConst(list))
    {
        MapEvent * mapEvent = dynamic_cast<MapEvent*>(item);
        if(mapEvent)
        {
            if(mapEvent->eventIsActive())
            {
                BushEventCoin * bushEventCoin = dynamic_cast<BushEventCoin*>(item);
                if(bushEventCoin)
                {
                    BagCoin * coins = static_cast<BagCoin*>(bushEventCoin->takeItems().first());
                    ShowPopUpInfo(ML_SHOW_BAG_COIN(coins->getPrice()));
                    mHero->takeItem(coins);
                    return;
                }
                BushEventEquipment * bushEventEquipment = dynamic_cast<BushEventEquipment*>(item);
                if(bushEventEquipment)
                {
                    Item * itemTook = bushEventEquipment->takeItems().first();
                    Sword * sword = dynamic_cast<Sword*>(itemTook);
                    if(sword)
                    {
                        if(!mHero->takeItem(sword))
                        {
                            mMap->putItemThrownInMap(itemTook);
                            return;

                        }else
                        {

                            ShowPopUpInfo(ML_SHOW_ITEM_TOOK(itemTook));
                            return;
                        }
                    }
                    else
                    {
                        if(!mHero->takeItem(itemTook))
                        {
                            mMap->putItemThrownInMap(itemTook);
                            return;

                        }else
                        {
                            ShowPopUpInfo(ML_SHOW_ITEM_TOOK(itemTook));
                            return;
                        }
                    }

                }
                ChestEvent * chestEvent = dynamic_cast<ChestEvent*>(mapEvent);
                if(chestEvent)
                {
                    ChestBurried * chestBurried = dynamic_cast<ChestBurried*>(chestEvent);
                    if(chestBurried)
                    {
                        if(w_tool)
                        {
                            if(w_tool->getTool()->getIdentifier() == TOOL_SHOVEL)
                            {
                                chestEvent->revealChest();
                                ShowPopUpInfo(ML_SHOW_CHEST_DUG_UP());

                                if(mHero->getSkillList()[PassiveSkill::LuckFactor]->isUnlock())
                                {
                                    chestEvent->addExtraItems();
                                }

                            }else
                                ShowPopUpInfo(ML_SHOW_CHEST_BURRIED());
                        }else
                            ShowPopUpInfo(ML_SHOW_CHEST_BURRIED());
                    }
                    return;
                }
                OreSpot * oreSpot = dynamic_cast<OreSpot*>(mapEvent);
                if(oreSpot)
                {
                    if(w_tool)
                    {
                        if(w_tool->getTool()->getIdentifier() == TOOL_PICKAXE)
                        {
                            if(mHero->getSkillList()[PassiveSkill::Gemologist]->isUnlock())
                            {
                                // 15% chance +1 gemme
                                if(QRandomGenerator::global()->bounded(100) < 15)
                                {
                                    Item * gem;
                                    switch(QRandomGenerator::global()->bounded(3))
                                    {
                                        case 0:
                                            gem = new EmeraldOre();
                                            break;

                                        case 1:
                                            gem = new RubisOre();
                                            break;

                                        case 2:
                                            gem = new SaphirOre();
                                            break;
                                    }
                                    mHero->takeItem(gem);
                                    ShowPopUpInfo(ML_SHOW_ORE_TOOK(gem));
                                    continue;
                                }
                            }
                            Item * ore = oreSpot->getItems().first();
                            if(mHero->takeItem(oreSpot->takeItem(ore)))
                                ShowPopUpInfo(ML_SHOW_ORE_TOOK(ore));

                        }else
                            ShowPopUpInfo(ML_SHOW_ORE_SPOT(oreSpot));
                    }else
                        ShowPopUpInfo(ML_SHOW_ORE_SPOT(oreSpot));

                    continue;
                }
                FishingEvent * fishes = dynamic_cast<FishingEvent*>(mapEvent);
                if(fishes)
                {
                    if(w_tool)
                    {
                        if(w_tool->getTool()->getIdentifier() == TOOL_FISHINGROD)
                        {
                            if(mHero->getSkillList()[PassiveSkill::ConfirmedFisherman]->isUnlock())
                            {
                                // 25% chance +1 fish
                                if(QRandomGenerator::global()->bounded(100) < 25)
                                {
                                    Fish * fish;
                                    switch(QRandomGenerator::global()->bounded(4))
                                    {
                                        case 0 :
                                            fish = new RedFish();
                                            break;
                                        case 1 :
                                            fish = new BlueFish();
                                            break;
                                        case 2 :
                                            fish = new Yellowfish();
                                            break;
                                        case 3 :
                                            fish = new CommonFish();
                                            break;
                                    }
                                    mHero->takeItem(fish);
                                    ShowPopUpInfo(ML_SHOW_FISH_TOOK(fish));
                                    continue;
                                }
                            }

                            Item * fish = fishes->getItems().first();
                            if(mHero->takeItem(fishes->takeItem(fish)))
                                ShowPopUpInfo(ML_SHOW_FISH_TOOK(fish));
                        }else
                            ShowPopUpInfo(ML_SHOW_FISH_SPOT());
                    }else
                        ShowPopUpInfo(ML_SHOW_FISH_SPOT());

                    continue;
                }
            }
            else
            {
                OreSpot * oreSpot = dynamic_cast<OreSpot*>(mapEvent);
                if(oreSpot)
                {
                    ShowPopUpInfo(ML_SHOW_NO_MORE_TO_HARVEST());
                    continue;
                }
                FishingEvent * fishes = dynamic_cast<FishingEvent*>(mapEvent);
                if(fishes)
                {
                    ShowPopUpInfo(ML_SHOW_NO_MORE_TO_HARVEST());
                    continue;
                }
            }
        }
        Monster * monsterDead = dynamic_cast<Monster*>(item);
        if(monsterDead)
        {
            if(monsterDead->isDead() && monsterDead->isSkinned())
            {
                ShowPopUpInfo(ML_SHOW_MONSTER_SKINNED());
                continue;
            }

            if(w_tool)
            {
                if(monsterDead->isDead() && !monsterDead->isSkinned())
                {
                    if(w_tool->getTool()->getIdentifier() == TOOL_KNIFE)
                    {
                        if(mHero->getSkillList()[PassiveSkill::SkinningExpert]->isUnlock())
                        {
                            monsterDead->addExtraLoots();
                        }
                        QList<Item*> loots = monsterDead->skinMonster();
                        ShowPopUpInfo(ML_SHOW_LOOT_NUMBER(loots.size()));
                        for(Item * loot : qAsConst(loots))
                        {
                            mMap->heroThrowItemInMap(loot);
                        }
                        return;
                    }
                    else
                    {
                        ShowPopUpInfo(ML_SHOW_CARCASS());
                        continue;
                    }
                }
            }

            if(w_tool == nullptr && monsterDead->isDead() && !monsterDead->isSkinned())
            {
                ShowPopUpInfo(ML_SHOW_CARCASS());
                continue;
            }
        }
    }
}

void CTRWindow::on_Inventory_clicked()
{
    if(w_inventory)
    {
        hideInventary();
        return;
    }

    hideInventary();
    hideInventaryGear();
    hideSkillWindow();

    w_inventory = new Win_Inventory(this);
    connect(w_inventory, SIGNAL(sig_itemThrown(Item*)), this, SLOT(unuseToolIfThrown(Item*)));
    connect(w_inventory, SIGNAL(sig_itemThrown(Item*)), mMap, SLOT(putItemThrownInMap(Item*)));
    connect(w_inventory, SIGNAL(sig_closeWindow()), this, SLOT(hideInventary()));
    connect(w_inventory, SIGNAL(sig_useTool(Tool*)), this, SLOT(useTool(Tool*)));
    connect(w_inventory, SIGNAL(sig_useScroll(Scroll*)), this, SLOT(useScroll(Scroll*)));
    w_inventory->setGeometry((width()-w_inventory->width())/2, (height()-w_inventory->height())/2, w_inventory->width(), w_inventory->height());
    w_inventory->diplayInventory();
}

void CTRWindow::hideInventary()
{
    if(w_inventory!=nullptr)
        delete w_inventory;
    w_inventory = nullptr;
    setButtonsEnable(true);
}

void CTRWindow::on_Gear_clicked()
{
    if(w_gear)
    {
        hideInventaryGear();
        return;
    }

    hideInventary();
    hideInventaryGear();
    hideSkillWindow();

    w_gear = new Win_Gear(this);
    connect(w_gear, SIGNAL(sig_closeWindow()), this, SLOT(hideInventaryGear()));
    connect(w_gear, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    connect(w_gear, SIGNAL(sig_itemThrown(Item*)), mMap, SLOT(putItemThrownInMap(Item*)));
    w_gear->setGeometry(width()-w_gear->width()-100, 0, w_gear->width()-100, w_gear->height());
    w_gear->diplayWindow();
}

void CTRWindow::hideInventaryGear()
{
    if(w_gear!=nullptr)
        delete w_gear;
    w_gear = nullptr;
    setButtonsEnable(true);
}

void CTRWindow::on_Map_clicked()
{
    if(!mHero)
        return;

    if(!mHero->getAdventurerMap().unlocked)
        return;

    hideInventary();
    hideInventaryGear();
    hideSkillWindow();

    if(w_map)
    {
        unuseMapTool();
        return;
    }  

    useTool(&mHero->getAdventurerMap().map);
}

void CTRWindow::hideFightWindow()
{
    if(w_fight!=nullptr)
        delete w_fight;
    w_fight = nullptr;
}

void CTRWindow::hideSkillWindow()
{
    if(w_skill)
        delete w_skill;
    w_skill = nullptr;
    setButtonsEnable(true);
}

void CTRWindow::showReplenish(QObject * shop)
{
    Blacksmith * blacksmith = dynamic_cast<Blacksmith*>(shop);
    if(blacksmith)
    {
        ShowPopUpInfo(LM_SHOW_BLACKSMITH_RESPLENISH());
    }

    Merchant * merchant = dynamic_cast<Merchant*>(shop);
    if(merchant)
    {
        ShowPopUpInfo(LM_SHOW_MERCHANT_RESPLENISH());
    }

    Alchemist * alchemist = dynamic_cast<Alchemist*>(shop);
    if(alchemist)
    {
        ShowPopUpInfo(LM_SHOW_ALCHEMIST_RESPLENISH());
    }
}

void CTRWindow::useTool(Tool * tool)
{
    // Map Scroll specific
    MapScroll * mapScroll = dynamic_cast<MapScroll*>(tool);
    if(mapScroll)
    {
        if(w_map)
        {
            w_map->hideTool();
            delete w_map;
        }
        w_map = nullptr;
        
        w_map = new W_UseToolMap(this, tool);

        connect(w_map->getExitButton(), &QPushButton::clicked, this, &CTRWindow::unuseMapTool);
        w_map->setGeometry(20, height() - w_map->height() - 20, w_map->width(), w_map->height());
        w_map->displayTool();

        return;
    }

    // Other tools
    if(w_tool)
    {
        w_tool->hideTool();
        delete w_tool;
    }
    w_tool = nullptr;

    Compass * compass = dynamic_cast<Compass*>(tool);
    if(compass)
        w_tool = new W_UseToolCompass(this, tool);

    if(!w_tool)
        w_tool = new W_UseTool(this, tool);

    connect(w_tool->getExitButton(), &QPushButton::clicked, this, &CTRWindow::unuseTool);
    w_tool->setGeometry(ui->Inventory->x()-w_tool->width()-30, 30, w_tool->width(), w_tool->height());
    w_tool->displayTool();
}

void CTRWindow::useScroll(Scroll* scroll)
{
    mHero->setPos(mMap->getVillage()->getHeroHouse()->getHouse()->x(), mMap->getVillage()->getHeroHouse()->getHouse()->y()+350);
    ui->graphicsView->centerOn(mHero);
    mSoundManager->playSound(SOUND_TELEPORT);

    mHero->useScroll(scroll);
}

void CTRWindow::unuseToolIfThrown(Item * item)
{
    if(!item)
        return;

    if(!w_tool)
        return;

    Tool * tool = dynamic_cast<Tool*>(item);
    if(tool)
        if(w_tool->getTool() == tool)
            unuseTool();
}

void CTRWindow::unuseTool()
{
    if(w_tool)
    {
        w_tool->hideTool();
        delete w_tool;
    }
    w_tool = nullptr;
}

void CTRWindow::unuseMapTool()
{
    if(w_map)
    {
        w_map->hideTool();
        delete w_map;
    }
    w_map = nullptr;
}

void CTRWindow::setButtonsEnable(bool toggle)
{
    ui->Inventory->setEnabled(toggle);
    ui->Gear->setEnabled(toggle);
    ui->Skills->setEnabled(toggle);
    ui->Map->setEnabled(toggle ? (mHero->getAdventurerMap().unlocked ? toggle : false) : toggle);
}

void CTRWindow::showBagFull()
{
    ShowPopUpInfo(SHOW_BAG_FULL(mHero->getBag()));
}

void CTRWindow::showItemOnGround(Item * item)
{
    ShowPopUpInfo(ML_SHOW_ITEM_ON_GROUND(item));
}

void CTRWindow::showItemPicked(Item * item)
{
    BagCoin * coins = dynamic_cast<BagCoin*>(item);
    if(coins)
    {
        ShowPopUpInfo(ML_SHOW_BAG_COIN(coins->getPrice()));
        item->deleteLater();
        item = nullptr;
    }
    else
    {
        ShowPopUpInfo(ML_SHOW_ITEM_TOOK(item));
    }
}

void CTRWindow::openInterface(QGraphicsItem * item)
{
    ChestEvent * chest = dynamic_cast<ChestEvent*>(item);
    if(chest){
        displayInterfaceTrading(new Win_Chest(this, chest));
    }
    BlacksmithHouse * blacksmith = dynamic_cast<BlacksmithHouse*>(item);
    if(blacksmith){
        Win_BlackSmith * interface = new Win_BlackSmith(this);
        connect(interface, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
        connect(interface, SIGNAL(sig_itemThrown(Item*)), mMap, SLOT(putItemThrownInMap(Item*)));
        displayInterfaceTrading(interface);
        return;
    }
    MerchantHouse * merchant = dynamic_cast<MerchantHouse*>(item);
    if(merchant){
        displayInterfaceTrading(new Win_Merchant(this));
        return;
    }
    AlchemistHouse * alchemist = dynamic_cast<AlchemistHouse*>(item);
    if(alchemist){
        displayInterfaceTrading(new Win_Alchemist(this));
        return;
    }
    Taverne * heroHouse = dynamic_cast<Taverne*>(item);
    if(heroHouse){
        mHero->freeze(true);
        unuseMapTool();
        mHero->getAdventurerMap().map.init();
        if(w_night)
            delete w_night;
        w_night = new W_Animation_Night(this);
        w_night->setGeometry(0,0,width(),height());
        connect(w_night, SIGNAL(sig_playMusic(int)), mSoundManager, SLOT(startMusicEvent(int)));
        connect(mMap, SIGNAL(sig_loadingGameUpdate(quint8)), w_night, SLOT(onUpdateSleep(quint8)));
        connect(mMap, SIGNAL(sig_generationMapComplete()), w_night, SLOT(onStartNewDay()));
        loadingStep = 0;
        QApplication::processEvents();
        mMap->reGenerateMap();
        mHero->freeze(false);
        return;
    }
    HeroChest * heroChest = dynamic_cast<HeroChest*>(item);
    if(heroChest){
        mHero->freeze(true);
        Win_HeroChest * w_chest = new Win_HeroChest(this);
        connect(w_chest, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
        w_chest->setGeometry((width()-w_chest->width())/2, (height()-w_chest->height())/2, w_chest->width(), w_chest->height());
        w_chest->show();
        w_trading = w_chest;
        return;
    }
    AltarBuilding * altar = dynamic_cast<AltarBuilding*>(item);
    if(altar)
    {
        displayInterfaceTrading(new Win_Altar(this));
        return;
    }
}

void CTRWindow::showPNJinfo(QGraphicsItem * interaction)
{
    House * house = dynamic_cast<House*>(interaction);
    if(house){
        ShowPopUpInfo(ML_SHOW_PNG_INFO(house));
    }
}

void CTRWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/graphicItems/screenBackground.png"));
}

void CTRWindow::on_Skills_clicked()
{
    if(w_skill)
    {
        hideSkillWindow();
        return;
    }

    hideInventary();
    hideInventaryGear();
    hideSkillWindow();

    w_skill = new Win_Skills(this);
    connect(w_skill, SIGNAL(sig_closeWindow()), this, SLOT(hideSkillWindow()));
    //connect(w_skill, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    w_skill->setGeometry((width() - w_skill->width())/2, 0, w_skill->width(), w_skill->height());
    w_skill->diplayWindow();
}

void CTRWindow::onPeriodicalEvents()
{
    static quint8 fiveSeconds = 1;

    if(mHero->getStamina().current < mHero->getStamina().maximum)
    {
        mHero->setStamina(mHero->getStamina().current + 10);
    }
    
    if(fiveSeconds >= 5)
    {
        if(mHero->getSkillList()[PassiveSkill::LifeCollector]->isUnlock())
        {
            mHero->setLife(mHero->getLife().current+1);
        }
        if(mHero->getSkillList()[PassiveSkill::ManaCollector]->isUnlock())
        {
            mHero->setMana(mHero->getMana().current+1);
        }
        fiveSeconds = 1;
    }

    fiveSeconds++;
}

void CTRWindow::onQuitGame()
{
    if(w_menu)
        delete w_menu;
    w_menu = nullptr;
    close();
}

void CTRWindow::on_buttonQuit_clicked()
{
    mHero->freeze(true);
    mMap->freeze(true);

    mHero->stopMoving();

    ConfirmationDialog dialog(this);
    if(dialog.exec() == QDialog::Rejected)
    {
        mHero->freeze(false);
        mMap->freeze(false);
        return;
    }  

    /* Copy map chest to save */
    HeroChest * chest = new HeroChest();
    for(Item * item : mMap->getVillage()->getHeroHouse()->getChest()->getItems())
    {
        chest->addItem(item);
    }
    mCurrentSave->setChest(chest);

    /* Avoid instances destruction with scene destruction */
    mMap->getScene()->removeItem(mHero);
    mMap->getVillage()->removeFromScene(mMap->getScene());

    /* Save current game */
    QFile file(QString(QDir::currentPath()+"/"+FILE_SAVE+"/%1").arg(mCurrentSave->getName()));
    file.open(QIODevice::WriteOnly);
    QDataStream stream(&file);
    stream << *mCurrentSave;
    file.close();

    w_menu->enableButtons(true);
    w_menu->show();

    closeGame();

    hide();
}

void CTRWindow::showQuickToolDrawer(Tool * tool)
{
    if(w_quickItemDrawer)
        delete w_quickItemDrawer;

    if(tool)
    {
        w_quickItemDrawer = new W_QuickToolDrawer(this, tool);
        w_quickItemDrawer->setGeometry(ui->Inventory->x()-w_quickItemDrawer->width()-30, 300, w_quickItemDrawer->width(), w_quickItemDrawer->height());
        w_quickItemDrawer->show();
        connect(w_quickItemDrawer, SIGNAL(sig_useTool(Tool*)), this, SLOT(useTool(Tool*)));
    }
}

void CTRWindow::hideQuickToolDrawer()
{
    if(w_quickItemDrawer)
    {
        delete w_quickItemDrawer;
        w_quickItemDrawer = nullptr;
    }
}
