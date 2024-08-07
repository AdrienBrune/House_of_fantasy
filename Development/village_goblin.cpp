#include "village_goblin.h"

// TODO : 3 obstacles !!

Village_Goblin::Village_Goblin():
    mImage(QPixmap(":/MapItems/village_goblin_ground.png")),
    mRoadsWeight(200)
{
    mTownHall = new TownHall();
    for(int n=0;n<2;n++)
    {
        mWatchTowers.append(new WatchTower());
    }
    for(int n=0;n<9;n++)
    {
        mHuts.append(new Hut());
    }
    mChest = new GoblinChest();
    connect(mChest, SIGNAL(sig_clicToOpenChest(ChestEvent*)), this, SIGNAL(sig_clicToOpenChest(ChestEvent*)));
}

Village_Goblin::~Village_Goblin()
{
    delete mTownHall;
    while(!mHuts.isEmpty())
        delete mHuts.takeLast();
    while(!mWatchTowers.isEmpty())
        delete mWatchTowers.takeLast();
    delete mChest;
}

void Village_Goblin::addInScene(QGraphicsScene * scene)
{
    scene->addItem(this);

    for(WatchTower * tower : mWatchTowers)
    {
        scene->addItem(tower);
    }
    for(Hut * hut : mHuts)
    {
        scene->addItem(hut);
    }
    scene->addItem(mTownHall);
    scene->addItem(mChest);
    mChest->setZValue(Z_VILLAGE);
}

void Village_Goblin::setPosition(QPointF pos)
{
    mPosition = QPointF(pos.x()+boundingRect().width()/2, pos.y()+boundingRect().height()/2);
    setPos(pos);

    mTownHall->setPos(mPosition.x()-mTownHall->boundingRect().width()/2, mPosition.y()-mTownHall->boundingRect().height()/2-150);
    mChest->setPos(mTownHall->x()+60, mTownHall->y()+315);

    QList<WatchTower*> towersPut;
    for(WatchTower * tower : mWatchTowers)
    {
        tower->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-tower->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-tower->boundingRect().height())));
        bool verifTowersPosition = false;
        while(!verifTowersPosition)
        {
            tower->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-tower->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-tower->boundingRect().height())));
            while(tower->collidesWithItem(this))
                tower->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-tower->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-tower->boundingRect().height())));

            verifTowersPosition = false;
            for(WatchTower * towerToCheck : towersPut)
            {
                if(ToolFunctions::getDistanceBeetween(towerToCheck, tower) < 300){
                    break;
                }
                if(towersPut.last() == towerToCheck){
                    verifTowersPosition = true;
                }
            }
            if(towersPut.size() == 0){
                verifTowersPosition = true;
            }
        }
        towersPut.append(tower);
    }

    QList<Hut*> hutsPut;
    for(Hut * hut : mHuts)
    {
        hut->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-hut->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-hut->boundingRect().height())));
        hut->setRotation(QRandomGenerator::global()->bounded(20)-10);

        bool verifHutsPosition = false, verifTowersPosition = false;
        while(!verifHutsPosition || !verifTowersPosition)
        {
            hut->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-hut->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-hut->boundingRect().height())));
            hut->setRotation(QRandomGenerator::global()->bounded(20)-10);
            while(hut->collidesWithItem(this)){
                hut->setPos(this->pos().x() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().width()-hut->boundingRect().width())), this->pos().y() + QRandomGenerator::global()->bounded(static_cast<int>(boundingRect().height()-hut->boundingRect().height())));
                hut->setRotation(QRandomGenerator::global()->bounded(20)-10);
            }

            verifHutsPosition = false;
            for(Hut * hutToCheck : hutsPut)
            {
                int dx = static_cast<int>(hut->x() - hutToCheck->x());
                int dy = static_cast<int>(hut->y() - hutToCheck->y());
                if( (dy < 100 && dy > -hutToCheck->boundingRect().height()) && (abs(dx) < hutToCheck->boundingRect().width()) ){
                    break;
                }
                if(ToolFunctions::getDistanceBeetween(hutToCheck, hut) < 50){
                    break;
                }
                if(hutsPut.last() == hutToCheck){
                    verifHutsPosition = true;
                }
            }
            verifTowersPosition = false;
            for(WatchTower * tower : mWatchTowers)
            {
                if( ((hut->y() < tower->y()+tower->boundingRect().height()-hut->boundingRect().height()+50) && (hut->y() > tower->y()-hut->boundingRect().height()))
                        && (abs(hut->x()-tower->x()) < hut->boundingRect().width()) ){
                    break;
                }
                if(mWatchTowers.last() == tower){
                    verifTowersPosition = true;
                }
            }
            if(hutsPut.size() == 0){
                verifHutsPosition = true;
            }
        }
        hutsPut.append(hut);
    }
}

QPointF Village_Goblin::getPosition()
{
    return mPosition;
}

