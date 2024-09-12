#include "toolfunctions.h"

#define IS_LEFT_SIDE(angle)  (angle < 270 && angle >= 90)
#define IS_RIGHT_SIDE(angle) (angle >= 270 || angle < 90)

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

int ToolFunctions::getAngleBetween(QGraphicsItem * instance1, QGraphicsItem * instance2)
{
    int angle = static_cast<int>(qRadiansToDegrees(qAtan2(
                        (instance1->y() + instance1->boundingRect().height()/2) - (instance2->y() + instance2->boundingRect().height()/2),
                        (instance1->x() + instance1->boundingRect().width()/2) - (instance2->x() + instance2->boundingRect().width()/2)
                    )));

    if(angle < 0)   // Hero on top side
        angle = angle + 360;

    return angle;
}

int ToolFunctions::getRandomAngle()
{
    int angle = 0;

    do{
        angle = QRandomGenerator::global()->bounded(360);
    }while(!ToolFunctions::isAllowedAngle(angle));

    return angle;
}

bool ToolFunctions::isOppositeDirection(int previous, int current)
{
    if( (IS_LEFT_SIDE(previous) && IS_RIGHT_SIDE(current))
     || (IS_LEFT_SIDE(current) && IS_RIGHT_SIDE(previous)))
    {
        return true;
    }
    return false;
}

int ToolFunctions::correct(int angle)
{
    if(angle > 90 && angle < 270)
    {
        // Correction when Y mirroring is applied
        return (180 - angle) - 180;
    }
    else
        return angle;
}

bool ToolFunctions::isAllowedAngle(int angle)
{
#define ANGLE_ALLOWED 90
    if((angle > (360 - ANGLE_ALLOWED/2) || angle < (ANGLE_ALLOWED/2))
    || (angle > (180 - ANGLE_ALLOWED/2) && angle < (180 + ANGLE_ALLOWED/2)))
    {
        return true;
    }
    else
        return false;
}

int ToolFunctions::getDistanceBeetween(QGraphicsItem * item1, QGraphicsItem * item2)
{
    QLineF line(item1->mapToScene(item1->boundingRect().width()/2, item1->boundingRect().height()/2),
                item2->mapToScene(item2->boundingRect().width()/2, item2->boundingRect().height()/2));
    return line.length();
}

int ToolFunctions::getNumberObstacles(QList<QGraphicsItem*> list)
{
    int count = 0;
    for(QGraphicsItem * item : qAsConst(list))
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










