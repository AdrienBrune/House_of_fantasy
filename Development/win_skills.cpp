#include "win_skills.h"
#include "ui_win_skills.h"

#include "win_skillinfo.h"
#include "entitieshandler.h"

Win_Skills::Win_Skills(QWidget * parent) :
    QWidget(parent),
    mPopUpSkill(nullptr),
    ui(new Ui::Win_Skills)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    ui->setupUi(this);

    ui->scrollAreaWidgetContents->setAutoFillBackground(false);
    ui->scrollArea->setAutoFillBackground(false);
    ui->scrollArea->setStyleSheet("QScrollArea{background-color:transparent;/*border-top:1px solid white;*/}");

#define MARGIN  15
#define SPACE   10
#define COLUMN  4

    int currentIndex = 0;
    for(int i = 0; i < PassiveSkill::NbSkills; i++)
    {
        if(IS_ABLE(hero->getHeroClass(), hero->getSkillList()[i]->getLearnable()))
        {
            W_Skill * widget = new W_Skill(this, hero->getPassiveSkill(i));
            int size = (width())/4 - (2 * MARGIN) - ((COLUMN-1) * SPACE);
            widget->setMinimumSize(QSize(size, size*1.2));
            widget->setMaximumSize(QSize(size, size*1.2));
            ui->gridSkills->addWidget(widget, static_cast<int>(currentIndex/4), currentIndex%4);
            connect(widget, SIGNAL(sig_clicked(W_Skill*)), this, SLOT(onResumeSkillAsked(W_Skill*)));
            connect(widget->getSkillAttached(), SIGNAL(sig_skillUnlock(Skill*)), this, SLOT(skillUnlockAnimation(Skill*)));
            currentIndex++;
        }
    }
    currentIndex = 0;
    for(int i = 0; i < SpellSkill::NbSpells; i++)
    {
        if(IS_ABLE(hero->getHeroClass(), hero->getSpellList()[i]->getLearnable()))
        {
            W_Skill * widget = new W_Skill(this, hero->getSpellSkill(i));
            int size = (width())/4 - (2 * MARGIN) - ((COLUMN-1) * SPACE);
            widget->setMinimumSize(QSize(size, size*1.2));
            widget->setMaximumSize(QSize(size, size*1.2));
            ui->gridSpells->addWidget(widget, static_cast<int>(currentIndex/4), currentIndex%4);
            connect(widget, SIGNAL(sig_clicked(W_Skill*)), this, SLOT(onResumeSkillAsked(W_Skill*)));
            connect(widget->getSkillAttached(), SIGNAL(sig_skillUnlock(Skill*)), this, SLOT(skillUnlockAnimation(Skill*)));
            currentIndex++;
        }
    }

    ui->skillPoints->setText(QString("%1%2").arg("Points de compétence : ").arg(hero->getSkillPoints()));
    connect(hero, SIGNAL(sig_lifeChanged()), this, SLOT(heroStatChanged()));
    connect(hero, SIGNAL(sig_manaChanged()), this, SLOT(heroStatChanged()));
    connect(hero, SIGNAL(sig_staminaMaxChanged()), this, SLOT(heroStatChanged()));
    connect(hero->getBag(), SIGNAL(sig_bagEvent()), this, SLOT(heroStatChanged()));
    connect(hero, &Hero::sig_skillPointsChanged, this, [=](){
        ui->skillPoints->setText(QString("%1%2").arg("Points de compétence : ").arg(hero->getSkillPoints()));
        if(hero->getSkillPoints() < 1)
        {
            ui->buttonUpLife->setEnabled(false);
            ui->buttonUpMana->setEnabled(false);
            ui->buttonUpStamina->setEnabled(false);
            ui->buttonUpStrength->setEnabled(false);
        }
        else
        {
            ui->buttonUpLife->setEnabled(true);
            ui->buttonUpMana->setEnabled(true);
            ui->buttonUpStamina->setEnabled(true);
            ui->buttonUpStrength->setEnabled(true);
        }
    });

    heroStatChanged();
    if(hero->getSkillPoints() < 1)
    {
        ui->buttonUpLife->setEnabled(false);
        ui->buttonUpMana->setEnabled(false);
        ui->buttonUpStamina->setEnabled(false);
        ui->buttonUpStrength->setEnabled(false);
    }

    if(hero->getHeroClass() == Hero::eSwordman)
    {
        ui->label->setText("");ui->line_3->hide();
    }
}

