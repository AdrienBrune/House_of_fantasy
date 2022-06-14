#include "equipment.h"

Equipment::Equipment(QString name, QPixmap image, int weight, int price, uint32_t usable):
    Item(name, image, weight, price),
    mUsable(usable)
{

}

Equipment::Equipment():
    Item("", QPixmap(), 0, 0)
{

}

Equipment::~Equipment()
{
    
}

const uint32_t &Equipment::getUsable() const
{
    return mUsable;
}

void Equipment::setUsable(uint32_t usable)
{
    mUsable = usable;
}

EquipmentToForge::EquipmentToForge(Equipment* equipment, QList<Loot> loots):
    mEquipment(equipment),
    mLoots(loots)
{

}

EquipmentToForge::~EquipmentToForge()
{

}

Equipment *EquipmentToForge::getEquipment()
{
    return mEquipment;
}

QList<EquipmentToForge::Loot> EquipmentToForge::getLootList()
{
    return mLoots;
}


Weapon::Weapon(QString name, QPixmap image, int damage, int speed, int weight, int price, uint32_t usable):
    Equipment(name, image, weight, price, usable),
    mDamage(damage),
    mSpeed(speed)
{

}

int Weapon::getDamage()
{
    return mDamage;
}

int Weapon::getSpeed()
{
    return mSpeed;
}

void Weapon::setDamage(int damage)
{
    mDamage = damage;
    emit sig_itemStatsChanged();
}

void Weapon::setSpeed(int speed)
{
    mSpeed = speed;
    emit sig_itemStatsChanged();
}

Item::Feature Weapon::getFirstCaracteristic()
{
    return Feature{getDamage(),QPixmap(":/icons/sword_logo.png")};
}

Item::Feature Weapon::getSecondCaracteristic()
{
    return Feature{getWeight(),QPixmap(":/icons/payload_logo.png")};
}

Item::Feature Weapon::getThirdCaracteristic()
{
    return Feature{getSpeed(),QPixmap(":/icons/speed_logo.png")};
}

Item::Feature Weapon::getFourthCaracteristic()
{
    return Feature{getPrice(),QPixmap(":/icons/coin_logo.png")};
}

Weapon::~Weapon()
{

}

Sword::Sword(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable):
    Weapon(name, image, damage, speed, weight, price, usable)
{
    mIdentifier = WEAPON_SWORD;
    setInformation(info);
    Item::setShape();
}

Sword::Sword():
    Weapon("", QPixmap(), 0, 0, 0, 0)
{
    mIdentifier = WEAPON_SWORD;
}

Sword::~Sword()
{

}

Bow::Bow(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable):
    Weapon(name, image, damage, speed, weight, price, usable)
{
    mIdentifier = WEAPON_BOW;
    setInformation(info);
    Item::setShape();
}

Bow::Bow():
    Weapon("", QPixmap(), 0, 0, 0, 0)
{
    mIdentifier = WEAPON_BOW;
}

Bow::~Bow()
{

}

Staff::Staff(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable):
    Weapon(name, image, damage, speed, weight, price, usable)
{
    mIdentifier = WEAPON_STAFF;
    setInformation(info);
    Item::setShape();
}

Staff::Staff():
    Weapon("", QPixmap(), 0, 0, 0, 0)
{
    mIdentifier = WEAPON_STAFF;
}

Staff::~Staff()
{

}





ArmorPiece::ArmorPiece(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, uint32_t usable):
    Equipment(name, image, weight, price, usable),
    mDefense(defense),
    mDodgingStat(dodge)
{

}

int ArmorPiece::getDefense()
{
    return mDefense;
}

qreal ArmorPiece::getDodgingStat()
{
    return mDodgingStat;
}

void ArmorPiece::setDefense(int defense)
{
    mDefense = defense;
    emit sig_itemStatsChanged();
}

void ArmorPiece::setDodgingStat(qreal dodge)
{
    mDodgingStat = dodge;
    emit sig_itemStatsChanged();
}

Item::Feature ArmorPiece::getFirstCaracteristic()
{
    return Feature{getDefense(),QPixmap(":/icons/shield_logo.png")};
}

