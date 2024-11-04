#include "village.h"

#include "item.h"
#include "entitieshandler.h"

House::House():
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

//    painter->setBrush(QBrush("#7700FF00"));
//    painter->drawPath(mShape);
//    if(isObstacle())
//    {
//        painter->setBrush(QBrush("#77FF0000"));
//        painter->drawPath(mCollisionShape->shape());
//    }

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
    mBoundingRect = QRect(0,0,400,400);
    mZOffset = mBoundingRect.height()*2/3;
    setImage(QPixmap(":/MapItems/blacksmith_house.png"), true);

    QPainterPath collidingShape;
    QPolygon polygon;
    static const int points[] = {
        23, 273,
        130, 327,
        202, 281,
        245, 302,
        312, 320,
        367, 276,
        170, 215,
    };
    polygon.setPoints(7, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);
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
    mBoundingRect = QRect(0,0,400,300);
    mZOffset = mBoundingRect.height()*2/3;
    setImage(QPixmap(":/MapItems/merchant_house.png"), true);

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        55, 228,
        63, 255,
        150, 286,
        167, 270,
        225, 297,
        315, 222,
        142, 177
    };
    polygon.setPoints(7, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);
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
    if(!QRandomGenerator().global()->bounded(3)) // 33%
        addItemInShop(new Compass);
    if(!QRandomGenerator().global()->bounded(3)) // 33%
        addItemInShop(new Shovel);
    if(!QRandomGenerator().global()->bounded(3)) // 33%
        addItemInShop(new Pickaxe);
    if(!QRandomGenerator().global()->bounded(3)) // 33%
        addItemInShop(new FishingRod);
    if(!QRandomGenerator().global()->bounded(3)) // 33%
        addItemInShop(new Knife);
    if(!QRandomGenerator().global()->bounded(1)) // 100%
        addItemInShop(new MapScroll());

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
    MapScroll * mapScroll = dynamic_cast<MapScroll*>(item);
    if(mapScroll)
    {
        mItemsToSell.removeOne(item);
        hero->unlockAdventurerMap();
        emit sig_adventurerMapUnlock();
        return;
    }

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

    emit hero->sig_SellItem(item);
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
    mBoundingRect = QRect(0,0,400,500);
    mZOffset = mBoundingRect.height()*2/3;
    setImage(QPixmap(":/MapItems/alchemist_house.png"), true);

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        70, 341,
        96, 388,
        232, 310,
        361, 365,
        388, 327,
        246, 262,
        90, 301
    };
    polygon.setPoints(7, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);
}


Alchemist::Alchemist():
    QObject ()
{
    mHouse = new AlchemistHouse();

    addItemInShop(new PotionLife);
    addItemInShop(new PotionLife);
    addItemInShop(new PotionMana);

    for(int i = 0; i < NUMBER_POTION_SLOT; i++)
    {
        PotionCookingSlot * potionSlot = new PotionCookingSlot(this);
        connect(potionSlot, &PotionCookingSlot::sig_cookingRequested, this, &Alchemist::onCookingRequested);
        connect(potionSlot, &PotionCookingSlot::sig_cookingDone, this, &Alchemist::onCookingDone);
        mPotionSlots.append(potionSlot);
    }
}

Alchemist::~Alchemist()
{
    while(!mItemsToSell.isEmpty())
        delete mItemsToSell.takeLast();
    if(mHouse)
        delete mHouse;
}

void Alchemist::replenish()
{
    emit sig_replenish(this);
}

void Alchemist::onCookingRequested(PotionCookingSlot * potionSlot)
{
    // Add to queue
    addPotionInQueue(potionSlot);

    // Check to start a cooking
    bool alreadyCooking = false;
    for(PotionCookingSlot * slot : qAsConst(mCookingQueue))
    {
        if(slot->getCooking())
        {
            alreadyCooking = true;
            break;
        }
    }
    if(!alreadyCooking)
    {
        for(PotionCookingSlot * slot : qAsConst(mCookingQueue))
        {
            slot->setCooking(true); // Start a cooking
            return;
        }
    }
}

void Alchemist::onCookingDone(PotionCookingSlot * potionSlot)
{
    // Add potion in inventory
    Item * item = potionSlot->takePotion();
    addItemInShop(item);
    emit sig_addPotion(item);

    // Remove from queue
    removePotionInQueue(potionSlot);

    replenish();

    // Check to start a cooking
    bool alreadyCooking = false;
    for(PotionCookingSlot * slot : qAsConst(mCookingQueue))
    {
        if(slot->getCooking())
            alreadyCooking = true;
    }
    if(!alreadyCooking)
    {
        for(PotionCookingSlot * slot : qAsConst(mCookingQueue))
        {
            slot->setCooking(true); // Start a cooking
            return;
        }
    }
}

void Alchemist::setPosition(QPointF position)
{
    mHouse->setPos(position);
}

