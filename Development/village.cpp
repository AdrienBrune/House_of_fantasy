#include "village.h"
#include "item.h"


House::House():
    mBounding(QRect()),
    mCollisionShape(QPainterPath()),
    mHover(false),
    mInformation("")
{
    setAcceptHoverEvents(true);
    t_delayHover = new QTimer(this);
    t_delayHover->setSingleShot(true);
    connect(t_delayHover, SIGNAL(timeout()), this, SLOT(showBuildingInfo()));
}

House::~House()
{

}

void House::showBuildingInfo()
{
    emit sig_villageShowInfo(this);
}

QString House::getInformation()
{
    return mInformation;
}

bool House::isObstacle()
{
    return true;
}

void House::setShape()
{
    QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    tmp->setPixmap(mImage.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
    mShape = tmp->shape();
    delete tmp;
}

QPainterPath House::shape() const
{
    return mCollisionShape;
}

QRectF House::boundingRect() const
{
    return mBounding;
}

void House::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(t_delayHover->isActive())
            t_delayHover->stop();
        sig_villageInteraction(this);
        event->accept();
    }else{
        event->ignore();
    }
}

void House::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverEnter == QGraphicsSceneHoverEvent::HoverEnter)
    {
        mHover = true;
        update();
        event->accept();
        t_delayHover->start(TIME_TO_SHOW_ITEM);
    }
}

void House::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverLeave == QGraphicsSceneHoverEvent::HoverLeave)
    {
        if(t_delayHover->isActive())
            t_delayHover->stop();
        mHover = false;
        update();
        event->accept();
    }
}

void House::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, 0, 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));

    if(mHover){
        painter->setPen(QPen(QBrush(BORDERS_COLOR), 3));
        painter->drawPath(mShape);
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}









BlacksmithHouse::BlacksmithHouse():
    House ()
{
    initGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Forgeron\nLieux pour forger des équipements";
}

BlacksmithHouse::~BlacksmithHouse()
{

}

void BlacksmithHouse::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/blacksmith_house.png");

    mBounding = QRect(0,0,400,400);

    House::setShape();
    QPolygon polygon;
    static const int points[] = {
        30, 40,
        200, 5,
        205, 60,
        250, 50,
        280, 150,
        350,170,
        375, 220,
        315, 260,
        200, 220,
        130, 240,
        5, 200
    };
    polygon.setPoints(11, points);
    mCollisionShape.addPolygon(QPolygon(polygon));

    House::setShape();

    update();
}


Blacksmith::Blacksmith():
    QObject ()
{
    mHouse = new BlacksmithHouse();
    QList<EquipmentToForge::Loot> list_1, list_2, list_3;
    list_1 << EquipmentToForge::Loot{new WolfFang(),10} << EquipmentToForge::Loot{new BearClaw(),5} << EquipmentToForge::Loot{new IronOre(),10};
    list_2 << EquipmentToForge::Loot{new WolfFang(),4} << EquipmentToForge::Loot{new WolfPelt(),2} << EquipmentToForge::Loot{new BearClaw(),2} << EquipmentToForge::Loot{new BearPelt(),2} << EquipmentToForge::Loot{new IronOre(),5} << EquipmentToForge::Loot{new StoneOre(),5};
    list_3 << EquipmentToForge::Loot{new WolfFang(),5} << EquipmentToForge::Loot{new WolfPelt(),2} << EquipmentToForge::Loot{new BearClaw(),5} << EquipmentToForge::Loot{new BearPelt(),2} << EquipmentToForge::Loot{new IronOre(),5} << EquipmentToForge::Loot{new StoneOre(),2};
    mEquipmentsToForge << new EquipmentToForge(new Sword("Épée du nord", QPixmap(":/equipment/sword_18.png"), 360, 2, 15, 120, "Grande épée magnifique de très bonne facture forgée à partir de composants de loup et d'ours"), list_1)
                        << new EquipmentToForge(new Breastplate("Tunique d'éclaireur\nnordique", QPixmap(":/equipment/breastplate_24.png"), 210, 4, 10, 45, "Équipement utilisé par les éclaireur des armées nordiques. Il offre une résistance au froid et une grande liberté de mouvement au détriment de la défense."), list_2)
                        << new EquipmentToForge(new Pant("Tasette nordique", QPixmap(":/equipment/pant_8.png"), 110, 2, 10, 0, "Tasette utilisée par les guerriers nordiques. Offre une bonne résistance au froid en plus d'être résistante pour le combat."), list_3);
}

