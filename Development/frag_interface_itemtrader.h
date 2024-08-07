#ifndef W_ITEMTRADER_H
#define W_ITEMTRADER_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QDebug>
#include "item.h"
#include "w_itemdisplayer.h"

namespace Ui {
class Frag_interface_itemTrader;
}

class Frag_Interface_ItemTrader : public QWidget
{
    Q_OBJECT
public:
    struct SizeData{
        int xPosSplitter;  
        int horizontalOffset, verticalOffset;
        int deep;
        int numberRows, numberLines;
    };
public:
    explicit Frag_Interface_ItemTrader(QWidget *parent = nullptr, QPixmap pictureLeft = QPixmap(), QPixmap pictureRight = QPixmap());
    ~Frag_Interface_ItemTrader();
signals:
    void sig_itemSwipedToRight(ItemQuickDisplayer*);
    void sig_itemSwipedToLeft(ItemQuickDisplayer*);
    void sig_itemClicked(ItemQuickDisplayer*);
    void sig_itemSelected(ItemQuickDisplayer*);
private slots:
    void onItemMovedHandler(ItemQuickDisplayer*);
    void onItemSelected(ItemQuickDisplayer*);
    void onItemRightClicked(ItemQuickDisplayer*);
public:
    void setSceneDeep(int);
    void addItemLeftSide(Item*);
    void addItemRightSide(Item*);
    void addItemsLeftSide(QList<Item*>);
    void addItemsRightSide(QList<Item*>);
    QList<Item*> getItemsLeftSide();
    QList<Item*> getItemsRightSide();
    void removeItemRightSide(Item*);
    void removeItemLeftSide(Item*);
private:
    void moveItemLeftToRight(ItemQuickDisplayer*);
    void moveItemRightToLeft(ItemQuickDisplayer*);
protected:
    void paintEvent(QPaintEvent *event);
private:
    QPixmap mPictureLeft, mPictureRight;
    QGraphicsScene * mScene;
    QList<ItemQuickDisplayer*> itemsLeftSide;
    QList<ItemQuickDisplayer*> itemsRightSide;
    SizeData mSizeData;
private:
    Ui::Frag_interface_itemTrader *ui;
};

#endif // W_ITEMTRADER_H
