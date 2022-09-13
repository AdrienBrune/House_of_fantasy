#include "win_skills.h"
#include "ui_win_skills.h"
#include "win_skillinfo.h"

Win_Skills::Win_Skills(QWidget * parent, Hero * hero) :
    QWidget(parent),
    mHero(hero),
    mPopUpSkill(nullptr),
    ui(new Ui::Win_Skills)
{
    ui->setupUi(this);

    ui->scrollAreaWidgetContents->setAutoFillBackground(false);
    ui->scrollArea->setAutoFillBackground(false);
    ui->scrollArea->setStyleSheet("QScrollArea{background-color:transparent;/*border-top:1px solid white;*/}");

    connect(ui->buttonExit, SIGNAL(clicked()), this, SLOT(closeWindow()));

#define MARGIN  15
#define SPACE   10
#define COLUMN  4

    int currentIndex = 0;
    for(int i = 0; i < PassiveSkill::NbSkills; i++)
    {
        if(IS_ABLE(hero->getHeroClass(), hero->getSkillList()[i]->getLearnable()))
        {
            W_Skill * widget = new W_Skill(this, mHero->getPassiveSkill(i));
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
            W_Skill * widget = new W_Skill(this, mHero->getSpellSkill(i));
            int size = (width())/4 - (2 * MARGIN) - ((COLUMN-1) * SPACE);
            widget->setMinimumSize(QSize(size, size*1.2));
            widget->setMaximumSize(QSize(size, size*1.2));
            ui->gridSpells->addWidget(widget, static_cast<int>(currentIndex/4), currentIndex%4);
            connect(widget, SIGNAL(sig_clicked(W_Skill*)), this, SLOT(onResumeSkillAsked(W_Skill*)));
            connect(widget->getSkillAttached(), SIGNAL(sig_skillUnlock(Skill*)), this, SLOT(skillUnlockAnimation(Skill*)));
            currentIndex++;
        }
    }

    ui->skillPoints->setText(QString("%1%2").arg("Points de compétence : ").arg(mHero->getSkillPoints()));
    connect(mHero, SIGNAL(sig_lifeChanged()), this, SLOT(heroStatChanged()));
    connect(mHero, SIGNAL(sig_manaChanged()), this, SLOT(heroStatChanged()));
    connect(mHero, SIGNAL(sig_staminaMaxChanged()), this, SLOT(heroStatChanged()));
    connect(mHero->getBag(), SIGNAL(sig_bagEvent()), this, SLOT(heroStatChanged()));
    connect(mHero, &Hero::sig_skillPointsChanged, this, [=](){
        ui->skillPoints->setText(QString("%1%2").arg("Points de compétence : ").arg(mHero->getSkillPoints()));
        if(mHero->getSkillPoints() < 1)
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
    if(mHero->getSkillPoints() < 1)
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
    ui->statLife->setText(QString("%1").arg(mHero->getLife().maxLife));
    ui->statMana->setText(QString("%1").arg(mHero->getMana().maxMana));
    ui->statStamina->setText(QString("%1").arg(mHero->getStamina().maxStamina));
    ui->statStrength->setText(QString("%1").arg(mHero->getBag()->getPayload().max));

    if(mHero->getSkillPoints() < 1)
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

    mPopUpSkill = new Win_SkillInfo(this->parentWidget(), mHero, skill->getSkillAttached());
    mPopUpSkill->setGeometry((x()-mPopUpSkill->width())/2 + 10, this->parentWidget()->height() - mPopUpSkill->height() - 30, mPopUpSkill->width(), mPopUpSkill->height());
    mPopUpSkill->show();
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

    painter.drawPixmap(QRect(ui->buttonExit->width()/2, 0, width()-ui->buttonExit->width()/2, height()), QPixmap(":/graphicItems/background_window_2.png"));
    //painter.setPen(QPen(Qt::white, 1));
    //painter.drawLine(QLine(ui->buttonExit->width()/2, 0, ui->buttonExit->width()/2, height()));

    /* Icon top left */
    painter.setOpacity(1);
    painter.setBrush(QBrush("#FFFFFF"));
    painter.drawRect(QRect(ui->skillPoints->x(), ui->skillPoints->y(), ui->skillPoints->rect().width(), ui->skillPoints->rect().height()));
    //painter.drawPixmap(QRect(ui->skillPoints->x() - ui->skillPoints->height()/2, ui->skillPoints->y(), ui->skillPoints->height(), ui->skillPoints->height()), QPixmap(":/graphicItems/icon_skill.png"));

    /* Button drawing */
    painter.setOpacity(1);
    painter.setPen(QPen());
    QPolygon shape, shape2;
    static const int points[12] = {
        ui->buttonExit->x(), ui->buttonExit->y(),
        ui->buttonExit->x()+ui->buttonExit->width(), ui->buttonExit->y()+ui->buttonExit->height()/2,
        ui->buttonExit->x(), ui->buttonExit->y()+ui->buttonExit->height(),
        ui->buttonExit->x(), ui->buttonExit->y()+ui->buttonExit->height()*3/4,
        ui->buttonExit->x()+ui->buttonExit->width()/2, ui->buttonExit->y()+ui->buttonExit->height()/2,
        ui->buttonExit->x(), ui->buttonExit->y()+ui->buttonExit->height()/4,
    };
    shape.setPoints(6, points);
    painter.setBrush(QBrush("#FFFFFF"));
    painter.drawPolygon(shape);

    static const int points2[6] = {
        ui->buttonExit->x(), ui->buttonExit->y()+ui->buttonExit->height()/4,
        ui->buttonExit->x()+ui->buttonExit->width()/2, ui->buttonExit->y()+ui->buttonExit->height()/2,
        ui->buttonExit->x(), ui->buttonExit->y()+ui->buttonExit->height()*3/4
    };
    shape2.setPoints(3, points2);
    painter.setBrush(QBrush("#252525"));
    painter.drawPolygon(shape2);
}

void Win_Skills::on_buttonUpLife_clicked()
{
    mHero->learnPassiveSkill(Hero::Life);
}

void Win_Skills::on_buttonUpMana_clicked()
{
    mHero->learnPassiveSkill(Hero::Mana);
}

void Win_Skills::on_buttonUpStamina_clicked()
{
    mHero->learnPassiveSkill(Hero::Stamina);
}

void Win_Skills::on_buttonUpStrength_clicked()
{
    mHero->learnPassiveSkill(Hero::Strength);
}
