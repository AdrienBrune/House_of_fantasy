
#ifndef HERO_H
#define HERO_H

#include "character.h"
#include "equipment.h"
#include "bag.h"
#include "skill.h"

extern bool gEnableMovementWithMouseClic;

enum{ KEY_MASK_Z = 0x1, KEY_MASK_S = 0x2, KEY_MASK_D = 0x4, KEY_MASK_Q = 0x8, KEY_MASK_NB = 0x16 };


class Map;

class Hero : public Character
{
    Q_OBJECT

public:
    enum HeroClasses{
        eSwordman,
        eArcher,
        eWizard,
        eNbHeroClasses
    };
    enum SkillStepEnum{
        life,
        mana,
        stamina,
        Strength,
        NbSkillSteps
    };

    struct HeroCaracteristics{
        QString name;
        int life;
        int mana;
        int stamina;
        int coin;
        int skillPoints;
        Weapon * weapon;
        SkillStep steps;
    };

    struct HeroMovementHandler{
        qreal angle = 180;
        QPointF destPos = QPointF();
        QTimer * t_move = nullptr;
        QPointF lastPos = QPointF();
        int movementMask = 0;
    };

    struct Experience{
        int points;
        int level;
        int pointsToLevelUp;
    };

    struct AdventurerMap{
        MapScroll map;
        bool unlocked;
    };

public:
    Hero();
    virtual ~Hero();
signals:
    void sig_equipmentChanged();
    void sig_bagEvent();
    void sig_bagFull();
    void sig_coinChanged();
    void sig_experienceChanged();
    void sig_skillPointsChanged();
    void sig_skillUnlocked();
    void sig_heroMoved();
    void sig_showHeroData();
    void sig_ThrowItem(Item*);
    void sig_SellItem(Item*);
    void sig_dodge();
    void sig_playSound(int);
    void sig_enterMapEvent(Tool * tool);
    void sig_leaveMapEvent();
public slots:
    void nextMovement();
private slots:
    void move();
public:
    QPointF getLocation();
    Bag * getBag();
    Gear * getGear();
    AdventurerMap & getAdventurerMap() { return mAdventurerMap; }
    int getCoin();
    Experience getExperience();
    int getSkillPoints();
    Skill* getPassiveSkill(int index);
    Skill* getSpellSkill(int index);
    PassiveSkill ** getSkillList();
    SpellSkill ** getSpellList();
    virtual int getHeroClass() { return mClass; };

