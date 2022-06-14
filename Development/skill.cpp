#include "skill.h"


Skill::Skill(uint8_t index, uint32_t who, QString name, uint8_t unlockPoints, QString resume, bool unlock, QPixmap imageLock, QPixmap imageUnlock):
    QObject(),
    mIndex(index),
    mLearnable(who),
    mName(name),
    mUnlockPoints(unlockPoints),
    mResume(resume),
    mUnlock(unlock),
    mImageLock(imageLock),
    mImageUnlock(imageUnlock)
{

}

Skill::~Skill()
{

}

const uint8_t &Skill::getIndex() const
{
    return mIndex;
}

const uint32_t &Skill::getLearnable() const
{
    return mLearnable;
}

const QString &Skill::getName() const
{
    return mName;
}

const uint8_t &Skill::getUnlockPoints() const
{
    return mUnlockPoints;
}

const QString &Skill::getResume() const
{
    return mResume;
}

const bool &Skill::isUnlock() const
{
    return mUnlock;
}

const QPixmap Skill::getImage(uint8_t index)
{
    switch(index)
    {
        case 0:
            return mImageLock;

        case 1:
            return mImageUnlock;
    }

    return mImageLock;
}

void Skill::setIndex(uint8_t index)
{
    mIndex = index;
}

void Skill::setLearnable(uint32_t who)
{
    mLearnable = who;
}

void Skill::setName(QString name)
{
    mName = name;
}

void Skill::setUnlockPoints(uint8_t points)
{
    mUnlockPoints = points;
}

void Skill::setResume(QString resume)
{
    mResume = resume;
}

void Skill::unlockSkill()
{
    mUnlock = true;
    emit sig_skillUnlock(this);
}







PassiveSkill::PassiveSkill(uint8_t index, uint32_t who, QString name, uint8_t unlockPoints, QString resume, bool unlock, QPixmap imageLock, QPixmap imageUnlock):
    Skill(index, who, name, unlockPoints, resume, unlock, imageLock, imageUnlock)
{

}

PassiveSkill::~PassiveSkill()
{

}







SpellSkill::SpellSkill(uint8_t index, uint32_t who, QString name, uint8_t unlockPoints, QString resume, bool unlock, QPixmap imageLock, QPixmap imageUnlock, uint16_t manaCost, uint16_t capacity, QString capacityLabel):
    Skill(index, who, name, unlockPoints, resume, unlock, imageLock, imageUnlock),
    mManaCost(manaCost),
    mCapacity(capacity),
    mCapacityLabel(capacityLabel)
{

}

SpellSkill::~SpellSkill()
{

}

uint16_t SpellSkill::getManaCost()
{
    return mManaCost;
}

uint16_t SpellSkill::getCapacity()
{
    return mCapacity;
}

QString SpellSkill::getCapacityLabel()
{
    return mCapacityLabel;
}

void SpellSkill::setManaCost(uint16_t cost)
{
    mManaCost = cost;
}

void SpellSkill::setCapacity(QString label)
{
    mCapacityLabel = label;
}
