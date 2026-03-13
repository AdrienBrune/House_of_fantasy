#include "w_daynightcycle.h"
#include "daynightcycle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

W_DayNightCycle::W_DayNightCycle(QWidget *parent)
    : QWidget(parent),
      mTime(0.25)
{
    setMouseTracking(true);
}

void W_DayNightCycle::mouseMoveEvent(QMouseEvent *event)
{
    bool wasHovered = mIconHovered;
    mIconHovered = mIconRect.contains(event->pos());
    if(mIconHovered != wasHovered)
        update();
}

void W_DayNightCycle::leaveEvent(QEvent *)
{
    if(mIconHovered) { mIconHovered = false; update(); }
}

void W_DayNightCycle::OnUpdateTime(qreal time)
{
    mTime = time;
    update();
}

void W_DayNightCycle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal W  = width();
    const qreal H  = height();
    const qreal cx = W * 0.5;
    const qreal cy = H * 0.85;

    const qreal outerR = qMin(cx * 0.90, cy * 0.88);
    const qreal trackW = outerR * 0.20;
    const qreal midR   = outerR - trackW * 0.5;

    struct Key { qreal pos; qreal span; int r, g, b; };
    static const Key keys[] = {
        {0.00, 0.20,  35,  33, 50},  // night
        {0.20, 0.10, 255, 150, 40},  // dawn
        {0.30, 0.40, 255, 215, 85},  // noon
        {0.70, 0.10, 255, 150, 40},  // dusk
        {0.80, 0.20,  35,  33, 50},  // night
    };
    const int nK = static_cast<int>(sizeof(keys) / sizeof(keys[0]));

    // shadown halo
    {
        QRectF r(cx - midR, cy - midR, midR * 2, midR * 2);
        p.setPen(QPen(QColor(0, 0, 0, 90), trackW + 1.0, Qt::SolidLine, Qt::FlatCap));
        p.setBrush(Qt::NoBrush);
        p.drawArc(r, 0, 180 * 16);
        p.setPen(QPen(QColor(0, 0, 0, 45), trackW + 2.0, Qt::SolidLine, Qt::FlatCap));
        p.drawArc(r, 0, 180 * 16);
    }

    // panel colored
    {
        QRectF r(cx - midR, cy - midR, midR * 2, midR * 2);
        for(int i = 0; i < nK; i++)
        {
            qreal p0 = (qreal)i / nK;
            qreal p1 = (qreal)(i + 1) / nK;
            QColor c = QColor(keys[i].r, keys[i].g, keys[i].b);
            p.setPen(QPen(c, trackW - 1.0, Qt::SolidLine, Qt::FlatCap));

            int startAngle = (int)((1-keys[i].pos) * 180.0 * 16);
            int spanAngle  = (int)(-keys[i].span * 180.0 * 16);
            p.drawArc(r, startAngle, spanAngle);
        }
    }

    // shadow background
    {
        qreal rimR = midR + trackW * 0.5 - 1.0;
        QRectF r(cx - rimR, cy - rimR, rimR * 2, rimR * 2);
        p.setPen(QPen(QColor(10, 10, 10, 60), 1.5, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);
        p.drawArc(r, 0, 180 * 16);
    }

    // white main cursor
    {
        // mTime ∈ [0,1] : 0=gauche (angle 180°), 1=droite (angle 0°)
        qreal angle_rad = (1.0 - mTime) * M_PI;
        qreal cosA = qCos(angle_rad);
        qreal sinA = qSin(angle_rad);

        qreal rIn  = midR - trackW * 0.5 - 5.0;
        qreal rOut = midR + trackW * 0.5 + 5.0;

        QPointF ptIn( cx + cosA * rIn,  cy - sinA * rIn);
        QPointF ptOut(cx + cosA * rOut, cy - sinA * rOut);

        p.setPen(QPen(QColor(0, 0, 0, 130), 4.5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(ptIn, ptOut);
        p.setPen(QPen(QColor(255, 255, 255, 230), 2.5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(ptIn, ptOut);
    }

    // icon
    {
        qreal iconR  = trackW * 0.85;
        const qreal logo_cy = H * 0.7;
        qreal iconCx = cx;
        qreal iconCy = logo_cy;

        mIconRect = QRectF(iconCx - iconR * 1.4, iconCy - iconR * 1.4, iconR * 2.8, iconR * 2.8);

        QString tooltipMsg;

        if(mTime < 0.20 || mTime > 0.80)
        {
            // moon
            QPainterPath moon;
            moon.addEllipse(QPointF(iconCx, iconCy), iconR, iconR);
            QPainterPath cutout;
            cutout.addEllipse(QPointF(iconCx + iconR * 0.38, iconCy - iconR * 0.18),
                              iconR * 0.80, iconR * 0.80);
            p.setBrush(QColor(200, 215, 255, 220));
            p.setPen(QPen(QColor(160, 180, 255, 80), 1.0));
            p.drawPath(moon.subtracted(cutout));
            tooltipMsg = "Nuit\nles monstres seront agressifs";
        }
        else if(mTime < 0.30 || mTime > 0.70)
        {
            // rising sun
            p.setBrush(QColor(255, 155, 50, 220));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(iconCx, iconCy), iconR * 0.60, iconR * 0.60);
            p.setPen(QPen(QColor(255, 175, 70, 200), 1.8, Qt::SolidLine, Qt::RoundCap));
            for(int ray = 0; ray < 6; ray++)
            {
                qreal ra = ray * M_PI / 3.0;
                p.drawLine(QPointF(iconCx + qCos(ra) * iconR * 0.70,
                                   iconCy - qSin(ra) * iconR * 0.70),
                           QPointF(iconCx + qCos(ra) * iconR * 1.10,
                                   iconCy - qSin(ra) * iconR * 1.10));
            }
            tooltipMsg = (mTime < 0.5)
                ? "Aube\nles monstres se calment"
                : "Crépuscule\nles monstres s'agitent";
        }
        else
        {
            // sun
            p.setBrush(QColor(255, 228, 70, 235));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(iconCx, iconCy), iconR * 0.60, iconR * 0.60);
            p.setPen(QPen(QColor(255, 215, 50, 210), 2.0, Qt::SolidLine, Qt::RoundCap));
            for(int ray = 0; ray < 8; ray++)
            {
                qreal ra = ray * M_PI / 4.0;
                p.drawLine(QPointF(iconCx + qCos(ra) * iconR * 0.78,
                                   iconCy - qSin(ra) * iconR * 0.78),
                           QPointF(iconCx + qCos(ra) * iconR * 1.25,
                                   iconCy - qSin(ra) * iconR * 1.25));
            }
            tooltipMsg = "Jour\nles monstres sont calmes";
        }

        // popup text
        if(mIconHovered && !tooltipMsg.isEmpty())
        {
            QFont font = p.font();
            font.setPointSize(7);
            p.setFont(font);
            QFontMetrics fm(font);
            int textW = fm.horizontalAdvance(tooltipMsg);
            int textH = fm.height();

            const int pad  = 8;
            const int rad  = 5;
            qreal bx = iconCx - textW * 0.5 - pad;
            qreal by = iconCy - iconR * 1.6 - textH - pad * 2;
            QRectF bubble(bx, by, textW + pad * 2, textH + pad * 2);

            p.setBrush(QColor(20, 20, 40, 160));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(bubble, rad, rad);

            p.setPen(QColor(220, 225, 255, 240));
            p.drawText(bubble, Qt::AlignCenter, tooltipMsg);
        }
    }
}
