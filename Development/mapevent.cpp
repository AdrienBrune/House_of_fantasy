#include "mapevent.h"

MapEvent::MapEvent():
    MapItem ()
{

}

QList<Item*> MapEvent::getItems()
{
    return mItems;
}

QList<Item*> MapEvent::takeItems()
{
    QList<Item*> items = mItems;
    while(!mItems.isEmpty())
        mItems.removeLast();
    itemsTook();
    return items;
}

Item *MapEvent::takeItem(Item * item)
{
    if(!mItems.isEmpty()){
        for(int i=0;i<mItems.size();i++)
        {
            if(mItems[i] == item){
                Item * item = mItems.takeAt(i);
                if(mItems.isEmpty())
                    itemsTook();
                return item;
            }
        }
    }
    return nullptr;
}

void MapEvent::returnItems(QList<Item*> items)
{
    for(Item * item : items)
    {
        mItems.append(item);
    }
}

bool MapEvent::eventIsActive()
{
    if(mItems.size()!=0)
        return true;
    else{
        return false;
    }
}

MapEvent::~MapEvent()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();
}






FishingEvent::FishingEvent():
    MapEvent ()
{
    initGraphicStuff();

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));

    t_startAnimation = new QTimer(this);
    connect(t_startAnimation, SIGNAL(timeout()), this, SLOT(startAnimation()));
    t_startAnimation->start(10000);

    QList<Fish*> list = generateRandomFishes();
    for(Fish * fish : list)
    {
        mItems.append(fish);
    }
}

void FishingEvent::animate()
{
    mNextFrame++;
    if(mNextFrame >= 6)
    {
        t_animation->stop();
    }
    update();
}

void FishingEvent::startAnimation()
{
    mNextFrame = 0;
    if(!t_animation->isActive()){
        t_animation->start(200);
    }
}

bool FishingEvent::isAnimated()
{
    return t_animation->isActive();
}

void FishingEvent::itemsTook()
{
    t_startAnimation->stop();
    mNextFrame = 6;
    update();
}

bool FishingEvent::isObstacle()
{
    return mObstacle;
}

void FishingEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mNextFrame), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void FishingEvent::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/animation_fish.png");
    mImageSelected = 0;
    mNextFrame = 6;
    setZValue(Z_FISH_EVENT);

    mBoundingRect = QRect(0,0,200,100);
    mShape.addRect(mBoundingRect);
}

QList<Fish*> FishingEvent::generateRandomFishes()
{
    QList<Fish*> loots;
    for(int f=0;f<QRandomGenerator::global()->bounded(2, 6);f++)
    {
        switch(QRandomGenerator::global()->bounded(4))
        {
        case 0 :
            loots.append(new RedFish);
            break;
        case 1 :
            loots.append(new BlueFish);
            break;
        case 2 :
            loots.append(new Yellowfish);
            break;
        case 3 :
            loots.append(new CommonFish);
            break;
        }
    }
    return loots;
}

FishingEvent::~FishingEvent()
{

}





BushEvent::BushEvent():
    MapEvent ()
{
    mMapItemName = "Bush event";
    setRotation(QRandomGenerator::global()->bounded(31)-15);
    mNextFrame = 0;
    setZValue(Z_BUSH);

    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(QRect(20,20,60,60));

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));
}

void BushEvent::animate()
{
    mNextFrame++;
    if(mNextFrame > 2)
    {
        mNextFrame = 0;
        t_animation->stop();
    }
    update();
}

void BushEvent::startAnimation()
{
    if(!t_animation->isActive()){
        t_animation->start(400);
    }
}

bool BushEvent::isAnimated()
{
    return t_animation->isActive();
}

void BushEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mNextFrame), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void BushEvent::restoreGraphicStuff()
{
    mImage = QPixmap(":/MapItems/bush.png");
}

BushEvent::~BushEvent()
{

}







BushEventCoin::BushEventCoin():
    BushEvent ()
{
    initGraphicStuff();

    mItems.append(new BagCoin(QRandomGenerator::global()->bounded(1, 5)));
}

void BushEventCoin::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/bush_event_coin.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_BUSH);
}

bool BushEventCoin::isObstacle()
{
    return mObstacle;
}

void BushEventCoin::itemsTook()
{
    BushEvent::restoreGraphicStuff();
}

BushEventCoin::~BushEventCoin()
{

}






BushEventEquipment::BushEventEquipment():
    BushEvent ()
{
    initGraphicStuff();

    mItems.append(gItemGenerator->generateEquipment());
}

void BushEventEquipment::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/bush_event_equipment.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_BUSH);
}

bool BushEventEquipment::isObstacle()
{
    return mObstacle;
}

void BushEventEquipment::itemsTook()
{
    BushEvent::restoreGraphicStuff();
}

BushEventEquipment::~BushEventEquipment()
{

}








ChestEvent::ChestEvent():
    MapEvent (),
    mRevealChest(0),
    mHover(false)
{
    mMapItemName = "Chest event";
    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(mBoundingRect);

    setAcceptHoverEvents(false);

    while(QRandomGenerator::global()->bounded(3) == 0)
    {
        mItems.append(gItemGenerator->generateEquipment());
    }
    while(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(gItemGenerator->generateRandomSword());
    }
    while(QRandomGenerator::global()->bounded(2) == 0)
    {
        mItems.append(gItemGenerator->generateRandomConsumable());
    }
    mItems.append(new BagCoin(QRandomGenerator::global()->bounded(4, 11)));
}

QList<Item *> ChestEvent::getItems()
{
    return mItems;
}

void ChestEvent::revealChest()
{
    mRevealChest = 1;
    setShape();
    setAcceptHoverEvents(true);
    update();
}

