#ifndef MONSTERFIGHTVIEW_H
#define MONSTERFIGHTVIEW_H

#include <QWidget>
#include <QPainter>

#include "monster.h"

class IMonsterFightView
{
public:
    IMonsterFightView(QString monster, QRect dimensions):
        mDimensions(dimensions),
        mImage(QPixmap(QString(":/monsters/%1/%1_fight_view.png").arg(monster)))
    {}
    ~IMonsterFightView() {}

public:
    QRect getDimensions() { return mDimensions; }
    QPixmap getImage() { return mImage; }

protected:
    QRect mDimensions;
    QPixmap mImage;
};



class MonsterFightView : public QWidget
{
    Q_OBJECT
public:
    enum eBiomes
    {
        forest,
        plain
    };
public:
    MonsterFightView(QWidget * parent = nullptr);

public:
    void setMonster(Monster * monster) { mMonster = monster; }

public:
    void attackedAnimate() { restart(mTimerRestore); mAttack = false; mAttacked = true; update(); }
    void attackAnimate()   { restart(mTimerRestore); mAttack = true;  mAttacked = false; update(); }

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void onRestore() { mAttack = false; mAttacked = false; update(); }

private:
    void restart(QTimer & timer) { timer.stop(); timer.start(600); }

private:
    Monster *mMonster;
    eBiomes mBiome;
    bool mAttack;
    bool mAttacked;
    quint32 mAffliction; // Monster::Status
    QTimer mTimerRestore;
};



// All monster view descriptions

class SpiderFightView : public IMonsterFightView
{
public:
    SpiderFightView() : IMonsterFightView("spider", QRect(150, 150, 300, 200)) {}
    ~SpiderFightView() {}
};

class BearFightView : public IMonsterFightView
{
public:
    BearFightView() : IMonsterFightView("bear", QRect(100, 0, 350, 400)) {}
    ~BearFightView() {}
};

class TrollFightView : public IMonsterFightView
{
public:
    TrollFightView() : IMonsterFightView("troll", QRect(150, 100, 250, 200)) {}
    ~TrollFightView() {}
};

class GobelinFightView : public IMonsterFightView
{
public:
    GobelinFightView() : IMonsterFightView("goblin", QRect(200, 100, 200, 200)) {}
    ~GobelinFightView() {}
};

class WolfFightView : public IMonsterFightView
{
public:
    WolfFightView() : IMonsterFightView("wolf", QRect(200, 100, 220, 200)) {}
    ~WolfFightView() {}
};

class WolfAlphaFightView : public IMonsterFightView
{
public:
    WolfAlphaFightView() : IMonsterFightView("wolfAlpha", QRect(200, 100, 220, 200)) {}
    ~WolfAlphaFightView() {}
};

class OggreFightView : public IMonsterFightView
{
public:
    OggreFightView() : IMonsterFightView("oggre", QRect(50, 0, 500, 400)) {}
    ~OggreFightView() {}
};

class LaoShanLungFightView : public IMonsterFightView
{
public:
    LaoShanLungFightView() : IMonsterFightView("laoShanLung", QRect(50, 0, 600, 400)) {}
    ~LaoShanLungFightView() {}
};

#endif // MONSTERFIGHTVIEW_H
