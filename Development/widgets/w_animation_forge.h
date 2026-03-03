#ifndef W_ANIMATION_FORGE_H
#define W_ANIMATION_FORGE_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "constants.h"

class W_Animation_Forge : public QWidget
{
    Q_OBJECT

public:
    explicit W_Animation_Forge(QWidget *parent = nullptr);
    ~W_Animation_Forge();
signals:
    void sig_playSound(int);
private slots:
    void animate();
    void closeAnimation();
protected:
    void paintEvent(QPaintEvent *event);
private:
    int mAnimationStep;
    QTimer * t_animation;
    QPixmap mPixmap;
    int mCurPixmap;
    int mAnimationVar;
};

#endif // W_ANIMATION_FORGE_H
