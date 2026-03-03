#ifndef W_ITEMDISPLAYER_H
#define W_ITEMDISPLAYER_H

#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include "equipment.h"
#include "consumable.h"
#include "item.h"
#include "hero.h"

class ItemQuickDisplayer : public Item
{
    Q_OBJECT
public:
    ItemQuickDisplayer(Item *);
    ~ItemQuickDisplayer();
signals:
    void sig_itemClicked(ItemQuickDisplayer*);
    void sig_itemRightClicked(ItemQuickDisplayer*);
    void sig_itemDoubleClicked(ItemQuickDisplayer*);
    void sig_itemMoved(ItemQuickDisplayer*);
    void sig_itemHoverIn(ItemQuickDisplayer*);
    void sig_itemHoverOut(ItemQuickDisplayer*);
private slots:
    void updateStats();
public:
    QPointF getInitialPosition();
    Item * getItem();
    void setInitialPosition(QPointF);
    void setMovable(bool toggle);
    void setItemSelected(bool toggle);
    bool isItemSelected();
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
public:
    void setSelectedHero();
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
private:
    Item * mItem;
    QPointF mPosition;
    bool isMovable;
    bool mSelected;
    Hero * mSelectedHero;
};

class W_ItemDisplayer : public QWidget
{
    Q_OBJECT
public:
    explicit W_ItemDisplayer(QWidget *parent = nullptr, Item * item = nullptr);
    ~W_ItemDisplayer();
signals:
    void sig_itemUsed(Item*);
private slots:
    void useItem();
public:
    Item * getItem();
    QPushButton * getUseButton();
    void hideUseButton();
    void paintEvent(QPaintEvent *);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    Item * mItem;
    int mItemType;
    QPushButton * mButtonUse;
};


#endif // W_ITEMDISPLAYER_H
