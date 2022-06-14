#ifndef W_ANIMATION_COMPASS_H
#define W_ANIMATION_COMPASS_H

#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QTimer>
#include "hero.h"
#include "village.h"
#include "toolfunctions.h"
#include "item.h"

class W_UseTool : public QWidget
{
    Q_OBJECT
public:
    explicit W_UseTool(QWidget *parent = nullptr, Tool * tool = nullptr, Hero * hero = nullptr, Village * village = nullptr);
    ~W_UseTool();
private slots:
    void refresh();
public:
    QPushButton * getExitButton();
    void displayTool();
    Tool * getTool();
private:
    void hideTool();
protected:
    void paintEvent(QPaintEvent*);
private:
    Tool * mTool;
    Hero * mHero;
    QPointF mVillagePosition;
    QPushButton * mExitButton;
};

#endif // W_ANIMATION_COMPASS_H
