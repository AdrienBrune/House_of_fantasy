#ifndef SKILL_H
#define SKILL_H

#include <QObject>
#include <QPixmap>

class Skill : public QObject
{
    Q_OBJECT
public:
    Skill(uint8_t, uint32_t, QString, uint8_t, QString, bool, QPixmap, QPixmap);
    virtual ~Skill();

signals:
    void sig_skillUnlock(Skill*);

public:
    const uint8_t & getIndex()const;
    const uint32_t & getLearnable()const;
    const QString & getName()const;
    const uint8_t & getUnlockPoints()const;
    const QString & getResume()const;
    const bool & isUnlock()const;
    const QPixmap getImage(uint8_t);

    void setIndex(uint8_t);
    void setLearnable(uint32_t);
    void setName(QString);
    void setUnlockPoints(uint8_t);
    void setResume(QString);
    void unlockSkill();

protected:
    uint8_t mIndex;
    uint32_t mLearnable;
    QString mName;
    uint8_t mUnlockPoints;
    QString mResume;
    bool mUnlock;
    QPixmap mImageLock, mImageUnlock;
};






class PassiveSkill : public Skill
{
public:
    enum PassiveSkillNames{
        ForceOfNature,
       InnateRobustness,
       GodBenediction,
       OutstandingMerchant,
       LuckFactor,
       Gemologist,
       ConfirmedFisherman,
       SkinningExpert,
       ManaCollector,
       LifeCollector,
       MageApprentice,
       Archmage,
       NbSkills
    };

public:
    PassiveSkill(uint8_t, uint32_t, QString, uint8_t, QString, bool, QPixmap, QPixmap);
    ~PassiveSkill();
};






class SpellSkill : public Skill
{
public:
    enum SpellNames{
        FireBall,
        HealingHalo,
        Smoke,
        PrimitiveShield,
        DeathTouch,
        Poisoning,
        Benediction,
        Confusion,
        NbSpells
    };

public:
    SpellSkill(uint8_t, uint32_t, QString, uint8_t, QString, bool, QPixmap, QPixmap, uint16_t, uint16_t, QString);
    ~SpellSkill();

public:
    uint16_t getManaCost();
    uint16_t getCapacity();
    QString getCapacityLabel();

    void setManaCost(uint16_t);
    void setCapacity(QString);

private:
    uint16_t mManaCost;
    uint16_t mCapacity;
    QString mCapacityLabel;
};

#endif // SKILL_H
