#include "item.h"
#include "material.h"
#include "equipment.h"
#include "consumable.h"
#include <QRandomGenerator>
#include <QDebug>

Item::Item(QString name, QPixmap image, int weight, int price):
    QObject (),
    QGraphicsPixmapItem (),
    mIdentifier(IDENT_ITEM_NOT_ATTRIBUATE),
    mName(name),
    mImage(image),
    mWeight(weight),    
    mPrice(price),
    mInformation(QString()),
    mHover(false),
    mShape(QPainterPath())
{
    setZValue(Z_ITEM);
    setAcceptHoverEvents(true);
    t_delayHover = new QTimer(this);
    t_delayHover->setSingleShot(true);
    connect(t_delayHover, SIGNAL(timeout()), this, SLOT(showItemInfo()));
}

Item::~Item()
{

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
    qDebug() << "INSTANCE CREATION : (" << identifier << ")";
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
        return new Sword();

    case WEAPON_STAFF:
        return new Sword();

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

    default:
        qDebug() << "INSTANCE CREATION : Not found";
        break;
    }

    return new BagCoin(0);
}




Scroll::Scroll(QString name, QPixmap image, int weight, int price):
    Item(name, image, weight, price)
{

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
