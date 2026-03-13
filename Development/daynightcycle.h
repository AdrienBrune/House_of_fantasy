#ifndef DAYNIGHTCYCLE_H
#define DAYNIGHTCYCLE_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QtMath>
#include <QJsonObject>
#include <QJsonValue>

#define TIME_MIDNGIHT   0.00
#define TIME_DAWN       0.25
#define TIME_NOON       0.50
#define TIME_DUSK       0.75

#define DAYNIGHT_TIMER_MS           500
#define DAYNIGHT_STEPS_PER_CYCLE    2400
#define DAYNIGHT_TIME_STEP          (1.0 / DAYNIGHT_STEPS_PER_CYCLE)
#define DAYNIGHT_CYCLE_DURATION_MS  (DAYNIGHT_TIMER_MS * DAYNIGHT_STEPS_PER_CYCLE)

// Overlay : QGraphicsRectItem invisible to collisions
class DayNightOverlay : public QGraphicsRectItem
{
public:
    using QGraphicsRectItem::QGraphicsRectItem;
    QPainterPath shape() const override { return QPainterPath(); }
};

class DayNightCycle : public QObject
{
    Q_OBJECT

public:
    enum eDayTime {
        night,
        dawn,
        noon,
        dusk
    };

    explicit DayNightCycle(QGraphicsScene* scene, QObject* parent = nullptr);
    ~DayNightCycle();

    inline qreal timeOfDay() const { return mTime; }
    inline bool isNight() const { return getDayTime() == night ? true : false; }
    inline eDayTime getDayTime() const
    {
        const qreal TIME_DAWN_DURATION = 0.1;
        const qreal TIME_DUSK_DURATION = 0.1;
        const qreal TIME_NOON_DURATION = 0.4;
        const qreal TIME_NIGHT_DURATION = 0.4;
        assert((int)(TIME_DAWN_DURATION + TIME_DUSK_DURATION + TIME_NOON_DURATION + TIME_NIGHT_DURATION) == 1);

        if (mTime < TIME_DAWN - TIME_DAWN_DURATION/2.0 || mTime > TIME_DUSK + TIME_DUSK_DURATION/2.0)
        {
            return night;
        }
        else if (mTime < TIME_NOON - TIME_NOON_DURATION/2.0)
        {
            return dawn;
        }
        else if (mTime < TIME_DUSK - TIME_DUSK_DURATION/2.0)
        {
            return noon;
        }
        else
        {
            return dusk;
        }

    }
    void resetTime(qreal time = 0.2); // start at dawn

signals:
    void sig_nightBegins();
    void sig_dayBegins();
    void sig_timeChanged(qreal time);

public slots:
    void freeze(bool enable);

private slots:
    void onTick();

public:
    inline void toJson(QJsonObject &json) const
    {
        json["time_normalized"] = mTime;
    }

    inline void fromJson(const QJsonObject &json)
    {
        if (json.contains("time_normalized") && json["time_normalized"].isDouble())
        {
            mTime = json["time_normalized"].toDouble();
        }
    }

private:
    class DayNightOverlay* mOverlay;
    QTimer* mTimer;
    qreal mTime;
    bool mWasNight;

    void updateOverlay();
};

#endif // DAYNIGHTCYCLE_H