Item::Feature ArmorPiece::getSecondCaracteristic()
{
    return Feature{getWeight(),QPixmap(":/icons/payload_logo.png")};
}

Item::Feature ArmorPiece::getThirdCaracteristic()
{
    return Feature{static_cast<int>(mDodgingStat),QPixmap(":/icons/dodge_logo.png")};
}

Item::Feature ArmorPiece::getFourthCaracteristic()
{
    return Feature{getPrice(),QPixmap(":/icons/coin_logo.png")};
}


ArmorPiece::~ArmorPiece()
{

}

Helmet::Helmet(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, QString info, uint32_t usable):
    ArmorPiece(name, image, defense, dodge, weight, price, usable)
{
    mIdentifier = ARMOR_HELMET;
    setInformation(info);
    Item::setShape();
}

Helmet::Helmet():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Helmet::~Helmet()
{

}

Breastplate::Breastplate(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, QString info, uint32_t usable):
    ArmorPiece(name, image, defense, dodge, weight, price, usable)
{
    mIdentifier = ARMOR_BREASTPLATE;
    setInformation(info);
    Item::setShape();
}

Breastplate::Breastplate():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Breastplate::~Breastplate()
{

}

Gloves::Gloves(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, QString info, uint32_t usable):
    ArmorPiece(name, image, defense, dodge, weight, price, usable)
{
    mIdentifier = ARMOR_GLOVES;
    setInformation(info);
    Item::setShape();
}

Gloves::Gloves():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Gloves::~Gloves()
{

}

Pant::Pant(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, QString info, uint32_t usable):
    ArmorPiece(name, image, defense, dodge, weight, price, usable)
{
    mIdentifier = ARMOR_PANT;
    setInformation(info);
    Item::setShape();
}

Pant::Pant():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Pant::~Pant()
{

}

Footwears::Footwears(QString name, QPixmap image, int defense, qreal dodge, int weight, int price, QString info, uint32_t usable):
    ArmorPiece(name, image, defense, dodge, weight, price, usable)
{
    mIdentifier = ARMOR_FOOTWEARS;
    setInformation(info);
    Item::setShape();
}

Footwears::Footwears():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Footwears::~Footwears()
{

}

Amulet::Amulet(QString name, QPixmap image, int defense, qreal dodgingStat, int weight, int price, QString info, uint32_t usable):
    ArmorPiece (name, image, defense, dodgingStat, weight, price, usable)
{
    mIdentifier = ARMOR_AMULET;
    setInformation(info);
    Item::setShape();
}

Amulet::Amulet():
    ArmorPiece("", QPixmap(), 0, 0, 0, 0)
{

}

Amulet::~Amulet()
{

}







Gear::Gear():
    QObject ()
{

}

Gear::Gear(Helmet * helmet, Breastplate * breastplate, Gloves * gloves, Pant * pant, Footwears * footwears, Amulet * amulet, Weapon * weapon):
    QObject ()
{
    while(!mEquipmentParts.isEmpty())
        mEquipmentParts.removeLast();
    mEquipmentParts << helmet << breastplate << gloves << pant << footwears << amulet << weapon;
}

