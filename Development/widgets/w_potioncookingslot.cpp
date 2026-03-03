#include "w_potioncookingslot.h"

void W_PotionCookingSlot::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect(10, 10, width() - 20, height() - 20);

    painter.setPen(QPen(QColor(250,250,250,0), 1));
    painter.setBrush(QBrush(QColor(250, 250, 250, 10)));
    painter.drawRect(0,0,width(),height());

    if(!mSlot)
        return;

    if(mSlot->getCookTime().current == 0)
    {
        QPen pen(QColor(250,250,250,250), 14);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        QPointF center = rect.center();
        qreal plusSize = qMin(rect.width(), rect.height()) / 3;

        painter.drawLine(QPointF(center.x(), center.y() - plusSize / 2),
                        QPointF(center.x(), center.y() + plusSize / 2));
        painter.drawLine(QPointF(center.x() - plusSize / 2, center.y()),
                        QPointF(center.x() + plusSize / 2, center.y()));
    }
    else
    {
        QRect spanArea(rect.x() + 5, rect.y() + 5, rect.width() - 10, rect.height() - 10);

        painter.setPen(QPen(QColor(250,250,250,5), 20));
        painter.drawArc(spanArea, 0, 360 * 16);

        qreal spanAngle = 360.0 * (1.0 - (qreal)mSlot->getCookTime().current / mSlot->getCookTime().max);

        painter.setPen(QPen(QColor(250,250,250,250), 20));
        painter.drawArc(spanArea, 90 * 16, -spanAngle * 16);

        if(mSlot->getPotion())
        {
            QPixmap potionImage = mSlot->getPotion()->getImage();
            painter.drawPixmap(rect, mSlot->getPotion()->getImage(),  mSlot->getPotion()->getImage().rect());
        }
    }
}
