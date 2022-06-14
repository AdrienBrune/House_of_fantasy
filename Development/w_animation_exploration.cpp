#include "w_animation_exploration.h"

W_Animation_exploration::W_Animation_exploration(QWidget *parent) :
    QWidget(parent),
    mNextStep(0)
{
    QTimer * timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(15);
    setStyleSheet("background-color:black;");
}

void W_Animation_exploration::animate()
{
    mNextStep++;
    update();
    if(mNextStep > 200){
        mNextStep = 200;
        emit sig_explorationCompleted();
    }
}

void W_Animation_exploration::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = static_cast<int>(width());
    int h = static_cast<int>(height());
    QConicalGradient gradient(QPointF(w/2,h/2), 96.0);
    gradient.setColorAt(1.0, QColor("#14FFFFFF"));
    gradient.setColorAt(0.1, QColor("#FF000000"));

    int penWidth = 20;
    int angle = static_cast<int>(360.0*mNextStep/200.0);
    int startAngle = (270 - angle/2);
    int offset = 50;

    QPen pen(gradient, penWidth);
    pen.setDashPattern(QVector<qreal>{1.0,1.2});
    painter.setPen(pen);

    painter.drawArc(QRect(penWidth,penWidth,w-2*penWidth,h-2*penWidth),90*16, -angle*16);

    painter.setPen(QPen(QBrush("#000000"), 2));
    painter.setBrush(QBrush("#c8c8c8"));
    painter.drawChord(QRect(offset, offset, w-2*offset, h-2*offset), startAngle*16, angle*16);

    if(angle/360.0 > 0.6){
        painter.setOpacity(((angle/360.0)-0.6)*2.2);
        painter.drawPixmap(QRect(offset+30, offset+30, w-2*(offset+30), h-2*(offset+30)), QPixmap(":/graphicItems/glass.png"));
    }

    //int dynamicHeight = static_cast<int>((1.0-angle/360.0)*(h-2*(offset)));
    //painter.eraseRect(QRectF(offset, offset, w-2*(offset), dynamicHeight));
}

W_Animation_exploration::~W_Animation_exploration()
{

}
