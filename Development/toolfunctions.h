#ifndef TOOLS_H
#define TOOLS_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtMath>
#include "mapitem.h"

class ToolFunctions
{
public:
    ToolFunctions();
public:
    static QRectF getVisibleView(QGraphicsView*);
    static QRectF getBiggerView(QGraphicsView*);
    static qreal getAngleWithHero(QGraphicsItem *, QGraphicsItem *);
    static qreal getRandomAngle();
    static bool isAllowedAngle(qreal);
    static qreal heroDistanceWith(QGraphicsItem *, QGraphicsItem *);
    static qreal getDistanceBeetween(QGraphicsItem*, QGraphicsItem*);
    static qreal getAngleWithVillage( QGraphicsItem *, QPointF);
    static int getNumberObstacles(QList<QGraphicsItem*>);
    static int getNumberObstacles(QList<QGraphicsItem*>, QList<QGraphicsItem*>);
    static bool compareObstacles(QList<QGraphicsItem*>, QList<QGraphicsItem*>);
    static QRect getSpawnChunk(QSize size, QList<QGraphicsItem*> itemsToAvoid);
};

#endif // TOOLS_H
