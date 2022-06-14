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
    struct Life{
        int curLife;
        int maxLife;
    };

    struct Mana{
        int curMana;
        int maxMana;
    };

    struct Stamina{
        int curStamina;
        int maxStamina;
    };

    struct SkillStep{
        int life;
        int mana;
        int stamina;
        int strength;
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
public slots:
    void setNextFrame();
public:
    QString getName();
    Life getLife();
    Mana getMana();
    Stamina getStamina();
    QPixmap getImage();

    void setName(QString);
    void setLife(int);
    void setLifeMax(int);
    void setMana(int);
    void setManaMax(int);
    void setStamina(int);
    void setStaminaMax(int);

    QPainterPath shape() const;
    QRectF boundingRect()const;

    virtual bool isDead()=0;
protected:
    QString mName;
    Life mLife;
    Mana mMana;
    Stamina mStamina;

    QRect mBoundingRect;
    QPainterPath mShape;
    QPixmap mImage;
    int mNextFrame;
    int mNumberFrame;
    QPixmap mCurrentPixmap;
    QTimer * t_animation;
    double mSize;
};

#endif // CHARACTER_H
