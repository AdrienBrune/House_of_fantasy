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
    ToolFunctions() = delete;
    ToolFunctions(const ToolFunctions&) = delete;
    ToolFunctions& operator=(const ToolFunctions&) = delete;
public:
    static QRectF getVisibleView(QGraphicsView*);
    static QRectF getBiggerView(QGraphicsView*);
    static int getAngleBetween(QGraphicsItem *, QGraphicsItem *);
    static int getRandomAngle();
    static bool isOppositeDirection(int previous, int current);
    static int correct(int angle);
    static bool isAllowedAngle(int);
    static int getDistanceBeetween(QGraphicsItem*, QGraphicsItem*);
    static int getNumberObstacles(QList<QGraphicsItem*>);
    static QRect getSpawnChunk(QSize size, QList<QGraphicsItem*> itemsToAvoid);
    template<typename T>
    static void matrix2DResize(QVector<QVector<T>> & matrix, int x, int y)
    {
        matrix.resize(y);
        for(int index = 0; index < matrix.size(); index++)
            matrix[index].resize(x);
    }
    template<typename T>
    static void matrix2DInit(QVector<QVector<T>> & matrix, T value)
    {
        for(int row = 0; row < matrix.size(); row++)
            for(int col = 0; col < matrix[row].size(); col++)
                matrix[row][col] = value;
    }
};

#endif // TOOLS_H
