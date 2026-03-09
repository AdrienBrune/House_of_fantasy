#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "mapevent.h"
#include "toolfunctions.h"
#include "skill.h"
#include "common.h"

class Character : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    struct SkillStep{
        int life;
        int mana;
        int stamina;
        int strength;
    };

    struct Experience{
        int points;
        int level;
        int pointsToLevelUp;
    };

    enum class eStatus
    {
        poisoned,
        confused,
        benediction,
        shield,
        heal
    };

public:
    Character();
    virtual ~Character();
signals:
    void sig_movedInBush(Bush*);
    void sig_movedInBushEvent(BushEvent*);
    void sig_lifeChanged();
    void sig_manaChanged();
    void sig_nameChanged();
    void sig_staminaMaxChanged();
    void sig_statusChanged();
public slots:
    void setNextFrame();
public:
    QString getName();
    Gauge getLife();
    Gauge getMana();
    Gauge getStamina();
    QPixmap getImage();

    void setName(QString);
    void setLife(int);
    void setLifeMax(int);
    void setMana(int);
    void setManaMax(int);
    void setStamina(int);
    void setStaminaMax(int);
    inline void setPos(qreal x, qreal y)
    {
        QGraphicsItem::setPos(x, y);
        mPositionInMap = QPointF(x, y);
    }
    inline void setPos(QPointF position)
    {
        QGraphicsItem::setPos(position);
        mPositionInMap = position;
    }

    virtual void setupFight(bool setup)=0;
    void applyStatus(eStatus status, QVariant value = QVariant());
    void removeStatus(eStatus status);
    bool isApplied(eStatus status) const { return mStatus.contains(status); }
    QVariant getStatus(eStatus status) const { return mStatus.value(status, QVariant()); }
    bool updateStatus(eStatus status, QVariant value)
    {
        if(mStatus.contains(status))
        {
            mStatus[status] = value;
            return true;
        }
        return false;
    }

    QPainterPath shape() const;
    QRectF boundingRect() const;

    virtual bool isDead()=0;

    inline virtual void toJson(QJsonObject &json) const
    {
        json["name"] = mName;

        QJsonObject jsonLife;
        jsonLife["current"] = mLife.current;
        jsonLife["maximum"] = mLife.maximum;
        json["life"] = jsonLife;

        QJsonObject jsonMana;
        jsonMana["current"] = mMana.current;
        jsonMana["maximum"] = mMana.maximum;
        json["mana"] = jsonMana;

        QJsonObject jsonStamina;
        jsonStamina["current"] = mStamina.current;
        jsonStamina["maximum"] = mStamina.maximum;
        json["stamina"] = jsonStamina;

        QJsonObject jsonExperience;
        jsonExperience["current"] = mExperience.points;
        jsonExperience["maximum"] = mExperience.pointsToLevelUp;
        jsonExperience["level"] = mExperience.level;
        json["experience"] = jsonExperience;

        json["coin"] = mCoin;

        QJsonObject jsonPosition;
        jsonPosition["x"] = mPositionInMap.x();
        jsonPosition["y"] = mPositionInMap.y();
        json["position"] = jsonPosition;
    }

    inline virtual void fromJson(const QJsonObject &json)
    {
        if (json.contains("name") && json["name"].isString())
        {
            mName = json["name"].toString();
        }

        if (json.contains("life") && json["life"].isObject())
        {
            QJsonObject jsonLife = json["life"].toObject();
            if (jsonLife.contains("current") && jsonLife["current"].isDouble())
            {
                mLife.current = jsonLife["current"].toInt();
            }
            if (jsonLife.contains("maximum") && jsonLife["maximum"].isDouble())
            {
                mLife.maximum = jsonLife["maximum"].toInt();
            }
        }

        if (json.contains("mana") && json["mana"].isObject())
        {
            QJsonObject jsonMana = json["mana"].toObject();
            if (jsonMana.contains("current") && jsonMana["current"].isDouble())
            {
                mMana.current = jsonMana["current"].toInt();
            }
            if (jsonMana.contains("maximum") && jsonMana["maximum"].isDouble())
            {
                mMana.maximum = jsonMana["maximum"].toInt();
            }
        }

        if (json.contains("stamina") && json["stamina"].isObject())
        {
            QJsonObject jsonStamina = json["stamina"].toObject();
            if (jsonStamina.contains("current") && jsonStamina["current"].isDouble())
            {
                mStamina.current = jsonStamina["current"].toInt();
            }
            if (jsonStamina.contains("maximum") && jsonStamina["maximum"].isDouble())
            {
                mStamina.maximum = jsonStamina["maximum"].toInt();
            }
        }

        if (json.contains("experience") && json["experience"].isObject())
        {
            QJsonObject jsonExperience = json["experience"].toObject();
            if (jsonExperience.contains("current") && jsonExperience["current"].isDouble())
            {
                mExperience.points = jsonExperience["current"].toInt();
            }
            if (jsonExperience.contains("maximum") && jsonExperience["maximum"].isDouble())
            {
                mExperience.pointsToLevelUp = jsonExperience["maximum"].toInt();
            }
            if (jsonExperience.contains("level") && jsonExperience["level"].isDouble())
            {
                mExperience.level = jsonExperience["level"].toInt();
            }
        }

        if (json.contains("coin") && json["coin"].isDouble())
        {
            mCoin = json["coin"].toInt();
        }

        if (json.contains("position") && json["position"].isObject()) {
            QJsonObject jsonPosition = json["position"].toObject();
            mPositionInMap.setX(jsonPosition["x"].toDouble());
            mPositionInMap.setY(jsonPosition["y"].toDouble());
        }
        setPos(mPositionInMap);
    }

protected:
    QString mName;
    Gauge mLife;
    Gauge mMana;
    Gauge mStamina;
    Experience mExperience;
    int mCoin;
    QPointF mPositionInMap; // TODO use it !!
    QMap<eStatus, QVariant> mStatus;

    QTimer * t_poisonning;
    QTimer * t_healing;

    QRect mBoundingRect;
    QPainterPath mShape;
    CollisionShape * mCollisionShape;
    QPixmap mImage;
    int mNextFrame;
    int mNumberFrame;
    QPixmap mCurrentPixmap;
    QTimer * t_animation;
};

#endif // CHARACTER_H
