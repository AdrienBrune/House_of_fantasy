#ifndef W_POTIONCOOKINGSLOT_H
#define W_POTIONCOOKINGSLOT_H

#include <QObject>
#include <QComboBox>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "consumable.h"
#include "common.h"

class PotionCookingSlot : public QObject
{
    Q_OBJECT

public:
    explicit PotionCookingSlot(QObject * parent = nullptr):
        QObject(parent),
        mLock(false),
        mCooking(false),
        mInQueue(false),
        mCookTime(Gauge{0,0}),
        mPotion(nullptr)
    {
        mTimer.setInterval(1000);
        connect(&mTimer, &QTimer::timeout, this, &PotionCookingSlot::onPotionCookingTick);
    }
    ~PotionCookingSlot()
    {
        if(mPotion)
            delete mPotion;
    }

signals:
    void sig_cookingDone(PotionCookingSlot * slot);
    void sig_cookingRequested(PotionCookingSlot * slot);
    void sig_updateDisplay();
    void sig_unlock(bool);

public slots:
    void onPotionSelected(const QString &potionName)
    {
        if(potionName == QString("Potion de vie"))
        {
            mCookTime.current = 100;
            mCookTime.maximum = 100;
            mPotion = new PotionLife();
        }
        else if(potionName == QString("Potion de mana"))
        {
            mCookTime.current = 120;
            mCookTime.maximum = 120;
            mPotion = new PotionMana();
        }
        else if(potionName == QString("Potion de vitalité"))
        {
            mCookTime.current = 220;
            mCookTime.maximum = 220;
            mPotion = new PotionStamina();
        }
        else if(potionName == QString("Potion de force"))
        {
            mCookTime.current = 200;
            mCookTime.maximum = 200;
            mPotion = new PotionStrenght();
        }
        else if(potionName == QString("Potion ancienne"))
        {
            mCookTime.current = 260;
            mCookTime.maximum = 260;
            mPotion = new PotionKnowledge();
        }
        else
        {
            qDebug() << "unknown potion name !!";
            return;
        }

        setLocked(true);
        mTimer.start();
        emit sig_updateDisplay();
        emit sig_cookingRequested(this);
    }

private slots:
    void onPotionCookingTick()
    {
        if(!mCooking)
            return;

        if(mCookTime.current == 0)
        {
            mTimer.stop();
            mCooking = false;
            emit sig_cookingDone(this);
            setLocked(false);
        }
        else
            mCookTime.current--;

        emit sig_updateDisplay();
    }

public:
    void setInQueue(bool enable) { mInQueue = enable; }
    void setCooking(bool enable) { mCooking = enable; mTimer.start(); }

    bool getInQueue() { return mInQueue; }
    bool getLocked() { return mLock; }
    bool getCooking() { return mCooking; }
    Gauge getCookTime() { return mCookTime; }
    Consumable * getPotion() { return mPotion; }

    Consumable * takePotion()
    {
        Consumable * potion = mPotion;
        mPotion = nullptr;
        return potion;
    }

private:
    void setLocked(bool lock) { emit sig_unlock(!lock); mLock = lock; }

public:
    void serialize(QDataStream& stream)const
    {
        quint16 identifier = mPotion ? mPotion->getIdentifier() : 0;

        stream << static_cast<quint8>(mLock);
        stream << static_cast<quint8>(mCooking);
        stream << static_cast<quint8>(mInQueue);
        stream << static_cast<quint16>(mCookTime.current) << static_cast<quint16>(mCookTime.maximum);
        stream << identifier;
        if(mPotion)
            mPotion->serialize(stream);
        DEBUG("SERIALIZED[in]  : PotionCookingSlot");
    }

