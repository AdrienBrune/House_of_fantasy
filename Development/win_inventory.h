#ifndef W_INVENTORY_H
#define W_INVENTORY_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "hero.h"
#include "frag_interface_itemsorter.h"
#include "w_itemdisplayer.h"

namespace Ui {
class Win_inventory;
}

class Win_Inventory : public QWidget
{
    Q_OBJECT

public:
    explicit Win_Inventory(QWidget *parent = nullptr, Hero * hero = nullptr);
    ~Win_Inventory();
signals:
    void sig_closeWindow();
    void sig_itemThrown(Item*);
    void sig_useTool(Tool*);
    void sig_useScroll(Scroll*);
private slots:
    void showItem(ItemQuickDisplayer*);
    void showItemHover(ItemQuickDisplayer*);
    void hideItemHover(ItemQuickDisplayer*);
    void useItem(ItemQuickDisplayer*);
    void useItem(Item*);
    void onItemThrown(Item*);
public:
    void diplayInventory();
    void closeInventory();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Hero * mHero;
    Frag_Interface_ItemSorter * itemRowTop;
    Frag_Interface_ItemSorter * itemRowMid;
    Frag_Interface_ItemSorter * itemRowBot;
    Frag_Interface_ItemSorter * mItemsContainer;
    QRect mItemShownRect;
    W_ItemDisplayer * mItemToDisplay;
    int mItemSelected;
    int mShowItemDescription;
private:
    Ui::Win_inventory *ui;
};

#endif // W_INVENTORY_H
