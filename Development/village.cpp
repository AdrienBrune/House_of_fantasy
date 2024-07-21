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
        emit sig_villageInteraction(this);
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
    mInformation = "Forgeron\nLieu pour forger des armes et armmures";
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
    QObject (),
    mEquipmentsToForge(nullptr)
{
    mHouse = new BlacksmithHouse();

    replenish();

    t_resplenish = new QTimer(this);
    connect(t_resplenish, SIGNAL(timeout()), this, SLOT(replenish()));
    t_resplenish->start(5*60*1000);
}

Blacksmith::~Blacksmith()
{
    if(mHouse)
        delete mHouse;
    if(mEquipmentsToForge)
        delete mEquipmentsToForge;
}

void Blacksmith::replenish()
{
    if(mEquipmentsToForge)
        delete mEquipmentsToForge;

    mEquipmentsToForge = gItemGenerator->generateEquipmentToForge();

    emit sig_replenish(this);
}

void Blacksmith::setPosition(QPointF position)
{
    mHouse->setPos(position);
}

BlacksmithHouse *Blacksmith::getHouse()
{
    return mHouse;
}

EquipmentToForge *Blacksmith::getEquipmentToCraft()
{
    return mEquipmentsToForge;
}

int Blacksmith::getTimeBeforeResplenish()
{
    return t_resplenish->remainingTime()/1000;
}














MerchantHouse::MerchantHouse():
    House ()
{
    initGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Marchant\nAchetez ou vendez des objets ou des équipements";
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


Merchant::Merchant():
    QObject ()
{
    mHouse = new MerchantHouse();

    replenish();

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(replenish()));
    timer->start(10*60*1000);
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
    for(Item * item : qAsConst(mItemsToSell))
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
    addItemInShop(gItemGenerator->generateEquipment());

    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(gItemGenerator->generateRandomSword());
    }
    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(gItemGenerator->generateRandomBow());
    }
    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        addItemInShop(gItemGenerator->generateRandomStaff());
    }
    while(QRandomGenerator::global()->bounded(4) != 0)
    {
        addItemInShop(gItemGenerator->generateEquipment());
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
    for(Item * item : qAsConst(mItemsToSell))
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
    mInformation = "Alchimiste\nLieu pour acheter des potions";
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


Alchemist::Alchemist():
    QObject ()
{
    mHouse = new AlchemistHouse();

    addItemInShop(new PotionLife);
    addItemInShop(new PotionMana);
    addItemInShop(new PotionStamina);
    addItemInShop(new PotionStrenght);
    addItemInShop(new PotionKnowledge);

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(replenish()));
    timer->start(3*60*1000);

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
    if(mItemsToSell.length() > 15)
        delete mItemsToSell.takeFirst();
    emit sig_replenish(this);

    if(mPotionPreferencies.size()!=0)
    {
        Consumable * item = nullptr;
        bool validateConsumable = false;
        while(!validateConsumable)
        {
            item = gItemGenerator->generateRandomConsumable();
            for(Consumable * preferencies : qAsConst(mPotionPreferencies))
            {
                if(item->getName() == preferencies->getName()){
                    validateConsumable = true;
                }
            }
            if(!validateConsumable)
                delete item;
        }
        if(item == nullptr)
            DEBUG_ERR("item in alchemist shop = nullptr");
        addItemInShop(item);
    }else{
        addItemInShop(gItemGenerator->generateRandomConsumable());
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
    for(Item * item : qAsConst(mItemsToSell))
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










AltarBuilding::AltarBuilding(QList<Offering>* offers):
    House (),
    mAnimation(0),
    mOffers(offers)
{
    initGraphicStuff();
    setZValue(Z_VILLAGE+1);
    mInformation = "Autel\nLa légende raconte qu'une puissante créature peut être invoquée grâce à des offrandes";

    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(100);
}

AltarBuilding::~AltarBuilding()
{

}

void AltarBuilding::animate()
{
    if(++mAnimation > 7)
    {
        mAnimation = 0;
    }
    update();
}

void AltarBuilding::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/altar.png");

    mBounding = QRect(0,0,400,300);

    mCollisionShape.addRect(QRect(0,0,400,190));

    QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    QPixmap tmpImg(":/MapItems/altar_bound.png");
    tmpImg.scroll(0,0,0,0,boundingRect().width(), boundingRect().height());
    tmp->setPixmap(tmpImg);
    mShape = tmp->shape();
    delete tmp;

    update();
}

void AltarBuilding::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0, 0, mImage, 0, mAnimation*static_cast<int>(boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));

    if(mHover){
        painter->setPen(QPen(QBrush(BORDERS_COLOR), 3));
        painter->drawPath(mShape);
    }

    if(mOffers)
    {
        QRect offerAreas[] = {
            QRect(113, 226, 50, 50),
            QRect(187, 237, 50, 50),
            QRect(254, 226, 50, 50)
        };
        for(int i = 0; i < mOffers->size(); i++)
        {
            if(mOffers->at(i).item)
            {
                painter->drawPixmap(offerAreas[i], QPixmap(mOffers->at(i).item->getImage()));
            }
        }
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}

