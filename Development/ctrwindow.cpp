#include "ctrwindow.h"
#include "ui_ctrwindow.h"
#include "win_loadinggamescreen.h"
#include <QDir>

quint8 loadingStep = 0;
extern Hero * gSelectedHero;

CTRWindow::CTRWindow(QWidget *parent) :
    QMainWindow(parent),
    mMap(nullptr),
    mHero(nullptr),
    mSoundManager(nullptr),
    mGameItems(nullptr),
    heroIsSearching(false),
    t_unfreezeMap(nullptr),
    t_PeriodicalEvents(nullptr),
    mCurrentSave(nullptr),
    mLoadingAvancement(0),
    w_menu(nullptr),
    w_heroStats(nullptr),
    w_panel(nullptr),
    w_explorationLoading(nullptr),
    w_inventory(nullptr),
    w_gear(nullptr),
    w_fight(nullptr),
    w_trading(nullptr),
    w_skill(nullptr),
    w_tool(nullptr),
    ui(new Ui::CTRWindow)
{
    ui->setupUi(this);
    setButtonsEnable(true);

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
}

CTRWindow::~CTRWindow()
{
    if(mLoadingThread.isRunning())
    {
        mLoadingThread.quit();
        mLoadingThread.wait();
    }
    if(mGameItems)
        delete mGameItems;
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
    mGameItems = new ItemGenerator();
    qDebug() << "GENERATED : ItemGenerator";
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // MAP CREATION
    mMap = new Map(this, ui->graphicsView, mGameItems);
    t_unfreezeMap = new QTimer(this);
    t_unfreezeMap->setSingleShot(true);
    connect(mMap, SIGNAL(sig_monsterEncountered(Monster*)), this, SLOT(GoToMonsterFight(Monster*)));
    connect(t_unfreezeMap, SIGNAL(timeout()), mMap, SLOT(unfreezeMap()));
    connect(mMap, SIGNAL(sig_replenish(QObject*)), this, SLOT(showReplenish(QObject*)));
    connect(mMap, SIGNAL(sig_showPNJinfo(QGraphicsItem*)), this, SLOT(showPNJinfo(QGraphicsItem*)));
    connect(mMap, SIGNAL(sig_openInterface(QGraphicsItem*)), this, SLOT(openInterface(QGraphicsItem*)));
    connect(mMap, SIGNAL(sig_showItemInfo(Item*)), this, SLOT(showItemOnGround(Item*)));
    connect(mMap, SIGNAL(sig_itemPickedInMap(Item*)), this, SLOT(showItemPicked(Item*)));
    connect(mMap, SIGNAL(sig_displayMonsterData(Monster*)), this, SLOT(displayMonsterData(Monster*)));
    connect(mMap, SIGNAL(sig_calamitySpawned()), this, SLOT(displayCalamitySpawned()));
    ui->graphicsView->setScene(mMap->getScene());
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qDebug() << "GENERATED : Map";

    // SOUND EFFECTS
    mSoundManager = new SoundManager(this);
    connect(mMap, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    connect(mMap, SIGNAL(sig_heroEnterVillage()), mSoundManager, SLOT(heroEnterVillage()));
    connect(mMap, SIGNAL(sig_heroLeftVillage()), mSoundManager, SLOT(heroLeftVillage()));
    connect(mMap, SIGNAL(sig_heroEnterEvent(int)), mSoundManager, SLOT(startMusicEvent(int)));
    connect(mMap, SIGNAL(sig_heroLeaveEvent(int)), mSoundManager, SLOT(endMusicEvent(int)));
    qDebug() << "GENERATED : SoundManager";
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    // HERO CREATION
    mHero = mCurrentSave->getHero();
    mMap->setHero(mHero);
    mHero->setPos(mHero->getLocation() != QPointF(0,0) ? mHero->getLocation() : QPointF(mMap->getVillage()->pos().x()+500, mMap->getVillage()->pos().y()+800));
    connect(mHero, SIGNAL(sig_bagFull()), this, SLOT(showBagFull()));
    connect(mHero, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    qDebug() << "GENERATED : Hero";
    emit sig_loadingGameUpdate(UPDATE_STEP(loadingStep));

    for(Monster * m : mMap->getMonsters())
    {
        connect(m, SIGNAL(sig_showMonsterData(Monster*)), this, SLOT(displayMonsterData(Monster*)));
    }

    // HERO INTERFACE
    w_heroStats = new W_Interface_HeroStats(this, mHero);
    w_heroStats->setGeometry(50,50,w_heroStats->width(), w_heroStats->height());
    w_heroStats->show();
    qDebug() << "GENERATED : Hero interface";
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

    mLoadingThread.quit();
    mLoadingThread.wait();

    w_loadingScreen->hide();
    showFullScreen();
}

void CTRWindow::closeGame()
{
    if(mMap)
        delete mMap;
    mMap = nullptr;
    qDebug() << "DELETED : Map";

    if(mGameItems)
        delete mGameItems;
    mGameItems = nullptr;
    qDebug() << "DELETED : Game items";

    if(mSoundManager)
        delete mSoundManager;
    mSoundManager = nullptr;
    qDebug() << "DELETED : Sound manager";

    if(w_heroStats)
        delete w_heroStats;
    w_heroStats = nullptr;
    qDebug() << "DELETED : Hero stats";

    if(t_unfreezeMap)
        delete t_unfreezeMap;
    if(t_PeriodicalEvents)
        delete t_PeriodicalEvents;
    t_unfreezeMap = nullptr;
    t_PeriodicalEvents = nullptr;
    qDebug() << "DELETED : Timers";
}

void CTRWindow::scrollSceneView()
{
    ui->graphicsView->centerOn(mHero);
}

void CTRWindow::removeCurrentPanel()
{
    if(w_panel != nullptr)
        delete w_panel;
    w_panel = nullptr;
}

void CTRWindow::displayPanel(W_DialogPannel * pannel)
{
    removeCurrentPanel();
    connect(pannel, SIGNAL(sig_removeWidget()), this, SLOT(removeCurrentPanel()));
    w_panel = pannel;
    pannel->setGeometry(ui->Inventory->x()-pannel->width()-30,height()-pannel->height()-30,pannel->width(),pannel->height());
    pannel->showWidget();
}

void CTRWindow::displayInterfaceTrading(Win_Interface_Trading * interface)
{
    if(!w_trading)
        delete w_trading;
    w_trading = interface;
    connect(interface, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    w_trading->show();
}

void CTRWindow::displayMonsterData(Monster * monster)
{
    removeCurrentPanel();
    displayPanel( new W_ShowMonsterData(this, monster));
}

void CTRWindow::displayCalamitySpawned()
{
    removeCurrentPanel();
    displayPanel(new W_ShowCalamitySpawned(this));
}

void CTRWindow::GoToMonsterFight(Monster * monster)
{
    if(!w_fight){
        mHero->freeze(true);
        hideInventaryGear();
        hideInventary();
        if(w_trading){
            delete w_trading;
            w_trading = nullptr;
        }
        mSoundManager->startMusicFight(monster);
        w_fight = new Win_Fight(this, mHero, monster);
        connect(w_fight, SIGNAL(sig_closeWindow()), this, SLOT(hideFightWindow()));
        connect(w_fight, SIGNAL(sig_endFight(Character*)), this, SLOT(fightResult(Character*)));
        connect(w_fight, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
        w_fight->displayInterface();
        mMap->freezeMap();
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
            qDebug() << "Le Hero a été tué - GAME OVER";
        }
        Monster* monster = dynamic_cast<Monster*>(entityKilled);
        if(monster){
            mSoundManager->playSound(monster->getSoundIndexFor(DIE));
            monster->killMonster();
            mMap->unfreezeMap();
            if(mHero->addExperience(monster->getExperience()))
            {
                displayPanel(new W_ShowLevelUp(this, mHero->getExperience().level));
            }
            if(mHero->getLife().curLife < 30)
                mSoundManager->startMusicEvent(MUSICEVENT_CLOSE_FIGHT);
        }
    }else
    {
        t_unfreezeMap->start(2000);
    }
}

bool CTRWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->graphicsView && event->type() == QEvent::KeyPress)
    {
        event->ignore();
        return true;
    }
    if(mMap)
    {
        if(target == mMap->getScene())
        {
            if (event->type() == QEvent::GraphicsSceneMousePress)
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
    return QMainWindow::eventFilter(target, event);
}

void CTRWindow::keyPressEvent(QKeyEvent *event)
{
    if( (event->key() == Qt::Key::Key_F) && !event->isAutoRepeat())
    {
        if(w_explorationLoading != nullptr)
            delete w_explorationLoading;
        w_explorationLoading = new W_Animation_exploration(this);
        connect(w_explorationLoading, SIGNAL(sig_explorationCompleted()), this, SLOT(explorationCompleted()));
        w_explorationLoading->setGeometry(20, ui->graphicsView->y()+ui->graphicsView->height()-20-260, 260,260);
        w_explorationLoading->show();
        mHero->stopMoving();
        heroIsSearching = true;

        event->accept();
    }else{
        event->ignore();
    }
}

void CTRWindow::keyReleaseEvent(QKeyEvent *event)
{
    if( (event->key() == Qt::Key::Key_F) && !event->isAutoRepeat())
    {
        heroIsSearching = false;
        if(w_explorationLoading != nullptr)
            delete w_explorationLoading;
        w_explorationLoading = nullptr;

        event->accept();

    }else if(event->key() == Qt::Key::Key_Escape)
    {
        if(w_trading != nullptr)
        {
            Win_Chest * chest = dynamic_cast<Win_Chest*>(w_trading);
            if(chest)
                chest->closeChest();
            Win_HeroChest * heroChest = dynamic_cast<Win_HeroChest*>(w_trading);
            if(heroChest)
                heroChest->closeChest();

            delete w_trading;
            w_trading = nullptr;
        }
        if(w_inventory != nullptr)
        {
            hideInventary();
        }
        if(w_gear != nullptr)
        {
            hideInventaryGear();
        }

    }else{
        event->ignore();
    }
}

void CTRWindow::explorationCompleted()
{
    if(w_explorationLoading != nullptr)
        delete w_explorationLoading;
    w_explorationLoading = nullptr;
    heroIsSearching = false;

    QList<QGraphicsItem*> list = mHero->collidingItems();
    for(QGraphicsItem * item : list)
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
                    W_ShowItemTook * panel = new W_ShowItemTook(this, coins);
                    displayPanel(panel);
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
                            W_ShowEquipmentTook * panel = new W_ShowEquipmentTook(this, itemTook);
                            displayPanel(panel);
                            return;
                        }
                    }else
                    {
                        if(!mHero->takeItem(itemTook))
                        {
                            mMap->putItemThrownInMap(itemTook);
                            return;

                        }else
                        {
                            W_ShowEquipmentTook * panel = new W_ShowEquipmentTook(this, itemTook);
                            displayPanel(panel);
                            return;
                        }
                    }

                }
                ChestEvent * chestEvent = dynamic_cast<ChestEvent*>(mapEvent);
                if(chestEvent)
                {
                    if(chestEvent->isRevealed())
                    {
                        if(!chestEvent->isOpen())
                        {
                            mSoundManager->playSound(SOUND_OPENCHEST);
                            Win_Chest * interfaceChest = new Win_Chest(this, mHero, chestEvent);
                            connect(interfaceChest, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
                            interfaceChest->setGeometry(0,0,interfaceChest->width(), interfaceChest->height());
                            interfaceChest->show();
                        }
                    }else
                    {
                        ChestBurried * chestBurried = dynamic_cast<ChestBurried*>(chestEvent);
                        if(chestBurried)
                        {
                            if(w_tool != nullptr)
                            {
                                if(w_tool->getTool()->getIdentifier() == TOOL_SHOVEL)
                                {
                                    chestEvent->revealChest();
                                    displayPanel(new W_ShowChestFound(this));

                                    if(mHero->getSkillList()[PassiveSkill::LuckFactor]->isUnlock())
                                    {
                                        chestEvent->addExtraItems();
                                    }

                                }else
                                    displayPanel(new W_ShowChestBurried(this));
                            }else
                                displayPanel(new W_ShowChestBurried(this));
                        }else
                        {
                            chestEvent->revealChest();
                            displayPanel(new W_ShowChestFound(this));
                        }
                    }
                    return;
                }
                OreSpot * oreSpot = dynamic_cast<OreSpot*>(mapEvent);
                if(oreSpot)
                {
                    if(oreSpot->getItems().isEmpty())
                        continue;

                    if(w_tool != nullptr)
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
                                    displayPanel(new W_ShowOreTook(this, gem));
                                    continue;
                                }
                            }
                            Item * ore = oreSpot->getItems().first();
                            if(mHero->takeItem(oreSpot->takeItem(ore)))
                                displayPanel(new W_ShowOreTook(this, ore));

                        }else
                            displayPanel(new W_ShowOreSpot(this, oreSpot));
                    }else
                        displayPanel(new W_ShowOreSpot(this, oreSpot));

                    continue;
                }
                FishingEvent * fishes = dynamic_cast<FishingEvent*>(mapEvent);
                if(fishes){

                    if(fishes->getItems().isEmpty())
                        continue;

                    if(w_tool != nullptr)
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
                                    displayPanel(new W_ShowFishTook(this, dynamic_cast<Fish*>(fish)));
                                    continue;
                                }
                            }

                            Item * fish = fishes->getItems().first();
                            if(mHero->takeItem(fishes->takeItem(fish)))
                                displayPanel(new W_ShowFishTook(this, dynamic_cast<Fish*>(fish)));
                            else
                                displayPanel(new W_ShowFishSpot(this));
                        }
                    }else
                        displayPanel(new W_ShowFishSpot(this));

                    continue;
                }
            }
        }
        Monster * monsterDead = dynamic_cast<Monster*>(item);
        if(monsterDead)
        {
            if(w_tool != nullptr)
            {
                if(monsterDead->isDead() && !monsterDead->isSkinned() && w_tool->getTool()->getIdentifier() == TOOL_KNIFE)
                {
                    if(mHero->getSkillList()[PassiveSkill::SkinningExpert]->isUnlock())
                    {
                        monsterDead->addExtraLoots();
                    }
                    QList<Item*> loots = monsterDead->skinMonster();
                    for(Item * loot : loots)
                    {
                        mMap->heroThrowItemInMap(loot);
                    }
                    return;
                }
                else
                    displayPanel(new W_ShowCarcass(this, monsterDead));

                continue;
            }

            if(w_tool == nullptr && monsterDead->isDead() && !monsterDead->isSkinned())
                displayPanel(new W_ShowCarcass(this, monsterDead));

            continue;
        }
    }
}

