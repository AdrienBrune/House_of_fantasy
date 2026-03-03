#include "w_animation_fight.h"

W_Animation_Fight::W_Animation_Fight(QWidget *parent, QPixmap attackImage) :
    QWidget(parent),
    pAttackPixmap(attackImage)
{
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_hideAnimation()));
    timer->setSingleShot(true);
    timer->start(1500);

    show();
}

W_Animation_Fight::~W_Animation_Fight()
{

}

void W_Animation_Fight::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(QRect(0,0,width(),height()), pAttackPixmap);
}
