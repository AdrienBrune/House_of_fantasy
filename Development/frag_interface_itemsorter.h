#ifndef ITEMSORTER_H
#define ITEMSORTER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QDebug>
#include "item.h"
#include "equipment.h"
#include "w_itemdisplayer.h"


class Bin : public QGraphicsPixmapItem
{
public:
    Bin(QPixmap, QPointF);
    ~Bin();
public:
    QRectF boundingRect()const;
};




namespace Ui {
class Frag_interface_itemSorter;
}

class Frag_Interface_ItemSorter : public QWidget
{
    Q_OBJECT
public:
    explicit Frag_Interface_ItemSorter(QWidget *parent = nullptr);
    ~Frag_Interface_ItemSorter();
signals:
    void sig_itemClicked(ItemQuickDisplayer*);
    void sig_itemHoverIn(ItemQuickDisplayer*);
    void sig_itemHoverOut(ItemQuickDisplayer*);
    void sig_itemThrown(Item*);
private slots:
    void itemMoved(ItemQuickDisplayer*);
    void itemClicked(ItemQuickDisplayer*);
public:
    void setSceneDeep(int);
    void addItem(Item*);
    void removeQuickItemDisplayers();
    void removeQuickItemDisplayer(Item*);
    void refreshItemsPosition();
    void unselectItems();
    ItemQuickDisplayer * getSelectedItem();
private:
    void displayItem(ItemQuickDisplayer*);
public:
    QGraphicsScene * mScene;
    QList<ItemQuickDisplayer*> mItems;
private:
    Bin * mBinItem;
private:
    Ui::Frag_interface_itemSorter *ui;
};




#endif // ITEMSORTER_H
