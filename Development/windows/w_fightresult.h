#ifndef W_FIGHTRESULT_H
#define W_FIGHTRESULT_H

#include <QWidget>
#include <QPainter>
#include <QTimer>


class W_FightResult : public QWidget
{
    Q_OBJECT

public:
    explicit W_FightResult(QWidget* parent, int level, int expBefore, int expGained, int expToLevelUp, int coinGained);
    ~W_FightResult();

signals:
    void sig_closed();

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void onAnimate();

private:
    int    mLevel;
    int    mExpBefore;
    int    mExpGained;
    int    mExpToLevelUp;
    int    mCoinGained;
    bool   mLevelUp;

    qreal  mPreDeg;
    qreal  mCurrentDeg;
    qreal  mTargetDeg;
    qreal  mStep;

    QTimer mAnimTimer;
    bool   mClosing;

    QRect  mPanel;
};

#endif // W_FIGHTRESULT_H
