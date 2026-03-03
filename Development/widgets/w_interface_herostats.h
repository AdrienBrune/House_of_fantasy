#ifndef W_DIALOGHEROSTATS_H
#define W_DIALOGHEROSTATS_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "hero.h"

namespace Ui {
class W_interface_herostats;
}

class W_Interface_HeroStats : public QWidget
{
    Q_OBJECT
public:
    explicit W_Interface_HeroStats(QWidget *parent = nullptr, Hero * mHero = nullptr);
    ~W_Interface_HeroStats();
public slots:
    void refreshLifeDisplayed();
    void refreshManaDisplayed();
    void refreshExperienceDisplayed();
    void refreshCoinDisplayed();
    void refreshEquipmentDisplayed();
    void refreshNameDisplayed();
    void initInterface();
protected:
    void paintEvent(QPaintEvent *event);
    virtual bool eventFilter(QObject *o, QEvent *e);
private:
    Hero * mHero;
private:
    QPixmap mImage_halo;
    QPixmap mImage_bar;
private:
    Ui::W_interface_herostats *ui;
};

#endif // W_DIALOGHEROSTATS_H
