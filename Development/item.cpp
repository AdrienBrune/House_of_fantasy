#include "item.h"
#include "material.h"
#include "equipment.h"
#include "consumable.h"
#include "toolfunctions.h"
#include "entitieshandler.h"
#include <QRandomGenerator>
#include <QDebug>
#include "mapevent.h"

quint32 Item::sNbInstances = 0;

Item::Item(QString name, QPixmap image, int weight, int price):
    QObject (),
    QGraphicsPixmapItem (),
    mIdentifier(IDENT_ITEM_NOT_ATTRIBUATE),
    mName(name),
    mImage(image),
    mWeight(weight),    
    mPrice(price),
    mInformation(QString()),
    mIsUsable(false),
    mHover(false),
    mShape(QPainterPath())
{
    setZValue(Z_ITEM);
    setAcceptHoverEvents(true);
    t_delayHover = new QTimer(this);
    t_delayHover->setSingleShot(true);
    connect(t_delayHover, SIGNAL(timeout()), this, SLOT(showItemInfo()));
    //qDebug() << "[C] " << ++sNbInstances << " " << mName;
}

Item::~Item()
{
    //qDebug() << "[D] " << --sNbInstances << " " << mName;
}

void Item::showItemInfo()
{
    emit sig_showItemInfo(this);
}

quint32 Item::getIdentifier()
{
    return mIdentifier;
}

QString Item::getName()
{
    return mName;
}

int Item::getWeight()
{
    return mWeight;
}

QPixmap Item::getImage()
{
    return mImage;
}

int Item::getPrice()
{
    return mPrice;
}

QString Item::getInformation()
{
    return mInformation;
}

void Item::setShape()
{
    QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    tmp->setPixmap(mImage.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
    if(mImage.isNull())
        qDebug() << getName() << " item null ici !!!";
    mShape = tmp->shape();
    delete tmp;
}

void Item::setName(QString name)
{
    mName = name;
    emit sig_itemStatsChanged();
}

void Item::setweight(int weight)
{
    mWeight = weight;
    emit sig_itemStatsChanged();
}

void Item::setPrice(int price)
{
    if(price <= 0)
        mPrice = 1;
    else
        mPrice = price;
    emit sig_itemStatsChanged();
}

void Item::setInformation(QString info)
{
    mInformation = info;
}

void Item::setHover(bool toggle)
{
    mHover = toggle;
    update();
}

bool Item::isUsable()
{
    return mIsUsable;
}

QPainterPath Item::shape() const
{
    QPainterPath path;
    path.addEllipse(QRect(0,0,static_cast<int>(boundingRect().width()),static_cast<int>(boundingRect().height())));
    return path;
}

QRectF Item::boundingRect() const
{
    return QRect(0,0,50,50);
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()), mImage);

    if(mHover){
        painter->setPen(QPen(QBrush(BORDERS_COLOR), 2));
        painter->drawPath(mShape);
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void Item::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(t_delayHover->isActive())
            t_delayHover->stop();
        sig_itemClicked(this);
        event->accept();
    }else{
        event->ignore();
    }
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverEnter == QGraphicsSceneHoverEvent::HoverEnter)
    {
        mHover = true;
        update();
        event->accept();
        t_delayHover->start(TIME_TO_SHOW_ITEM);
    }
}

void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
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