Blacksmith::~Blacksmith()
{
    while(!mEquipmentsToForge.isEmpty())
        delete mEquipmentsToForge.takeLast();
    if(mHouse)
        delete mHouse;
}

void Blacksmith::replenish()
{
    // TODO - add item to forge
}

void Blacksmith::setPosition(QPointF position)
{
    mHouse->setPos(position);
}

BlacksmithHouse *Blacksmith::getHouse()
{
    return mHouse;
}

QList<EquipmentToForge *> Blacksmith::getEquipmentsToForge()
{
    return mEquipmentsToForge;
}

void Blacksmith::buyEquipment(Hero * hero, EquipmentToForge * equipmentBought)
{
    Sword * sword = dynamic_cast<Sword*>(equipmentBought->getEquipment());
    if(sword){
        hero->takeItem(new Sword(sword->getName(), sword->getImage(), sword->getDamage(), sword->getSpeed(), sword->getWeight(), sword->getPrice(), sword->getInformation()));
    }
}













MerchantHouse::MerchantHouse():
    House ()
{
    initGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Marchant\nAchetez ou vendez des équipements";
}

MerchantHouse::~MerchantHouse()
{

}

void MerchantHouse::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/merchant_house.png");

    mBounding = QRect(0,0,400,300);

    QPolygon polygon;
    static const int points[] = {
        100, 0,
        145, 20,
        200, 15,
        280, 10,
        350, 25,
        345, 65,
        320, 65,
        345, 155,
        265, 230,
        180, 210,
        170, 210,
        160, 230,
        55, 200,
        50, 150,
        80, 60,
        70, 45,
    };
    polygon.setPoints(16, points);
    mCollisionShape.addPolygon(polygon);

    House::setShape();

    update();
}


Merchant::Merchant(ItemGenerator * gameItems):
    QObject (),
    mGameItems(gameItems)
{
    mHouse = new MerchantHouse();

    replenish();

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(replenish()));
    timer->start(480000);
}

Merchant::~Merchant()
{
    while(!mItemsToSell.isEmpty())
        delete mItemsToSell.takeLast();
    if(mHouse)
        delete mHouse;
}

void Merchant::replenish()
{
    emit sig_replenish(this);
    for(Item * item : mItemsToSell)
    {
        mItemsToSell.removeOne(item);
        delete item;
    }

    addItemInShop(new Scroll_X);
    addItemInShop(new Compass);
    addItemInShop(new Shovel);
    addItemInShop(new Pickaxe);
    addItemInShop(new FishingRod); 
    addItemInShop(new Knife);
    addItemInShop(mGameItems->generateEquipment());

    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(mGameItems->generateRandomSword());
    }
    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(mGameItems->generateRandomBow());
    }
    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(mGameItems->generateRandomStaff());
    }
    while(QRandomGenerator::global()->bounded(4) != 0)
    {
        addItemInShop(mGameItems->generateEquipment());
    }
}

void Merchant::setPosition(QPointF position)
{
    mHouse->setPos(position);
}

MerchantHouse *Merchant::getHouse()
{
    return mHouse;
}

QList<Item *> Merchant::getItemsToSell()
{
    return mItemsToSell;
}

bool Merchant::itemIsInShop(Item * itemToCheck)
{
    for(Item * item : mItemsToSell)
    {
        if(item == itemToCheck){
            return true;
        }
    }
    return false;
}

void Merchant::addItemInShop(Item * item)
{
    if(item)
    {
        item->setPrice(static_cast<int>(item->getPrice()*2.0));
        mItemsToSell.append(item);
    }
}

