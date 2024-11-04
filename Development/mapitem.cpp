#include "mapitem.h"

quint32 MapItem::sNbInstances = 0;

MapItem::MapItem():
    QObject (),
    QGraphicsPixmapItem(),
    mBoundingRect(QRect()),
    mShape(QPainterPath()),
    mCollisionShape(nullptr),
    mZOffset(0),
    mImageSelected(0),
    mImageEvent(0),
    mMapItemName(""),
    mDestroyed(false)
{
    //qDebug() << "[C] " << ++sNbInstances << " " << mMapItemName;
}

MapItem::~MapItem()
{
    if(mCollisionShape)
        delete mCollisionShape;
    mCollisionShape = nullptr;
    //qDebug() << "[D] " << --sNbInstances << " " << mMapItemName;
}

void MapItem::setImage(QPixmap image, bool trueShape, bool scale)
{
    mImage = image;
    setShape(image, trueShape, scale);
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

    // painter->setBrush(QBrush("#7700FF00"));
    // painter->drawPath(mShape);
    // if(isObstacle())
    // {
    //     painter->setBrush(QBrush("#77FF0000"));
    //     painter->drawPath(mCollisionShape->shape());
    // }
}

bool MapItem::isObstacle()
{
    return mCollisionShape ? true : false;
}

bool MapItem::isDestroyed()
{
    return mDestroyed;
}

CollisionShape * MapItem::getObstacleShape()
{
    return mCollisionShape;
}

const int & MapItem::getZOffset()
{
    return mZOffset;
}

void MapItem::setShape(QPixmap image, bool trueShape, bool scale)
{
    if(trueShape)
    {
        QGraphicsPixmapItem * tmp = new QGraphicsPixmapItem(this);
        if(scale)
            tmp->setPixmap(mImage.scaled(static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
        else
            tmp->setPixmap(image.copy(static_cast<int>(mImageSelected*boundingRect().width()), 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height())));
        mShape = tmp->shape();
        delete tmp;
    }
    else
        mShape.addEllipse(boundingRect());
}

void MapItem::destructIt()
{
    mDestroyed = true;
    mImageEvent = 1;
    if(mCollisionShape)
        delete mCollisionShape;
    mCollisionShape = nullptr;
    update();
}


Bush::Bush():
    MapItem ()
{
    initMapItem();
}

void Bush::initMapItem()
{
    mMapItemName = "Bush";

    mBoundingRect = QRect(0,0,100,100);
    mZOffset = mBoundingRect.height()*9/10;
    mImageSelected = QRandomGenerator::global()->bounded(RES_BUSH);
    setImage(QPixmap(":/MapItems/bush.png"));

    mNextFrame = 0;
    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));

    setZValue(Z_BUSH);
    setRotation(QRandomGenerator::global()->bounded(40)-20);
}

void Bush::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, static_cast<int>(mImageSelected*boundingRect().width()), static_cast<int>(boundingRect().height()*mNextFrame), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)

    // painter->setBrush(QBrush("#7700FF00"));
    // painter->drawPath(mShape);
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

    mBoundingRect = QRect(0,0,200,400);
    mZOffset = mBoundingRect.height()*19/20;
    mImageSelected = QRandomGenerator::global()->bounded(RES_TREE);
    setImage(QPixmap(":/MapItems/tree.png"));

    setZValue(Z_TREE);
    setRotation(QRandomGenerator::global()->bounded(20)-10);
}

void Tree::destructIt()
{
    MapItem::destructIt();
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

    mBoundingRect = QRect(0,0,200,100);
    mZOffset = mBoundingRect.height()*3/4;
    mImageSelected = QRandomGenerator::global()->bounded(RES_TREE_FALLEN);
    setImage(QPixmap(":/MapItems/tree_fallen.png"));

    setZValue(Z_TREE_FALLEN);
    setRotation(QRandomGenerator::global()->bounded(20)-10);
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

    mBoundingRect = QRect(0,0,150,150);
    mZOffset = mBoundingRect.height()*3/4;
    mImageSelected = QRandomGenerator::global()->bounded(RES_ROCK);
    setImage(QPixmap(":/MapItems/rock.png"));

    setZValue(Z_ROCK);
    setRotation(QRandomGenerator::global()->bounded(20)-10);
}

void Rock::destructIt()
{
    MapItem::destructIt();
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
    QString temp = "";
    mMapItemName = "Ground";

    mBoundingRect = QRect(0,0,500,300);
    mImageSelected = 0;
    setImage(QPixmap(":/grounds/grounds/ground_" + temp.asprintf("%d", QRandomGenerator::global()->bounded(RES_GROUND)) + ".png"));

    setZValue(Z_GROUND);
    setRotation(QRandomGenerator::global()->bounded(30)-15);
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

void MapItemMovable::setImage(QPixmap image)
{
    MapItem::setImage(image, true);
    mBorders = mShape;
}

void MapItemMovable::setInitialPosition(QPointF p)
{
    mInitialPosition = p;
}

void MapItemMovable::setReadyToDelete()
{
    mReadyToDelete = true;
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

    mBoundingRect = QRect(0,0,100,100);
    mZOffset = mBoundingRect.height()*3/4;
    mImageSelected = QRandomGenerator::global()->bounded(RES_PLANK);
    setImage(QPixmap(":/MapItems/plank.png"));

    setZValue(Z_PLANK);
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

    mBoundingRect = QRect(0,0,70,70);
    mZOffset = mBoundingRect.height()*3/4;
    mImageSelected = QRandomGenerator::global()->bounded(RES_STONE);
    setImage(QPixmap(":/MapItems/stone.png"));

    setZValue(Z_STONE);
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

    mBoundingRect = QRect(0,0,700,700);
    mImageSelected = QRandomGenerator::global()->bounded(RES_LAKE);
    setImage(QPixmap(":/MapItems/lake.png"));

    QPainterPath collidingShape;
    QPolygon polygon;
    static const int points[] = {
        10, 395,
        33, 486,
        185, 590,
        450, 610,
        503, 512,
        541, 376,
        517, 185,
        390, 121,
        333, 158,
        138, 207,
        18, 329
    };
    polygon.setPoints(11, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_LAKE);
}

MapItem *Lake::getEvent()
{
    return mEvent;
}

Lake::~Lake()
{
    delete mEvent;
}