    void deserialize(QDataStream& stream)
    {
        // Remove attributes
        if(mPotion)
            delete mPotion;
        mPotion = nullptr;

        quint8 data_8;
        quint16 current, max;
        quint16 identifier;

        stream >> data_8; mLock = data_8;
        stream >> data_8; mCooking = data_8;
        stream >> data_8; mInQueue = data_8;
        stream >> current; mCookTime.current = current;
        stream >> max; mCookTime.maximum = max;
        stream >> identifier;
        if(identifier)
        {
            mPotion = dynamic_cast<Consumable*>(Item::Factory(identifier));
            mPotion->deserialize(stream);
        }
        if(mCooking)
            mTimer.start();
        setLocked(mLock);
        DEBUG("SERIALIZED[out] : PotionCookingSlot");
    }
    friend QDataStream& operator<<(QDataStream& stream, const PotionCookingSlot& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, PotionCookingSlot& object)
    {
        object.deserialize(stream);
        return stream;
    }
    inline void toJson(QJsonObject &json) const
    {
        json["lock"] = mLock;
        json["cooking"] = mCooking;
        json["queue"] = mInQueue;

        QJsonObject jsonCook;
        jsonCook["current"] = mCookTime.current;
        jsonCook["maximum"] = mCookTime.maximum;
        json["cook"] = jsonCook;

        json.remove("potion");
        if (mPotion)
        {
            QJsonObject jsonItem;
            mPotion->toJson(jsonItem);
            json["potion"] = jsonItem;
        }
    }
    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("lock") && json["lock"].isBool())
        {
            mLock = json["lock"].toBool();
        }
        if (json.contains("cooking") && json["cooking"].isBool())
        {
            mCooking = json["cooking"].toBool();
        }
        if (json.contains("queue") && json["queue"].isBool())
        {
            mInQueue = json["queue"].toBool();
        }

        if (json.contains("cook") && json["cook"].isObject())
        {
            QJsonObject jsonCook = json["cook"].toObject();
            if (jsonCook.contains("current") && jsonCook["current"].isDouble())
            {
                mCookTime.current = jsonCook["current"].toInt();
            }
            if (jsonCook.contains("maximum") && jsonCook["maximum"].isDouble())
            {
                mCookTime.maximum = jsonCook["maximum"].toInt();
            }
        }

        if (json.contains("potion") && json["potion"].isObject())
        {
            QJsonObject jsonPotion = json["potion"].toObject();
            if (jsonPotion.contains("type") && jsonPotion["type"].isDouble())
            {
                Item* item = Item::Factory(jsonPotion["type"].toInt());
                Consumable* consumable = dynamic_cast<Consumable*>(item);
                if (consumable)
                {
                    consumable->fromJson(jsonPotion);
                    mPotion = consumable;
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                DEBUG("potion type not found, item can't be reconstructed !");
                assert(false);
            }
        }

        if(mCooking)
        {
            mTimer.start();
        }
        setLocked(mLock);
    }

private:
    bool mLock;
    bool mCooking;
    bool mInQueue;
    Gauge mCookTime;
    Consumable * mPotion;
    QTimer mTimer;
};

class W_PotionCookingSlot : public QComboBox
{
    Q_OBJECT

public:
    explicit W_PotionCookingSlot(QWidget * parent = nullptr):
        QComboBox(parent),
        mSlot(nullptr)
    {
        addItem(PotionLife().getImage(), PotionLife().getName());
        addItem(PotionMana().getImage(), PotionMana().getName());
        addItem(PotionStamina().getImage(), PotionStamina().getName());
        addItem(PotionStrenght().getImage(), PotionStrenght().getName());
        addItem(PotionKnowledge().getImage(), PotionKnowledge().getName());
    }
    ~W_PotionCookingSlot() { unregisterSlot(); }

private slots:
    void onUpdate() { update(); }

public:
    void registerSlot(PotionCookingSlot *slot)
    {
        assert(slot);
        mSlot = slot;
        connect(this, &QComboBox::textActivated, mSlot, &PotionCookingSlot::onPotionSelected);
        connect(mSlot, &PotionCookingSlot::sig_updateDisplay, this, &W_PotionCookingSlot::onUpdate);
        connect(mSlot, &PotionCookingSlot::sig_unlock, this, &QWidget::setEnabled);
        setEnabled(!mSlot->getLocked());
    }
    void unregisterSlot()
    {
        if(mSlot)
        {
            disconnect(this, &QComboBox::textActivated, mSlot, &PotionCookingSlot::onPotionSelected);
            disconnect(mSlot, &PotionCookingSlot::sig_updateDisplay, this, &W_PotionCookingSlot::onUpdate);
        }
        mSlot = nullptr;
    }

protected:
    void paintEvent(QPaintEvent*);

private:
    PotionCookingSlot *mSlot;
};

#endif // W_POTIONCOOKINGSLOT_H