void Merchant::buyItem(Hero * hero, Item * item)
{
    hero->removeCoin(item->getPrice());
    hero->takeItem(item);
    mItemsToSell.removeOne(item);
    item->setPrice(static_cast<int>(item->getPrice()*0.5));
}

void Merchant::sellItem(Hero * hero, Item * item)
{
    float outstandingMerchantCoef = 1.0;

    if(hero->getSkillList()[PassiveSkill::OutstandingMerchant]->isUnlock())
        outstandingMerchantCoef = 1.5;

    hero->addCoin(item->getPrice()*outstandingMerchantCoef);
    mItemsToSell.append(hero->getBag()->takeItem(item));
    item->setPrice(static_cast<int>(item->getPrice()*2.0));
}











AlchemistHouse::AlchemistHouse():
    House ()
{
    initGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Alchimiste\nVous pouvez y acheter des potions";
}

AlchemistHouse::~AlchemistHouse()
{

}

void AlchemistHouse::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/alchemist_house.png");

    mBounding = QRect(0,0,400,500);

    QPolygon polygon;
    static const int points[] = {
        240, 20,
        260, 45,
        330, 55,
        355, 30,
        385, 110,
        375, 150,
        400, 190,
        400, 245,
        385, 290,
        355, 300,
        275, 255,
        240, 245,
        130, 290,
        60, 330,
        0, 270,
        15, 210,
        40, 190,
        80, 190,
        80, 150,
        125, 120,
        150, 170,
        205, 165,
        205, 100,
        230, 80,
        225, 35
    };
    polygon.setPoints(25, points);
    mCollisionShape.addPolygon(polygon);

    House::setShape();

    update();
}


Alchemist::Alchemist(ItemGenerator * gameItems):
    QObject (),
    mGameItems(gameItems)
{
    mHouse = new AlchemistHouse();

    addItemInShop(new PotionLife);
    addItemInShop(new PotionMana);
    addItemInShop(new PotionStamina);
    addItemInShop(new PotionStrenght);
    addItemInShop(new PotionKnowledge);

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(replenish()));
    timer->start(180000);

    mPotionPreferencies.append(new PotionLife);
    mPotionPreferencies.append(new PotionMana);
    mPotionPreferencies.append(new PotionStamina);
}

Alchemist::~Alchemist()
{
    while(!mItemsToSell.isEmpty())
        delete mItemsToSell.takeLast();
    while(!mPotionPreferencies.isEmpty())
        delete mPotionPreferencies.takeLast();
    if(mHouse)
        delete mHouse;
}

void Alchemist::replenish()
{
    if(mItemsToSell.length()>15)
        delete mItemsToSell.takeFirst();
    emit sig_replenish(this);

    if(mPotionPreferencies.size()!=0)
    {
        Consumable * item = nullptr;
        bool validateConsumable = false;
        while(!validateConsumable)
        {
            item = mGameItems->generateRandomConsumable();
            for(Consumable * preferencies : mPotionPreferencies)
            {
                if(item->getName() == preferencies->getName()){
                    validateConsumable = true;
                }
            }
            if(!validateConsumable)
                delete item;
        }
        if(item == nullptr)
            qDebug() << "item in alchemist shop = nullptr";
        addItemInShop(item);
    }else{
        addItemInShop(mGameItems->generateRandomConsumable());
    }
}

void Alchemist::setPosition(QPointF position)
{
    mHouse->setPos(position);
}

