#include "win_skillinfo.h"
#include "ui_win_skillinfo.h"

Win_SkillInfo::Win_SkillInfo(QWidget * parent, Hero * hero, Skill * skill) :
    QWidget(parent),
    mHero(hero),
    mSkill(skill),
    ui(new Ui::Win_SkillInfo)
{
    ui->setupUi(this);

    ui->skillImage->setScaledContents(true);
    ui->skillPointsCost->setText(QString("%1").arg(mSkill->getUnlockPoints()));
    ui->skillImage->setPixmap(mSkill->getImage(mSkill->isUnlock()));
    ui->skillName->setText(mSkill->getName());
    ui->skillResume->setText(mSkill->getResume());
    ui->skillAddInfo->hide();

    if(mSkill->isUnlock())
        ui->buttonUnlock->hide();

    SpellSkill * spell = dynamic_cast<SpellSkill*>(mSkill);
    if(!spell)
    {
        ui->spellManaCost->hide();
        ui->labelManaCost->hide();
        ui->ManaCost_logo->hide();

        if(!mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
        {
            if(mSkill->getIndex() == PassiveSkill::Archmage)
            {
                ui->skillAddInfo->show();
                ui->skillAddInfo->setText("Nécéssite l'apprentissage de la compétence\n'Apprenti mage'");
            }
        }
    }

    if(spell)
    {
        ui->spellManaCost->setText(QString("%1").arg(spell->getManaCost()));

        if(!mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
        {
            ui->skillAddInfo->show();
            ui->skillAddInfo->setText("Nécéssite l'apprentissage de la compétence\n'Apprenti mage'");
        }
        else if(!mHero->getSkillList()[PassiveSkill::Archmage]->isUnlock())
        {
            /* Superior spells */
            if(mSkill->getIndex() > SpellSkill::PrimitiveShield)
            {
                ui->skillAddInfo->show();
                ui->skillAddInfo->setText("Nécéssite l'apprentissage de la compétence\n'Archimage'");
            }
        }
    }
}

Win_SkillInfo::~Win_SkillInfo()
{
    delete ui;
}

void Win_SkillInfo::on_buttonUnlock_clicked()
{
    if(mHero->learnSkill(mSkill))
    {
        emit sig_close();
    }
    else
    {
        ui->buttonUnlock->setStyleSheet("QPushButton{ background-color:rgba(250, 250, 250, 100); color:#CC2233; border:1px solid #CC2233; border-radius:5px; }");
        QTimer::singleShot(1000, this, [&]{ui->buttonUnlock->setStyleSheet("QPushButton{ background-color:rgba(250, 250, 250, 100); color:#3c3c3c; } QPushButton::hover{ background-color:rgba(250, 250, 250, 180); color:#3c3c3c; }");});
    }
}

void Win_SkillInfo::on_buttonBack_clicked()
{
    emit sig_close();
}

void Win_SkillInfo::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#2c2c2c"));
    painter.drawRoundedRect(QRect(width()/50,width()/50,width() - width()/50,height() - width()/50), 10, 10);

//    if(!mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
//    {
//        SpellSkill * spell = dynamic_cast<SpellSkill*>(mSkill);
//        if(spell)
//        {

//        }
//    }
}
