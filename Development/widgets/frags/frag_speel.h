#ifndef FRAG_SPEEL_H
#define FRAG_SPEEL_H

#include <QWidget>
#include <QPushButton>
#include <QPainter>

#include "skill.h"

namespace Ui {
class Frag_Speel;
}

class Frag_Speel : public QPushButton
{
    Q_OBJECT

public:
    explicit Frag_Speel(QWidget *parent = nullptr, Skill * skill = nullptr);
    ~Frag_Speel();

signals:
    void sig_spellClicked(Skill*);

protected:
    void paintEvent(QPaintEvent*);
    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);
    virtual void mouseReleaseEvent(QMouseEvent * event);

public:
    void enable(bool);

private:
    Skill* mSpell;
    bool mHover;

private:
    Ui::Frag_Speel *ui;
};

#endif // FRAG_SPEEL_H
