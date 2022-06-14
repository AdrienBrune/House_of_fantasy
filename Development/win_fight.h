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

class AnimatedProgressBar : public QWidget
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
    explicit AnimatedProgressBar(QWidget * parent = nullptr, Character * entity = nullptr, Settings = {});
    ~AnimatedProgressBar();
public slots:
    void onStatChanged();
    void onNotEnoughtStat();
private slots:
    void onAnimate();
    void onEndAnimation();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Character * mEntity;
    Settings mSetting;
    StatBar mStatBar;
    Animation mAnimation;
    bool mNotEnoughtStat;
};






namespace Ui {
class Win_Fight;
}

class Win_Fight : public QWidget
{
    Q_OBJECT
public:
    explicit Win_Fight(QWidget * parent = nullptr, Hero * hero = nullptr, Monster * monster = nullptr);
    ~Win_Fight();
signals:
    void sig_playSound(int);
    void sig_closeWindow();
    void sig_endFight(Character*);
private slots:
    void onButtonHeavyAttackClicked();
    void onButtonUseObjectClicked();
    void onButtonLightAttackClicked();
    void onButtonFleeClicked();
    void onButtonUseSpellClicked();

    void heroStaminaRecovery();
    void monsterStaminaRecovery();
    void monsterStaminaRecovered();
    void useConsumable(ItemQuickDisplayer*);
    void on_buttonPause_clicked();
    void hideAnimation();
    void hideAnimationSpell();
    void restoreMonsterImage();
    void onFightEvent();
    void onUseSpell(Skill*);

public:
    void displayInterface();
    void hideInterface();
private:
    void initInterface();
    void loadFightAnimationsPixmap();
    bool heroUseStamina(int);
    void monsterUseStamina(int);
    void heroStaminaRecovered();
    void enableButtons(bool);
    void enableItemsUtilisation(bool);
    void addConsumablesOnScreen();
    void checkFightIssue();
protected:
    void paintEvent(QPaintEvent *event);

private:
    Hero * mHero;
    Monster * mMonster;
    QList<ItemQuickDisplayer*> mConsumables;
    AnimatedProgressBar * mLifeMonster;
    AnimatedProgressBar * mLifeHero;
    AnimatedProgressBar * mManaHero;

    QTimer * t_heroStaminaRecovery;
    QTimer * t_monsterStaminaRecovery;
    QTimer * t_restoreMonsterImage;
    QGraphicsScene * mScenePotion;
    QPixmap pMonsterHeavyAttack;
    QPixmap pMonsterLightAttack;
    QTimer * t_fightEvent;
    bool mDodgeSucces;
    bool mFleeFail;
    bool mBreak;
    W_Animation_Fight * w_fightAnimation;
    W_AnimationSpell * w_spellAnimation;
    W_SpellListSelection * w_spellList;

    // Spells effect
private:
    bool mPrimitiveShield;
    uint8_t mBenedictionCounter;

private:
    Ui::Win_Fight *ui;
};

#endif // WIN_FIGHT_H