AlchemistHouse *Alchemist::getHouse()
{
    return mHouse;
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
    mBoundingRect = QRect(0,0,400,300);
    mZOffset = mBoundingRect.height()*2/3;
    setImage(QPixmap(":/MapItems/altar.png"), true);

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        100, 155,
        98, 238,
        205, 251,
        308, 230,
        308, 165
    };
    polygon.setPoints(5, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    // QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    // QPixmap tmpImg(":/MapItems/altar_bound.png");
    // tmpImg.scroll(0,0,0,0,boundingRect().width(), boundingRect().height());
    // tmp->setPixmap(tmpImg);
    // mShape = tmp->shape();
    // delete tmp;
    //update();
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








Taverne::Taverne():
    House ()
{
    setGraphicStuff();
    setZValue(Z_VILLAGE);
    mInformation = "Taverne\nVous permet de passer la nuit";
}

Taverne::~Taverne()
{

}


void Taverne::setGraphicStuff()
{
    mBoundingRect = QRect(0,0,400,400);
    mZOffset = mBoundingRect.height()*2/3;
    setImage(QPixmap(":/MapItems/house.png"), true);

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        44, 231,
        44, 261,
        176, 370,
        344, 274,
        200, 200,
    };
    polygon.setPoints(4, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);
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

void HeroChest::setGraphicStuff()
{
    mBoundingRect = QRect(0,0,70,70);
    mZOffset = mBoundingRect.height()/2;
    setImage(QPixmap(":/MapItems/heroChest.png"), true);
}



HeroHouse::HeroHouse()
{
    mHouse = new Taverne();
    mChest = new HeroChest();
}

HeroHouse::~HeroHouse()
{
    if(mHouse)
        delete mHouse;
    if(mChest)
        delete mChest;
}

Taverne *HeroHouse::getHouse()
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



void ChevalDeFrise::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage.scaled(boundingRect().size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    Q_UNUSED(widget)
    Q_UNUSED(option)
}
void ChevalDeFriseFront::setGraphicStuff()
{
    mBoundingRect = QRect(0, 0, 250 * mRatio, 200 * mRatio);
    mZOffset = mBoundingRect.height()/2;
    setImage(QPixmap(":/MapItems/cheval_de_frise_front.png"), false, true);

    QPainterPath collidingShape;
    collidingShape.addRect(QRect(0, mBoundingRect.height() - mBoundingRect.height()/2 - mBoundingRect.height()/20, mBoundingRect.width(), mBoundingRect.height()/10));
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_VILLAGE-1);
}

void ChevalDeFriseDiag::setGraphicStuff()
{
    mBoundingRect = QRect(0, 0, 240 * mRatio, 260 * mRatio);
    mZOffset = mBoundingRect.height()/2;
    setImage(QPixmap(":/MapItems/cheval_de_frise_diag.png"), false, true);

    QPainterPath collidingShape;
    collidingShape.addRect(QRect(mBoundingRect.width()/3, mBoundingRect.height() - mBoundingRect.height()/2 - mBoundingRect.height()/20, mBoundingRect.width()/3, mBoundingRect.height()/10));
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_VILLAGE-1);
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
    
    mChevalDeFriseList.append(new ChevalDeFriseFront(1.0));
    mChevalDeFriseList.append(new ChevalDeFriseFront(1.0));
    mChevalDeFriseList.append(new ChevalDeFriseFront(0.7));
    mChevalDeFriseList.append(new ChevalDeFriseFront(0.7));
    mChevalDeFriseList.append(new ChevalDeFriseDiag(0.8));
    mChevalDeFriseList.append(new ChevalDeFriseDiag(1.0));
    mChevalDeFriseList.append(new ChevalDeFriseDiag(0.8));
    mChevalDeFriseList.append(new ChevalDeFriseDiag(1.0));
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
    if(mAltar)
        delete mAltar;
    while(!mChevalDeFriseList.isEmpty())
        delete mChevalDeFriseList.takeLast();
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
    for(ChevalDeFrise * mapItem : mChevalDeFriseList)
        scene->addItem(mapItem);
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
    for(ChevalDeFrise * mapItem : mChevalDeFriseList)
        scene->removeItem(mapItem);
}

void Village::setPosition(QPointF position)
{
    setPos(position);
    mPosition = position;
    mBlacksmith->setPosition(QPointF(position.x()+328, position.y()+367));
    mMerchant->setPosition(QPointF(position.x()+1267, position.y()+393));
    mHouse->setPosition(QPointF(position.x()+840, position.y()+225));
    mAlchemist->setPosition(QPointF(position.x()+1447, position.y()+628));
    mAltar->setPosition(QPointF(position.x()+1670, position.y()+1000));

    mChevalDeFriseList.at(0)->setPosition(QPointF(position.x()+845, position.y()+1255));
    mChevalDeFriseList.at(1)->setPosition(QPointF(position.x()+1600, position.y()+1186));
    mChevalDeFriseList.at(1)->setTransform(QTransform(-1, 0, 0, 1, 0, 0));
    mChevalDeFriseList.at(1)->setRotation(8.0);
    mChevalDeFriseList.at(2)->setPosition(QPointF(position.x()+619, position.y()+110));
    mChevalDeFriseList.at(2)->setRotation(-6.0);
    mChevalDeFriseList.at(3)->setPosition(QPointF(position.x()+1234, position.y()+100));
    mChevalDeFriseList.at(3)->setRotation(5.0);
    mChevalDeFriseList.at(4)->setPosition(QPointF(position.x()+100, position.y()+462));
    mChevalDeFriseList.at(5)->setPosition(QPointF(position.x()+266, position.y()+863));
    mChevalDeFriseList.at(5)->setTransform(QTransform(-1, 0, 0, 1, 0, 0));
    mChevalDeFriseList.at(6)->setPosition(QPointF(position.x()+1950, position.y()+358));
    mChevalDeFriseList.at(6)->setTransform(QTransform(-1, 0, 0, 1, 0, 0));
    mChevalDeFriseList.at(6)->setRotation(5.0);
    mChevalDeFriseList.at(7)->setPosition(QPointF(position.x()+1850, position.y()+925));
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