bool ChestEvent::isRevealed()
{
    if(mRevealChest == 0)
        return false;
    else {
        return true;
    }
}

void ChestEvent::openChest(bool toggle)
{
    mIsOpen = toggle;
}

bool ChestEvent::isOpen()
{
    return mIsOpen;
}

void ChestEvent::addExtraItems()
{
    if(gItemGenerator == nullptr)
        return;

    if(QRandomGenerator::global()->bounded(3) == 0)
    {
        mItems.append(gItemGenerator->generateEquipment());
    }
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(gItemGenerator->generateRandomSword());
    }
    if(QRandomGenerator::global()->bounded(2) == 0)
    {
        mItems.append(gItemGenerator->generateRandomConsumable());
    }
}

void ChestEvent::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit sig_clicToOpenChest(this);
        event->accept();
    }else{
        event->ignore();
    }
}

void ChestEvent::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverEnter == QGraphicsSceneHoverEvent::HoverEnter)
    {
        mHover = true;
        update();
        event->accept();
    }
}

void ChestEvent::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverLeave == QGraphicsSceneHoverEvent::HoverLeave)
    {
        mHover = false;
        update();
        event->accept();
    }
}

void ChestEvent::setShape()
{
    QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    tmp->setPixmap(mImage.copy(0, static_cast<int>(mRevealChest*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
    mShape = tmp->shape();
    delete tmp;
}

void ChestEvent::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mRevealChest), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));

    if(mHover){
        painter->setPen(QPen(QBrush(BORDERS_COLOR), 2));
        painter->drawPath(mShape);
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void ChestEvent::itemsTook()
{
    mRevealChest = 2;
    setAcceptHoverEvents(false);
    mHover = false;
    disconnect(this, SIGNAL(sig_clicToOpenChest(ChestEvent*)), nullptr, nullptr);
    update();
}

bool ChestEvent::isObstacle()
{
    return mObstacle;
}

ChestEvent::~ChestEvent()
{

}









ChestBurried::ChestBurried():
    ChestEvent ()
{
    initGraphicStuff();
}

void ChestBurried::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/chest_event_burried.png");
    mImageSelected = 0;
}

ChestBurried::~ChestBurried()
{

}


GoblinChest::GoblinChest():
    ChestEvent ()
{
    initGraphicStuff();
    revealChest();

    mItems.append(new Sword("Gourdin", QPixmap(":/equipment/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));
    for(int n=0;n<QRandomGenerator::global()->bounded(1, 3);n++){
        mItems.append(new EmeraldOre);
    }
    for(int n=0;n<QRandomGenerator::global()->bounded(1, 3);n++){
        mItems.append(new SaphirOre);
    }
    for(int n=0;n<QRandomGenerator::global()->bounded(1, 3);n++){
        mItems.append(new RubisOre);
    }
    for(int n=0;n<QRandomGenerator::global()->bounded(5, 8);n++){
        mItems.append(new IronOre);
    }
}

void GoblinChest::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/chest_openClose.png");
    mImageSelected = 0;
    setZValue(Z_CHEST_BURRIED);
}

GoblinChest::~GoblinChest()
{

}






OreSpot::OreSpot():
    MapEvent ()
{
    initGraphicStuff();
}

OreSpot::~OreSpot()
{

}

void OreSpot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mType), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void OreSpot::initGraphicStuff()
{
    mImage = QPixmap(":/MapItems/oreSpot.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_ORE_SPOT);
    setZValue(Z_ORESPOT);
    mType = Ore::none;

    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(mBoundingRect);
}

void OreSpot::itemsTook()
{
    mType = Ore::none;
    update();
}

bool OreSpot::isObstacle()
{
    return mObstacle;
}


StoneOreSpot::StoneOreSpot():
    OreSpot ()
{
    OreSpot::initGraphicStuff();

    mMapItemName = "stone";
    mType = Ore::stone;

    mItems.append(new StoneOre());
    for(int i=0;i<QRandomGenerator::global()->bounded(4);i++)
    {
        mItems.append(new StoneOre());
    }
}

StoneOreSpot::~StoneOreSpot()
{

}

IronOreSpot::IronOreSpot():
    OreSpot ()
{
    OreSpot::initGraphicStuff();

    mMapItemName = "iron";
    mType = Ore::iron;

    mItems.append(new IronOre());
    for(int i=0;i<QRandomGenerator::global()->bounded(4);i++)
    {
        mItems.append(new IronOre());
    }
}

IronOreSpot::~IronOreSpot()
{

}


SaphirOreSpot::SaphirOreSpot():
    OreSpot ()
{
    OreSpot::initGraphicStuff();

    mMapItemName = "saphir";
    mType = Ore::saphir;

    mItems.append(new SaphirOre());
    for(int i=0;i<QRandomGenerator::global()->bounded(4);i++)
    {
        mItems.append(new SaphirOre());
    }
}

SaphirOreSpot::~SaphirOreSpot()
{

}

EmeraldOreSpot::EmeraldOreSpot():
    OreSpot ()
{
    OreSpot::initGraphicStuff();

    mMapItemName = "emeraude";
    mType = Ore::emerald;

    mItems.append(new EmeraldOre());
    for(int i=0;i<QRandomGenerator::global()->bounded(4);i++)
    {
        mItems.append(new EmeraldOre());
    }
}

EmeraldOreSpot::~EmeraldOreSpot()
{

}

RubisOreSpot::RubisOreSpot():
    OreSpot ()
{
    OreSpot::initGraphicStuff();
    
    mMapItemName = "rubis";
    mType = Ore::rubis;

    mItems.append(new RubisOre());
    for(int i=0;i<QRandomGenerator::global()->bounded(4);i++)
    {
        mItems.append(new RubisOre());
    }
}

RubisOreSpot::~RubisOreSpot()
{

}
