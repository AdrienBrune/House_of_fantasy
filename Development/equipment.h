#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "item.h"
#include "material.h"
#include "constants.h"

class Equipment : public Item
{
public:
    Equipment(QString name, QPixmap image, int weight, int price, uint32_t usable = 0xFFFFFFFF);
    Equipment();
    virtual ~Equipment();
public:
    const uint32_t & getUsable()const;
    void setUsable(uint32_t);
public:
    virtual void serialize(QDataStream& stream)const=0;
    virtual void deserialize(QDataStream& stream)=0;
protected:
    uint32_t mUsable;
};

class EquipmentToForge
{
public:
    struct Loot{
        Material * material;
        int quantities;
    };
public:
    EquipmentToForge(Equipment* equipment, QList<Loot> loots, int price = 20):mEquipment(equipment), mLoots(loots), mCraftingPrice(price){};
    ~EquipmentToForge()
    {
        while(!mLoots.isEmpty())
        {
            if(mLoots.last().material)
                delete mLoots.last().material;
            mLoots.takeLast();
        }
        if(mEquipment)
            delete mEquipment;
        mEquipment = nullptr;
    };
public:
    Equipment *getEquipment(){ return mEquipment; };
    Equipment *takeEquipment(){ Equipment * equipment = mEquipment; mEquipment = nullptr; return equipment; };
    QList<EquipmentToForge::Loot> getLootList(){ return mLoots; };
    int getCraftingPrice(){ return mCraftingPrice; };
private:
    Equipment * mEquipment;
    QList<Loot> mLoots;
    int mCraftingPrice;
};





class Weapon : public Equipment
{
    Q_OBJECT
public:
    Weapon(QString name, QPixmap image, int damage, int speed, int weight, int price, uint32_t usable = 0xFFFFFFFF);
    virtual ~Weapon();
public:
    int getDamage();
    int getSpeed();
    void setDamage(int);
    void setSpeed(int);
public:
    Feature getFirstCaracteristic()override;
    Feature getSecondCaracteristic()override;
    Feature getThirdCaracteristic()override;
    Feature getFourthCaracteristic()override;
    void serialize(QDataStream& stream)const override
    {
        Item::serialize(stream);
        stream << mUsable << mDamage << mSpeed;
        DEBUG("SERIALIZED[in]  : Weapon");
    }
    void deserialize(QDataStream& stream)override
    {
        Item::deserialize(stream);
        stream >> mUsable >> mDamage >> mSpeed;
        DEBUG("SERIALIZED[out] : Weapon");
    }
private:
    int mDamage;
    int mSpeed;
};

class Sword : public Weapon
{
public:
    Sword(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable = ABLE(HeroAbleToUSe::eSwordman));
    Sword();
    ~Sword();
};

class Bow : public Weapon
{
public:
    Bow(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable = ABLE(HeroAbleToUSe::eArcher));
    Bow();
    ~Bow();
};

class Staff : public Weapon
{
public:
    Staff(QString name, QPixmap image, int damage, int speed, int weight, int price, QString info, uint32_t usable = ABLE(HeroAbleToUSe::eWizard));
    Staff();
    ~Staff();
};




class ArmorPiece : public Equipment
{
    Q_OBJECT
public:
    ArmorPiece(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, uint32_t usable = 0xFFFFFFFF);
    virtual ~ArmorPiece();
public:
    int getDefense();
    qreal getDodgingStat();

    void setDefense(int);
    void setDodgingStat(qreal);
public:
    Feature getFirstCaracteristic()override;
    Feature getSecondCaracteristic()override;
    Feature getThirdCaracteristic()override;
    Feature getFourthCaracteristic()override;
    void serialize(QDataStream& stream)const override
    {
        Item::serialize(stream);
        stream << mUsable << mDefense << mDodgingStat;
        DEBUG("SERIALIZED[in]  : Armor");
    }
    void deserialize(QDataStream& stream)override
    {
        Item::deserialize(stream);
        stream >> mUsable >> mDefense >> mDodgingStat;
        DEBUG("SERIALIZED[out] : Armor");
    }
private:
    int mDefense;
    qreal mDodgingStat;
};

class Helmet : public ArmorPiece
{
public:
    Helmet(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
    Helmet();
    ~Helmet();
};

class Breastplate : public ArmorPiece
{
public:
     Breastplate(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
     Breastplate();
    ~Breastplate();
};

class Gloves : public ArmorPiece
{
public:
    Gloves(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
    Gloves();
    ~Gloves();
};

class Pant : public ArmorPiece
{
public:
    Pant(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
    Pant();
    ~Pant();
};

class Footwears : public ArmorPiece
{
public:
    Footwears(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
    Footwears();
    ~Footwears();
};

class Amulet : public ArmorPiece
{
public:
    Amulet(QString name, QPixmap image, int defense, qreal dodgingStats, int weight, int price, QString info, uint32_t usable = 0xFFFFFFFF);
    Amulet();
    ~Amulet();
};






class Gear : public QObject
{
    Q_OBJECT
public:
    Gear();
    Gear(Helmet*, Breastplate*, Gloves*, Pant*, Footwears*, Amulet*, Weapon*);
    ~Gear();
signals:
    void sig_equipmentSet();
public:
    Helmet *getHelmet();
    Breastplate *getBreastplate();
    Gloves * getGloves();
    Pant * getPant();
    Footwears * getFootWears();
    Amulet *getAmulet();
    Weapon *getWeapon();

    void setArmor(Helmet*,Breastplate*,Gloves*,Pant*,Footwears*,Amulet*);
    void setEquipment(Equipment*);
    bool removeEquipment(Equipment*);
public:
    int weight();
    qreal dodgingStat();
    int defenseStat();
    int damageStat();
public:
    void serialize(QDataStream& stream)const
    {
        quint16 numberItems = mEquipmentParts.size();
        stream << numberItems;
        for(Equipment * equipment : mEquipmentParts)
        {
            stream << equipment->getIdentifier();
            equipment->serialize(stream);
        }

        DEBUG("SERIALIZED[in]  : Gear");
    }
    void deserialize(QDataStream& stream)
    {
        quint16 numberItems = 0;
        stream >> numberItems;
        for(int i = 0; i < numberItems; i++)
        {
            quint32 identifier = 0;
            stream >> identifier;
            Equipment * item = dynamic_cast<Equipment*>(Item::getInstance(identifier));
            item->deserialize(stream);
            if(item)
                mEquipmentParts.append(item);
            else
            {
                DEBUG("SERIALIZED[out] : Item is not an equipment");
            }
        }
        DEBUG("SERIALIZED[out] : Gear");
    }
private:
    QList<Equipment*> mEquipmentParts;
};



#endif // EQUIPMENT_H
