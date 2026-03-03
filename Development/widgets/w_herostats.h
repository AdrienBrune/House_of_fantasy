#ifndef W_HEROSTATS_H
#define W_HEROSTATS_H

#include <QWidget>
#include <QPainter>
#include "hero.h"

class W_HeroStats : public QWidget
{
    Q_OBJECT

public:
    explicit W_HeroStats(QWidget *parent = nullptr, Hero *hero = nullptr);
    ~W_HeroStats();
public slots:
    void onUpdateStats();
protected:
    void paintEvent(QPaintEvent *event);
private:
    Hero * mHero;
};

#endif // W_HEROSTATS_H
