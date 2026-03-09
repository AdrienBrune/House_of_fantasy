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
    explicit DayNightCycle(QGraphicsScene* scene, QObject* parent = nullptr);
    ~DayNightCycle();

    qreal timeOfDay() const { return mTime; }
    bool  isNight() const { return mDarkness > TIME_NOON; }
    qreal darkness() const { return mDarkness; }

    void resetTime(qreal time = TIME_DAWN);

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
    qreal mDarkness;
    bool mWasNight;

    void updateOverlay();
};

#endif // DAYNIGHTCYCLE_H
