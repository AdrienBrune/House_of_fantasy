#ifndef W_INTERFACETRADING_H
#define W_INTERFACETRADING_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QComboBox>
#include "hero.h"
#include "w_itemdisplayer.h"
#include "frag_interface_itemtrader.h"
#include "village.h"
#include "w_animation_forge.h"
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
    explicit Win_Interface_Trading(QWidget *parent = nullptr);
    virtual ~Win_Interface_Trading();
signals:
    void sig_heroBagFull();
    void sig_playSound(int);
protected:
    void paintEvent(QPaintEvent *);
protected:
    Frag_Interface_ItemTrader * mItemTrader;
    Hero * mHero;
};


class QPushButton_Offering : public QPushButton
{
    Q_OBJECT
public:
    explicit QPushButton_Offering(QWidget *parent = nullptr, Item * offering = nullptr, int id = 0):QPushButton(parent),mOffering(offering), mId(id)
    {
        setStyleSheet("QPushButton_Offering{color:white;background-color:rgba(250,250,250,20);}"
                      "QPushButton_Offering::hover{color:white;background-color:rgba(250,250,250,30);}");
        setText("Déposer");
        setFont(QFont("Sitka Small", 12));
        connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
    }
    ~QPushButton_Offering(){}
signals:
    void sig_offer(Item*, int);
private slots:
    void onClicked(){ emit sig_offer(mOffering, mId); deleteLater(); }
private:
    Item * mOffering;
    int mId;
};

class Win_Altar : public Win_Interface_Trading
{
    Q_OBJECT
public:
    explicit Win_Altar(QWidget *parent = nullptr);
    ~Win_Altar();
private slots:
    void onGiveOffering(Item*, int);
    void closeWindow();
protected:
    void paintEvent(QPaintEvent*);
private:
    Altar * mAltar;
    QList<QRect> mAreasOffering;
};


class Win_Chest : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_Chest(QWidget *parent = nullptr, ChestEvent * chest = nullptr);
    ~Win_Chest();
public slots:
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
    explicit Win_BlackSmith(QWidget *parent = nullptr);
    ~Win_BlackSmith();
signals:
    void sig_playSound(int);
    void sig_itemThrown(Item*);
private slots:
    void on_button_validate_clicked();
    void showItem();
    void hideItem();
    void paymentHoverIn();
    void paymentHoverOut();
    void updateWindow();
    void closeWindow();
protected:
    void paintEvent(QPaintEvent *);
private:
    Blacksmith * mBlacksmith;
    Item * mItemSold;
    W_ItemDisplayer * mItemDisplayer;
    bool mHoverPayment;
    bool mHoverUnknownItem;
    QTimer * t_chronoResplenish;
private:
    Ui::Win_Blacksmith *ui;
};

class Win_Merchant : public Win_Interface_Trading
{
    Q_OBJECT

public:
    explicit Win_Merchant(QWidget *parent = nullptr);
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
    explicit Win_Alchemist(QWidget *parent = nullptr);
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
    explicit Win_HeroChest(QWidget *parent = nullptr);
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
