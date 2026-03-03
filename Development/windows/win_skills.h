#ifndef WIN_SKILLS_H
#define WIN_SKILLS_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "hero.h"
#include "w_skill.h"
#include "win_skillinfo.h"

namespace Ui {
class Win_Skills;
}

class Win_Skills : public QWidget
{
    Q_OBJECT

public:
    explicit Win_Skills(QWidget *parent = nullptr);
    ~Win_Skills();

signals:
    void sig_closeWindow();

private slots:
    void heroStatChanged();
    void onResumeSkillAsked(W_Skill*);
    void onSkillInfoClosed();
    void skillUnlockAnimation(Skill*);
    void closeWindow();
    void on_buttonUpLife_clicked();
    void on_buttonUpMana_clicked();
    void on_buttonUpStamina_clicked();
    void on_buttonUpStrength_clicked();

public:
    void diplayWindow();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Win_SkillInfo * mPopUpSkill;

private:
    Ui::Win_Skills *ui;
};

#endif // WIN_SKILLS_H