void Alchemist::setPotionPreferencies(QList<int> potionIndexes)
{
    Consumable * item = nullptr;
    for(int i=0;i<3;i++)
    {
        PotionLife * pLife = dynamic_cast<PotionLife*>(mPotionPreferencies[i]);
        if(pLife){
            if(potionIndexes[i] == 0){
                continue;
            }else{
                delete mPotionPreferencies.takeAt(i);
                Consumable * item = nullptr;
                switch (potionIndexes[i]){
                case 0 :
                    item = new PotionLife;
                    break;
                case 1 :
                    item = new PotionMana;
                    break;
                case 2 :
                    item = new PotionStamina;
                    break;
                case 3 :
                    item = new PotionStrenght;
                    break;
                case 4 :
                    item = new PotionKnowledge;
                    break;
                }
                mPotionPreferencies.insert(i, item);
                continue;
            }
        }
        PotionMana * pMana = dynamic_cast<PotionMana*>(mPotionPreferencies[i]);
        if(pMana){
            if(potionIndexes[i] == 1){
                continue;
            }else{
                delete mPotionPreferencies.takeAt(i);
                Consumable * item = nullptr;
                switch (potionIndexes[i]){
                case 0 :
                    item = new PotionLife;
                    break;
                case 1 :
                    item = new PotionMana;
                    break;
                case 2 :
                    item = new PotionStamina;
                    break;
                case 3 :
                    item = new PotionStrenght;
                    break;
                case 4 :
                    item = new PotionKnowledge;
                    break;
                }
                mPotionPreferencies.insert(i, item);
                continue;
            }
        }
        PotionStamina * pStamina = dynamic_cast<PotionStamina*>(mPotionPreferencies[i]);
        if(pStamina){
            if(potionIndexes[i] == 2){
                continue;
            }else{
                delete mPotionPreferencies.takeAt(i);
                item = nullptr;
                switch (potionIndexes[i]){
                case 0 :
                    item = new PotionLife;
                    break;
                case 1 :
                    item = new PotionMana;
                    break;
                case 2 :
                    item = new PotionStamina;
                    break;
                case 3 :
                    item = new PotionStrenght;
                    break;
                case 4 :
                    item = new PotionKnowledge;
                    break;
                }
                mPotionPreferencies.insert(i, item);
                continue;
            }
        }
        PotionStrenght * pStrenght = dynamic_cast<PotionStrenght*>(mPotionPreferencies[i]);
        if(pStrenght){
            if(potionIndexes[i] == 3){
                continue;
            }else{
                delete mPotionPreferencies.takeAt(i);
                Consumable * item = nullptr;
                switch (potionIndexes[i]){
                case 0 :
                    item = new PotionLife;
                    break;
                case 1 :
                    item = new PotionMana;
                    break;
                case 2 :
                    item = new PotionStamina;
                    break;
                case 3 :
                    item = new PotionStrenght;
                    break;
                case 4 :
                    item = new PotionKnowledge;
                    break;
                }
                mPotionPreferencies.insert(i, item);
                continue;
            }
        }
        PotionKnowledge * pKnowledge = dynamic_cast<PotionKnowledge*>(mPotionPreferencies[i]);
        if(pKnowledge){
            if(potionIndexes[i] == 4){
                continue;
            }else{
                delete mPotionPreferencies.takeAt(i);
                Consumable * item = nullptr;
                switch (potionIndexes[i]){
                case 0 :
                    item = new PotionLife;
                    break;
                case 1 :
                    item = new PotionMana;
                    break;
                case 2 :
                    item = new PotionStamina;
                    break;
                case 3 :
                    item = new PotionStrenght;
                    break;
                case 4 :
                    item = new PotionKnowledge;
                    break;
                }
                mPotionPreferencies.insert(i, item);
                continue;
            }
        }
    }


}

AlchemistHouse *Alchemist::getHouse()
{
    return mHouse;
}

QList<Consumable *> Alchemist::getPotionPreferencies()
{
    return mPotionPreferencies;
}

QList<Item *> Alchemist::getItemsToSell()
{
    return mItemsToSell;
}

bool Alchemist::itemIsInShop(Item * itemToCheck)
{
    for(Item * item : mItemsToSell)
    {
        if(item == itemToCheck){
            return true;
        }
    }
    return false;
}

void Alchemist::addItemInShop(Item * item)
{
    item->setPrice(static_cast<int>(item->getPrice()*1.2));
    mItemsToSell.append(item);
}