Win_Skills::~Win_Skills()
{
    delete ui;
    delete mPopUpSkill;
}

void Win_Skills::heroStatChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    ui->statLife->setText(QString("%1").arg(hero->getLife().maximum));
    ui->statMana->setText(QString("%1").arg(hero->getMana().maximum));
    ui->statStamina->setText(QString("%1").arg(hero->getStamina().maximum));
    ui->statStrength->setText(QString("%1").arg(hero->getBag()->getPayload().max));

    if(hero->getSkillPoints() < 1)
    {
        ui->buttonUpLife->setEnabled(false);
        ui->buttonUpMana->setEnabled(false);
        ui->buttonUpStamina->setEnabled(false);
        ui->buttonUpStrength->setEnabled(false);
    }
    else
    {
        ui->buttonUpLife->setEnabled(true);
        ui->buttonUpMana->setEnabled(true);
        ui->buttonUpStamina->setEnabled(true);
        ui->buttonUpStrength->setEnabled(true);
    }
}

void Win_Skills::onResumeSkillAsked(W_Skill * skill)
{
    if(mPopUpSkill)
    {
        delete mPopUpSkill;
        mPopUpSkill = nullptr;
    }

    mPopUpSkill = new Win_SkillInfo(this->parentWidget(), skill->getSkillAttached());
    mPopUpSkill->setGeometry(this->parentWidget()->x() + (this->parentWidget()->width() - mPopUpSkill->width())/2,
                             this->parentWidget()->y() + this->parentWidget()->height() - mPopUpSkill->height() - 50,
                             mPopUpSkill->width(),
                             mPopUpSkill->height());
    connect(mPopUpSkill, SIGNAL(sig_close()), this, SLOT(onSkillInfoClosed()));
    mPopUpSkill->show();
}

void Win_Skills::onSkillInfoClosed()
{
    update();
    if(mPopUpSkill)
    {
        delete mPopUpSkill;
        mPopUpSkill = nullptr;
    }
}

void Win_Skills::skillUnlockAnimation(Skill * skill)
{
    update();
}

void Win_Skills::diplayWindow()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
    b->setDuration(500);
    b->setStartValue(QRect(this->x()+this->width(),this->y(),this->width(),this->height()));
    b->setEndValue(QRect(this->x(),this->y(),this->width(),this->width()));
    b->start(QPropertyAnimation::DeleteWhenStopped);

    show();
}

void Win_Skills::closeWindow()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
    b->setDuration(500);
    b->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    b->setEndValue(QRect(this->x()+this->width(),this->y(),this->width(),this->width()));
    b->start(QPropertyAnimation::DeleteWhenStopped);

    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_closeWindow()));
    timer->start(1000);
}

void Win_Skills::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setOpacity(0.9);

    painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/graphicItems/background_window_2.png"));
    //painter.setPen(QPen(Qt::white, 1));
    //painter.drawLine(QLine(ui->buttonExit->width()/2, 0, ui->buttonExit->width()/2, height()));

    /* Icon top left */
    painter.setOpacity(1);
    painter.setBrush(QBrush("#FFFFFF"));
    painter.drawRect(QRect(ui->skillPoints->x(), ui->skillPoints->y(), ui->skillPoints->rect().width(), ui->skillPoints->rect().height()));
    //painter.drawPixmap(QRect(ui->skillPoints->x() - ui->skillPoints->height()/2, ui->skillPoints->y(), ui->skillPoints->height(), ui->skillPoints->height()), QPixmap(":/graphicItems/icon_skill.png"));
}

void Win_Skills::on_buttonUpLife_clicked()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    hero->learnPassiveSkill(Hero::life);
}

void Win_Skills::on_buttonUpMana_clicked()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    hero->learnPassiveSkill(Hero::mana);
}

void Win_Skills::on_buttonUpStamina_clicked()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    hero->learnPassiveSkill(Hero::stamina);
}

void Win_Skills::on_buttonUpStrength_clicked()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    hero->learnPassiveSkill(Hero::Strength);
}
