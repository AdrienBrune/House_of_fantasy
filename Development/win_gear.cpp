#include "win_gear.h"
#include "ui_win_gear.h"

#include "entitieshandler.h"

Win_Gear::Win_Gear(QWidget *parent) :
    QWidget(parent),
    mItemShownRect(QRect(0,0,0,0)),
    mItemSelected(0),
    ui(new Ui::Win_gear)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    ui->setupUi(this);
//    ui->scrollAreaWidgetContents->setAutoFillBackground(false);
//    ui->scrollArea->setAutoFillBackground(false);
//    ui->scrollArea->setStyleSheet("background-color:transparent;border:0px solid white;");
    ui->item_info->setWordWrap(true);
    mItemEquipment = new Frag_Interface_Gear(this);
    connect(mItemEquipment, SIGNAL(sig_equipItem(Item*)), this, SLOT(equipItem(Item*)));
    connect(mItemEquipment, SIGNAL(sig_unequipItem(Item*)), this, SLOT(unequipItem(Item*)));
    connect(mItemEquipment, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(showItem(ItemQuickDisplayer*)));
    connect(mItemEquipment, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SLOT(showItemHover(ItemQuickDisplayer*)));
    connect(mItemEquipment, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SLOT(hideItemHover(ItemQuickDisplayer*)));
    ui->layout_inventoryEquipment->addWidget(mItemEquipment, 0, Qt::AlignCenter);
    mItemToDisplay = nullptr;

    mLife = new Frag_Stats_Displayer(this, "Points de vie", QPixmap(":/icons/lifeStat.png"), hero->getLife().current, hero->getLife().maximum);
    mMana = new Frag_Stats_Displayer(this, "Gauge", QPixmap(":/icons/manaStat.png"), hero->getMana().current, hero->getMana().maximum);
    mExperience =  new Frag_Stats_Displayer(this, "Expérience", QPixmap(":/icons/exp_logo.png"), hero->getExperience().points, hero->getExperience().pointsToLevelUp);
    mDamage = new Frag_Stats_Displayer(this, "Dégats du héro", QPixmap(":/icons/sword_logo.png"), hero->getGear()->damageStat(), 0, 1);
    mHitSpeed = new Frag_Stats_Displayer(this, "Vitesse d'attaque", QPixmap(":/icons/speed_logo.png"), hero->getAttackSpeed(), 0, 1);
    mDefense = new Frag_Stats_Displayer(this, "Armure du héro", QPixmap(":/icons/shield_logo.png"), hero->getGear()->defenseStat(), 0, 1);
    mDodge = new Frag_Stats_Displayer(this, "Taux d'esquive", QPixmap(":/icons/dodge_logo.png"), static_cast<int>(hero->getGear()->dodgingStat()), 0, 1);
    mBagPayload = new Frag_Stats_Displayer(this, "Capacité du sac", QPixmap(":/images/bag.png"), hero->getBag()->getPayload().current, hero->getBag()->getPayload().max);
    mStamina = new Frag_Stats_Displayer(this, "Endurance", QPixmap(":/icons/staminaStat.png"), hero->getStamina().maximum, 0, 1);

    ui->layoutStats->addWidget(mLife, 0, 0);
    ui->layoutStats->addWidget(mMana, 1, 0);
    ui->layoutStats->addWidget(mExperience, 2, 0);
    ui->layoutStats->addWidget(mStamina, 3, 0);

    ui->layoutStats->addWidget(mDamage, 0, 1);
    ui->layoutStats->addWidget(mDefense, 1, 1);
    ui->layoutStats->addWidget(mDodge, 2, 1);
    ui->layoutStats->addWidget(mHitSpeed, 3, 1);
    ui->layoutStats->addWidget(mBagPayload, 4, 1);

    connect(hero, SIGNAL(sig_lifeChanged()), this, SLOT(displayLifeChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero, SIGNAL(sig_manaChanged()), this, SLOT(displayManaChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero->getGear(), SIGNAL(sig_equipmentSet()), this, SLOT(displayDamageChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero->getGear(), SIGNAL(sig_equipmentSet()), this, SLOT(displayHitSpeedChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero->getGear(), SIGNAL(sig_equipmentSet()), this, SLOT(displayDefenseChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero->getGear(), SIGNAL(sig_equipmentSet()), this, SLOT(displayDodgeChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero->getBag(), SIGNAL(sig_bagEvent()), this, SLOT(displayBagPayloadChanged()), Qt::ConnectionType::QueuedConnection);
    connect(hero, SIGNAL(sig_staminaMaxChanged()), this, SLOT(displayStaminaChanged()), Qt::ConnectionType::QueuedConnection);

}

void Win_Gear::equipItem(Item * item)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    hero->getGear()->setEquipment(static_cast<Equipment*>(hero->getBag()->takeItem(item)));
    emit sig_playSound(SOUND_EQUIP);
}

void Win_Gear::unequipItem(Item * item)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    hero->getGear()->removeEquipment(static_cast<Equipment*>(item));
    if(!hero->getBag()->addItem(item))
    {
        emit sig_itemThrown(item);
    }
    emit sig_playSound(SOUND_EQUIP);
}

void Win_Gear::showItem(ItemQuickDisplayer * item)
{
    ItemQuickDisplayer * lastItemSelected = mItemEquipment->getSelectedItem();

    mItemEquipment->unselectItems();

    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }

    mItemToDisplay = new W_ItemDisplayer(this, item->getItem());
    mItemToDisplay->setGeometry(mItemShownRect);
    mItemToDisplay->show();

    ui->item_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->item_info->setText(item->getItem()->getInformation());

    if(lastItemSelected != nullptr && lastItemSelected == item)
    {
        mItemSelected = 0;
        item->setItemSelected(false);

    }else{
        mItemSelected = 1;
        item->setItemSelected(true);
    }
}

void Win_Gear::showItemHover(ItemQuickDisplayer * item)
{
    if(mItemSelected != 0)
        return;

    if(mItemToDisplay != nullptr)
    {
         delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }

    mItemToDisplay = new W_ItemDisplayer(this, item->getItem());
    mItemToDisplay->setGeometry(mItemShownRect);

    mItemToDisplay->show();

    ui->item_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->item_info->setText(item->getItem()->getInformation());
}

void Win_Gear::hideItemHover(ItemQuickDisplayer *)
{
    if(mItemSelected != 0)
        return;

    if(mItemToDisplay != nullptr)
         delete mItemToDisplay;
    mItemToDisplay = nullptr;
    ui->item_info->setStyleSheet("QLabel{}");
    ui->item_info->setText("");
}

void Win_Gear::displayLifeChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mLife->changeStats(hero->getLife().current, hero->getLife().maximum);
}

void Win_Gear::displayManaChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mMana->changeStats(hero->getMana().current, hero->getMana().maximum);
}