    void setLocation(QPointF);
    void setGear(Gear*);
    void setCoin(int);
    void setIsInVillage(bool);
    void setSkillPoints(int);

public:
    void unlockAdventurerMap() { mAdventurerMap.unlocked = true; }
    void useConsumable(Consumable*);
    void useScroll(Scroll*);
    bool takeItem(Item*);
    qreal calculateDamage();
    int getAttackSpeed();
    void takeDamage(int);
    void addCoin(int);
    bool removeCoin(int);
    bool addExperience(int);
    bool learnSkill(Skill*);
    bool learnPassiveSkill(int);
    void levelUpHero();
    void startMovingTo(int,int);
    void stopMoving();
    void interactWithPNG(bool);
    bool isInteractingWithPNG();
    bool isInVillage();
    bool isDead();
    void freeze(bool);
    bool isFreeze();
    void checkMapInteractions();
    void setupFight(bool setup) override
    {
        if(setup) // Enter fight
        {
            if(getSkillList()[PassiveSkill::GodBenediction]->isUnlock())
                setup ? applyStatus(Character::eStatus::heal) : removeStatus(Character::eStatus::heal);
        }
        else // Leave fight
        {
            removeStatus(Character::eStatus::shield);
            removeStatus(Character::eStatus::benediction);
            removeStatus(Character::eStatus::confused);
            removeStatus(Character::eStatus::poisoned);
            removeStatus(Character::eStatus::heal);
        }
    }

public:
    virtual void serialize(QDataStream& stream) const
    {
        stream << mClass
               << mName
               << mMoveHandler.lastPos
               << mLife.current << mLife.maximum
               << mMana.current << mMana.maximum
               << mStamina.current << mStamina.maximum
               << mCoin
               << mExperience.level << mExperience.points << mExperience.pointsToLevelUp
               << mSkillPoints;
        for(int i = 0; i < PassiveSkill::NbSkills; i++)
        {
            stream << mSkillList[i]->isUnlock();
        }
        for(int i = 0; i < SpellSkill::NbSpells; i++)
        {
            stream << mSpellList[i]->isUnlock();
        }
        stream << mAdventurerMap.unlocked;
        mBag->serialize(stream);
        mGear->serialize(stream);

        DEBUG("SERIALIZED[in] : Hero");
    }
    virtual void deserialize(QDataStream& stream)
    {
        QPointF location(0,0);
        stream >> mName
               >> location
               >> mLife.current >> mLife.maximum
               >> mMana.current >> mMana.maximum
               >> mStamina.current >> mStamina.maximum
               >> mCoin
               >> mExperience.level >> mExperience.points >> mExperience.pointsToLevelUp
               >> mSkillPoints;
        for(int i = 0; i < PassiveSkill::NbSkills; i++)
        {
            bool isUnlock = false;
            stream >> isUnlock;
            if(isUnlock)
                mSkillList[i]->unlockSkill();
        }
        for(int i = 0; i < SpellSkill::NbSpells; i++)
        {
            bool isUnlock = false;
            stream >> isUnlock;
            if(isUnlock)
                mSpellList[i]->unlockSkill();
        }
        stream >> mAdventurerMap.unlocked;
        mBag->deserialize(stream);
        mGear->deserialize(stream);

        setLocation(location);

        DEBUG("SERIALIZED[out] : Hero");
    }
    friend QDataStream& operator<<(QDataStream& stream, const Hero& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Hero& object)
    {
        object.deserialize(stream);
        return stream;
    }

public:
    static Hero * getInstance(Hero::HeroClasses);

public:
    HeroMovementHandler mMoveHandler;

protected:
    Bag * mBag;
    Gear * mGear;
    AdventurerMap mAdventurerMap;
    int mCoin;
    Experience mExperience;
    int mImageSelected;
    bool mInteractionPNG;
    bool mIsInVillage;
    bool mFreeze;
    int mSkillPoints;
    quint8 mClass;
    bool mIsInMapEvent;
    HeroCaracteristics mHeroList[Hero::eNbHeroClasses] = {
        { "Maphistos", 200, 20, 160, 10, 5, new Staff("baton", QPixmap(":/equipment/sword_stick.png"), 5, 8, 5, 1, "Bâton en bois", ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard)), {6, 1, 4, 8} },
        { "Sophia", 120, 120, 240, 10, 5, new Bow("bow", QPixmap(":/equipment/sword_stick.png"), 12, 8, 5, 1, "Arc de fortune", ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard)), {3, 3, 7, 3} },
        { "Archangelie", 80, 250, 120, 10, 12, new Staff("baton", QPixmap(":/equipment/sword_stick.png"), 5, 8, 5, 1, "Bâton en boid", ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard)), {2, 8, 3, 5} }
    };
    PassiveSkill* mSkillList[PassiveSkill::NbSkills] = {
        new PassiveSkill( PassiveSkill::ForceOfNature, ABLE(eSwordman), "Force de la nature", 2, "Accorde un pourcentage de chance de frapper plus fort en combat", false, QPixmap(":/icons/skill/skill00.png"), QPixmap(":/icons/skill/skill01.png")) ,
        new PassiveSkill( PassiveSkill::InnateRobustness, ABLE(eSwordman), "Robustesse innée", 2, "En combat, les dommages reçus seront parfois atténués", false, QPixmap(":/icons/skill/skill10.png"), QPixmap(":/icons/skill/skill11.png")),
        new PassiveSkill( PassiveSkill::GodBenediction, ABLE(eSwordman)|ABLE(eArcher), "Bénédiction de Dieux", 10, "Accorde parfois un léger soin pendant les combats", false, QPixmap(":/icons/skill/skill20.png"), QPixmap(":/icons/skill/skill21.png")),
        new PassiveSkill( PassiveSkill::OutstandingMerchant, ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard), "Marchand hors pair", 8, "Permet de revendre vos objets plus chère chez le marchant", false, QPixmap(":/icons/skill/skill30.png"), QPixmap(":/icons/skill/skill31.png")),
        new PassiveSkill( PassiveSkill::LuckFactor, ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard), "Facteur chance", 2, "Les objets dans les coffres seront présent en plus grand nombre", false, QPixmap(":/icons/skill/skill40.png"), QPixmap(":/icons/skill/skill41.png")),
        new PassiveSkill( PassiveSkill::Gemologist, ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard), "Gemmologue", 2, "Vous avez des chances de récolter des pierres précieuses sur n’importe quel filon de minerai", false, QPixmap(":/icons/skill/skill50.png"), QPixmap(":/icons/skill/skill51.png")),
        new PassiveSkill( PassiveSkill::ConfirmedFisherman, ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard), "Pêcheur confirmé", 2, "Les poissons seront plus nombreux aux points de pêche", false, QPixmap(":/icons/skill/skill60.png"), QPixmap(":/icons/skill/skill61.png")),
        new PassiveSkill( PassiveSkill::SkinningExpert, ABLE(eSwordman)|ABLE(eArcher)|ABLE(eWizard), "Expert en dépeçage", 2, "Vous obtiendrez plus de composants de monstre", false, QPixmap(":/icons/skill/skill70.png"), QPixmap(":/icons/skill/skill71.png")),
        new PassiveSkill( PassiveSkill::ManaCollector, ABLE(eWizard), "Collecteur de mana", 15, "Vous puisez en continue le mana ambiant, ce qui vous restore votre mana sur la durée", false, QPixmap(":/icons/skill/skill80.png"), QPixmap(":/icons/skill/skill81.png")),
        new PassiveSkill( PassiveSkill::LifeCollector, ABLE(eWizard), "Collecteur de vie", 15, "Vous puisez l’énergie vitale ambiante, ce qui vous restore de la vie sur la durée", false, QPixmap(":/icons/skill/skill90.png"), QPixmap(":/icons/skill/skill91.png")),
        new PassiveSkill( PassiveSkill::MageApprentice, ABLE(eArcher)|ABLE(eWizard), "Apprenti mage", 5, "Permet d’apprendre des sorts basiques", false, QPixmap(":/icons/skill/skill100.png"), QPixmap(":/icons/skill/skill101.png")),
        new PassiveSkill( PassiveSkill::Archmage, ABLE(eWizard), "Archimage", 20, "Permet d’apprendre des sorts de haut niveau", false, QPixmap(":/icons/skill/skill110.png"), QPixmap(":/icons/skill/skill111.png") )
    };

    SpellSkill* mSpellList[SpellSkill::NbSpells] = {
        new SpellSkill( SpellSkill::FireBall, ABLE(eWizard), "Boule de feu", 4, "Lance une boule de feu sur l’ennemi", false, QPixmap(":/icons/skill/spell00.png"), QPixmap(":/icons/skill/spell01.png"), 20, 100, "Dégâts" ),
        new SpellSkill( SpellSkill::HealingHalo, ABLE(eArcher)|ABLE(eWizard), "Halo de guérison", 4, "Vous soigne légèrement de vos blessures", false, QPixmap(":/icons/skill/spell10.png"), QPixmap(":/icons/skill/spell11.png"), 30, 30, "Soin" ),
        new SpellSkill( SpellSkill::Smoke, ABLE(eArcher)|ABLE(eWizard), "Nuage de fumée", 4, "Permet de fuir un combat", false, QPixmap(":/icons/skill/spell20.png"), QPixmap(":/icons/skill/spell21.png"), 5, 100, "Taux de réussite" ),
        new SpellSkill( SpellSkill::PrimitiveShield, ABLE(eWizard), "Bouclier primitif", 4, "Augmente votre résistance pour le combat en cours", false, QPixmap(":/icons/skill/spell30.png"), QPixmap(":/icons/skill/spell31.png"), 20, 10, "Résistance(%)" ),
        new SpellSkill( SpellSkill::DeathTouch, ABLE(eWizard), "Touché de la mort", 8, "Sort mortel pour l’ennemi mais avec peu de chance de réussite", false, QPixmap(":/icons/skill/spell40.png"), QPixmap(":/icons/skill/spell41.png"), 100, 20, "Taux de réussite" ),
        new SpellSkill( SpellSkill::Poisoning, ABLE(eWizard), "Empoisonnement", 7, "Sort d’empoisonnement qui fait des dégâts sur la durée", false, QPixmap(":/icons/skill/spell50.png"), QPixmap(":/icons/skill/spell51.png"), 40, 90, "Taux de réussite" ),
        new SpellSkill( SpellSkill::Benediction, ABLE(eWizard), "Bénédiction", 5, "Sort permettant d’encaisser les trois prochaines attaques sans dégâts", false, QPixmap(":/icons/skill/spell60.png"), QPixmap(":/icons/skill/spell61.png"), 100, 3, "Tours d'invincibilité" ),
        new SpellSkill( SpellSkill::Confusion, ABLE(eWizard), "Confusion", 10, "Réduit drastiquement le taux d’attaque de l’ennemi", false, QPixmap(":/icons/skill/spell70.png"), QPixmap(":/icons/skill/spell71.png"), 60, 70, "Taux de réussite" )
    };

