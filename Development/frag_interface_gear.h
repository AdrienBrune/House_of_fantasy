#ifndef W_ITEMEQUIPMENT_H
#define W_ITEMEQUIPMENT_H

#include <QWidget>
#include <QGraphicsScene>
#include "item.h"
#include "equipment.h"
#include "hero.h"
#include "w_itemdisplayer.h"

namespace Ui {
class Frag_interface_gear;
}

class Frag_Interface_Gear : public QWidget
{
    Q_OBJECT

public:
    explicit Frag_Interface_Gear(QWidget *parent = nullptr);
    ~Frag_Interface_Gear();
signals:
    void sig_itemClicked(ItemQuickDisplayer*);
    void sig_itemHoverIn(ItemQuickDisplayer*);
    void sig_itemHoverOut(ItemQuickDisplayer*);
    void sig_equipItem(Item*);
    void sig_unequipItem(Item*);
private slots:
    void onItemMovedHandler(ItemQuickDisplayer*); // check travels R->L or L->R - connect to move signals
    void onItemSelected(ItemQuickDisplayer*);
public:
    void addItemLeftSide(Item*);
    void initEquipmentRightSide();
    void addItemsLeftSide();
    QList<Item*> getItemsLeftSide();
    void unselectItems();
    ItemQuickDisplayer * getSelectedItem();
private:
    void swapItemFromRightToLeft(ItemQuickDisplayer*);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QGraphicsScene * mScene;
    QList<ItemQuickDisplayer*> mItemsLeftSide;
    QList<ItemQuickDisplayer*> equipmentRightSide;
    int xPosSplitter;
    QPointF helmetPos;
    QPointF breastplatePos;
    QPointF glovesPos;
    QPointF pantPos;
    QPointF footwearsPos;
    QPointF amuletPos;
    QPointF weaponPos;
private:
    Ui::Frag_interface_gear *ui;
};

#endif // W_ITEMEQUIPMENT_H
