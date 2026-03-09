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
    static MapItem* Factory(quint32 identifier);
    void setImage(QPixmap image, bool trueShape = false, bool scale = false);
    QPixmap getImage();
    QString invocName();
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    inline virtual bool isMovable() { return false; }
    bool isObstacle();
    bool isDestroyed();
    CollisionShape * getObstacleShape();
    const int & getZOffset();
    void setPos(QPointF position)
    {
        QGraphicsPixmapItem::setPos(position);
        mPosition = position;
    }
    void setPos(qreal x, qreal y)
    {
        QGraphicsPixmapItem::setPos(x, y);
        mPosition = QPointF(x, y);
    }
private:
    void setShape(QPixmap image, bool trueShape = false, bool scale = false);
protected:
    void destructIt();
public:
    inline virtual void toJson(QJsonObject &json) const
    {
        json["name"] = mMapItemName;
        json["type"] = mIdentifier;
        QJsonObject jsonPosition;
        jsonPosition["x"] = mPosition.x();
        jsonPosition["y"] = mPosition.y();
        json["position"] = jsonPosition;
    }
    inline virtual void fromJson(const QJsonObject &json)
    {
        if (json.contains("name") && json["name"].isString())
        {
            mMapItemName = json["name"].toString();
        }

        if (json.contains("type") && json["type"].isDouble())
        {
            mIdentifier = json["type"].toInt();
        }
        
        if (json.contains("position") && json["position"].isObject())
        {
            QJsonObject jsonPosition = json["position"].toObject();
            if (jsonPosition.contains("x") && jsonPosition["x"].isDouble())
            {
                mPosition.setX(jsonPosition["x"].toDouble());
            }
            if (jsonPosition.contains("y") && jsonPosition["y"].isDouble())
            {
                mPosition.setY(jsonPosition["y"].toDouble());
            }
        }
        setPos(mPosition);
    }
protected:
    int mIdentifier;
    QPointF mPosition;
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
    inline virtual bool isMovable() override { return true; }
    void setInitialPosition(QPointF);
    void setReadyToDelete();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    inline virtual void toJson(QJsonObject &json) const override
    {
        MapItem::toJson(json);

        json["movable"] = mMove;
    }
    inline virtual void fromJson(const QJsonObject &json) override
    {
        MapItem::fromJson(json);

        if (json.contains("movable") && json["movable"].isBool())
        {
            mMove = json["movable"].toBool();
        }
        if(!mMove)
        {
            setFlag(QGraphicsItem::ItemIsMovable, false);
            setAcceptHoverEvents(false);
            mHover = false;
        }
        mInitialPosition = mPosition;
    }
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
    Lake();
    ~Lake();
public:
    MapItem * getEvent();
private:
    void initMapItem();
private:
    MapItem * mEvent;
};

#endif // MAPITEM_H
