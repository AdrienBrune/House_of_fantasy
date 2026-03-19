#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"
#include "village.h"
#include "daynightcycle.h"
#include "constants.h"

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
        QVector2D netDirection = QVector2D(1, 0);  // direction nette souhaitée (normalisée)
        QVector2D stepDirection = QVector2D(1, 0); // direction du segment en cours (normalisée)
        bool zigzagSide = false;                   // côté actuel du zigzag
        int zigzagCounter = 0;                     // compteur de frames du segment actuel
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
    virtual int type() const override { return eQGraphicItemType::monster; }

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
    inline int getZOffset() { return mBoundingRect.height()/2; }

    void setDamage(int);
    QList<Item*> skinMonster();
    void setAngle(int);
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

    virtual void nextAction(Hero*, DayNightCycle*);
    virtual void addExtraLoots()=0;

public:
    inline virtual void toJson(QJsonObject &json) const override
    {
        Character::toJson(json);

        json["status"] = static_cast<int>(mAction);
        json["skin"] = mSkin;

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

        if (json.contains("status") && json["status"].isDouble())
        {
            mAction = static_cast<Action>(json["status"].toInt());
            if (mAction == Action::dead)
            {
                killMonster();
            }
            if (mAction == Action::skinned)
            {
                QList<Item*> items = skinMonster();
                qDeleteAll(items);
                items.clear();
            }
        }
        if (json.contains("skin") && json["skin"].isDouble())
        {
            mSkin = json["skin"].toInt();
        }
    }

protected:
    int getNumberFrame();
    void chooseAction(Hero*, DayNightCycle*);
    void advance(int);
    virtual void doCollision();
    void applyZigzagStep();
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
    QTimer * t_isRunning;
    int mSkin;

    QList<Item*> mItems;

    IMonsterFightView * mFightView;
    QTimer t_fight;

    QVector2D mCollisionVector;
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
    int getSpeed();
    int getBoostedSpeed();
    void generateRandomLoots();
protected:
    void doCollision() override;
};



#endif // MONSTER_H
