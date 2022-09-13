#include "material.h"

Material::Material(QString name, QPixmap image, int weight, int price):
    Item(name, image, weight, price)
{

}

Material::~Material()
{

}

Item::Feature Material::getFirstCaracteristic()
{
    return Feature{getPrice(),QPixmap(":/icons/coin_logo.png")};
}

Item::Feature Material::getSecondCaracteristic()
{
    return Feature{getWeight(),QPixmap(":/icons/payload_logo.png")};
}

Item::Feature Material::getThirdCaracteristic()
{
    return Feature{-1,QPixmap("")};
}

Item::Feature Material::getFourthCaracteristic()
{
    return Feature{-1,QPixmap("")};
}



MonsterMaterial::MonsterMaterial(QString name, QPixmap image, int weight, int price):
    Material (name, image, weight, price)
{
    //qDebug() << "[C] " << ++sNbInstances << " " << mName;
}

MonsterMaterial::~MonsterMaterial()
{
    //qDebug() << "[D] " << --sNbInstances << " " << mName;
}

StoneOre::StoneOre():
    Material("Minerai de pierre", QPixmap(":/materials/stoneOre.png"), 3, 4)
{
    mIdentifier = ORE_STONE;
    setInformation("Minerai très commun mais toujours très utile.");
    Item::setShape();
}

StoneOre::~StoneOre()
{

}

IronOre::IronOre():
    Material("Minerai de fer", QPixmap(":/materials/ironOre.png"), 5, 8)
{
    mIdentifier = ORE_IRON;
    setInformation("Minerai idéal pour la confaction d'armes et d'armures.");
    Item::setShape();
}

IronOre::~IronOre()
{

}

SaphirOre::SaphirOre():
    Material("Saphir", QPixmap(":/materials/saphirOre.png"), 5, 15)
{
    mIdentifier = ORE_SAPHIR;
    setInformation("Minerai époustouflant. Cette gemme est constituée d'un bleu profond.");
    Item::setShape();
}

SaphirOre::~SaphirOre()
{

}

EmeraldOre::EmeraldOre():
    Material("Emeraude", QPixmap(":/materials/emeraldOre.png"), 5, 20)
{
    mIdentifier = ORE_EMERALD;
    setInformation("Minerai époustouflant. Cette gemme est constituée d'un vert vif.");
    Item::setShape();
}

EmeraldOre::~EmeraldOre()
{

}

RubisOre::RubisOre():
    Material("Rubis", QPixmap(":/materials/rubisOre.png"), 5, 25)
{
    mIdentifier = ORE_RUBIS;
    setInformation("Minerai époustouflant. Cette gemme est constituée d'un rouge transcendant.");
    Item::setShape();
}

RubisOre::~RubisOre()
{

}




WolfPelt::WolfPelt():
    MonsterMaterial ("Fourure de loup", QPixmap(":/monsters/wolf/wolf_pelt.png"), 5, 20)
{
    mIdentifier = MONSTERMATERIAL_WOLF_PELT;
    setInformation("Fourrure de qualité. C'est une ressource très recherchée par les marchants.");
    Item::setShape();
}

WolfPelt::~WolfPelt()
{

}

WolfFang::WolfFang():
    MonsterMaterial ("Croc de loup", QPixmap(":/monsters/wolf/wolf_fang.png"), 1, 7)
{
    mIdentifier = MONSTERMATERIAL_WOLF_FANG;
    setInformation("Croc de loup adapté pour déchirer la chair.");
    Item::setShape();
}

WolfFang::~WolfFang()
{

}

WolfMeat::WolfMeat():
    MonsterMaterial ("Viande de loup", QPixmap(":/monsters/wolf/wolf_meat.png"), 5, 12)
{
    mIdentifier = MONSTERMATERIAL_WOLF_MEAT;
    setInformation("Morceau de viande provenant d'un loup.");
    Item::setShape();
}

WolfMeat::~WolfMeat()
{

}

GoblinEar::GoblinEar():
    MonsterMaterial ("Oreille de gobelin", QPixmap(":/monsters/goblin/goblin_ear.png"), 3, 5)
{
    mIdentifier = MONSTERMATERIAL_GOBLIN_EAR;
    setInformation("Oreille de gobelin ayant très peu d'utilité.");
    Item::setShape();
}

