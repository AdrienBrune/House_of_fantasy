#ifndef ITEMGENERATOR_H
#define ITEMGENERATOR_H

#include "equipment.h"
#include "consumable.h"
#include "material.h"

enum{ RARENESS_LV_1 = 0x1, RARENESS_LV_2 = 0x2, RARENESS_LV_3 = 0x4, RARENESS_LV_4 = 0x8, RARENESS_LV_5 = 0x10, RARENESS_LV_NB = 0x20 };
enum{ RARENESS_MSK_LV_1 = 0, RARENESS_MSK_LV_2, RARENESS_MSK_LV_3, RARENESS_MSK_LV_4, RARENESS_MSK_LV_5 };

class ItemGenerator
{
public:
    ItemGenerator();
    ~ItemGenerator();
public:
    Sword * generateRandomSword(int rarenessMask = (RARENESS_LV_NB-1));
    Bow * generateRandomBow(int rarenessMask = (RARENESS_LV_NB-1));
    Staff * generateRandomStaff(int rarenessMask = (RARENESS_LV_NB-1));
    Helmet * generateRandomHelmet(int rarenessMask = (RARENESS_LV_NB-1));
    Breastplate * generateRandomBreastplate(int rarenessMask = (RARENESS_LV_NB-1));
    Gloves * generateRandomGloves(int rarenessMask = (RARENESS_LV_NB-1));
    Pant * generateRandomPant(int rarenessMask = (RARENESS_LV_NB-1));
    Footwears * generateRandomFootwears(int rarenessMask = (RARENESS_LV_NB-1));
    Amulet * generateRandomAmulet(int rarenessMask = (RARENESS_LV_NB-1));
    Item * generateEquipment(int rarenessMask = (RARENESS_LV_NB-1));
    EquipmentToForge * generateEquipmentToForge();
    Material * generateRandomMaterial();
    Consumable * generateRandomConsumable();
private:
    QList<Sword*> mSwords;
    QList<Bow*> mBows;
    QList<Staff*> mStaffs;
    QList<Helmet*> mHelmets;
    QList<Breastplate*> mBreastplates;
    QList<Gloves*> mGloves;
    QList<Pant*> mPants;
    QList<Footwears*> mFootwears;
    QList<Amulet*> mAmulets;
};

extern ItemGenerator * gItemGenerator;

#endif // ITEMGENERATOR_H
