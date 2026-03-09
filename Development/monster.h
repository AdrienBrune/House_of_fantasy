#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"
#include "village.h"

#define VANISHING_COUNTERS 10

class IMonsterFightView;

class Monster : public Character
{
    Q_OBJECT

public:
    struct Obstacle
    {
        int vanishing = VANISHING_COUNTERS;
        QGraphicsItem* item;

        bool operator==(const Obstacle& other) const {
        return this->item == other.item;
    }
    };

    struct MovementHandler
    {
        QList<Obstacle> obstacles;
        int currentAngle = 0;
        int targetAngle = 0;
    };

    struct ImageHandler
    {
        QPixmap walk = QPixmap();
        QPixmap run = QPixmap();
        QPixmap stand = QPixmap();
        QPixmap dead = QPixmap();
        QPixmap skinned = QPixmap();
        QPixmap heavyAttack = QPixmap();
        QPixmap lightAttack = QPixmap();
    };

    struct FramesAvailable{
        int stand = 0;
        int move = 0;
        int run = 0;
        int dead = 0;
        int skinned = 0;
    };

    enum class Action {
        moving,
        stand,
        aggro,
        dead,
        skinned,
        NbActions
    };

public:
    Monster(QGraphicsView*);
    virtual ~Monster();

    static Monster* Factory(QString name, QGraphicsView* view);

signals:
    void sig_monsterEncountered(Monster*);
    void sig_showMonsterData(Monster*);
    void sig_monsterSound(int);
    void sig_heavyAttack();
    void sig_lightAttack();

public:
    virtual const QString GetName()=0;
 
    int getDamage();
    int getThreatLevel();
    Action getAction();
    int getExperience();
    QPixmap getHeavyAttackAnimation();
    QPixmap getLightAttackAnimation();
    int getSoundIndexFor(int);
    QString getDescription();
    ImageHandler getImageHandler();
    IMonsterFightView *getFightView();

    void setDamage(int);
    QList<Item*> skinMonster();
    void setAngle(int);
    void setTargetAngle(int);
    void setBoundingRect(QRectF);

    bool isSkinned();
    bool isDead() override;
    void killMonster();
    bool isInView();
    bool isInBiggerView();
    void enableMonsterAnimation(bool);
    void setupFight(bool setup) override
    {
        if(setup) // Enter fight
        {
            setStamina(QRandomGenerator::global()->bounded(60, 80));
            t_fight.start(500);
        }
        else // Leave fight
        {
            t_fight.stop();
            removeStatus(eStatus::shield);
            removeStatus(eStatus::benediction);
            removeStatus(eStatus::confused);
            removeStatus(eStatus::poisoned);
            removeStatus(eStatus::heal);
        }
    }

public:
    virtual void nextAction(Hero*)=0;
    virtual void addExtraLoots()=0;

public:
    inline virtual void toJson(QJsonObject &json) const override
    {
        Character::toJson(json);

        json.remove("life");
        json.remove("mana");
        json.remove("stamina");
        json.remove("experience");
        if (mCoin == 0)
        {
            json.remove("coin");
        }
    }

    inline virtual void fromJson(const QJsonObject &json) override
    {
        Character::fromJson(json);
    }

protected:
    int getNumberFrame();
    void chooseAction(Hero*);
    void advance(int);
    virtual void doCollision();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void onStaminaRecovery();

private:
    virtual int getSpeed()=0;
    virtual int getBoostedSpeed()=0;
    virtual void generateRandomLoots()=0;

protected:
    QGraphicsView * mView;
    bool mIsInView;
    int mHover;

    int mDamage;
    int mThreatLevel;
    QString mDescription;

    int mSpeed;
    Action mAction;
    FramesAvailable mFrames;
    MovementHandler mMove;
    ImageHandler mPixmap;
    int mSounds[6] = {0,0,0,0,0,0};
    QTimer * t_isWalking;
    int mSkin;

    QList<Item*> mItems;

    IMonsterFightView * mFightView;
    QTimer t_fight;
};


class Spider : public Monster
{
    Q_OBJECT
public:
    Spider(QGraphicsView*);
    ~Spider();
public:
    inline const QString GetName() override { return Spider::Name(); }
    inline static const QString Name() { return "Araignée"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};


class Wolf : public Monster
{
    Q_OBJECT
public:
    Wolf(QGraphicsView*);
    ~Wolf();
public:
    inline const QString GetName() override { return Wolf::Name(); }
    inline static const QString Name() { return "Loup"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};

class WolfAlpha : public Wolf
{
    Q_OBJECT
public:
    WolfAlpha(QGraphicsView*);
    ~WolfAlpha();
public:
    inline const QString GetName() override { return WolfAlpha::Name(); }
    inline static const QString Name() { return "Loup Alpha"; }
private:
    int getBoostedSpeed();
    void generateRandomLoots();
};

class Goblin : public Monster
{
    Q_OBJECT
public:
    Goblin(QGraphicsView*);
    ~Goblin();
public:
    inline const QString GetName() override { return Goblin::Name(); }
    inline static const QString Name() { return "Gobelin"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};



class Bear : public Monster
{
    Q_OBJECT
public:
    Bear(QGraphicsView*);
    ~Bear();
public:
    inline const QString GetName() override { return Bear::Name(); }
    inline static const QString Name() { return "Ours"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};



class Troll : public Monster
{
    Q_OBJECT
public:
    Troll(QGraphicsView*);
    ~Troll();
public:
    inline const QString GetName() override { return Troll::Name(); }
    inline static const QString Name() { return "Troll"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};


class Oggre : public Monster
{
    Q_OBJECT
public:
    Oggre(QGraphicsView*);
    ~Oggre();
public:
    inline const QString GetName() override { return Oggre::Name(); }
    inline static const QString Name() { return "Ogre"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
};


class LaoShanLung : public Monster
{
    Q_OBJECT
public:
    LaoShanLung(QGraphicsView*);
    ~LaoShanLung();
public:
    inline const QString GetName() override { return LaoShanLung::Name(); }
    inline static const QString Name() { return "Lao Shan Lung"; }
    void addExtraLoots();
private:
    void nextAction(Hero*);
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
protected:
    void doCollision() override;
};



#endif // MONSTER_H
