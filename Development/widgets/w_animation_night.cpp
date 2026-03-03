#include "w_animation_night.h"
#include <QApplication>

W_Animation_Night::W_Animation_Night(QWidget *parent) :
    QWidget(parent),
    mNightStep(1.0),
    mZzzStep(0)
{
    show();
}

W_Animation_Night::~W_Animation_Night()
{

}

void W_Animation_Night::onStartNewDay()
{
    emit sig_playMusic(MUSICEVENT_START_DAY);
    close();
}

void W_Animation_Night::onUpdateSleep(quint8 step)
{
    mZzzStep > 2 ? mZzzStep = 1 : mZzzStep++;
    mNightStep = (100-step)/100.0;
    update();
    QApplication::processEvents(); // Needed, function call in a slot
}

void W_Animation_Night::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect loadingArea(width()/4, height()-height()/10, width()/2, 20);

    painter.drawPixmap(QRect(0,0,width(),height()), QPixmap(":/graphicItems/bedroom.png"));

    painter.setPen(QPen("#CCCCCC"));
    painter.setFont(QFont("Sitka Small", 20));
    painter.drawText(QRect(0,0,width(),height()), Qt::AlignCenter, "Zzz");
    //painter.drawText(QRect(0,0,width(),height()), Qt::AlignCenter, mZzzStep == 1 ? "Z" : mZzzStep == 2 ? "Zz" : mZzzStep == 3 ? "Zzz" : "");

    painter.setOpacity(mNightStep);
    painter.setPen(QPen(QBrush(), 0));
    painter.setBrush(QBrush(QColor(0,0,0)));
    painter.drawRect(QRect(0,0,width(),height()));

    painter.setBrush(QBrush(QColor(250,250,250)));
    painter.setOpacity(0.2);
    painter.drawRect(loadingArea);
    painter.setOpacity(1);
    painter.drawRect(loadingArea.x(), loadingArea.y(), loadingArea.width()*(1-mNightStep)*1.3, loadingArea.height());
}
