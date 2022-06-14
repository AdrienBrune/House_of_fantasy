#ifndef DIALOGPANNEL_H
#define DIALOGPANNEL_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "monster.h"
#include "item.h"
#include "w_itemdisplayer.h"

class W_DialogPannel : public QWidget
{
    Q_OBJECT

public:
    explicit W_DialogPannel(QWidget *parent = nullptr);
    ~W_DialogPannel();
signals:
    void sig_removeWidget();
public slots:
    void hideWidget();
public:
    void showWidget();
protected:
    void paintEvent(QPaintEvent *event);
};


namespace Ui {
class W_showMonsterData;
class W_showPotentialAction;
class W_showInformation;
}

class W_ShowMonsterData : public W_DialogPannel
{
    Q_OBJECT
public:
    explicit W_ShowMonsterData(QWidget * parent = nullptr, Monster * m = nullptr);
    ~W_ShowMonsterData();
private:
    QPixmap pMonsterLogo;
public:
    Ui::W_showMonsterData *ui;
};






class ShowPotentialAction : public W_DialogPannel
{
    Q_OBJECT
public:
    explicit ShowPotentialAction(QWidget * parent = nullptr);
    ~ShowPotentialAction();
private:
    QPixmap pKeyBoard;
public:
    Ui::W_showPotentialAction *ui;
};


class W_ShowItemOnGround : public W_DialogPannel
{
    Q_OBJECT
public:
    explicit W_ShowItemOnGround(QWidget * parent = nullptr, Item * item = nullptr);
    ~W_ShowItemOnGround();
private slots:
    void itemDestroyed();
};






class W_ShowInformation : public W_DialogPannel
{
    Q_OBJECT
public:
    explicit W_ShowInformation(QWidget * parent = nullptr);
    ~W_ShowInformation();
protected:
    QPixmap pImage;
public:
    Ui::W_showInformation *ui;
};

class W_ShowBagFull : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowBagFull(QWidget * parent = nullptr);
    ~W_ShowBagFull();
};


class W_ShowEquipmentTook : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowEquipmentTook(QWidget * parent = nullptr, Item * item = nullptr);
    ~W_ShowEquipmentTook();
};

class W_ShowItemTook : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowItemTook(QWidget * parent = nullptr, Item * item = nullptr);
    ~W_ShowItemTook();
};

class W_ShowEquipmentForged : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowEquipmentForged(QWidget * parent = nullptr, Item * item = nullptr);
    ~W_ShowEquipmentForged();
};

class W_ShowChestFound : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowChestFound(QWidget * parent = nullptr);
    ~W_ShowChestFound();
};

class W_ShowChestBurried : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowChestBurried(QWidget * parent = nullptr);
    ~W_ShowChestBurried();
};

class W_ShowLevelUp : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowLevelUp(QWidget * parent = nullptr, int level = 0);
    ~W_ShowLevelUp();
};

class W_ShowOreSpot : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowOreSpot(QWidget * parent = nullptr, OreSpot * ore = nullptr);
    ~W_ShowOreSpot();
};

class W_ShowOreTook : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowOreTook(QWidget * parent = nullptr, Item * ore = nullptr);
    ~W_ShowOreTook();
};

class W_ShowFishSpot : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowFishSpot(QWidget * parent = nullptr);
    ~W_ShowFishSpot();
};

class W_ShowFishTook : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowFishTook(QWidget * parent = nullptr, Fish * fish = nullptr);
    ~W_ShowFishTook();
};

class W_ShowReplenish : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowReplenish(QWidget * parent = nullptr, QObject * shop = nullptr);
    ~W_ShowReplenish();
};

class W_ShowConsumableTook : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowConsumableTook(QWidget * parent = nullptr, Consumable * item = nullptr);
    ~W_ShowConsumableTook();
};

class W_ShowVilageInfo : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowVilageInfo(QWidget * parent = nullptr, QGraphicsItem * interaction = nullptr);
    ~W_ShowVilageInfo();
};

class W_ShowCarcass : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowCarcass(QWidget * parent = nullptr, Monster * monster = nullptr);
    ~W_ShowCarcass();
};

class W_ShowCalamitySpawned : public W_ShowInformation
{
    Q_OBJECT
public:
    explicit W_ShowCalamitySpawned(QWidget * parent = nullptr);
    ~W_ShowCalamitySpawned();
};

#endif // DIALOGPANNEL_H
