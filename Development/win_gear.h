#ifndef W_INVENTORYEQUIPMENT_H
#define W_INVENTORYEQUIPMENT_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "hero.h"
#include "frag_interface_gear.h"
#include "w_itemdisplayer.h"
#include "frag_stats_displayer.h"

namespace Ui {
class Win_gear;
}

class Win_Gear : public QWidget
{
    Q_OBJECT

public:
    explicit Win_Gear(QWidget *parent = nullptr, Hero * hero = nullptr);
    ~Win_Gear();
signals:
    void sig_closeWindow();
    void sig_playSound(int);
private slots:
    void equipItem(Item*);
    void unequipItem(Item*);
    void showItem(ItemQuickDisplayer*);
    void showItemHover(ItemQuickDisplayer*);
    void hideItemHover(ItemQuickDisplayer*);
    void displayLifeChanged();
    void displayManaChanged();
    void displayDamageChanged();
    void displayHitSpeedChanged();
    void displayDefenseChanged();
    void displayDodgeChanged();
    void displayBagPayloadChanged();
    void displayStaminaChanged();
public:
    void diplayWindow();
    void closeWindow();
protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_buttonExit_clicked();

private:
    Frag_Interface_Gear * mItemEquipment;
    Hero * mHero;
    QRect mItemShownRect;
    W_ItemDisplayer * mItemToDisplay;
    int mItemSelected;
    Frag_Stats_Displayer * mLife;
    Frag_Stats_Displayer * mMana;
    Frag_Stats_Displayer * mExperience;
    Frag_Stats_Displayer * mDamage;
    Frag_Stats_Displayer * mHitSpeed;
    Frag_Stats_Displayer * mDefense;
    Frag_Stats_Displayer * mDodge;
    Frag_Stats_Displayer * mBagPayload;
    Frag_Stats_Displayer * mStamina;

private:
    Ui::Win_gear *ui;
};

#endif // W_INVENTORYEQUIPMENT_H
