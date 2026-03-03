#ifndef W_ANIMATION_NIGHT_H
#define W_ANIMATION_NIGHT_H

#include <QWidget>
#include <QPainter>
#include "map.h"

class W_Animation_Night : public QWidget
{
    Q_OBJECT
public:
    explicit W_Animation_Night(QWidget *parent = nullptr);
    ~W_Animation_Night();
signals:
    void sig_playMusic(int);
private slots:
    void onStartNewDay();
    void onUpdateSleep(quint8 step);
protected:
    void paintEvent(QPaintEvent *event);
private:
    float mNightStep;
    quint8 mZzzStep;
};

#endif // W_ANIMATION_NIGHT_H
