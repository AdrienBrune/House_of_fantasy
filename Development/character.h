#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include "mapevent.h"
#include "toolfunctions.h"
#include "skill.h"

class Character : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    struct Gauge{
        int current;
        int maximum;
    };

    struct SkillStep{
        int life;
        int mana;
        int stamina;
        int strength;
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

protected:
    QString mName;
    Gauge mLife;
    Gauge mMana;
    Gauge mStamina;
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