void Alchemist::buyItem(Hero * hero, Item * item)
{
    hero->removeCoin(item->getPrice());
    hero->takeItem(item);
    mItemsToSell.removeOne(item);
    item->setPrice(static_cast<int>(item->getPrice()*0.8));
}











MainHouse::MainHouse():
    House ()
{
    setGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Maison\nVotre maison vous permet de passer la nuit";
}

MainHouse::~MainHouse()
{

}


void MainHouse::setGraphicStuff()
{
    mImage = QPixmap(":/MapItems/house.png");

    mBounding = QRect(0,0,400,400);

    QPolygon polygon;
    static const int points[] = {
        115, 0,
        145, 30,
        200, 60,
        255, 75,
        350, 80,
        335, 130,
        370,145,
        375, 180,
        280, 235,
        295, 245,
        185, 320,
        195, 340,
        145, 340,
        150, 320,
        55, 250,
        30, 265,
        30, 215,
        70, 205,
        70, 180,
        30, 150,
        25, 125,
        80, 70,
        80, 20,
    };
    polygon.setPoints(23, points);
    mCollisionShape.addPolygon(polygon);

    House::setShape();

    update();
}

HeroChest::HeroChest():
    House (),
    mItems(QList<Item*>()),
    mIsOpen(false)
{
    setGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Coffre\nEspace de stockage pour vos objets";
}

HeroChest::~HeroChest()
{
//    while(!mItems.isEmpty())
//        delete mItems.takeLast();
}

bool HeroChest::itemIsInChest(Item * itemToCheck)
{
    for(Item * item : qAsConst(mItems))
    {
        if(item == itemToCheck){
            return true;
        }
    }
    return false;
}

QList<Item*> HeroChest::getItems()
{
    return mItems;
}

void HeroChest::addItem(Item * item)
{
    mItems.append(item);
}

Item * HeroChest::takeItem(Item * item)
{
    mItems.removeOne(item);
    return item;
}

void HeroChest::openChest(bool toggle)
{
    mIsOpen = toggle;
}

bool HeroChest::isOpen()
{
    return mIsOpen;
}

bool HeroChest::isObstacle()
{
    return false;
}

void HeroChest::setGraphicStuff()
{
    mImage = QPixmap(":/MapItems/heroChest.png");

    mBounding = QRect(0,0,70,70);

    mCollisionShape.addRect(mBounding);

    House::setShape();

    update();
}



HeroHouse::HeroHouse()
{
    mHouse = new MainHouse();
    mChest = new HeroChest();
}

HeroHouse::~HeroHouse()
{
    if(mHouse)
        delete mHouse;
    if(mChest)
        delete mChest;
}

MainHouse *HeroHouse::getHouse()
{
    return mHouse;
}

HeroChest *HeroHouse::getChest()
{
    return mChest;
}

void HeroHouse::setPosition(QPointF position)
{
    mHouse->setPos(position);
    mChest->setPos(position.x()+20, position.y()+300);
}







RampartBot::RampartBot()
{
    setAcceptHoverEvents(false);
    setGraphicStuff();
    setZValue(Z_VILLAGE-1);
}

RampartBot::~RampartBot()
{

}


void RampartBot::setGraphicStuff()
{
    mImage = QPixmap(":/MapItems/rampartBot.png");

    mBounding = QRect(0,0,2000,750);

    QPolygon formLeft, formRight;
    static const int pointsleft[] = {
        35, 0,
        110, 0,
        80, 235,
        25, 270
    };
    static const int pointsRight[] = {
        1910, 0,
        1910, 240,
        1690, 400,
        510, 400,
        505, 575,
        1305, 595,
        1680, 550,
        1940, 330,
        1970, 0
    };
    formLeft.setPoints(4, pointsleft);
    formRight.setPoints(9, pointsRight);
    mCollisionShape.addPolygon(formLeft);
    mCollisionShape.addPolygon(formRight);

    House::setShape();

    update();
}

void RampartBot::setPosition(QPointF position)
{
    setPos(position.x(),position.y());
}








