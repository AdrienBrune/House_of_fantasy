#include "monsterfightview.h"
#include <QtGlobal>

MonsterFightView::MonsterFightView(QWidget * parent):
    QWidget(parent),
    mMonster(nullptr),
    mBiome(eBiomes::forest),
    mAttack(false),
    mAttacked(false),
    mAffliction(0),
    mPrevStamina(0)
{
    setBiome(eBiomes::forest);
    mTimerRestore.setSingleShot(true);
    connect(&mTimerRestore, &QTimer::timeout, this, &MonsterFightView::onRestore);
}

void MonsterFightView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(rect(), mBackground);

    if(!mMonster)
        return;

    // Scale monster coordinates proportionally to the widget's actual size
    // Reference size is 800x400 (the original fixed dimensions)
    const qreal scaleX = static_cast<qreal>(width())  / 800.0;
    const qreal scaleY = static_cast<qreal>(height()) / 400.0;
    const QRect dims = mMonster->getFightView()->getDimensions();
    QRect source(static_cast<int>(dims.x() * scaleX),
                 static_cast<int>(dims.y() * scaleY),
                 static_cast<int>(dims.width()  * scaleX),
                 static_cast<int>(dims.height() * scaleY));

    QRect biggerArea(source.x() + source.width()/10, source.y() + source.height()/10, source.width()*12/10, source.height()*12/10);
    QRect smallerArea(source.x() - source.width()/10, source.y() - source.height()/10, source.width()*8/10, source.height()*8/10);
    QRect *monsterArea = nullptr;

    // Image selection when monster is hurt / attack
    quint8 imageIndex = qBound(0, static_cast<int>(4-(static_cast<double>(mMonster->getLife().current)/static_cast<double>(mMonster->getLife().maximum))*4), 3);

    if(mAttacked)
        monsterArea = &smallerArea;
    else if(mAttack)
        monsterArea = &biggerArea;
    else
        monsterArea = &source;

    // Draw shadow  (source rect uses original sprite dims, dest rect uses scaled dims)
    painter.drawPixmap(*monsterArea, mMonster->getFightView()->getImage(), QRect(imageIndex*dims.width(), dims.height(), dims.width(), dims.height()));
    // Draw monster
    painter.drawPixmap(*monsterArea, mMonster->getFightView()->getImage(), QRect(imageIndex*dims.width(), 0, dims.width(), dims.height()));

    // Draw stamina bar above the monster
    {
        Gauge stamina = mMonster->getStamina();
        if(stamina.maximum > 0)
        {
            const int barH = 10;
            const int barW = source.width();
            const int barX = source.x();
            const int barY = qMax(30, source.y() - 18);

            // Background
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(40, 40, 40, 200));
            painter.drawRoundedRect(barX, barY, barW, barH, 4, 4);

            // Fill (yellow → orange gradient)
            const int r = 4;
            int fillW = static_cast<int>(barW * static_cast<qreal>(stamina.current) / stamina.maximum);

            int prevFillW = fillW;
            if(mAttacked && mPrevStamina > stamina.current)
                prevFillW = static_cast<int>(barW * static_cast<qreal>(mPrevStamina) / stamina.maximum);
            const int drainW = prevFillW - fillW;

            painter.setPen(Qt::NoPen);

            // White drain bar drawn first (behind yellow), fully rounded
            if(drainW > 0)
            {
                painter.setBrush(QColor(255, 255, 255, 200));
                painter.drawRoundedRect(barX, barY, prevFillW, barH, r, r);
            }

            // Yellow fill on top: flat right edge when drain follows, fully rounded otherwise
            if(fillW > 0)
            {
                QLinearGradient grad(barX, barY, barX + barW, barY);
                grad.setColorAt(0.0, QColor(255, 220, 0));
                grad.setColorAt(1.0, QColor(255, 140, 0));
                if(drainW > 0)
                {
                    QPainterPath p;
                    p.moveTo(barX + r, barY);
                    p.arcTo(barX, barY, 2*r, 2*r, 90, 90);
                    p.lineTo(barX, barY + barH - r);
                    p.arcTo(barX, barY + barH - 2*r, 2*r, 2*r, 180, 90);
                    p.lineTo(barX + fillW, barY + barH);
                    p.lineTo(barX + fillW, barY);
                    p.closeSubpath();
                    painter.fillPath(p, grad);
                }
                else
                {
                    painter.setBrush(grad);
                    painter.drawRoundedRect(barX, barY, fillW, barH, r, r);
                }
            }
        }
    }

    // Draw red mask if monster takes damages
    if(mAttacked)
    {
        QPixmap overlay(monsterArea->size());
        overlay.fill(Qt::transparent);
        QPainter overlayPainter(&overlay);                                                                                              

        overlayPainter.drawPixmap(overlay.rect(), mMonster->getFightView()->getImage(), QRect(imageIndex * dims.width(), 0, dims.width(), dims.height()));

        overlayPainter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        overlayPainter.fillRect(overlay.rect(), QColor(255, 0, 0, 80));
        overlayPainter.end();

        painter.drawPixmap(*monsterArea, overlay);
    }
}
