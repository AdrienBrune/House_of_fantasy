#include "w_herostats.h"
#include <QVector>

W_HeroStats::W_HeroStats(QWidget *parent, Hero *hero):
    QWidget(parent),
    mHero(hero)
{
    setAttribute(Qt::WA_TransparentForMouseEvents); // Check if enought

    connect(mHero, SIGNAL(sig_lifeChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_manaChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_coinChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_experienceChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_equipmentChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_bagEvent()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_nameChanged()), this, SLOT(onUpdateStats()), Qt::ConnectionType::QueuedConnection);

}

W_HeroStats::~W_HeroStats()
{

}

void W_HeroStats::onUpdateStats()
{
    update();
}

void W_HeroStats::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(!mHero)
        return;

    QColor colorOrange(255,146,5);
    QColor colorWhite(255,255,255);
    QColor colorGreen(89,187,103);
    QColor colorBlue(143,206,242);
    QColor colorGrey(40,40,40);

    /* Portrait (background + picture) */
    QRect portraitPicture(0.12*width(), 0.197*height(), 0.162*width(), 0.27*height());
    /* Experience (background) */
    QRect experienceBackground(0.11*width(), 0.18*height(), 0.18*width(), 0.3*height());
    /* Name (label position) */
    QPointF posName(experienceBackground.x()+experienceBackground.width(),0.284*height());
    /* Coins (picture) */
    QRect coinsPicture(0.104*width(), 0.547*height(), 0.2*width(), 0.334*height());
    /* Damage (background + picture + label position) */
    quint16 logoSize = 0.06*width();
    QRect damageBackground(0.28*width(), 0.413*height(), 0.16*width(), 0.11*height());
    QRect damagePicture(damageBackground.x(), damageBackground.y(), logoSize, logoSize);
    /* touchness (background + picture + label position) */
    QRect touchnessBackground(0.45*width(), 0.413*height(), 0.16*width(), 0.11*height());
    QRect touchnessPicture(touchnessBackground.x(), touchnessBackground.y(), logoSize, logoSize);
    /* payload (background + picture + label position) */
    QRect payloadBackground(0.618*width(), 0.413*height(), 0.16*width(), 0.11*height());
    QRect payloadPicture(payloadBackground.x(), payloadBackground.y(), logoSize, logoSize);
    /* Gauge (background + chunk) */
    qreal manaPourcentage = static_cast<qreal>(mHero->getMana().current)/static_cast<qreal>(mHero->getMana().maximum);
    QRect manaBackground(0.274*width(), 0.3567*height(), 0.552*width(), 0.0334*height());
    QRect manaChunk(manaBackground.x(), manaBackground.y(), manaPourcentage*(manaBackground.width()-1), manaBackground.height()-1);
    /* Gauge (background + chunk) */
    QRect lifeBackgroundRect(0.278*width(), 0.3*height(), 0.626*width(), 0.0567*height());
    const int pBackground[] = {
        lifeBackgroundRect.x(), lifeBackgroundRect.y(),
        lifeBackgroundRect.x()+static_cast<int>(0.9713*lifeBackgroundRect.width()), lifeBackgroundRect.y(),
        lifeBackgroundRect.x()+lifeBackgroundRect.width(), lifeBackgroundRect.y()+lifeBackgroundRect.height(),
        lifeBackgroundRect.x(), lifeBackgroundRect.y()+lifeBackgroundRect.height(),
    };
    qreal lifePourcentage = static_cast<qreal>(mHero->getLife().current)/static_cast<qreal>(mHero->getLife().maximum);
    const int pLife[] = {
        lifeBackgroundRect.x(), lifeBackgroundRect.y(),
        lifeBackgroundRect.x()+static_cast<int>(lifePourcentage*(0.9713*lifeBackgroundRect.width()-1)), lifeBackgroundRect.y(),
        lifeBackgroundRect.x()+static_cast<int>(lifePourcentage*(lifeBackgroundRect.width()-1)), lifeBackgroundRect.y()+lifeBackgroundRect.height(),
        lifeBackgroundRect.x(), lifeBackgroundRect.y()+lifeBackgroundRect.height(),
    };
    QPolygon lifeBackground, lifeChunk;
    lifeBackground.setPoints(4, pBackground);
    lifeChunk.setPoints(4, pLife);

    /* Drawing --------------------------------------------------------------------------- */

    /* Coins */
    painter.drawPixmap(QRect(coinsPicture.x(), coinsPicture.y(), coinsPicture.width(), coinsPicture.height()), QPixmap(":/graphicItems/logo_coins.png"));
    painter.setOpacity(1);
    painter.setPen(QPen(Qt::black));
    painter.setFont(QFont("Britannic Bold",16));
    painter.drawText(QRect(coinsPicture.x()+2, coinsPicture.y()+2, coinsPicture.width(), coinsPicture.height()), Qt::AlignCenter, QString("%1").arg(mHero->getCoin()));
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Britannic Bold",16));
    painter.drawText(coinsPicture, Qt::AlignCenter, QString("%1").arg(mHero->getCoin()));
    /* Stats */
    painter.setPen(QPen(QBrush(), 0));
    painter.setOpacity(0.6);
    painter.setBrush(QBrush(colorWhite));
    painter.drawRect(damageBackground);
    painter.drawRect(touchnessBackground);
    painter.drawRect(payloadBackground);
    painter.setPen(QPen(Qt::black));
    painter.setOpacity(0.8);
    painter.setFont(QFont("Britannic Bold",10));
    painter.drawText(QRect(damageBackground.x()+damagePicture.width(), damageBackground.y(), damageBackground.width()-damagePicture.width(), damageBackground.height()), Qt::AlignCenter, QString("%1").arg(mHero->getGear()->damageStat()));
    painter.drawText(QRect(touchnessBackground.x()+touchnessPicture.width(), touchnessBackground.y(), touchnessBackground.width()-touchnessPicture.width(), touchnessBackground.height()), Qt::AlignCenter, QString("%1").arg(mHero->getGear()->defenseStat()));
    painter.drawText(QRect(payloadBackground.x()+payloadPicture.width(), payloadBackground.y(), payloadBackground.width()-payloadPicture.width(), payloadBackground.height()), Qt::AlignCenter, QString("%1").arg(mHero->getBag()->getPayload().current));
    painter.drawPixmap(damagePicture, QPixmap(":/icons/sword_logo_2.png"));
    painter.drawPixmap(touchnessPicture, QPixmap(":/icons/shield_logo.png"));
    painter.drawPixmap(payloadPicture, QPixmap(":/icons/payload_logo.png"));
    /* Name */
    painter.setOpacity(1);
    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Sitka Small",16));
    painter.drawText(posName, mHero->getName());
    /* Gauge */
    painter.setPen(QPen(QBrush(), 0));
    painter.setOpacity(0.5);
    painter.setBrush(QBrush(colorWhite));
    painter.drawPolygon(lifeBackground);
    painter.setOpacity(1);
    painter.setBrush(QBrush(colorGreen));
    painter.drawPolygon(lifeChunk);
    /* Gauge */
    painter.setPen(QPen(QBrush(), 0));
    painter.setOpacity(0.2);
    painter.setBrush(QBrush(colorWhite));
    painter.drawRect(manaBackground);
    painter.setOpacity(1);
    painter.setBrush(QBrush(colorBlue));
    painter.drawRect(manaChunk);
    /* Experience */
    painter.setPen(QPen(QBrush(), 0));
    painter.setOpacity(0.4);
    painter.setBrush(QBrush(colorWhite));
    painter.drawChord(experienceBackground, 60*16, 240*16);
    painter.setOpacity(1);
    painter.setBrush(QBrush(colorOrange));
    qreal experiencePourcentage = static_cast<qreal>(mHero->getExperience().points)/static_cast<qreal>(mHero->getExperience().pointsToLevelUp);
    painter.drawChord(experienceBackground, 305*16, -experiencePourcentage*(240*16));
    /* Portrait */
    painter.setPen(QPen(QBrush(), 0));
    painter.setBrush(colorGrey);
    painter.drawEllipse(portraitPicture);
    painter.drawPixmap(QRect(portraitPicture.x(),portraitPicture.y(),portraitPicture.width(),portraitPicture.height()),QPixmap(":/graphicItems/heros_logos.png"), QRect(mHero->getHeroClass()*90,0,90,90));
}
