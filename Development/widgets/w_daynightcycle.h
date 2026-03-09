#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QtMath>
#include <QMouseEvent>

class W_DayNightCycle : public QWidget
{
    Q_OBJECT

public:
    explicit W_DayNightCycle(QWidget *parent = nullptr);
    ~W_DayNightCycle(){}

public slots:
    void OnUpdateTime(qreal time);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override  { event->ignore(); }
    void mouseReleaseEvent(QMouseEvent *event) override { event->ignore(); }

private:
    qreal   mTime;
    bool    mIconHovered = false;
    QRectF  mIconRect;
};
