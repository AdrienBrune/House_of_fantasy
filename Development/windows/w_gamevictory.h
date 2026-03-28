#ifndef W_GAMEVICTORY_H
#define W_GAMEVICTORY_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class W_GameVictory : public QWidget
{
    Q_OBJECT

public:
    explicit W_GameVictory(QWidget* parent = nullptr);
    ~W_GameVictory() {}

signals:
    void sig_quit();

protected:
    void paintEvent(QPaintEvent*) override;

private:
    qreal  mOverlayOpacity;
    QTimer mFadeTimer;
    QTimer mCountdownTimer;
    int    mRemainingMs;

    static const int TOTAL_MS = 1.5 * 60 * 1000; // 3 minutes
};

#endif // W_GAMEVICTORY_H
