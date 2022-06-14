#include "w_animation_forge.h"

W_Animation_Forge::W_Animation_Forge(QWidget *parent) :
    QWidget(parent),
    mAnimationStep(0),
    mPixmap(QPixmap(":/graphicItems/animation_forge.png")),
    mCurPixmap(0),
    mAnimationVar(0)
{
    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));
    t_animation->start(600);
    show();
}

W_Animation_Forge::~W_Animation_Forge()
{

}

void W_Animation_Forge::animate()
{
   mAnimationVar++;
   if(mAnimationVar == 1){
        mCurPixmap = 0;
   }else{
        mCurPixmap = 1;
        mAnimationStep += 960;
        mAnimationVar = 0;
        sig_playSound(SOUND_FORGE);
   }

   if(mAnimationStep >= 5760){
       t_animation->stop();
       QTimer * timer = new QTimer(this);
       connect(timer, SIGNAL(timeout()), this, SLOT(closeAnimation()));
       timer->setSingleShot(true);
       timer->start(500);
   }
   update();
}

void W_Animation_Forge::closeAnimation()
{
    close();
}

void W_Animation_Forge::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.setBrush(QBrush());

    painter.setPen(QPen(QBrush("#FFFFFF"),100));
    painter.drawArc(QRect((width()-600)/2+70,(height()-600)/2+70,600-140, 600-140), 90*16, mAnimationStep);

    painter.drawPixmap((width()-600)/2,(height()-600)/2, mPixmap, 0, 600*mCurPixmap, 600, 600);
}
