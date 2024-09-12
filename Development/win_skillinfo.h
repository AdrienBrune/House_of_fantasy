#ifndef WIN_SKILLINFO_H
#define WIN_SKILLINFO_H

#include <QWidget>
#include <QPainter>
#include "hero.h"
#include "skill.h"

namespace Ui {
class Win_SkillInfo;
}

class Win_SkillInfo : public QWidget
{
    Q_OBJECT

public:
    explicit Win_SkillInfo(QWidget *parent = nullptr, Skill* skill = nullptr);
    ~Win_SkillInfo();

signals:
    void sig_close();

private slots:
    void on_buttonUnlock_clicked();
    void on_buttonBack_clicked();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Skill* mSkill;

private:
    Ui::Win_SkillInfo *ui;
};

#endif // WIN_SKILLINFO_H
