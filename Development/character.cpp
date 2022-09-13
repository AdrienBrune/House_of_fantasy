#include "character.h"

Character::Character():
    QObject(),
    QGraphicsPixmapItem(),
    mBoundingRect(QRect()),
    mShape(QPainterPath())
{

}

QString Character::getName()
{
    return mName;
}

Character::Life Character::getLife()
{
    return mLife;
}

Character::Mana Character::getMana()
{
    return mMana;
}

Character::Stamina Character::getStamina()
{
    return mStamina;
}

QPixmap Character::getImage()
{
    return mImage;
}

void Character::setName(QString name)
{
    mName = name;
    emit sig_nameChanged();
}

void Character::setLife(int life)
{
    if(life > mLife.maxLife)
        mLife.curLife = mLife.maxLife;
    else if(life < 0)
        mLife.curLife = 0;
    else
        mLife.curLife = life;
    emit sig_lifeChanged();
}

void Character::setLifeMax(int life)
{
    mLife.maxLife = life;
}

void Character::setMana(int mana)
{
    if(mana > mMana.maxMana)
        mMana.curMana = mMana.maxMana;
    else
        mMana.curMana = mana;
    emit sig_manaChanged();
}

void Character::setManaMax(int mana)
{
    mMana.maxMana = mana;
}

void Character::setStamina(int stamina)
{
    if(stamina < 0)
    {
        mStamina.curStamina = 0;
    }
    if(stamina > mStamina.maxStamina)
    {
        mStamina.curStamina = mStamina.maxStamina;
    }
    else
    {
        mStamina.curStamina = stamina;
    }
}

void Character::setStaminaMax(int stamina)
{
    mStamina.maxStamina = stamina;
    emit sig_staminaMaxChanged();
}

QPainterPath Character::shape() const
{
    return mShape;
}

QRectF Character::boundingRect() const
{
    return mBoundingRect;
}

bool Character::isDead()
{
    if(mLife.curLife <= 0)
        return false;
    else {
        return true;
    }
}

void Character::setNextFrame()
{
    mNextFrame++;
    if(mNextFrame >= mNumberFrame){
        mNextFrame = 0;
    }
}

Character::~Character()
{

}
