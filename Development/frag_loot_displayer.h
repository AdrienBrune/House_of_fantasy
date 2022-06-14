#ifndef FRAG_LOOT_DISPLAYER_H
#define FRAG_LOOT_DISPLAYER_H

#include <QWidget>
#include <QPainter>
#include "equipment.h"

namespace Ui {
class Frag_loot_displayer;
}

class Frag_loot_displayer : public QWidget
{
    Q_OBJECT
public:
    explicit Frag_loot_displayer(QWidget *parent = nullptr, EquipmentToForge::Loot loot = {nullptr,0});
    ~Frag_loot_displayer();
protected:
    void paintEvent(QPaintEvent *event);
private:
    EquipmentToForge::Loot mLoot;
public:
    bool redBorders;
private:
    Ui::Frag_loot_displayer *ui;
};

#endif // FRAG_LOOT_DISPLAYER_H
