#include "mapitem.h"

MapItem::MapItem():
    QObject (),
    QGraphicsPixmapItem(),
    mBoundingRect(QRect()),
    mShape(QPainterPath()),
    mImageSelected(0),
    mImageEvent(0),
    mMapItemName(""),
    mObstacle(false)
{

}

#include <QtDebug>
MapItem::~MapItem()
{

}

QPixmap MapItem::getImage()
{
    return mImage.copy(static_cast<int>(mImageSelected*boundingRect().width()), 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
}


QString MapItem::invocName()
{
    return mMapItemName;
}

QRectF MapItem::boundingRect() const
{
    return mBoundingRect;
}

QPainterPath MapItem::shape() const
{
    return mShape;
}

void MapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(mImageEvent*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}




Bush::Bush():
    MapItem ()
{
    initMapItem();
}

void Bush::initMapItem()
{
    mMapItemName = "Bush";
    mImage = QPixmap(":/MapItems/bush.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_BUSH);
    setZValue(Z_BUSH);

    mBoundingRect = QRect(0,0,100,100);
    mShape.addRect(QRectF(20, 20, boundingRect().width()-20, boundingRect().height()-20));

    mNextFrame = 0;
    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));

    setRotation(QRandomGenerator::global()->bounded(40)-20);
}

bool Bush::isObstacle()
{
    return mObstacle;
}

void Bush::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mNextFrame), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void Bush::animate()
{
    mNextFrame++;
    if(mNextFrame > 2)
    {
        mNextFrame = 0;
        t_animation->stop();
    }
    update();
}

void Bush::startAnimation()
{
    if(!t_animation->isActive()){
        t_animation->start(400);
    }
}

bool Bush::isAnimated()
{
    return t_animation->isActive();
}

Bush::~Bush()
{

}









Tree::Tree():
    MapItem ()
{
    initMapItem();
}

void Tree::initMapItem()
{
    mMapItemName = "Tree";
    mImage = QPixmap(":/MapItems/tree.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_TREE);
    setZValue(Z_TREE);
    mObstacle = true;

    mBoundingRect = QRect(0,0,200,400);
    mShape.addEllipse(QRectF(boundingRect().width()/3,boundingRect().height()*3/4,boundingRect().width()*2/5,boundingRect().height()/4));

    setRotation(QRandomGenerator::global()->bounded(20)-10);
}

bool Tree::isObstacle()
{
    return mObstacle;
}

void Tree::destructIt()
{
    mImageEvent = 1;
    setZValue(Z_TREE_FALLEN);
    mObstacle = false;
    update();
    emit sig_playSound(SOUND_TREE_FALL);
}

Tree::~Tree()
{

}










TreeFallen::TreeFallen():
    MapItem ()
{
    initMapItem();
}

void TreeFallen::initMapItem()
{
    mMapItemName = "Tree fallen";
    mImage = QPixmap(":/MapItems/tree_fallen.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_TREE_FALLEN);
    setZValue(Z_TREE_FALLEN);

    mBoundingRect = QRect(0,0,200,100);

    setRotation(QRandomGenerator::global()->bounded(32)-16);
}

bool TreeFallen::isObstacle()
{
    return mObstacle;
}

TreeFallen::~TreeFallen()
{

}








Rock::Rock():
    MapItem ()
{
    initMapItem();
}

void Rock::initMapItem()
{
    mMapItemName = "Rock";
    mImage = QPixmap(":/MapItems/rock.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_ROCK);
    setZValue(Z_ROCK);
    mObstacle = true;

    mBoundingRect = QRect(0,0,150,150);
    mShape.addEllipse(QRect(40,0,70,50));

    setRotation(QRandomGenerator::global()->bounded(20)-10);
}

bool Rock::isObstacle()
{
    return mObstacle;
}

void Rock::destructIt()
{
    mImageEvent = 1;
    mObstacle = false;
    update();
    emit sig_playSound(SOUND_ROCK_CRUSH);
}

Rock::~Rock()
{

}







Ground::Ground():
    MapItem ()
{
    initMapItem();
}

void Ground::initMapItem()
{
    QString buff = "";
    mMapItemName = "Ground";
    mImageSelected = 0;
    mImage = QPixmap(":/grounds/grounds/ground_" + buff.asprintf("%d", QRandomGenerator::global()->bounded(RES_GROUND)) + ".png");
    setZValue(Z_GROUND);

    mBoundingRect = QRect(0,0,500,300);

    setRotation(QRandomGenerator::global()->bounded(30)-15);
}

bool Ground::isObstacle()
{
    return mObstacle;
}

Ground::~Ground()
{

}






MapItemMovable::MapItemMovable():
    MapItem (),
    mHover(false),
    mMove(true),
    mReadyToDelete(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    setAcceptHoverEvents(true);

    t_delay_sound = new QTimer(this);
    t_delay_sound->setSingleShot(true);
}

MapItemMovable::~MapItemMovable()
{

}

void MapItemMovable::setShape()
{
    QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
    tmp->setPixmap(mImage.copy(static_cast<int>(mImageSelected*boundingRect().width()), 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
    mBorders = tmp->shape();
    delete tmp;
}

void MapItemMovable::setInitialPosition(QPointF p)
{
    mInitialPosition = p;
}

void MapItemMovable::setReadyToDelete()
{
    mReadyToDelete = true;
}

bool MapItemMovable::isObstacle()
{
    return mObstacle;
}

void MapItemMovable::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
}

void MapItemMovable::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if(mReadyToDelete)
        emit sig_deleteItem(this);
}

void MapItemMovable::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{   
    qreal dx = abs(pos().x()-mInitialPosition.x()),
          dy = abs(pos().y()-mInitialPosition.y());

    if( qSqrt(qPow(dx,2)+qPow(dy,2)) > 90 && mMove){
        setFlag(QGraphicsItem::ItemIsMovable, false);
        setAcceptHoverEvents(false);
        mHover = false;
        mMove = false;
        update();
        emit sig_itemPositionFixed(this);
    }else{
        if(!t_delay_sound->isActive()){
            t_delay_sound->start(1000);
            emit sig_itemMoved(this);
        }
    }

    QGraphicsItem::mouseMoveEvent(event);
}

void MapItemMovable::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverEnter == QGraphicsSceneHoverEvent::HoverEnter)
    {
        mHover = true;
        update();
        event->accept();
    }
}

void MapItemMovable::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverLeave == QGraphicsSceneHoverEvent::HoverLeave)
    {
        mHover = false;
        update();
        event->accept();
    }
}