Item *Item::getInstance(quint32 identifier)
{
    DEBUG("INSTANCE CREATION : (" + QString("%1").arg(identifier) + ")");
    switch(identifier)
    {
    case TOOL_COMPASS:
        return new Compass();

    case TOOL_FISHINGROD:
        return new FishingRod();

    case TOOL_KNIFE:
        return new Knife();

    case TOOL_PICKAXE:
        return new Pickaxe();

    case TOOL_SHOVEL:
        return new Shovel();
    
    case TOOL_MAPSCROLL:
        return new MapScroll();

    case BAGCOIN:
        return new BagCoin(0);

    case ARMOR_HELMET:
        return new Helmet();

    case ARMOR_BREASTPLATE:
        return new Breastplate();

    case ARMOR_GLOVES:
        return new Gloves();

    case ARMOR_PANT:
        return new Pant();

    case ARMOR_FOOTWEARS:
        return new Footwears();

    case ARMOR_AMULET:
        return new Amulet();

    case WEAPON_SWORD:
        return new Sword();

    case WEAPON_BOW:
        return new Bow();

    case WEAPON_STAFF:
        return new Staff();

    case CONSUMABLE_LIFEPOTION:
        return new PotionLife();

    case CONSUMABLE_MANAPOTION:
        return new PotionMana();

    case CONSUMABLE_STAMINAPOTION:
        return new PotionStamina();

    case CONSUMABLE_STRENGTHPOTION:
        return new PotionStrenght();

    case CONSUMABLE_KNOWLEDGEPOTION:
        return new PotionKnowledge();

    case CONSUMABLE_REDFISH:
        return new RedFish();

    case CONSUMABLE_BLUEFISH:
        return new BlueFish();

    case CONSUMABLE_YELLOWFISH:
        return new Yellowfish();

    case CONSUMABLE_COMMUNFISH:
        return new CommonFish();

    case ORE_STONE:
        return new StoneOre();

    case ORE_IRON:
        return new IronOre();

    case ORE_SAPHIR:
        return new SaphirOre();

    case ORE_EMERALD:
        return new EmeraldOre();

    case ORE_RUBIS:
        return new EmeraldOre();

    case MONSTERMATERIAL_POISON_POUCH:
        return new PoisonPouch();

    case MONSTERMATERIAL_MANDIBLES:
        return new Mandibles();

    case MONSTERMATERIAL_WOLF_PELT:
        return new WolfPelt();

    case MONSTERMATERIAL_WOLF_FANG:
        return new WolfFang();

    case MONSTERMATERIAL_WOLF_MEAT:
        return new WolfMeat();

    case MONSTERMATERIAL_GOBLIN_EAR:
        return new GoblinEar();

    case MONSTERMATERIAL_GOBLIN_BONES:
        return new GoblinBones();

    case MONSTERMATERIAL_TROLL_MEAT:
        return new TrollMeat();

    case MONSTERMATERIAL_TROLL_SKULL:
        return new TrollSkull();

    case MONSTERMATERIAL_WOLFALPHA_PELT:
        return new WolfAlphaPelt();

    case MONSTERMATERIAL_BEAR_MEAT:
        return new BearPelt();

    case MONSTERMATERIAL_BEAR_PELT:
        return new BearMeat();

    case MONSTERMATERIAL_BEAR_TALON:
        return new BearClaw();

    case MONSTERMATERIAL_OGGRE_SKULL:
        return new OggreSkull();

    case MONSTERMATERIAL_LAOSHANLUNG_HEART:
        return new LaoshanlungHeart();

    case SCROLL_X:
        return new Scroll_X();

    case EARTH_CRISTAL:
        return new EarthCristal();

    default:
        DEBUG("INSTANCE CREATION : Not found id=" + QString("%1").arg(identifier));
        break;
    }

    return new BagCoin(0);
}

quint32 Item::getNbInstances()
{
    return sNbInstances;
}




Scroll::Scroll(QString name, QPixmap image, int weight, int price):
    Item(name, image, weight, price)
{
    mIsUsable = true;
}

Item::Feature Scroll::getFirstCaracteristic()
{
    return Feature{getPrice(), QPixmap(":/icons/coin_logo.png")};
}

Item::Feature Scroll::getSecondCaracteristic()
{
    return Feature{getWeight(), QPixmap(":/icons/payload_logo.png")};
}

Item::Feature Scroll::getThirdCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Item::Feature Scroll::getFourthCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Scroll::~Scroll()
{

}

Scroll_X::Scroll_X():
    Scroll("Parchemin X", QPixmap(":/consumables/scroll_X.png"), 2, 10)
{
    mIdentifier = SCROLL_X;
    setInformation("Parchemin à usage unique qui permet de se téléporter au village.");
    Item::setShape();
}

Scroll_X::~Scroll_X()
{

}





Tool::Tool(QString name, QPixmap image, int weight, int price):
    Item(name, image, weight, price)
{
    mDurability = QRandomGenerator::global()->bounded(3, 5);
    mIsUsable = true;
}

bool Tool::use()
{
    if(mDurability-- <= 0)
        return false;
    else{
        return true;
    }
}

Item::Feature Tool::getFirstCaracteristic()
{
    return Feature{getPrice(), QPixmap(":/icons/coin_logo.png")};
}

Item::Feature Tool::getSecondCaracteristic()
{
    return Feature{getWeight(), QPixmap(":/icons/payload_logo.png")};
}

Item::Feature Tool::getThirdCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Item::Feature Tool::getFourthCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Tool::~Tool()
{

}

Shovel::Shovel():
    Tool("Pelle", QPixmap(":/tools/shovel.png"), 10, 10)
{
    mIdentifier = TOOL_SHOVEL;
    setInformation("Outil permettant de creuser et de déterrer des objects cachés.");
    Item::setShape();
}

bool Shovel::use()
{
    return Tool::use();
}

Shovel::~Shovel()
{

}

Pickaxe::Pickaxe():
    Tool("Pioche", QPixmap(":/tools/pickaxe.png"), 10, 10)
{
    mIdentifier = TOOL_PICKAXE;
    setInformation("Outil permettant de miner du minerai.");
    Item::setShape();
}

Pickaxe::~Pickaxe()
{

}

bool Pickaxe::use()
{
    return Tool::use();
}

FishingRod::FishingRod():
    Tool("Canne à pêche", QPixmap(":/tools/fishingrod.png"), 10, 10)
{
    mIdentifier = TOOL_FISHINGROD;
    setInformation("Outil permettant de pêcher dans les lacs.");
    Item::setShape();
}

