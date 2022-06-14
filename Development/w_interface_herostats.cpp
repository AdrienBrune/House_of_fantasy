#include "w_interface_herostats.h"
#include "ui_w_interface_herostats.h"

W_Interface_HeroStats::W_Interface_HeroStats(QWidget * parent, Hero * hero) :
    QWidget(parent),
    mHero(hero),
    ui(new Ui::W_interface_herostats)
{
    ui->setupUi(this);
    ui->data_role->setScaledContents(true);

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);

    mImage_bar = QPixmap(":/graphicItems/bar.png");
    mImage_halo = QPixmap(":/graphicItems/halo.png");

    ui->data_damage->installEventFilter(this);

    connect(mHero, SIGNAL(sig_lifeChanged()), this, SLOT(refreshLifeDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_manaChanged()), this, SLOT(refreshManaDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_coinChanged()), this, SLOT(refreshCoinDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_experienceChanged()), this, SLOT(refreshExperienceDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_equipmentChanged()), this, SLOT(refreshEquipmentDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_bagEvent()), this, SLOT(refreshEquipmentDisplayed()), Qt::ConnectionType::QueuedConnection);
    connect(mHero, SIGNAL(sig_nameChanged()), this, SLOT(refreshNameDisplayed()), Qt::ConnectionType::QueuedConnection);
    initInterface();
}

void W_Interface_HeroStats::initInterface()
{
    ui->data_name->setText(mHero->getName());
    ui->data_life->setRange(0, mHero->getLife().maxLife);
    ui->data_mana->setRange(0, mHero->getMana().maxMana);
    ui->data_exp->setRange(0, mHero->getExperience().pointsToLevelUp);
    ui->data_role->setPixmap(mHero->getImage());

    refreshCoinDisplayed();
    refreshLifeDisplayed();
    refreshManaDisplayed();
    refreshEquipmentDisplayed();
    refreshExperienceDisplayed();
}

void W_Interface_HeroStats::refreshLifeDisplayed()
{
    ui->data_life->setRange(0,mHero->getLife().maxLife);
    ui->data_life->setValue(mHero->getLife().curLife);
}

void W_Interface_HeroStats::refreshManaDisplayed()
{
    ui->data_mana->setRange(0, mHero->getMana().maxMana);
    ui->data_mana->setValue(mHero->getMana().curMana);
}

void W_Interface_HeroStats::refreshExperienceDisplayed()
{
    ui->data_exp_number->setValue(mHero->getExperience().level);
    ui->data_exp->setValue(mHero->getExperience().points);
}

void W_Interface_HeroStats::refreshCoinDisplayed()
{
    ui->data_coin->setValue(mHero->getCoin());
}

void W_Interface_HeroStats::refreshEquipmentDisplayed()
{
    ui->data_damage->setValue(mHero->getGear()->damageStat());
    ui->data_armor->setValue(mHero->getGear()->defenseStat());
    ui->data_payload->setValue(mHero->getBag()->getPayload().current);
}

void W_Interface_HeroStats::refreshNameDisplayed()
{
    ui->data_name->setText(mHero->getName());
}


void W_Interface_HeroStats::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pBlack(QBrush("#212121"), 1);
    QBrush bGrey_0("#D4D4D4");
    QBrush bGrey_2("#5C5C5C");
    QBrush bBlack("#212121");

    painter.setBrush(bGrey_2);
    painter.setPen(QPen(pBlack));

    static const QPointF points[4] = {
        QPointF(ui->img_exp->x()+5, ui->img_exp->y()+ui->img_exp->height()+5),
        QPointF(ui->data_exp->x()+170, ui->img_exp->y()+ui->img_exp->height()+5),
        QPointF(ui->data_exp->x()+150, ui->img_exp->y()+ui->img_exp->height()/2),
        QPointF(ui->img_exp->x()+5, ui->img_exp->y()+ui->img_exp->height()/2)
    };
    painter.drawPolygon(points, 4);

    painter.drawEllipse(QRect(ui->data_role->x()-10, ui->data_role->y()-10, ui->data_role->width()+20, ui->data_role->height()+20));
