#ifndef W_INTERFACETRADING_H
#define W_INTERFACETRADING_H

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QComboBox>
#include "hero.h"
#include "w_itemdisplayer.h"
#include "frag_interface_itemtrader.h"
#include "village.h"
#include "frag_loot_displayer.h"
#include "w_animation_forge.h"
#include "w_dialogpannel.h"
#include "w_itemdisplayer.h"
#include "frag_interface_potionpreferencies.h"

namespace Ui {
class Win_chest;
class Win_Blacksmith;
class Win_Merchant;
class Win_Alchemist;
}

class Win_Interface_Trading : public QWidget
{
    Q_OBJECT
public:
    explicit Win_Interface_Trading(QWidget *parent = nullptr, Hero * hero = nullptr);
    ~Win_Interface_Trading();
signals:
    void sig_heroBagFull();
    void sig_playSound(int);
protected:
    void paintEvent(QPaintEvent *);
protected:
    Frag_Interface_ItemTrader * mItemTrader;
    Hero * mHero;
};


class Win_Chest : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_Chest(QWidget *parent = nullptr, Hero * hero = nullptr, ChestEvent * chest = nullptr);
    ~Win_Chest();
public:
    void closeChest();
private:
    void initInterface();
private:
    ChestEvent * mChest;
private:
    Ui::Win_chest *ui;
};

class Win_BlackSmith : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_BlackSmith(QWidget *parent = nullptr, Hero * hero = nullptr, Blacksmith * blacksmith = nullptr);
    ~Win_BlackSmith();
private slots:
    void on_button_validate_clicked();
    void showItem(ItemQuickDisplayer*);
    void displayItemForged();
    void displayBagFull();
    void removeCurrentPanel();
    void endRedBorders();
    void closeWindow();
private:
    void initInterface();
    void addItemToForge(Item*);
    bool materialListsMatch();
    void removeMaterialsNeeded();
protected:
    void paintEvent(QPaintEvent *);
private:
    QGraphicsScene * mScene;
    W_ItemDisplayer * itemToDisplay;
    QList<ItemQuickDisplayer*> mEquipmentsToForge;
    QList<Frag_loot_displayer*> lootsDisplayed;
    EquipmentToForge * mEquipmentToForgeSelected;
    Blacksmith * mBlacksmith;
    QTimer * t_redBorders;
    W_ShowInformation * w_panel;
private:
    Ui::Win_Blacksmith *ui;
};

class Win_Merchant : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_Merchant(QWidget *parent = nullptr, Hero * hero = nullptr, Merchant * merchant = nullptr);
    ~Win_Merchant();
private slots:
    void virtuallyBuyItemFromSeller(ItemQuickDisplayer*);
    void virtuallySellItemToSeller(ItemQuickDisplayer*);
    void on_button_validate_clicked();
    void showItem(ItemQuickDisplayer*);
    void endRedBorders();
    void closeWindow();
private:
    void initInterface();
    bool buyItem(Item*);
protected:
    void paintEvent(QPaintEvent *);
private:
    QGraphicsScene * mScene;
    W_ItemDisplayer * itemToDisplay;
    Merchant * mMerchant;
    QTimer * t_redBorders;
private:
    Ui::Win_Merchant *ui;
};

class Win_Alchemist : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_Alchemist(QWidget *parent = nullptr, Hero * hero = nullptr, Alchemist * alchemist = nullptr);
    ~Win_Alchemist();
private slots:
    void virtuallyBuyItemFromSeller(ItemQuickDisplayer*);
    void virtuallySellItemToSeller(ItemQuickDisplayer*);
    void on_button_validate_clicked();
    void showItem(ItemQuickDisplayer*);
    void endRedBorders();
    void potionPreferenciesChanged();
    void closeWindow();
private:
    void initInterface();
    void addPotionPreferenciesInterface();
    bool buyItem(Item*);
protected:
    void paintEvent(QPaintEvent *);
private:
    QGraphicsScene * mScene;
    W_ItemDisplayer * itemToDisplay;
    Alchemist * mAlchemist;
    QTimer * t_redBorders;
private:
    Frag_Interface_PotionPreferencies * w_potionPreferencies;
    Ui::Win_Alchemist *ui;
};


class Win_HeroChest : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_HeroChest(QWidget *parent = nullptr, Hero * hero = nullptr, HeroChest * chest = nullptr);
    ~Win_HeroChest();
public:
    void closeChest();
private:
    void initInterface();
private:
    HeroChest * mChest;
private:
    Ui::Win_chest *ui;
};

#endif // W_INTERFACETRADING_H
