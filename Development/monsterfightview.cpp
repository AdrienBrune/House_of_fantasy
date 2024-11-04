#include "monsterfightview.h"
#include <QtGlobal>

const QString str_biomes[] = {
    "forest",
    "plain"
};

MonsterFightView::MonsterFightView(QWidget * parent):
    QWidget(parent),
    mMonster(nullptr),
    mBiome(eBiomes::forest),
    mAttack(false),
    mAttacked(false),
    mAffliction(0)
{
    mTimerRestore.setSingleShot(true);
    connect(&mTimerRestore, &QTimer::timeout, this, &MonsterFightView::onRestore);
}

void MonsterFightView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(rect(), QString(":/graphicItems/fight_map_%1.png").arg(str_biomes[mBiome]));

    if(!mMonster)
        return;

    // Select target emplacement
    QRect source = mMonster->getFightView()->getDimensions();
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

    // Draw shadow
    painter.drawPixmap(*monsterArea, mMonster->getFightView()->getImage(), QRect(imageIndex*source.width(), source.height(), source.width(), source.height()));
    // Draw monster
    painter.drawPixmap(*monsterArea, mMonster->getFightView()->getImage(), QRect(imageIndex*source.width(), 0, source.width(), source.height()));

    // Draw red mask if monster takes damages
    if(mAttacked)
    {
        QPixmap monsterMask = mMonster->getFightView()->getImage().copy(QRect(0, 0, source.width(), source.height()));
        QImage image = monsterMask.scaled(monsterArea->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage();
        QPainterPath path;
        for(int y = 0; y < image.height(); ++y)
        {
            for(int x = 0; x < image.width(); ++x)
            {
                if(qAlpha(image.pixel(x, y)) > 0)
                    path.addRect(x, y, 1, 1);
            }
        }

        painter.setBrush(QColor(255, 0, 0, 30));
        painter.setPen(Qt::NoPen);
        painter.translate(monsterArea->topLeft());
        painter.drawPath(path);
    }
}