Altar::Altar():
    mOfferingPixmap(QPixmap(":/materials/earthCristal.png"))
{
    mBuilding = new AltarBuilding(&mOfferings);
}

Altar::~Altar()
{
    for(int i = 0; i < mOfferings.size(); i++)
    {
        if(mOfferings[i].item)
            delete mOfferings[i].item;
    }
    if(mBuilding)
        delete mBuilding;
}

void Altar::setPosition(QPointF position)
{
    mBuilding->setPos(position);
}

AltarBuilding *Altar::getBuilding()
{
    return mBuilding;
}

QList<Offering> Altar::getOfferings()
{
    return mOfferings;
}

QPixmap Altar::getOfferingPixmap()
{
    return mOfferingPixmap;
}

void Altar::setOffering(int idx, Item *item)
{
    if(idx >= mOfferings.size())
        return;

    mOfferings[idx].item = item;

    for(Offering offer : qAsConst(mOfferings))
    {
        if(!offer.item)
            return;
    }

    /* All the offerings have been put on the altar */
    emit sig_LaoShanLungSummoned();
}

bool Altar::isLaoShanLungSummoned()
{
    for(Offering offer : qAsConst(mOfferings))
    {
        if(!offer.item)
            return false;
    }
    return true;
}








MainHouse::MainHouse():
    House ()
{
    setGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Maison\nVous permet de passer la nuit";
}

MainHouse::~MainHouse()
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






Village::Village():
    QGraphicsPixmapItem (),
    mImage(QPixmap(":/MapItems/village_ground.png"))
{
    setZValue(0);
    mBlacksmith = new Blacksmith();
    mMerchant = new Merchant();
    mAlchemist = new Alchemist();
    mHouse = new HeroHouse();
    mAltar = new Altar();
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
    connect(mAltar->getBuilding(), SIGNAL(sig_villageShowInfo(QGraphicsItem*)), this, SIGNAL(sig_villageShowInfo(QGraphicsItem*)));
    connect(mAltar->getBuilding(), SIGNAL(sig_villageInteraction(QGraphicsItem*)), this, SIGNAL(sig_villageInteraction(QGraphicsItem*)));
    connect(mAltar, SIGNAL(sig_LaoShanLungSummoned()), this, SIGNAL(sig_LaoShanLungSummoned()));
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
    if(mAltar)
        delete mAltar;
}

void Village::addInScene(QGraphicsScene * scene)
{
    scene->addItem(this);
    scene->addItem(mBlacksmith->mHouse);
    scene->addItem(mMerchant->mHouse);
    scene->addItem(mHouse->mHouse);
    scene->addItem(mHouse->mChest);
    scene->addItem(mAltar->mBuilding);
    scene->addItem(mAlchemist->mHouse);
    scene->addItem(mRampartTop);
    scene->addItem(mRampartBot);
}

void Village::removeFromScene(QGraphicsScene * scene)
{
    scene->removeItem(this);
    scene->removeItem(mBlacksmith->mHouse);
    scene->removeItem(mMerchant->mHouse);
    scene->removeItem(mHouse->mHouse);
    scene->removeItem(mHouse->mChest);
    scene->removeItem(mAltar->mBuilding);
    scene->removeItem(mAlchemist->mHouse);
    scene->removeItem(mRampartTop);
    scene->removeItem(mRampartBot);
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
    mAltar->setPosition(QPointF(position.x()+700, position.y()+1160));
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

Altar *Village::getAltar()
{
    return mAltar;
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