GoblinEar::~GoblinEar()
{

}

GoblinBones::GoblinBones():
    MonsterMaterial ("Os de gobelin", QPixmap(":/monsters/goblin/goblin_bones.png"), 3, 6)
{
    mIdentifier = MONSTERMATERIAL_GOBLIN_BONES;
    setInformation("Os de gobelin pouvant servir à la création de babioles.");
    Item::setShape();
}

GoblinBones::~GoblinBones()
{

}

BearPelt::BearPelt():
    MonsterMaterial ("Fourrure d'ours", QPixmap(":/monsters/bear/bear_pelt.png"), 10, 55)
{
    mIdentifier = MONSTERMATERIAL_BEAR_PELT;
    setInformation("Fourrure de qualité. C'est une ressource très recherchée par les marchants.");
    Item::setShape();
}

BearPelt::~BearPelt()
{

}

BearMeat::BearMeat():
    MonsterMaterial ("Viande d'ours", QPixmap(":/monsters/bear/bear_meat.png"), 5, 35)
{
    mIdentifier = MONSTERMATERIAL_BEAR_MEAT;
    setInformation("Morceau de viande provenant d'un ours.");
    Item::setShape();
}

BearMeat::~BearMeat()
{

}

BearClaw::BearClaw():
    MonsterMaterial ("Griffe d'ours", QPixmap(":/monsters/bear/bear_claw.png"), 5, 15)
{
    mIdentifier = MONSTERMATERIAL_BEAR_TALON;
    setInformation("Griffe d'ours aussi tranchante qu'un rasoir.");
    Item::setShape();
}

BearClaw::~BearClaw()
{

}

TrollMeat::TrollMeat():
    MonsterMaterial("Viande de troll", QPixmap(":/monsters/troll/troll_meat.png"), 5, 2)
{
    mIdentifier = MONSTERMATERIAL_TROLL_MEAT;
    setInformation("Viande répugnante.");
    Item::setShape();
}

TrollMeat::~TrollMeat()
{

}

TrollSkull::TrollSkull():
    MonsterMaterial("Crâne de troll", QPixmap(":/monsters/troll/troll_skull.png"), 6, 10)
{
    mIdentifier = MONSTERMATERIAL_TROLL_SKULL;
    setInformation("Crâne de troll qui saura trouver preneur chez un collectionneur.");
    Item::setShape();
}

TrollSkull::~TrollSkull()
{

}

OggreSkull::OggreSkull():
    MonsterMaterial("Crâne de troll", QPixmap(":/monsters/oggre/oggre_skull.png"), 6, 20)
{
    mIdentifier = MONSTERMATERIAL_OGGRE_SKULL;
    setInformation("Crâne d'oggre qui saura trouver preneur chez un collectionneur.");
    Item::setShape();
}

OggreSkull::~OggreSkull()
{

}

WolfAlphaPelt::WolfAlphaPelt():
    MonsterMaterial("Fourrure\nde loup Alpha", QPixmap(":/monsters/wolf/wolfAlpha_pelt.png"), 5, 40)
{
    mIdentifier = MONSTERMATERIAL_WOLFALPHA_PELT;
    setInformation("Fourrure de loup alpha. C'est une ressource très recherchée par les marchants.");
    Item::setShape();
}

WolfAlphaPelt::~WolfAlphaPelt()
{

}

LaoshanlungHeart::LaoshanlungHeart():
    MonsterMaterial("Coeur de\nLao Shan Lung", QPixmap(":/monsters/laoshanlung/laoshanlung_heart.png"), 20, 500)
{
    mIdentifier = MONSTERMATERIAL_LAOSHANLUNG_HEART;
    setInformation("Coeur de Lao Shan Lung. Composant d'une valeur inestimable.");
    Item::setShape();
}

LaoshanlungHeart::~LaoshanlungHeart()
{

}

EarthCristal::EarthCristal():
    Material("Cristal de terre", QPixmap(":/materials/earthCristal.png"), 20, 80)
{
    mIdentifier = EARTH_CRISTAL;
    setInformation("Pierre rare contenant une puissante énergie terrestre");
    Item::setShape();
}

EarthCristal::~EarthCristal()
{

}
