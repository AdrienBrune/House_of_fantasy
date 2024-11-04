#include "character.h"

Character::Character():
    QObject(),
    QGraphicsPixmapItem(),
    mName(QString()),
    mLife({100, 100}),
    mMana({100, 100}),
    mStamina({100, 100}),
    t_poisonning(nullptr),
    mBoundingRect(QRect()),
    mShape(QPainterPath()),
    mCollisionShape(nullptr),
    mImage(QPixmap()),
    mNextFrame(0),
    mNumberFrame(0),
    mCurrentPixmap(QPixmap()),
    t_animation(nullptr)
{

}

QString Character::getName()
{
    return mName;
}

Character::Gauge Character::getLife()
{
    return mLife;
}

Character::Gauge Character::getMana()
{
    return mMana;
}

Character::Gauge Character::getStamina()
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
    bool changed = mLife.current != life ? true : false;

    if(life > mLife.maximum)
        mLife.current = mLife.maximum;
    else if(life < 0)
        mLife.current = 0;
    else
        mLife.current = life;

    if(changed)
        emit sig_lifeChanged();
}

void Character::setLifeMax(int life)
{
    mLife.maximum = life;
}

void Character::setMana(int mana)
{
    bool changed = mLife.current != mana ? true : false;

    if(mana > mMana.maximum)
        mMana.current = mMana.maximum;
    else
        mMana.current = mana;

    if(changed)
        emit sig_manaChanged();
}

void Character::setManaMax(int mana)
{
    mMana.maximum = mana;
}

void Character::setStamina(int stamina)
{
    if(stamina < 0)
        mStamina.current = 0;

    if(stamina > mStamina.maximum)
        mStamina.current = mStamina.maximum;
    else
        mStamina.current = stamina;
}

void Character::setStaminaMax(int stamina)
{
    mStamina.maximum = stamina;
    emit sig_staminaMaxChanged();
}

void Character::applyStatus(eStatus status, QVariant value)
{
    mStatus[status] = value;

    if(status == eStatus::poisoned)
    {
        t_poisonning = new QTimer(this);
        connect(t_poisonning, &QTimer::timeout, this, [&]()
        {
            setLife(mLife.current - mLife.maximum * 0.025);

            mStatus[eStatus::poisoned] = mStatus[eStatus::poisoned].toInt() - 1;
            if(mStatus[eStatus::poisoned].toInt() < 1)
                removeStatus(eStatus::poisoned);
        });
        t_poisonning->start(2000);
    }

    if(status == eStatus::heal)
    {
        t_healing = new QTimer(this);
        connect(t_healing, &QTimer::timeout, this, [&]()
        {
            setLife(mLife.current + 1);
        });
        t_healing->start(1000);
    }

    emit sig_statusChanged();
}

void Character::removeStatus(eStatus status)
{
    mStatus.remove(status);

    if(status == eStatus::poisoned)
    {
        if(t_poisonning)
        {
            t_poisonning->stop();
            delete t_poisonning;
            t_poisonning = nullptr;
        }
    }

    emit sig_statusChanged();
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
    if(mLife.current <= 0)
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
