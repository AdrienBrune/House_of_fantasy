#include "daynightcycle.h"
#include "constants.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DayNightCycle::DayNightCycle(QGraphicsScene* scene, QObject* parent)
    : QObject(parent),
      mTime(TIME_DAWN),
      mDarkness(0.0),
      mWasNight(false)
{
    mOverlay = new DayNightOverlay(scene->sceneRect());
    mOverlay->setPen(Qt::NoPen);
    mOverlay->setBrush(QColor(0, 0, 40, 0));
    mOverlay->setZValue(Z_DAYNIGHT_CYCLE);
    scene->addItem(mOverlay);

    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &DayNightCycle::onTick);
    mTimer->start(DAYNIGHT_TIMER_MS);

    updateOverlay();
}

DayNightCycle::~DayNightCycle()
{
    mTimer->stop();
    if(mOverlay && mOverlay->scene())
        mOverlay->scene()->removeItem(mOverlay);
    delete mOverlay;
}

void DayNightCycle::freeze(bool enable)
{
    if(enable)
        mTimer->stop();
    else
        mTimer->start(DAYNIGHT_TIMER_MS);
}

void DayNightCycle::resetTime(qreal time)
{
    mTime = time;
    mWasNight = isNight();
    updateOverlay();
}

void DayNightCycle::onTick()
{
    mTime += DAYNIGHT_TIME_STEP;
    if(mTime >= 1.0)
        mTime -= 1.0;

    updateOverlay();
    emit sig_timeChanged(mTime);

    bool nowNight = isNight();
    if(nowNight && !mWasNight)
        emit sig_nightBegins();
    else if(!nowNight && mWasNight)
        emit sig_dayBegins();
    mWasNight = nowNight;
}

void DayNightCycle::updateOverlay()
{
    // Logical darkness: 1.0 at midnight, 0.0 at noon (used for isNight())
    mDarkness = (qCos(2.0 * M_PI * mTime) + 1.0) / 2.0;

    // Color keyframes {time, RGBA overlay color}
    struct KeyFrame { qreal t; int r, g, b, a; };
    static const KeyFrame frames[] = {
        { 0.00,  10,  10,  50, 190 }, // minuit      : bleu nuit profond
        { 0.20,  20,  30,  90, 100 }, // fin de nuit : bleu sombre
        { 0.25,  250, 230, 80,  30 }, // aube        : jaune
        { 0.35,   0,   0,   0,   0 }, // matin       : transparent
        { 0.65,   0,   0,   0,   0 }, // après-midi  : transparent
        { 0.75, 240, 130,  60,  45 }, // crépuscule  : orangé
        { 0.85,  15,   5,  30, 150 }, // nuit tombante
        { 1.00,  10,  10,  50, 190 }, // minuit      : (boucle)
    };
    const int n = static_cast<int>(sizeof(frames) / sizeof(frames[0]));

    // Find surrounding keyframes
    int i = 0;
    while(i < n - 2 && frames[i + 1].t <= mTime)
        i++;

    qreal factor = (mTime - frames[i].t) / (frames[i + 1].t - frames[i].t);

    auto lerp = [](int a, int b, qreal f) { return static_cast<int>(a + f * (b - a)); };

    mOverlay->setBrush(QColor(
        lerp(frames[i].r, frames[i+1].r, factor),
        lerp(frames[i].g, frames[i+1].g, factor),
        lerp(frames[i].b, frames[i+1].b, factor),
        lerp(frames[i].a, frames[i+1].a, factor)
    ));
}
