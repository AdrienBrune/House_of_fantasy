#ifndef WIN_FIGHT_H
#define WIN_FIGHT_H

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPainter>
#include <QProgressBar>
#include "hero.h"
#include "monster.h"
#include "w_animation_fight.h"
#include "w_itemdisplayer.h"
#include "w_spelllistselection.h"
#include "w_animationspell.h"

#define BORDER_SIZE     2
#define CORNER_ROUNDED  5

#define ITEM_VIEW_ENABLED   "QGraphicsView { background-color:rgba(250,250,250,5); border: none; margin: 0px 0px 0px 0px; }" \
                            "QGraphicsView QScrollBar::horizontal { background: none; height: 12px; border-radius:2px; }" \
                            "QGraphicsView QScrollBar::groove { background: none; }" \
                            "QGraphicsView QScrollBar::left-arrow{ background: none; }" \
                            "QGraphicsView QScrollBar::right-arrow{ background: none; }" \
                            "QGraphicsView QScrollBar::sub-line:horizontal{ background: none; }" \
                            "QGraphicsView QScrollBar::add-line:horizontal{ background: none; }" \
                            "QScrollBar::sub-page:horizontal { background: none; }" \
                            "QGraphicsView QScrollBar::handle { background: rgba(250,250,250,15); border-radius:5px; max-width:10px; }"
#define ITEM_VIEW_DISABLED  "QGraphicsView { background-color:rgba(250,250,250,200); border: none; margin: 0px 0px 0px 0px; }" \
                            "QGraphicsView QScrollBar::horizontal { background: none; height: 12px; border-radius:2px; }" \
                            "QGraphicsView QScrollBar::groove { background: none; }" \
                            "QGraphicsView QScrollBar::left-arrow{ background: none; }" \
                            "QGraphicsView QScrollBar::right-arrow{ background: none; }" \
                            "QGraphicsView QScrollBar::sub-line:horizontal{ background: none; }" \
                            "QGraphicsView QScrollBar::add-line:horizontal{ background: none; }" \
                            "QScrollBar::sub-page:horizontal { background: none; }" \
                            "QGraphicsView QScrollBar::handle { background: rgba(250,250,250,15); border-radius:5px; max-width:10px; }"

#define STAMINA_NORMAL_STATUS   "QProgressBar { border:2px solid #575757; border-radius:2px; background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #393939, stop: 1 #393939); }" \
                                "QProgressBar::chunk { background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,  stop: 1 #ffd800, stop: 0 #ffd800); border-radius:2px; }"

#define STAMINA_NOT_ENOUGTH     "QProgressBar { border:2px solid #575757; border-radius:2px; background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #393939, stop: 1 #393939); }" \
                                "QProgressBar::chunk { background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 1,  stop: 1 #ff0000, stop: 0 #ff7474); border-radius:2px; }"

class W_AnimatedProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum{eLife, eMana, eStamina};
    struct Settings
    {
        uint8_t height;
        uint8_t entityStatToDisplay;
        QBrush colorBackground;
        QBrush colorFilling;
        QBrush colorBlink1;
        QBrush colorBlink2;
    };
    struct StatBar
    {
        uint32_t current;
        uint32_t lost;
        uint32_t max;
    };
    struct Animation{
        QTimer * blink;
        QTimer * end;
        bool blinkStatus;
    };

public:
    explicit W_AnimatedProgressBar(QWidget * parent = nullptr):
        QWidget(parent),
        mSetting(Settings()),
        mNotEnoughtStat(false)
    {
        mAnimation.blinkStatus = 0;
        mAnimation.blink = new QTimer(this);
        connect(mAnimation.blink, &QTimer::timeout, this, &W_AnimatedProgressBar::onAnimate);
        mAnimation.end = new QTimer(this);
        connect(mAnimation.end, &QTimer::timeout, this, &W_AnimatedProgressBar::onEndAnimation);
    }
    ~W_AnimatedProgressBar() {}

public slots:
    void onStatChanged();
    void onNotEnoughtStat();