void CTRWindow::on_Inventory_clicked()
{
    setButtonsEnable(false);
    w_inventory = new Win_Inventory(this, mHero);
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
    setButtonsEnable(false);
    w_gear = new Win_Gear(this, mHero);
    connect(w_gear, SIGNAL(sig_closeWindow()), this, SLOT(hideInventaryGear()));
    connect(w_gear, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    w_gear->setGeometry(width()-w_gear->width(), 0, w_gear->width(), w_gear->height());
    w_gear->diplayWindow();
}

void CTRWindow::hideInventaryGear()
{
    if(w_gear!=nullptr)
        delete w_gear;
    w_gear = nullptr;
    setButtonsEnable(true);
}

void CTRWindow::hideFightWindow()
{
    if(w_fight!=nullptr)
        delete w_fight;
    w_fight = nullptr;
}

void CTRWindow::hideSkillWindow()
{
    if(w_skill != nullptr)
        delete w_skill;
    w_skill = nullptr;
    setButtonsEnable(true);
}

void CTRWindow::showReplenish(QObject * shop)
{
    displayPanel(new W_ShowReplenish(this, shop));
}

void CTRWindow::useTool(Tool * tool)
{
    if(w_tool != nullptr)
        delete w_tool;
    w_tool = nullptr;

    w_tool = new W_UseTool(this, tool, mHero, mMap->getVillage());
    connect(w_tool->getExitButton(), SIGNAL(clicked()), this, SLOT(unuseTool()));
    w_tool->setGeometry(ui->Inventory->x()-w_tool->width()-30,30,w_tool->width(),w_tool->height());
    w_tool->displayTool();
}
// Téléportation
void CTRWindow::useScroll(Scroll* scroll)
{
    Scroll_X * scroll_X = dynamic_cast<Scroll_X*>(scroll);
    if(scroll_X)
    {
        mHero->setPos(mMap->getVillage()->getHeroHouse()->getHouse()->x(), mMap->getVillage()->getHeroHouse()->getHouse()->y()+350);
        ui->graphicsView->centerOn(mHero);
        // TODO : add sound

        QTimer::singleShot(1000, this, [&]{mHero->useScroll(scroll);});
    }
}

void CTRWindow::unuseTool()
{
    if(w_tool != nullptr)
        delete w_tool;
    w_tool = nullptr;
}

void CTRWindow::indicateBagFull()
{
    displayPanel(new W_ShowBagFull(this));
}

void CTRWindow::setButtonsEnable(bool toggle)
{
    ui->Inventory->setEnabled(toggle);
    ui->Gear->setEnabled(toggle);
    ui->Skills->setEnabled(toggle);
}

void CTRWindow::on_buttonHelp_clicked()
{
    HelpDialog * helpDialog = new HelpDialog(this);
    helpDialog->setGeometry((width()-helpDialog->width())/2,0,helpDialog->width(),helpDialog->height());
    helpDialog->displayWindow();
}

void CTRWindow::showBagFull()
{
    displayPanel(new W_ShowBagFull(this));
}

void CTRWindow::showItemOnGround(Item * item)
{
    displayPanel(new W_ShowItemOnGround(this, item));
}

void CTRWindow::showItemPicked(Item * item)
{
    displayPanel(new W_ShowItemTook(this, item));
}

void CTRWindow::openInterface(QGraphicsItem * item)
{
    ChestEvent * chest = dynamic_cast<ChestEvent*>(item);
    if(chest){
        displayInterfaceTrading(new Win_Chest(this, mHero, chest));
    }
    BlacksmithHouse * blacksmith = dynamic_cast<BlacksmithHouse*>(item);
    if(blacksmith){
        displayInterfaceTrading(new Win_BlackSmith(this, mHero, mMap->getVillage()->getBlacksmith()));
        return;
    }
    MerchantHouse * merchant = dynamic_cast<MerchantHouse*>(item);
    if(merchant){
        displayInterfaceTrading(new Win_Merchant(this, mHero, mMap->getVillage()->getMerchant()));
        return;
    }
    AlchemistHouse * alchemist = dynamic_cast<AlchemistHouse*>(item);
    if(alchemist){
        displayInterfaceTrading(new Win_Alchemist(this, mHero, mMap->getVillage()->getAlchemist()));
        return;
    }
    MainHouse * heroHouse = dynamic_cast<MainHouse*>(item);
    if(heroHouse){
        W_Animation_Night * w_night = new W_Animation_Night(this, mMap);
        w_night->setGeometry(0,0,width(),height());
        connect(w_night, SIGNAL(sig_playMusic(int)), mSoundManager, SLOT(startMusicEvent(int)));
        return;
    }
    HeroChest * heroChest = dynamic_cast<HeroChest*>(item);
    if(heroChest){
        Win_HeroChest * w_chest = new Win_HeroChest(this, mHero, heroChest);
        connect(w_chest, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
        w_chest->setGeometry((width()-w_chest->width())/2, (height()-w_chest->height())/2, w_chest->width(), w_chest->height());
        w_chest->show();
        w_trading = w_chest;
        return;
    }
}

void CTRWindow::showPNJinfo(QGraphicsItem * interaction)
{

    displayPanel(new W_ShowVilageInfo(this, interaction));
}

void CTRWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/graphicItems/screenBackground.png"));
}

void CTRWindow::on_Skills_clicked()
{
    setButtonsEnable(false);
    w_skill = new Win_Skills(this, mHero);
    connect(w_skill, SIGNAL(sig_closeWindow()), this, SLOT(hideSkillWindow()));
    //connect(w_skill, SIGNAL(sig_playSound(int)), mSoundManager, SLOT(playSound(int)));
    w_skill->setGeometry(width() - w_skill->width(), 0, w_skill->width(), w_skill->height());
    w_skill->diplayWindow();
}

void CTRWindow::onPeriodicalEvents()
{
    if(mHero->getSkillList()[PassiveSkill::LifeCollector]->isUnlock())
    {
        mHero->setLife(mHero->getLife().curLife+1);
    }
    if(mHero->getSkillList()[PassiveSkill::ManaCollector]->isUnlock())
    {
        mHero->setMana(mHero->getMana().curMana+1);
    }
}

void CTRWindow::onQuitGame()
{
    if(w_menu)
        delete w_menu;
    w_menu = nullptr;
    close();
}

void CTRWindow::on_buttonSaveGame_clicked()
{
    /* Copy map chest to save */
    HeroChest * chest = new HeroChest();
    for(Item * item : mMap->getVillage()->getHeroHouse()->getChest()->getItems())
    {
        chest->addItem(item);
    }
    mCurrentSave->setChest(chest);

    /* Avoid Hero instance destruction */
    mMap->getScene()->removeItem(mHero);

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
