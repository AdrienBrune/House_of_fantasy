#include "w_animation_night.h"

W_Animation_Night::W_Animation_Night(QWidget *parent, Map * map) :
    QWidget(parent),
    mMap(map),
    mCurAnimationStep(0),
    mAnimationStep(1)
{
    connect(mMap, SIGNAL(sig_generationMapComplete()), this, SLOT(startMapRevelation()));

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(animate()));
    t_animation->start(500);

    show();
}

W_Animation_Night::~W_Animation_Night()
{

}

void W_Animation_Night::startMapRevelation()
{
    mAnimationStep = -1;
    t_animation->start(1000);
    emit sig_playMusic(MUSICEVENT_START_DAY);
}

void W_Animation_Night::animate()
{
    mCurAnimationStep += mAnimationStep;
    update();
    if(mCurAnimationStep >= 6){
        t_animation->stop();
        mMap->reGenerateMap();
    }
    if(mCurAnimationStep <= 0){
        t_animation->stop();
        close();
    }
}

void W_Animation_Night::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#202020"));
    painter.setOpacity(static_cast<qreal>(mCurAnimationStep/5.0));
    painter.drawRect(0,0,width(),height());

    if(mCurAnimationStep >= 5){
        painter.setPen(QPen("#FFFFFF"));
        painter.setFont(QFont("Sitka Small", 30));
        painter.drawText(QRect(width()/4, height()/4, width()/2, height()/2), Qt::AlignCenter, "Vous dormez ...\nVeuillez patienter");
    }
}