public:
    QTimer * t_movement;
};


class SwordMan : public Hero
{
    Q_OBJECT

public:
    SwordMan(QString);
    ~SwordMan();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    void serialize(QDataStream& stream) const override
    {
        Hero::serialize(stream);
        DEBUG("SERIALIZED[in]  : Swordman");
    }
    void deserialize(QDataStream& stream) override
    {
        Hero::deserialize(stream);
        DEBUG("SERIALIZED[out] : Swordman");
    }
};


class Archer : public Hero
{
    Q_OBJECT

public:
    Archer(QString);
    ~Archer();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:

    void serialize(QDataStream& stream) const override
    {
        Hero::serialize(stream);
        DEBUG("SERIALIZED[in]  : Archer");
    }
    void deserialize(QDataStream& stream) override
    {
        Hero::deserialize(stream);
        DEBUG("SERIALIZED[out] : Archer");
    }
};






class Wizard : public Hero
{
    Q_OBJECT

public:
    Wizard(QString);
    ~Wizard();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:

    void serialize(QDataStream& stream) const override
    {
        Hero::serialize(stream);
        DEBUG("SERIALIZED[in]  : Wizard");
    }
    void deserialize(QDataStream& stream) override
    {
        Hero::deserialize(stream);
        DEBUG("SERIALIZED[out] : Wizard");
    }
};


#endif // HERO_H