Helmet *Gear::getHelmet()
{
    for(Equipment * part : mEquipmentParts)
    {
        Helmet * equipment = dynamic_cast<Helmet*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Breastplate *Gear::getBreastplate()
{
    for(Equipment * part : mEquipmentParts)
    {
        Breastplate * equipment = dynamic_cast<Breastplate*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Gloves *Gear::getGloves()
{
    for(Equipment * part : mEquipmentParts)
    {
        Gloves * equipment = dynamic_cast<Gloves*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Pant *Gear::getPant()
{
    for(Equipment * part : mEquipmentParts)
    {
        Pant * equipment = dynamic_cast<Pant*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Footwears *Gear::getFootWears()
{
    for(Equipment * part : mEquipmentParts)
    {
        Footwears * equipment = dynamic_cast<Footwears*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Amulet *Gear::getAmulet()
{
    for(Equipment * part : mEquipmentParts)
    {
        Amulet * equipment = dynamic_cast<Amulet*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

Weapon *Gear::getWeapon()
{
    for(Equipment * part : mEquipmentParts)
    {
        Weapon * equipment = dynamic_cast<Weapon*>(part);
        if(equipment){
            return equipment;
        }
    }
    return nullptr;
}

void Gear::setArmor(Helmet * helmet, Breastplate * breastplate, Gloves * gloves, Pant * pant, Footwears * footwears, Amulet * amulet)
{
    Weapon * weapon = getWeapon();
    while(!mEquipmentParts.isEmpty())
        mEquipmentParts.removeLast();
    mEquipmentParts << helmet << breastplate << gloves << pant << footwears << amulet << weapon;
    emit sig_equipmentSet();
}

void Gear::setEquipment(Equipment * equipment)
{
    for(Equipment * part : mEquipmentParts)
    {
        Helmet * helmet = dynamic_cast<Helmet*>(part);
        Helmet * helmetToSet = dynamic_cast<Helmet*>(equipment);
        if(helmet && helmetToSet){
            removeEquipment(helmet);
            mEquipmentParts.append(helmetToSet);
            emit sig_equipmentSet();
            return;
        }
        Breastplate * breastplate = dynamic_cast<Breastplate*>(part);
        Breastplate * breastplateToSet = dynamic_cast<Breastplate*>(equipment);
        if(breastplate && breastplateToSet){
            removeEquipment(breastplate);
            mEquipmentParts.append(breastplateToSet);
            emit sig_equipmentSet();
            return;
        }
        Gloves * gloves = dynamic_cast<Gloves*>(part);
        Gloves * glovesToSet = dynamic_cast<Gloves*>(equipment);
        if(gloves && glovesToSet){
            removeEquipment(gloves);
            mEquipmentParts.append(glovesToSet);
            emit sig_equipmentSet();
            return;
        }
        Pant * pant = dynamic_cast<Pant*>(part);
        Pant * pantToSet = dynamic_cast<Pant*>(equipment);
        if(pant && pantToSet){

        }
        Footwears * footwears = dynamic_cast<Footwears*>(part);
        Footwears * footwearsToSet = dynamic_cast<Footwears*>(equipment);
        if(footwears && footwearsToSet){
            removeEquipment(footwears);
            mEquipmentParts.append(footwearsToSet);
            emit sig_equipmentSet();
            return;
        }
        Amulet * amulet = dynamic_cast<Amulet*>(part);
        Amulet * amuletToSet = dynamic_cast<Amulet*>(equipment);
        if(amulet && amuletToSet){
            removeEquipment(amulet);
            mEquipmentParts.append(amuletToSet);
            emit sig_equipmentSet();
            return;
        }
        Weapon * weapon = dynamic_cast<Weapon*>(part);
        Weapon * weaponToSet = dynamic_cast<Weapon*>(equipment);
        if(weapon && weaponToSet){
            removeEquipment(weapon);
            mEquipmentParts.append(weaponToSet);
            emit sig_equipmentSet();
            return;
        }
    }
    emit sig_equipmentSet();
    mEquipmentParts.append(equipment);
}

bool Gear::removeEquipment(Equipment * equipment)
{
    emit sig_equipmentSet();
    return mEquipmentParts.removeOne(equipment);
}

int Gear::weight()
{
    int weight = 0;
    for(Equipment * part : mEquipmentParts)
    {
        weight += part->getWeight();
    }
    return weight;
}

qreal Gear::dodgingStat()
{
    qreal dodgingStat = 0;
    for(Equipment * part : mEquipmentParts)
    {
        ArmorPiece * armorPiece = dynamic_cast<ArmorPiece*>(part);
        if(armorPiece){
            dodgingStat += armorPiece->getDodgingStat();
        }
    }
    return dodgingStat;
}

int Gear::defenseStat()
{
    int defense = 0;
    for(Equipment * part : mEquipmentParts)
    {
        ArmorPiece * armorPiece = dynamic_cast<ArmorPiece*>(part);
        if(armorPiece){
            defense += armorPiece->getDefense();
        }
    }
    return defense;
}

int Gear::damageStat()
{
    int damage = HERO_RAW_DAMAGE;
    if(getWeapon())
        damage += getWeapon()->getDamage();
    return damage;
}

Gear::~Gear()
{
    while(!mEquipmentParts.isEmpty())
        delete mEquipmentParts.takeLast();
}
