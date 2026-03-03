#include "w_animation_exploration.h"

#define COLOR_BLACK         QColor(40,40,40)
#define COLOR_GREY_LIGHT    QColor(180,180,180)
#define COLOR_ORANGE        QColor(230,190,105)

W_Animation_exploration::W_Animation_exploration(QWidget *parent) :
    QWidget(parent),
    mNextStep(0)
{
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(10);
}

void W_Animation_exploration::animate()
{
    mNextStep++;
    update();
    if(mNextStep > 100){
        mNextStep = 100;
        emit sig_explorationCompleted();
    }
}

void W_Animation_exploration::paintEvent(QPaintEvent *)
{
#define ELIPSE_DIAMETER height()

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

/* New animation */
    QRect chunkBackground(QRect(0, height()/3, width(), height()/3));
    QRect chunkForeground(QRect(chunkBackground.x(), chunkBackground.y(), chunkBackground.width(), chunkBackground.height()-2));
    QRect chunkDynamicRight(QRect(chunkForeground.x()+chunkForeground.width()/2, chunkForeground.y(), (chunkForeground.width()/2)*(mNextStep/100.0), chunkForeground.height()));
    QRect chunkDynamicLeft(QRect((chunkForeground.x()+chunkForeground.width()/2)*(1-(mNextStep/100.0)), chunkForeground.y(), (chunkForeground.width()/2)*(mNextStep/100.0), chunkForeground.height()));
    QRect elipseArea(QRect(chunkBackground.x()+chunkBackground.width()/2-ELIPSE_DIAMETER/2, 0, ELIPSE_DIAMETER, height()));

    painter.setBrush(QBrush(COLOR_BLACK));
    painter.drawRoundedRect(chunkBackground, 10, 10, Qt::SizeMode::RelativeSize);

    painter.setBrush(QBrush(COLOR_GREY_LIGHT));
    painter.drawRoundedRect(chunkForeground, 10, 10, Qt::SizeMode::RelativeSize);

    painter.setBrush(QBrush(COLOR_BLACK));
    painter.drawRoundedRect(chunkDynamicLeft, 10, 10, Qt::SizeMode::RelativeSize);
    painter.setBrush(QBrush(COLOR_BLACK));
    painter.drawRoundedRect(chunkDynamicRight, 10, 10, Qt::SizeMode::RelativeSize);

    painter.setBrush(QBrush(COLOR_BLACK));
    painter.drawEllipse(elipseArea);
    painter.drawPixmap(QRect(elipseArea.x()+5, elipseArea.y()+5, elipseArea.width()-10, elipseArea.height()-10), QPixmap(":/graphicItems/glass.png"));


/* Old animation */
    // int w = static_cast<int>(width());
    // int h = static_cast<int>(height());
    // QConicalGradient gradient(QPointF(w/2,h/2), 96.0);
    // gradient.setColorAt(1.0, QColor("#14FFFFFF"));
    // gradient.setColorAt(0.1, QColor("#FF000000"));

    // int penWidth = 20;
    // int angle = static_cast<int>(360.0*mNextStep/200.0);
    // int startAngle = (270 - angle/2);
    // int offset = 50;

    // QPen pen(gradient, penWidth);
    // pen.setDashPattern(QVector<qreal>{1.0,1.2});
    // painter.setPen(pen);

    // painter.drawArc(QRect(penWidth,penWidth,w-2*penWidth,h-2*penWidth),90*16, -angle*16);

    // painter.setPen(QPen(QBrush("#000000"), 2));
    // painter.setBrush(QBrush("#c8c8c8"));
    // painter.drawChord(QRect(offset, offset, w-2*offset, h-2*offset), startAngle*16, angle*16);

    // if(angle/360.0 > 0.6){
    //     painter.setOpacity(((angle/360.0)-0.6)*2.2);
    //     painter.drawPixmap(QRect(offset+30, offset+30, w-2*(offset+30), h-2*(offset+30)), QPixmap(":/graphicItems/glass.png"));
    // }
}

W_Animation_exploration::~W_Animation_exploration()
{

}