RampartTop::RampartTop()
{
    setAcceptHoverEvents(false);
    setGraphicStuff();
    setZValue(Z_VILLAGE-1);
}

RampartTop::~RampartTop()
{

}


void RampartTop::setGraphicStuff()
{
    mImage = QPixmap(":/MapItems/rampartTop.png");

    mBounding = QRect(0,0,2000,750);

    QPolygon form;
    static const int points[] = {
        110, 750,
        130, 370,
        470, 215,
        1180, 215,
        1775, 330,
        1950, 540,
        1930, 750,
        1980, 750,
        1970, 750,
        1990, 400,
        1800, 130,
        1385, 30,
        1100, 0,
        825, 25,
        450, 50,
        140, 130,
        75, 210,
        25, 750
    };
    form.setPoints(18, points);
    mCollisionShape.addPolygon(form);

    House::setShape();

    update();
}

void RampartTop::setPosition(QPointF position)
{
    setPos(position.x(),position.y());
}






Village::Village(ItemGenerator * gameItems):
    QGraphicsPixmapItem (),
    mImage(QPixmap(":/MapItems/village_ground.png"))
{
    setZValue(0);
    mBlacksmith = new Blacksmith();
    mMerchant = new Merchant(gameItems);
    mAlchemist = new Alchemist(gameItems);
    mHouse = new HeroHouse();
    mRampartTop = new RampartTop();
    mRampartBot = new RampartBot();
    connect(mBlacksmith, SIGNAL(sig_replenish(QObject*)), this, SIGNAL(sig_replenish(QObject*)));
    connect(mBlacksmith->getHouse(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mBlacksmith->getHouse(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
    connect(mMerchant, SIGNAL(sig_replenish(QObject*)), this, SIGNAL(sig_replenish(QObject*)));
    connect(mMerchant->getHouse(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mMerchant->getHouse(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
    connect(mAlchemist, SIGNAL(sig_replenish(QObject*)), this, SIGNAL(sig_replenish(QObject*)));
    connect(mAlchemist->getHouse(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mAlchemist->getHouse(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
    connect(mHouse->getHouse(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mHouse->getHouse(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
    connect(mHouse->getChest(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mHouse->getChest(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
}

Village::~Village()
{
    if(mBlacksmith)
        delete mBlacksmith;
    if(mMerchant)
        delete mMerchant;
    if(mHouse)
        delete mHouse;
    if(mAlchemist)
        delete mAlchemist;
    if(mRampartTop)
        delete mRampartTop;
    if(mRampartBot)
        delete mRampartBot;
}

void Village::addInScene(QGraphicsScene * scene)
{
    scene->addItem(this);
    scene->addItem(mBlacksmith->mHouse);
    scene->addItem(mMerchant->mHouse);
    scene->addItem(mHouse->mHouse);
    scene->addItem(mHouse->mChest);
    scene->addItem(mAlchemist->mHouse);
    scene->addItem(mRampartTop);
    scene->addItem(mRampartBot);
}

void Village::setPosition(QPointF position)
{
    setPos(position);
    mPosition = position;
    mBlacksmith->setPosition(QPointF(position.x()+300, position.y()+200));
    mMerchant->setPosition(QPointF(position.x()+1250, position.y()+150));
    mHouse->setPosition(QPointF(position.x()+750, position.y()+550));
    mAlchemist->setPosition(QPointF(position.x()+1450, position.y()+450));
    mRampartTop->setPosition(QPointF(position.x(), position.y()));
    mRampartBot->setPosition(QPointF(position.x(), position.y()+748));
}

QPointF Village::getPosition()
{
    return mPosition;
}

Blacksmith * Village::getBlacksmith()
{
    return mBlacksmith;
}

Merchant * Village::getMerchant()
{
    return mMerchant;
}

HeroHouse * Village::getHeroHouse()
{
    return mHouse;
}

Alchemist * Village::getAlchemist()
{
    return mAlchemist;
}

QPainterPath Village::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

QRectF Village::boundingRect() const
{
    return QRect(0,0,2000,1500);
}

void Village::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, 0, 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}
