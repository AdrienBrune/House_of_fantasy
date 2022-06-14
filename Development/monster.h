#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"
#include "village.h"

class Monster : public Character
{
    Q_OBJECT

public:
    struct MovementHandler
    {
        QPointF posBeforeCollision;
        QVector<bool> collision;
        qreal angleOnMap;
        qreal currentangleOnMap;
        qreal angleUseToMove;
    };

    struct ImageHandler{
        QPixmap fightImage_1;
        QPixmap fightImage_2;
        QPixmap walk;
        QPixmap run;
        QPixmap stand;
        QPixmap dead;
        QPixmap skinned;
        QPixmap heavyAttack;
        QPixmap lightAttack;
    };

    struct FramesAvailable{
        int stand;
        int move;
        int run;
        int dead;
        int skinned;
    };

    enum class Action {
        moving,
        stand,
        aggro,
        dead,
        skinned,
        NbActions
    };

    enum Status{
        none = 0, poisoned = 0x01, confused = 0x02, NbStatus
    };

public:
    Monster(QGraphicsView*);
    ~Monster();
signals:
    void sig_monsterEncountered(Monster*);
    void sig_showMonsterData(Monster*);
    void sig_monsterSound(int);
public:
    int getDamage();
    quint32 getStatus();
    qreal getAngle();
    int getThreatLevel();
    Action getAction();
    int getExperience();
    QPixmap getFightImage(int);
    QPixmap getHeavyAttackAnimation();
    QPixmap getLightAttackAnimation();
    int getSoundIndexFor(int);
    ImageHandler getImageHandler();
    void setDamage(int);
    void addStatus(quint32);
    void removeStatus(quint32);
    void setAngle(qreal);
    void rotateSymetry(qreal, qreal);

    bool isStatus(quint32);
    bool isSkinned();
    bool isDead();
    void killMonster();
    bool isInView();
    bool isInBiggerView();
    void enableMonsterAnimation(bool);
public:
    virtual void nextAction(Hero*)=0;
    virtual QList<Item*> skinMonster()=0;
    virtual void addExtraLoots()=0;
protected:
    int getNumberFrame();
    void chooseAction(Hero*);
    void advance(int);
    void doCollision();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    virtual int getSpeed()=0;
    virtual int getBoostedSpeed()=0;
    virtual QList<Item*> generateRandomLoots()=0;
protected:
    QGraphicsView * mView;
    bool mIsInView;
    int mHover;

    int mDamage;
    quint32 mStatus;
    int mThreatLevel;
    int mExperience;

    int mSpeed;
    Action mAction;
    FramesAvailable mFrames;
    MovementHandler mMove;
    ImageHandler mPixmap;
    int mSounds[6];
    QTimer * t_isWalking;

    QList<Item*> mItems;
};


class Wolf : public Monster
{
    Q_OBJECT
public:
    Wolf(QGraphicsView*);
    ~Wolf();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};

class WolfAlpha : public Wolf
{
    Q_OBJECT
public:
    WolfAlpha(QGraphicsView*);
    ~WolfAlpha();
private:
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};

class Goblin : public Monster
{
    Q_OBJECT
public:
    Goblin(QGraphicsView*);
    ~Goblin();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};



class Bear : public Monster
{
    Q_OBJECT
public:
    Bear(QGraphicsView*);
    ~Bear();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};



class Troll : public Monster
{
    Q_OBJECT
public:
    Troll(QGraphicsView*);
    ~Troll();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};


class Oggre : public Monster
{
    Q_OBJECT
public:
    Oggre(QGraphicsView*);
    ~Oggre();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};


class LaoShanLung : public Monster
{
    Q_OBJECT
public:
    LaoShanLung(QGraphicsView*);
    ~LaoShanLung();
public:
    QList<Item*> skinMonster();
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    QList<Item*> generateRandomLoots();
};



#endif // MONSTER_H
