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

class CollisionShape : public QObject, public QGraphicsPixmapItem
{
public:
    explicit CollisionShape(QGraphicsItem *parent = nullptr, QRect bounding = QRect(), QPainterPath shape = QPainterPath()):
        QObject(),
        QGraphicsPixmapItem(parent),
        mShape(shape),
        mBoundingRect(bounding)
    {}
    ~CollisionShape(){}

    QPainterPath shape() const { return mShape; }
    QRectF boundingRect() const { return static_cast<QRectF>(mBoundingRect); }
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *){}
private:
    QPainterPath mShape;
    QRect mBoundingRect;
};

class MapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    MapItem();
    virtual ~MapItem();
signals:
    void sig_playSound(int);
public:
    void setImage(QPixmap image, bool trueShape = false, bool scale = false);
    QPixmap getImage();
    QString invocName();
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    bool isObstacle();
    bool isDestroyed();
    CollisionShape * getObstacleShape();
    const int & getZOffset();
private:
    void setShape(QPixmap image, bool trueShape = false, bool scale = false);
protected:
    void destructIt();
protected:
    QRect mBoundingRect;
    QPainterPath mShape;
    CollisionShape *mCollisionShape;
    int mZOffset;
    QPixmap mImage;
    int mImageSelected;
    int mImageEvent;
    QString mMapItemName;
    bool mDestroyed;
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
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void setImage(QPixmap image);
public slots:
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
    void destructIt();
private:
    void initMapItem();
};

class TreeFallen : public MapItem
{
public:
    TreeFallen();
    ~TreeFallen();
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
    void destructIt();
private:
    void initMapItem();
};

class Ground : public MapItem
{
public:
    Ground();
    ~Ground();
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
private:
    void initMapItem();
private:
    MapItem * mEvent;
};

#endif // MAPITEM_H