void MapItemMovable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));

    if(mHover){
        painter->setPen(QPen(QBrush(BORDERS_COLOR), 2));
        painter->drawPath(mBorders);
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}





Plank::Plank():
    MapItemMovable ()
{
    initMapItem();
}

void Plank::initMapItem()
{
    mMapItemName = "Plank";
    mImage = QPixmap(":/MapItems/plank.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_PLANK);
    setZValue(Z_PLANK);

    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(mBoundingRect);
    setShape();

    setRotation(QRandomGenerator::global()->bounded(30)-15);
}

Plank::~Plank()
{

}



Stone::Stone():
    MapItemMovable ()
{
    initMapItem();
}

void Stone::initMapItem()
{
    mMapItemName = "Stone";
    mImage = QPixmap(":/MapItems/stone.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_STONE);
    setZValue(Z_STONE);

    mBoundingRect = QRect(0,0,70,70);
    mShape.addEllipse(mBoundingRect);
    setShape();

    setRotation(QRandomGenerator::global()->bounded(30)-15);
}

Stone::~Stone()
{

}




Lake::Lake(MapItem * event):
    MapItem (),
    mEvent(event)
{
    event->setZValue(Z_FISH_EVENT);
    initMapItem();
}

void Lake::initMapItem()
{
    mMapItemName = "Lake";
    mImage = QPixmap(":/MapItems/lake.png");
    mImageSelected = QRandomGenerator::global()->bounded(RES_LAKE);
    setZValue(Z_LAKE);
    mObstacle = true;

    mBoundingRect = QRect(0,0,700,700);
    QPolygon polygon;
    static const int points[] = {
        10, 395,
        40, 315,
        120, 215,
        360, 120,
        600, 125,
        645, 245,
        635, 410,
        485, 550,
        365, 570,
        260, 540,
        135, 535,
        25, 420
    };
    polygon.setPoints(12, points);
    mShape.addPolygon(polygon);
}

MapItem *Lake::getEvent()
{
    return mEvent;
}

bool Lake::isObstacle()
{
    return mObstacle;
}

Lake::~Lake()
{
    delete mEvent;
}