void Win_Gear::displayDamageChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mDamage->changeStats(hero->getGear()->damageStat(), 0);
}

void Win_Gear::displayHitSpeedChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mHitSpeed->changeStats(hero->getAttackSpeed(), 0);
}

void Win_Gear::displayDefenseChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mDefense->changeStats(hero->getGear()->defenseStat(), 0);
}

void Win_Gear::displayDodgeChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mDodge->changeStats(static_cast<int>(hero->getGear()->dodgingStat()), 0);
}

void Win_Gear::displayBagPayloadChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mBagPayload->changeStats(hero->getBag()->getPayload().current, hero->getBag()->getPayload().max);
}

void Win_Gear::displayStaminaChanged()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();
    mStamina->changeStats(hero->getStamina().maximum, 0);
}

void Win_Gear::diplayWindow()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

//    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
//    b->setDuration(500);
//    b->setStartValue(QRect(this->x()+this->width(),this->y(),this->width(),this->height()));
//    b->setEndValue(QRect(this->x(),this->y(),this->width(),this->width()));
//    b->start(QPropertyAnimation::DeleteWhenStopped);

    show();

    mItemShownRect = QRect(ui->itemArea->x(), ui->itemArea->y(), ui->itemArea->rect().width(), ui->itemArea->height());
}

void Win_Gear::closeWindow()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

//    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
//    b->setDuration(500);
//    b->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
//    b->setEndValue(QRect(this->x()+this->width(),this->y(),this->width(),this->width()));
//    b->start(QPropertyAnimation::DeleteWhenStopped);

    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_closeWindow()));
    timer->start(1000);
}

void Win_Gear::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setOpacity(0.9);

    QRect iteamArea(QRect(mItemEquipment->x()-140, ui->title->y(), mItemEquipment->width()+280, mItemEquipment->height()+ui->title->height()+200));
    QRect statArea(QRect(mLife->x()-15, mLife->y()-10, (mBagPayload->x()+mBagPayload->width())-mLife->x()+20, (mBagPayload->y()+mBagPayload->height())-mLife->y()+20));
    painter.setPen(QPen(QBrush(Qt::white), 1));

    painter.drawPixmap(iteamArea, QPixmap(":/graphicItems/background_window_2.png"));
    painter.drawRect(iteamArea.x()-1, iteamArea.y()-1, iteamArea.width()+2, iteamArea.height()+2);
    painter.setOpacity(0.96);
    painter.drawPixmap(statArea, QPixmap(":/graphicItems/background_window_2.png"));
    painter.drawRect(statArea.x()-1, statArea.y()-1, statArea.width()+2, statArea.height()+2);

    painter.setOpacity(0.2);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen());
    painter.drawRect(QRect(iteamArea.x(), ui->title->y(), iteamArea.width(), ui->title->height()));

    painter.setOpacity(0.9);
    uint16_t sizeTitle = ui->title->height();
    painter.drawPixmap(QRect(ui->title->x()-sizeTitle*0.5, ui->title->y()-sizeTitle*0.5, sizeTitle*2, sizeTitle*2),QPixmap(":/graphicItems/plastron.png"));

    if(mItemToDisplay)
    {
        painter.drawPixmap(QRect(ui->item_info->x(), ui->item_info->y(), 100, 200), QPixmap(":/graphicItems/background_itemInfo_left.png"));
        painter.drawPixmap(QRect(ui->item_info->x()+100, ui->item_info->y(), ui->item_info->width()-200, 200), QPixmap(":/graphicItems/background_itemInfo_middle.png"));
        painter.drawPixmap(QRect(ui->item_info->x()+ui->item_info->width()-100, ui->item_info->y(), 100, 200), QPixmap(":/graphicItems/background_itemInfo_right.png"));
    }
}

Win_Gear::~Win_Gear()
{
    delete ui;
}


