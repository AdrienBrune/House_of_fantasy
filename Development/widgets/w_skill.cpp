#include "w_skill.h"
#include "ui_w_skill.h"

W_Skill::W_Skill(QWidget * parent, Skill * skill) :
    QPushButton(parent),
    mSkillToShow(skill),
    mHover(false),
    ui(new Ui::W_Skill)
{
    ui->setupUi(this);
    ui->unlockPoints->setText(QString("%1").arg(mSkillToShow->getUnlockPoints()));
    ui->name->setText(mSkillToShow->getName());
}

W_Skill::~W_Skill()
{
    delete ui;
}

Skill *W_Skill::getSkillAttached()
{
    return mSkillToShow;
}

void W_Skill::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect skillForm((width() - (height() - ui->name->height()))/4, 0, height() - ui->name->height(), height() - ui->name->height());
    QRect lockForm((ui->unlockPoints->width() - (ui->unlockPoints->height())) + ui->unlockPoints->height()/4, ui->unlockPoints->height()/4,ui->unlockPoints->height()/2, ui->unlockPoints->height()/2);

    if(!mHover)
    {
        skillForm.setRect(skillForm.x() + skillForm.width()/10, skillForm.y() + skillForm.height()/10, skillForm.width()*4/5, skillForm.height()*4/5);
        lockForm.setRect(lockForm.x() + lockForm.width()/10, lockForm.y() + lockForm.height()/10, lockForm.width()*4/5, lockForm.height()*4/5);
    }

    painter.drawPixmap(skillForm, mSkillToShow->getImage(mSkillToShow->isUnlock()));

    if(mSkillToShow->isUnlock())
    {
        /* Skill unlock */
        ui->unlockPoints->setText("");
    }
    else
    {
        if(mHover)
        {
            ui->unlockPoints->setText("");
        }
        else
        {
            painter.drawPixmap(lockForm, QPixmap(":/graphicItems/lock.png"));
            ui->unlockPoints->setText(QString("%1").arg(mSkillToShow->getUnlockPoints()));
        }
    }
}

void W_Skill::mouseReleaseEvent(QMouseEvent*)
{
    emit sig_clicked(this);
}

void W_Skill::leaveEvent(QEvent*)
{
    mHover = false;
}

void W_Skill::enterEvent(QEvent*)
{
    mHover = true;
}
