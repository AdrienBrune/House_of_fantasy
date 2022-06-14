#include "toolfunctions.h"

ToolFunctions::ToolFunctions()
{

}

QRectF ToolFunctions::getVisibleView(QGraphicsView * view)
{
    QPointF A = view->mapToScene( QPoint(0, 0) );
    QPointF B = view->mapToScene( QPoint(view->viewport()->width(), view->viewport()->height() ));
    return QRectF( A, B );
}

QRectF ToolFunctions::getBiggerView(QGraphicsView * view)
{
    QPointF A = view->mapToScene( QPoint(-300, -300) );
    QPointF B = view->mapToScene( QPoint(view->viewport()->width()+600, view->viewport()->height()+600 ));
    return QRectF( A, B );
}

qreal ToolFunctions::getAngleWithHero(QGraphicsItem * hero, QGraphicsItem * monster)
{
    qreal dy = monster->y()+monster->boundingRect().height()/2-hero->y()-hero->boundingRect().height()/2, dx = monster->x()+monster->boundingRect().width()/2-hero->x()+hero->boundingRect().width()/2;
    qreal angle = qRadiansToDegrees(qAtan(dy/dx));
    if(dy > 0){
        if(dx < 0){     // BOT-LEFT
            return 90 - abs(angle);
        }else{          // BOT-RIGHT
            return 270 + angle;
        }
    }
    else{
        if(dx < 0){     // TOP-LEFT
            return 90 + angle;
        }else{          // TOP-RIGHT
            return 180 + 90 + angle;
        }
    }
}

qreal ToolFunctions::getRandomAngle()
{
    qreal angle = 0;

    do{
    angle = QRandomGenerator::global()->bounded(360);
    }while(!ToolFunctions::isAllowedAngle(angle));

    return angle;
}

bool ToolFunctions::isAllowedAngle(qreal angle)
{
    if((angle>55 && angle<125) || (angle>235 && angle<305))
        return true;
     else
        return false;
}

qreal ToolFunctions::heroDistanceWith(QGraphicsItem * hero, QGraphicsItem * monster)
{
    QLineF line(hero->mapToScene(hero->boundingRect().width()/2,hero->boundingRect().height()/2),
                monster->mapToScene(monster->boundingRect().width()/2,monster->boundingRect().height()/2));
    return line.length();
    /*
    int dx = static_cast<int>(monster->x()+monster->boundingRect().width()/2 - hero->x()+hero->boundingRect().width()/2);
    int dy = static_cast<int>(monster->y()+monster->boundingRect().height()/2 - hero->y()+hero->boundingRect().height()/2);
    return qSqrt(qPow(dx,2)+qPow(dy,2));
    */
}

qreal ToolFunctions::getDistanceBeetween(QGraphicsItem * item1, QGraphicsItem * item2)
{
    QLineF line(item1->mapToScene(item1->boundingRect().width()/2,item1->boundingRect().height()/2),
                item2->mapToScene(item2->boundingRect().width()/2,item2->boundingRect().height()/2));
    return line.length();
    /*
    QPointF p1(item1->x()+item1->boundingRect().width()/2,item1->y()+item1->boundingRect().height()/2),
            p2(item2->x()+item2->boundingRect().width()/2,item2->y()+item2->boundingRect().height()/2);
    int dx = static_cast<int>(abs(p2.x()-p1.x())),
        dy = static_cast<int>(abs(p2.y()-p1.y()));
    return qSqrt(qPow(dx,2)+qPow(dy,2));
    */
}

qreal ToolFunctions::getAngleWithVillage(QGraphicsItem * hero, QPointF villagePos)
{
    //qreal dy = hero->y()-villagePos.y(), dx = hero->x()-villagePos.x();
    qreal dy = villagePos.y()-hero->y(), dx = villagePos.x()-hero->x();
    qreal angle = qRadiansToDegrees(qAtan(dy/dx));
    if(dy > 0){
        if(dx < 0){     // BOT-LEFT
            return abs(angle)+180;
        }else{          // BOT-RIGHT
            return 90 + abs(angle-90) + 180;
        }
    }
    else{
        if(dx < 0){     // TOP-LEFT
            return abs(angle-90) + 90;
        }else{          // TOP-RIGHT
            return abs(angle);
        }
    }
}

int ToolFunctions::getNumberObstacles(QList<QGraphicsItem*> list)
{
    int count = 0;
    for(QGraphicsItem * item : list)
    {
        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem){
            if(mapItem->isObstacle()){
                count++;
            }
        }
    }
    return count;
}

int ToolFunctions::getNumberObstacles(QList<QGraphicsItem*> list, QList<QGraphicsItem*> collidingWith)
{
    for(QGraphicsItem * item : list)
    {
        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem){
            if(mapItem->isObstacle()){
                collidingWith.append(mapItem);
            }
        }
    }
    return collidingWith.size();
}

bool ToolFunctions::compareObstacles(QList<QGraphicsItem *> list1, QList<QGraphicsItem *> list2)
{
    if(list1.length()!=list2.length()){
        return false;
    }
    int same = 0;
    for(QGraphicsItem * item1 : list1)
    {
        for(QGraphicsItem * item2 : list2)
        {
            if(item1 == item2){
                same++;
            }
        }
    }
    if(same == list1.length()){
        return true;
    }else{
        return false;
    }
}

QRect ToolFunctions::getSpawnChunk(QSize size, QList<QGraphicsItem *> itemsToAvoid)
{
    QRect spawn;
    bool validatePosition = false;
    while(!validatePosition){
        validatePosition = true;
        for(QGraphicsItem * itemToAvoid : itemsToAvoid)
        {
            spawn = QRect(QPoint(QRandomGenerator::global()->bounded(500+size.width()/2, MAP_WIDTH-500-size.width()), QRandomGenerator::global()->bounded(size.height()/2+500, MAP_HEIGHT-500-size.height())), size);
            if(spawn.intersects(QRect(static_cast<int>(itemToAvoid->x()), static_cast<int>(itemToAvoid->y()), static_cast<int>(itemToAvoid->boundingRect().width()), static_cast<int>(itemToAvoid->boundingRect().height())))){
                validatePosition = false;
                break;
            }
        }
    }
    return spawn;
}