bool FishingRod::use()
{
    return Tool::use();
}

FishingRod::~FishingRod()
{

}

Compass::Compass():
    Tool("Boussole", QPixmap(":/tools/compass.png"), 10, 7)
{
    mIdentifier = TOOL_COMPASS;
    setInformation("Boussole qui vous permettra de retrouver le chemin du village avec séreinité.");
    Item::setShape();
}

Compass::~Compass()
{

}

bool Compass::use()
{
    return true;
}

Knife::Knife():
    Tool("Couteau", QPixmap(":/tools/knife.png"), 10, 20)
{
    mIdentifier = TOOL_KNIFE;
    setInformation("Couteau servant à dépecer animaux et monstres.");
    Item::setShape();
}

Knife::~Knife()
{

}

bool Knife::use()
{
    return true;
}

MapScroll::MapScroll():
    Tool("Carte", QPixmap(":/tools/map_scroll.png"), 2, 50)
{
    mIdentifier = TOOL_MAPSCROLL;
    setInformation("Carte du monde, idéal pour les voyageurs.");
    Item::setShape();

    init();
}

MapScroll::~MapScroll()
{

}

bool MapScroll::use()
{
    return true;
}

void MapScroll::init()
{
    mMapDiscovery.timer.stop();
    disconnect(&mMapDiscovery.timer, &QTimer::timeout, this, &MapScroll::updateDiscovery);
    while(!mMapDiscovery.monsters.isEmpty())
        mMapDiscovery.monsters.takeLast();
    while(!mMapDiscovery.mapEvent.isEmpty())
        mMapDiscovery.mapEvent.takeLast();

    ToolFunctions::matrix2DResize(mMapDiscovery.fogMatrix, 100, 100);
    ToolFunctions::matrix2DInit(mMapDiscovery.fogMatrix, false);
    mMapDiscovery.timer.setInterval(1000);
    connect(&mMapDiscovery.timer, &QTimer::timeout, this, &MapScroll::updateDiscovery);
}

void MapScroll::updateDiscovery()
{
    Map *map = EntitiesHandler::getInstance().getMap();
    Hero *hero = EntitiesHandler::getInstance().getHero();

    // Update fog
    int xSplit = mMapDiscovery.fogMatrix[0].size();
    int ySplit = mMapDiscovery.fogMatrix.size();
    int heroCellX = static_cast<int>((hero->pos().x() + hero->boundingRect().width()/2) / (map->getScene()->sceneRect().size().width() / xSplit));
    int heroCellY = static_cast<int>((hero->pos().y() + hero->boundingRect().height()/2) / (map->getScene()->sceneRect().size().height() / ySplit));

    if(heroCellX > xSplit)
    {
        qDebug() << "erreur calcul X";
        return;
    }
    if(heroCellY > ySplit)
    {
        qDebug() << "erreur calcul Y";
        return;
    }

    int discoveryRadius = 0.06 * xSplit;
    for(int row = heroCellY - discoveryRadius; row <= heroCellY + discoveryRadius; row++)
    {
        for(int col = heroCellX - discoveryRadius; col <= heroCellX + discoveryRadius; col++)
        {
            if(row >= 0 && row < ySplit && col >= 0 && col < xSplit)
                mMapDiscovery.fogMatrix[row][col] = true;
        }
    }

    // Update monster list
    QGraphicsView * view = EntitiesHandler::getInstance().getView();
    QList<QGraphicsItem*> list = view->scene()->items(ToolFunctions::getVisibleView(view));
    for(QGraphicsItem * object : qAsConst(list))
    {
        Monster * monster = dynamic_cast<Monster*>(object);
        if(monster)
        {
            if(mMapDiscovery.monsters.contains(monster))
                continue;

            mMapDiscovery.monsters.append(monster);
        }
        MapEvent * mapEvent = dynamic_cast<MapEvent*>(object);
        if(mapEvent)
        {
            BushEvent * bush = dynamic_cast<BushEvent*>(mapEvent);
            if(bush)
                continue;

            if(mMapDiscovery.mapEvent.contains(mapEvent))
                continue;

            mMapDiscovery.mapEvent.append(mapEvent);
        }
    }

    emit sig_update();
}





BagCoin::BagCoin(int coinAmount):
    Item("Sac de pièces", QPixmap(":/consumables/bagCoin.png"), 0, coinAmount)
{
    mIdentifier = BAGCOIN;
    QString buff = "";
    setInformation("Sac contenant " + buff.asprintf("%d", mPrice) + " pièces d'or.");
    Item::setShape();
}

BagCoin::~BagCoin()
{

}

Item::Feature BagCoin::getFirstCaracteristic()
{
    return Feature{getPrice(),QPixmap(":/icons/coin_logo.png")};
}

Item::Feature BagCoin::getSecondCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Item::Feature BagCoin::getThirdCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Item::Feature BagCoin::getFourthCaracteristic()
{
    return Feature{-1,QPixmap("")};
}
