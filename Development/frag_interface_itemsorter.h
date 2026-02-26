#ifndef ITEMSORTER_H
#define ITEMSORTER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QDebug>
#include "item.h"
#include "equipment.h"
#include "w_itemdisplayer.h"

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
    void sig_itemRightClicked(ItemQuickDisplayer*);
    void sig_itemHoverIn(ItemQuickDisplayer*);
    void sig_itemHoverOut(ItemQuickDisplayer*);
    void sig_itemThrown(Item*);
    void sig_itemDoubleClicked(ItemQuickDisplayer*);
private slots:
    void itemMoved(ItemQuickDisplayer*);
    void itemClicked(ItemQuickDisplayer*);
    void itemRightClicked(ItemQuickDisplayer*);
public:
    void setSceneDeep(int);
    void addItem(Item*);
    void removeQuickItemDisplayers();
    void removeQuickItemDisplayer(Item*);
    void refreshItemsPosition();
    void unselectItems();
    ItemQuickDisplayer * getSelectedItem();
public slots:
    void throwItem(ItemQuickDisplayer*);
protected:
    void resizeEvent(QResizeEvent*) override { mScene->setSceneRect(0, 0, mScene->sceneRect().width(), mScene->sceneRect().height()); }
public:
    QGraphicsScene * mScene;
    QList<ItemQuickDisplayer*> mItems;
private:
    Ui::Frag_interface_itemSorter *ui;
};




#endif // ITEMSORTER_H
