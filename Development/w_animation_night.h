#ifndef W_ANIMATION_NIGHT_H
#define W_ANIMATION_NIGHT_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "map.h"

class W_Animation_Night : public QWidget
{
    Q_OBJECT
public:
    explicit W_Animation_Night(QWidget *parent = nullptr, Map * map = nullptr);
    ~W_Animation_Night();
signals:
    void sig_playMusic(int);
    void sig_startMapGeneration();
private slots:
    void startMapRevelation();
    void animate();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Map * mMap;
    int mCurAnimationStep;
    int mAnimationStep;
    QTimer * t_animation;
};

#endif // W_ANIMATION_NIGHT_H
