#ifndef CONSUMABLE_H
#define CONSUMABLE_H

#include "item.h"

class Consumable : public Item
{
public:
    Consumable(QString name, QPixmap image, int weight, int price, int capaciy);
    ~Consumable();
public:
    int getCapacity();
    void setCapacity(int);
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
    void serialize(QDataStream& stream)const
    {
        Item::serialize(stream);
        stream << mCapacity;
    }
    void deserialize(QDataStream& stream)
    {
        Item::deserialize(stream);
        stream >> mCapacity;
    }
private:
    int mCapacity;
};

class PotionLife : public Consumable
{
public:
    PotionLife();
    ~PotionLife();
};

class PotionMana : public Consumable
{
public:
    PotionMana();
    ~PotionMana();
};

class PotionStamina: public Consumable
{
public:
    PotionStamina();
    ~PotionStamina();
};

class PotionStrenght: public Consumable
{
public:
    PotionStrenght();
    ~PotionStrenght();
};

class PotionKnowledge: public Consumable
{
public:
    PotionKnowledge();
    ~PotionKnowledge();
};




class Fish : public Consumable
{
public:
    Fish(QString name, QPixmap image, int weight, int price, int staminaToAdd);
    ~Fish();
};

class RedFish : public Fish
{
public:
    RedFish();
    ~RedFish();
};

class BlueFish : public Fish
{
public:
    BlueFish();
    ~BlueFish();
};

class Yellowfish : public Fish
{
public:
    Yellowfish();
    ~Yellowfish();
};

class CommonFish : public Fish
{
public:
    CommonFish();
    ~CommonFish();
};

#endif // CONSUMABLE_H