#define USE_ROADS
QPainterPath Village_Goblin::shape() const
{
    QPainterPath path;
#ifndef USE_ROADS
    path.addRect(QRect(0,0,0,0));
    return path;
#else
    QPolygon polygon;
    static const int points[] = {
        //static_cast<int>(boundingRect().width())/2-mRoadsWeight/2,0,
        //static_cast<int>(boundingRect().width())/2+mRoadsWeight/2,0,

        static_cast<int>(boundingRect().width())/2+mRoadsWeight/2,static_cast<int>(boundingRect().height())/4,
        static_cast<int>(boundingRect().width())*3/4,static_cast<int>(boundingRect().height())/2-mRoadsWeight/2,

        static_cast<int>(boundingRect().width()),static_cast<int>(boundingRect().height())/2-mRoadsWeight/2,
        static_cast<int>(boundingRect().width()),static_cast<int>(boundingRect().height())/2+mRoadsWeight/2,

        static_cast<int>(boundingRect().width())*5/8,static_cast<int>(boundingRect().height())/2+mRoadsWeight/2,
        static_cast<int>(boundingRect().width()/2+mRoadsWeight/2),static_cast<int>(boundingRect().height())*5/8,


        static_cast<int>(boundingRect().width()/2+mRoadsWeight/2),static_cast<int>(boundingRect().height()),
        static_cast<int>(boundingRect().width()/2-mRoadsWeight/2),static_cast<int>(boundingRect().height()),

        static_cast<int>(boundingRect().width()/2-mRoadsWeight/2),static_cast<int>(boundingRect().height())*5/8,
        static_cast<int>(boundingRect().width())*3/8,static_cast<int>(boundingRect().height())/2+mRoadsWeight/2,

        0,static_cast<int>(boundingRect().height())/2+mRoadsWeight/2,
        0,static_cast<int>(boundingRect().height())/2-mRoadsWeight/2,

        static_cast<int>(boundingRect().width())/4,static_cast<int>(boundingRect().height())/2-mRoadsWeight/2,
        static_cast<int>(boundingRect().width()/2-mRoadsWeight/2),static_cast<int>(boundingRect().height())/4,
    };
    polygon.setPoints(14, points);
    path.addPolygon(polygon);
    return path;
#endif
}

QRectF Village_Goblin::boundingRect() const
{
    return QRect(0,0,1500,1500);
}

void Village_Goblin::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mImage, 0, 0, static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget);
    Q_UNUSED(option);
}






Hut::Hut():
    MapItem ()
{
    mBoundingRect = QRect(0,0,200,200);
    mZOffset = mBoundingRect.height()*3/4;
    setImage(QPixmap(":/MapItems/goblin_hut.png"));

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        28, 158,
        98, 191,
        174, 159,
        164, 134,
        104, 125,
        29, 132
    };
    polygon.setPoints(6, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_VILLAGE);
}

Hut::~Hut()
{

}






TownHall::TownHall():
    MapItem ()
{
    mBoundingRect = QRect(0,0,300,400);
    mZOffset = mBoundingRect.height()*3/4;
    setImage(QPixmap(":/MapItems/goblin_townhall.png"));

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        50, 273,
        50, 330,
        167, 370,
        255, 330,
        257, 253,
        155, 244
    };
    polygon.setPoints(6, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_VILLAGE);
}

TownHall::~TownHall()
{

}








WatchTower::WatchTower():
    MapItem ()
{
    mBoundingRect = QRect(0,0,200,300);
    mZOffset = mBoundingRect.height()*255/300;
    setImage(QPixmap(":/MapItems/goblin_watchtower.png"));

    QPolygon polygon;
    QPainterPath collidingShape;
    static const int points[] = {
        36, 258,
        100, 283,
        150, 261,
        100, 229
    };
    polygon.setPoints(4, points);
    collidingShape.addPolygon(polygon);
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

    setZValue(Z_VILLAGE);
}

WatchTower::~WatchTower()
{

}








Village_Goblin_Area::Village_Goblin_Area():
    QGraphicsItem ()
{
    mVillage = new Village_Goblin();
}

Village_Goblin_Area::~Village_Goblin_Area()
{
    delete mVillage;
}

Village_Goblin *Village_Goblin_Area::getVillage()
{
    return mVillage;
}

void Village_Goblin_Area::addInScene(QGraphicsScene * scene)
{
    scene->addItem(this);
    mVillage->addInScene(scene);
}

void Village_Goblin_Area::setPosition(QPointF pos)
{
    mVillage->setPosition(QPointF(pos.x()+(boundingRect().width()-mVillage->boundingRect().width())/2,
                                  pos.y()+(boundingRect().height()-mVillage->boundingRect().height())/2));
    setPos(pos);
}

QPainterPath Village_Goblin_Area::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

QRectF Village_Goblin_Area::boundingRect() const
{
    return QRect(0,0,2000,2000);
}

void Village_Goblin_Area::paint(QPainter *, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
}
