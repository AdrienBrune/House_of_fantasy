#ifndef W_SKILL_H
#define W_SKILL_H

#include <QPushButton>
#include <QPainter>
#include <QEvent>
#include <QHoverEvent>

#include "skill.h"

namespace Ui {
class W_Skill;
}

class W_Skill : public QPushButton
{
    Q_OBJECT

public:
    explicit W_Skill(QWidget * parent = nullptr, Skill * skill = nullptr);
    ~W_Skill();

signals:
    void sig_clicked(W_Skill*);

public:
    Skill * getSkillAttached();

protected:
    void paintEvent(QPaintEvent*);
    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);
    virtual void mouseReleaseEvent(QMouseEvent * event);

private:
    Skill* mSkillToShow;
    bool mHover;

private:
    Ui::W_Skill *ui;
};

#endif // W_SKILL_H