/*
    painter.drawPixmap(QRect(ui->data_skill1->x()-10, ui->data_skill1->y()-10, ui->data_skill1->width()+20, ui->data_skill1->height()+20), mImage_halo);
    painter.drawPixmap(QRect(ui->data_skill2->x()-10, ui->data_skill2->y()-10, ui->data_skill2->width()+20, ui->data_skill2->height()+20), mImage_halo);
    painter.drawPixmap(QRect(ui->data_skill3->x()-10, ui->data_skill3->y()-10, ui->data_skill3->width()+20, ui->data_skill3->height()+20), mImage_halo);
    painter.drawPixmap(QRect(ui->data_skill1->x()-30, ui->data_skill1->y()+ui->data_skill1->height()/2-10, 30*2+ui->data_skill1->width()*3+(ui->data_skill2->x()-ui->data_skill1->x()-ui->data_skill1->width())*2, 20), mImage_bar);
*/

    static const QPointF points2[7] = {
        QPointF(ui->data_role->x()+ui->data_role->width()/2, ui->data_role->y()+ui->data_role->height()/2),
        QPointF(ui->data_life->x()+ui->data_life->width()+10, ui->data_role->y()+ui->data_role->height()/2),
        QPointF(ui->data_life->x()+ui->data_life->width()+20, ui->data_role->y()+ui->data_role->height()*3/4),
        QPointF(ui->data_life->x()+ui->data_life->width()+20, ui->data_role->y()+ui->data_role->height()*3/4),
        QPointF(ui->data_life->x()+ui->data_life->width()+20, ui->data_role->y()+ui->data_role->height()),
        QPointF(ui->data_life->x()+ui->data_life->width()+10, ui->data_damage->y()+5),
        QPointF(ui->data_role->x()+ui->data_role->width()/2, ui->data_damage->y()+5)
    };
    painter.drawPolygon(points2, 7);

    painter.setBrush(bGrey_0);
    static const QPointF points3[7] = {
        QPointF(ui->data_mana->x(), ui->data_mana->y()+ui->data_mana->height()),
        QPointF(ui->data_mana->x(), ui->data_mana->y()),
        QPointF(ui->data_mana->x()+20, ui->data_mana->y()),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+15+5, ui->labelDamage->y()-5),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+15+5, ui->labelDamage->y()-5+ui->labelDamage->height()),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+15, ui->labelDamage->y()-5+ui->labelDamage->height()+10),
        QPointF(ui->labelDamage->x()-15, ui->labelDamage->y()-5+ui->labelDamage->height()+10)
    };
    painter.drawPolygon(points3, 7);


    static const QPointF points4[6] = {
        QPointF(ui->data_role->x()+ui->data_role->width()/2, ui->data_name->y()),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+10, ui->data_name->y()),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+15, ui->data_name->y()+5),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+20, ui->data_name->y()+10),
        QPointF(ui->labelDamage->x()+(ui->data_payload->x()+ui->data_payload->width())-ui->labelDamage->x()+20, ui->data_name->y()+ui->data_name->height()),
        QPointF(ui->data_role->x()+ui->data_role->width()/2, ui->data_name->y()+ui->data_name->height())
    };
    painter.drawPolygon(points4, 6);

    painter.setPen(QPen(bGrey_0, 10));
    painter.drawArc(QRect(ui->data_role->x()-5, ui->data_role->y()-5, ui->data_role->width()+10, ui->data_role->height()+10), 20*16, 70*16);

}

bool W_Interface_HeroStats::eventFilter(QObject *o, QEvent *e)
{
    if ( ((o == ui->data_damage) || (o == ui->data_coin) || (o == ui->data_armor) || (o == ui->data_payload) || (o == ui->data_exp_number)) && e->type() == QEvent::FocusIn) {
        e->ignore();
        return true;
    }
    return QWidget::eventFilter(o, e);
}

W_Interface_HeroStats::~W_Interface_HeroStats()
{
    delete ui;
}

