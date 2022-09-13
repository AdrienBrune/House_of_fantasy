#ifndef MAPITEM_H
#define MAPITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <qrandom.h>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include "item.h"
#include "constants.h"

class MapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    MapItem();
    virtual ~MapItem();
signals:
    void sig_playSound(int);
public:
    QPixmap getImage();
    QString invocName();
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    virtual bool isObstacle()=0;
protected:
    QRect mBoundingRect;
    QPainterPath mShape;
    QPixmap mImage;
    int mImageSelected;
    int mImageEvent;
    QString mMapItemName;
    bool mObstacle;
protected:
    static quint32 sNbInstances;
};

class MapItemMovable : public MapItem
{
    Q_OBJECT
public:
    MapItemMovable();
    virtual ~MapItemMovable();
signals:
    void sig_itemMoved(MapItem*);
    void sig_itemPositionFixed(MapItem*);
    void sig_deleteItem(MapItemMovable*);
public:
    void setInitialPosition(QPointF);
    void setReadyToDelete();

    bool isObstacle();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void setShape();

    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private:
    QPainterPath mBorders;
    bool mHover;
    QPointF mInitialPosition;
    QTimer * t_delay_sound;
    bool mMove;
    bool mReadyToDelete;
};

class Tree : public MapItem
{
public:
    Tree();
    ~Tree();
public:
    bool isObstacle();
    void destructIt();
private:
    void initMapItem();
};

class TreeFallen : public MapItem
{
public:
    TreeFallen();
    ~TreeFallen();
public:
    bool isObstacle();
private:
    void initMapItem();
};

class Bush : public MapItem
{
    Q_OBJECT
public:
    Bush();
    ~Bush();
public:
    bool isObstacle();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    void startAnimation();
    bool isAnimated();
private slots:
    void animate();
private:
    void initMapItem();
private:
    int mNextFrame;
    QTimer * t_animation;
};

class Rock : public MapItem
{
public:
    Rock();
    ~Rock();
public:
    bool isObstacle();
    void destructIt();
private:
    void initMapItem();
};

class Ground : public MapItem
{
public:
    Ground();
    ~Ground();
public:
    bool isObstacle();
private:
    void initMapItem();
};

class Plank : public MapItemMovable
{
    Q_OBJECT
public:
    Plank();
    ~Plank();
private:
    void initMapItem();
};

class Stone : public MapItemMovable
{
    Q_OBJECT
public:
    Stone();
    ~Stone();
private:
    void initMapItem();
};

class Lake : public MapItem
{
public:
    Lake(MapItem*);
    ~Lake();
public:
    MapItem * getEvent();

    bool isObstacle();
private:
    void initMapItem();
private:
    MapItem * mEvent;
};

#endif // MAPITEM_H