private slots:
    void onAnimate();
    void onEndAnimation();

public:
    void setupUi(Character * entity = nullptr, Settings settings = {});

protected:
    void paintEvent(QPaintEvent *event);

private:
    Character * mEntity;
    Settings mSetting;
    StatBar mStatBar;
    Animation mAnimation;
    bool mNotEnoughtStat;
};

class W_StatusBar : public QWidget
{
    Q_OBJECT
public:
    W_StatusBar(QWidget * parent = nullptr):
        QWidget(parent),
        mEntity(nullptr),
        mRightAligned(false)
    {}
    ~W_StatusBar() {}

private slots:
    void onUpdate() { update(); }

public:
    void setupUi(Character * entity, bool alignment = false)
    {
        mEntity = entity;
        mRightAligned = alignment;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event);

private:
    Character * mEntity;
    bool mRightAligned;
};




namespace Ui {
class Win_Fight;
}

class Win_Fight : public QWidget
{
    Q_OBJECT

public:
    explicit Win_Fight(QWidget * parent = nullptr, Monster * monster = nullptr);
    ~Win_Fight();

signals:
    void sig_playSound(int);
    void sig_closeWindow();
    void sig_endFight(Character*);

private slots:
    void onButtonHeavyAttackClicked();
    void onButtonLightAttackClicked();
    void onButtonFleeClicked();
    void onButtonUseSpellClicked();

    void onMonsterLightAttack();
    void onMonsterHeavyAttack();

    void onHeroStaminaRecovery();
    void onUseConsumable(ItemQuickDisplayer*);
    void onHideAnimation();
    void onUseSpell(Skill*);

    void onCheckFightIssue()
    {
        if(mHero->getLife().current <= 0)
        {
            endFight(mHero);
        }
        else if(mMonster->getLife().current <= 0)
        {
            endFight(mMonster);
        }
    }

public:
    void displayInterface()
    {
        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(eff);
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(500);
        a->setStartValue(0);
        a->setEndValue(1);
        a->setEasingCurve(QEasingCurve::InBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
        b->setDuration(600);
        b->setStartValue(QRect(this->x()-this->width(),this->y(),this->width(),this->height()));
        b->setEndValue(QRect(this->x(),this->y(),this->width(),this->height()));
        b->start(QPropertyAnimation::DeleteWhenStopped);

        showFullScreen();
    }
    void hideInterface()
    {
        mMonster->setupFight(false);

        QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(eff);
        QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
        a->setDuration(600);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::InBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);

        QTimer * timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_closeWindow()));
        timer->start(1000);
    }

private:
    void initInterface();
    void addConsumablesOnScreen();
    void enableButtons(bool);
    void showNotEnoughtStamina();
    void loadFightAnimationsPixmap()
    {
        pMonsterLightAttack = mMonster->getLightAttackAnimation();
        pMonsterHeavyAttack = mMonster->getHeavyAttackAnimation();
    }
    bool heroUseStamina(int loss)
    {
        if(mHero->getStamina().current - loss < 0)
            return false;

        mHero->setStamina(mHero->getStamina().current - loss);
        return true;
    }
    void endFight(Character * character = nullptr)
    {
        mHero->setupFight(false);
        mMonster->setupFight(false);
        emit sig_endFight(character);
        hideInterface();
    }

protected:
    void paintEvent(QPaintEvent *event);

private:
    Hero * mHero;
    Monster * mMonster;

    QGraphicsScene * mScenePotion;
    QList<ItemQuickDisplayer*> mConsumables;
    QPixmap pMonsterHeavyAttack;
    QPixmap pMonsterLightAttack;
    W_Animation_Fight * w_fightAnimation;
    W_SpellListSelection * w_spellList;

    QTimer * t_onHeroStaminaRecovery;
    QTimer * t_monsterStaminaRecovery;

    bool mDodgeSucces;
    bool mFleeFail;
    bool mConfused;

private:
    Ui::Win_Fight *ui;
};

#endif // WIN_FIGHT_H
