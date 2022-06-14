#ifndef MATERIAL_H
#define MATERIAL_H

#include "item.h"

class Material : public Item
{
public:
    Material(QString name, QPixmap image, int weight, int price);
    ~Material();
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
};

class StoneOre : public Material
{
public:
    StoneOre();
    ~StoneOre();
};

class IronOre : public Material
{
public:
    IronOre();
    ~IronOre();
};

class SaphirOre : public Material
{
public:
    SaphirOre();
    ~SaphirOre();
};

class EmeraldOre : public Material
{
public:
    EmeraldOre();
    ~EmeraldOre();
};

class RubisOre : public Material
{
public:
    RubisOre();
    ~RubisOre();
};

class MonsterMaterial : public Material
{

public:
    MonsterMaterial(QString name, QPixmap image, int weight, int price);
    ~MonsterMaterial();
};

class WolfPelt : public MonsterMaterial
{
public:
    WolfPelt();
    ~WolfPelt();
};

class WolfAlphaPelt : public MonsterMaterial
{
public:
    WolfAlphaPelt();
    ~WolfAlphaPelt();
};

class WolfFang : public MonsterMaterial
{
public:
    WolfFang();
    ~WolfFang();
};

class WolfMeat : public MonsterMaterial
{
public:
    WolfMeat();
    ~WolfMeat();
};

class GoblinEar : public MonsterMaterial
{
public:
    GoblinEar();
    ~GoblinEar();
};

class GoblinBones : public MonsterMaterial
{
public:
    GoblinBones();
    ~GoblinBones();
};

class BearPelt : public MonsterMaterial
{
public:
    BearPelt();
    ~BearPelt();
};

class BearMeat : public MonsterMaterial
{
public:
    BearMeat();
    ~BearMeat();
};

class BearClaw : public MonsterMaterial
{
public:
    BearClaw();
    ~BearClaw();
};

class TrollMeat : public MonsterMaterial
{
public:
    TrollMeat();
    ~TrollMeat();
};

class TrollSkull : public MonsterMaterial
{
public:
    TrollSkull();
    ~TrollSkull();
};

class OggreSkull : public MonsterMaterial
{
public:
    OggreSkull();
    ~OggreSkull();
};

class LaoshanlungHeart : public MonsterMaterial
{
public:
    LaoshanlungHeart();
    ~LaoshanlungHeart();
};

#endif // MATERIAL_H
