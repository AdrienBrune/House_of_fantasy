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

    void heroStaminaRecovery();
    void monsterStaminaRecovery();
    void monsterStaminaRecovered();
    void useConsumable(ItemQuickDisplayer*);
    void on_buttonPause_clicked();
    void hideAnimation();
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
    void addConsumablesOnScreen();
    void checkFightIssue();
    void showNotEnoughtStamina();
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
    W_SpellListSelection * w_spellList;

    // Spells effect
private:
    bool mPrimitiveShield;
    uint8_t mBenedictionCounter;

private:
    Ui::Win_Fight *ui;
};

#endif // WIN_FIGHT_H
