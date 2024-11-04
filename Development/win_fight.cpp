#include "win_fight.h"
#include "ui_win_fight.h"

#include "frag_speel.h"
#include "entitieshandler.h"
#include "monsterfightview.h"

#define ACTION_COST_HEAVY_ATTACK    ((mHero->getStamina().maximum > 400) ? 400 : mHero->getStamina().maximum)
#define ACTION_COST_LIGHT_ATTACK    100
#define ACTION_COST_USE_ITEM        80
#define ACTION_COST_FLEE            ((mHero->getStamina().maximum > 200) ? 200 : mHero->getStamina().maximum)
#define ACTION_COST_USE_SPELL       50

Win_Fight::Win_Fight(QWidget *parent, Monster * monster) :
    QWidget(parent),
    mHero(nullptr),
    mMonster(monster),
    mScenePotion(nullptr),
    pMonsterHeavyAttack(QPixmap()),
    pMonsterLightAttack(QPixmap()),
    w_fightAnimation(nullptr),
    w_spellList(nullptr),
    mDodgeSucces(false),
    mFleeFail(false),
    mConfused(false),
    ui(new Ui::Win_Fight)
{
    mHero = EntitiesHandler::getInstance().getHero();
    mHero->stopMoving();

    mHero->setupFight(true);
    mMonster->setupFight(true);

    ui->setupUi(this);
    if(parent)
        setGeometry(parent->x(), parent->y(), parent->width(), parent->height());
    hide();
    initInterface();

    t_onHeroStaminaRecovery = new QTimer(this);
    connect(t_onHeroStaminaRecovery, SIGNAL(timeout()), this, SLOT(onHeroStaminaRecovery()));
    t_onHeroStaminaRecovery->start(static_cast<int>(300.0*(static_cast<qreal>(abs(mHero->getAttackSpeed()-11.0))/5.0)));

    connect(mMonster, &Monster::sig_lightAttack, this, &Win_Fight::onMonsterLightAttack);
    connect(mMonster, &Monster::sig_heavyAttack, this, &Win_Fight::onMonsterHeavyAttack);
    connect(mMonster, &Monster::sig_monsterSound, this, [=](int sound){ emit sig_playSound(sound); });

    connect(ui->button_heavyAttack, &QPushButton::clicked, this, &Win_Fight::onButtonHeavyAttackClicked);
    connect(ui->button_lightAttack, &QPushButton::clicked, this, &Win_Fight::onButtonLightAttackClicked);
    connect(ui->button_flee, &QPushButton::clicked, this, &Win_Fight::onButtonFleeClicked);
    connect(ui->button_useSpell, &QPushButton::clicked, this, &Win_Fight::onButtonUseSpellClicked);

    connect(mHero, &Character::sig_lifeChanged, this, &Win_Fight::onCheckFightIssue);
    connect(mMonster, &Character::sig_lifeChanged, this, &Win_Fight::onCheckFightIssue);
}

Win_Fight::~Win_Fight()
{
    delete ui;
    if(w_fightAnimation)
        delete w_fightAnimation;
    if(w_spellList)
        delete w_spellList;
}

void Win_Fight::initInterface()
{
    ui->button_useSpell->setEnabled(false);

    if(mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
        ui->button_useSpell->setEnabled(true);

    ui->monsterLife->setupUi(mMonster, {25, W_AnimatedProgressBar::eLife, QBrush("#ABEBC6"), QBrush("#196F3D"), QBrush("#CCCCCC"), QBrush("#E74C3C")});
    ui->heroLife->setupUi(mHero, {25, W_AnimatedProgressBar::eLife, QBrush("#ABEBC6"), QBrush("#196F3D"), QBrush("#CCCCCC"), QBrush("#E74C3C")});
    ui->heroMana->setupUi(mHero, {12, W_AnimatedProgressBar::eMana, QBrush("#A9CCE3"), QBrush("#154360"), QBrush("#CCCCCC"), QBrush("#A6ACAF")});

    ui->heroStatus->setupUi(mHero, true);
    ui->monsterStatus->setupUi(mMonster);

    loadFightAnimationsPixmap();

    ui->data_playerStamina->setMaximum(mHero->getStamina().maximum);
    ui->data_playerStamina->setValue(mHero->getStamina().current);

    connect(mHero, &Hero::sig_lifeChanged, this, [&]{ update(); });
    connect(mHero, &Hero::sig_manaChanged, this, [&]{ update(); });
    connect(mHero, &Hero::sig_dodge, this, [&]{
        mDodgeSucces = true;
        update();
        QTimer::singleShot(800, this, [&]{ mDodgeSucces = false; update(); });
    });

    mScenePotion = new QGraphicsScene(this);
    ui->itemView->setScene(mScenePotion);

    addConsumablesOnScreen();

    ui->img_player->setPixmap(mHero->getImage());
    ui->img_player->setScaledContents(true);
    ui->img_monster->setPixmap(mMonster->getImage());
    ui->img_monster->setScaledContents(true);

    w_spellList = new W_SpellListSelection(this);
    w_spellList->setWindowFlags(Qt::FramelessWindowHint);
    connect(w_spellList, SIGNAL(sig_spellClicked(Skill*)), this, SLOT(onUseSpell(Skill*)));
    w_spellList->setGeometry(ui->button_useSpell->x() - w_spellList->width(), ui->button_useSpell->y(), w_spellList->width(), w_spellList->height());
    w_spellList->hide();

    ui->fight_view->setMonster(mMonster);
}

void Win_Fight::onHeroStaminaRecovery()
{
    mHero->setStamina(mHero->getStamina().current + 5);
    ui->data_playerStamina->setValue(mHero->getStamina().current);
    update();
}

void Win_Fight::onUseConsumable(ItemQuickDisplayer * w_item)
{
    if(!heroUseStamina(ACTION_COST_USE_ITEM))
    {
        showNotEnoughtStamina();
        return;
    }

    Consumable * item = dynamic_cast<Consumable*>(w_item->getItem());
    while(!mConsumables.isEmpty())
    {
        mScenePotion->removeItem(mConsumables.last());
        mConsumables.takeLast()->deleteLater();
    }
    mHero->useConsumable(item);
    addConsumablesOnScreen();
}

void Win_Fight::addConsumablesOnScreen()
{
    // Remove current list
    for(ItemQuickDisplayer *item : qAsConst(mConsumables))
    {
        disconnect(item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onUseConsumable(ItemQuickDisplayer*)));
        ui->itemView->scene()->removeItem(item);
        mConsumables.removeOne(item);
    }

    // Update current list
    mScenePotion->setSceneRect(0,0,ui->itemView->width(), ui->itemView->height());
    QList<Consumable*> list = mHero->getBag()->getItemList<Consumable*>();
    for(Consumable * item : qAsConst(list))
    {
        ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
        w_item->setMovable(false);
        ui->itemView->scene()->addItem(w_item);
        mConsumables.append(w_item);
        connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onUseConsumable(ItemQuickDisplayer*)));
    }
    int offset = 5;
    for(ItemQuickDisplayer * item : qAsConst(mConsumables))
    {
        item->setPos(offset, 5);
        offset += 105;
        if(mScenePotion->sceneRect().width() < offset + 105){
            mScenePotion->setSceneRect(0,0,mScenePotion->sceneRect().width()+105, mScenePotion->sceneRect().height());
        }
    }
}

void Win_Fight::enableButtons(bool toggle)
{
    ui->button_flee->setEnabled(toggle);
    ui->button_heavyAttack->setEnabled(toggle);
    ui->button_lightAttack->setEnabled(toggle);
    if(mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
        ui->button_useSpell->setEnabled(toggle);
}

void Win_Fight::showNotEnoughtStamina()
{
    ui->data_playerStamina->setStyleSheet(STAMINA_NOT_ENOUGTH);
    QTimer::singleShot(1200, this, [&](){ ui->data_playerStamina->setStyleSheet(STAMINA_NORMAL_STATUS); });
}

void Win_Fight::onButtonHeavyAttackClicked()
{
    if(!heroUseStamina(ACTION_COST_HEAVY_ATTACK))
    {
        showNotEnoughtStamina();
        return;
    }

    if(mHero->isApplied(Character::eStatus::confused))
    {
        mHero->updateStatus(Character::eStatus::confused, mHero->getStatus(Character::eStatus::confused).toInt() - 1);
        if(mHero->getStatus(Character::eStatus::confused).toInt() < 1)
            mHero->removeStatus(Character::eStatus::confused);

        if(!QRandomGenerator().global()->bounded(3)) // 33% chance to waste the attack
        {
            mConfused = true;
            update();
            QTimer::singleShot(800, this, [&]{ mConfused = false; update(); });
            return;
        } 
    }

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    float foreOfNatureCoef = 1.0;

    if(mHero->getSkillList()[PassiveSkill::ForceOfNature]->isUnlock())
    {
        // 30% chance +20% dmg
        if(QRandomGenerator::global()->bounded(100) < 30)
            foreOfNatureCoef = 1.2;
    }

    ui->fight_view->attackedAnimate();
    emit sig_playSound(SOUND_HERO_ATTACK);
    mMonster->setLife(mMonster->getLife().current-static_cast<int>(mHero->calculateDamage()*foreOfNatureCoef*(ACTION_COST_HEAVY_ATTACK/100.0)));
}

void Win_Fight::onButtonLightAttackClicked()
{
    if(!heroUseStamina(ACTION_COST_LIGHT_ATTACK))
    {
        showNotEnoughtStamina();
        return;
    }

    if(mHero->isApplied(Character::eStatus::confused))
    {
        mHero->updateStatus(Character::eStatus::confused, mHero->getStatus(Character::eStatus::confused).toInt() - 1);
        if(mHero->getStatus(Character::eStatus::confused).toInt() < 1)
            mHero->removeStatus(Character::eStatus::confused);

        if(!QRandomGenerator().global()->bounded(3)) // 33% chance to waste the attack
        {
            mConfused = true;
            update();
            QTimer::singleShot(800, this, [&]{ mConfused = false; update(); });
            return;
        }
    }

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    ui->fight_view->attackedAnimate();
    emit sig_playSound(SOUND_HERO_ATTACK);
    mMonster->setLife(mMonster->getLife().current-static_cast<int>(mHero->calculateDamage()));
}

void Win_Fight::onButtonFleeClicked()
{
    if(!heroUseStamina(ACTION_COST_FLEE))
    {
        showNotEnoughtStamina();
        return;
    }

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    if(QRandomGenerator::global()->bounded(3))
    {
        endFight();
    }
    else
    {
        mFleeFail = true;
        update();
        QTimer::singleShot(800, this, [&]{ mFleeFail = false; update(); });
    }
}

void Win_Fight::onButtonUseSpellClicked()
{
    if(!w_spellList->isHidden())
    {
        w_spellList->hide();
        return;
    }

    /* Check if Hero knows at least one spell */
    bool isSpellLearnt = false;
    for(int i=0; i < SpellSkill::NbSpells; i++)
    {
        if(mHero->getSpellList()[i]->isUnlock())
            isSpellLearnt = true;
    }
    if(!isSpellLearnt)
        return;

    w_spellList->show();
    w_spellList->setGeometry(ui->button_useSpell->x() - w_spellList->width(), ui->button_useSpell->y(), w_spellList->width(), w_spellList->height());
}

void Win_Fight::onMonsterLightAttack()
{
    ui->fight_view->attackAnimate();
    emit sig_playSound(mMonster->getSoundIndexFor(HEAVY_ATTACK));
    if(w_fightAnimation)
        delete w_fightAnimation;
    w_fightAnimation = new W_Animation_Fight(this, pMonsterLightAttack);
    connect(w_fightAnimation, SIGNAL(sig_hideAnimation()), this, SLOT(onHideAnimation()));
    w_fightAnimation->setGeometry((width()-700)/2,(height()-700)/2,700,700);

    mHero->takeDamage(mMonster->getDamage()*1.0);
    update();
}

void Win_Fight::onMonsterHeavyAttack()
{
    ui->fight_view->attackAnimate();
    emit sig_playSound(mMonster->getSoundIndexFor(HEAVY_ATTACK));
    if(w_fightAnimation)
        delete w_fightAnimation;
    w_fightAnimation = new W_Animation_Fight(this, pMonsterHeavyAttack);
    connect(w_fightAnimation, SIGNAL(sig_hideAnimation()), this, SLOT(onHideAnimation()));
    w_fightAnimation->setGeometry((width()-700)/2,(height()-700)/2,700,700);

    mHero->takeDamage(mMonster->getDamage()*1.5);
    update();
}

void Win_Fight::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(100,50,width()-200,height()-150), QPixmap(":/graphicItems/background_window_1.png"));

    painter.drawPixmap(QRect(ui->button_useSpell->x(), ui->button_heavyAttack->y(), ui->button_useSpell->width()*3, ui->button_heavyAttack->height()*3),
                       QPixmap(":/graphicItems/buttons_background.png"));

    painter.setBrush(QBrush("#434343"));
    painter.setPen(QPen(QBrush("#FFFFFF"), 3));

    painter.drawLine(ui->img_monster->x()+ui->img_monster->width()/2, ui->monsterLife->y()-10,
                     ui->monsterLife->x()+ui->monsterLife->width()+50, ui->monsterLife->y()-10);

    painter.drawEllipse(QRect(ui->img_monster->x()-15, ui->img_monster->y()-15, ui->img_monster->width()+30, ui->img_monster->height()+30));

    painter.setPen(QPen(QBrush("#ffd800"),1));

    QString buff = "";
    float stamina = static_cast<float>(ui->data_playerStamina->value());
    painter.drawText(QRect(ui->data_playerStamina->x()-10, ui->data_playerStamina->y()+ui->data_playerStamina->height(), 30, 20), Qt::AlignCenter,  buff.asprintf("%d", static_cast<int>(stamina)));
    if(stamina < mHero->getStamina().maximum){
        int heightReach = static_cast<int>(ui->data_playerStamina->y()+ui->data_playerStamina->height() - static_cast<float>(stamina/mHero->getStamina().maximum * ui->data_playerStamina->height()));
        painter.drawLine(ui->data_playerStamina->x()-10, heightReach, ui->data_playerStamina->x()+ui->data_playerStamina->width()+10, heightReach);
    }
    painter.drawText(QPoint(ui->button_useSpell->x()+ui->button_useSpell->width()+5, ui->button_useSpell->y()+ui->button_useSpell->height()/2+2), QString("%1").arg(ACTION_COST_USE_SPELL));
    painter.drawText(QPoint(ui->button_heavyAttack->x()+ui->button_heavyAttack->width()/2-10, ui->button_heavyAttack->y()+ui->button_heavyAttack->height()+15), QString("%1").arg(ACTION_COST_HEAVY_ATTACK));
    painter.drawText(QPoint(ui->button_lightAttack->x()-18, ui->button_lightAttack->y()+ui->button_lightAttack->height()/2+2), QString("%1").arg(ACTION_COST_LIGHT_ATTACK));
    painter.drawText(QPoint(ui->button_flee->x()+ui->button_flee->width()/2-10, ui->button_flee->y()-5), QString("%1").arg(ACTION_COST_FLEE));

    QFont font;
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(QPen(QBrush("#FFFFFF"),1));

    painter.drawText(QPoint(ui->img_monster->x()+ui->img_monster->width()+40, ui->monsterLife->y()+ui->monsterLife->height()+25), buff.asprintf("%d/%d", mMonster->getLife().current, mMonster->getLife().maximum));

    painter.setPen(QPen("#2944AD"));
    painter.drawText(QPoint(ui->heroMana->x()+10, ui->heroMana->y()+ui->heroMana->height()+25), buff.asprintf("%d/%d", mHero->getMana().current, mHero->getMana().maximum));
    painter.setPen(QPen("#308D32"));
    painter.drawText(ui->heroLife->x()+10,  ui->heroLife->y()-10, buff.asprintf("%d/%d", mHero->getLife().current, mHero->getLife().maximum));
    painter.setPen(QPen(QBrush("#FFFFFF"), 1));

    if(mDodgeSucces)
    {
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(ui->heroLife->x()+ui->heroLife->width()*2/3, ui->heroLife->y()-10, "Esquive");
    }
    if(mFleeFail)
    {
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(ui->heroLife->x()+ui->heroLife->width()*2/3, ui->heroLife->y()-10, "Échec");
    }
    if(mConfused)
    {
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(ui->heroLife->x()+ui->heroLife->width()*2/3, ui->heroLife->y()-10, "Confusion");
    }
}

void Win_Fight::onHideAnimation()
{
    if(w_fightAnimation)
        delete w_fightAnimation;
    w_fightAnimation = nullptr;
}

void Win_Fight::onUseSpell(Skill * skill)
{
    if(!heroUseStamina(ACTION_COST_USE_SPELL))
    {
        showNotEnoughtStamina();
        return;
    }

    SpellSkill * spell = dynamic_cast<SpellSkill*>(skill);
    if(mHero->getMana().current < spell->getManaCost())
    {
        ui->heroMana->onNotEnoughtStat();
        return;
    }

    enableButtons(false);
    QTimer::singleShot(3000, this, [&]{enableButtons(true);});

    w_spellList->hide();

    W_AnimationSpell * spellAnimation = new W_AnimationSpell(this, spell->getIndex());
    spellAnimation->setGeometry((width()-700)/2, (height()-700)/2, 700, 700);
    emit sig_playSound(SOUND_SPELL_0+skill->getIndex());

    /* Spell can be cast */
    switch(spell->getIndex())
    {
        case SpellSkill::Benediction:
            mHero->applyStatus(Character::eStatus::benediction, QRandomGenerator::global()->bounded(1, 4));
            break;

        case SpellSkill::Confusion:
            mMonster->applyStatus(Character::eStatus::confused, QRandomGenerator::global()->bounded(1, 6));
            break;

        case SpellSkill::DeathTouch:
            if(QRandomGenerator::global()->bounded(100) < spell->getCapacity())
                mMonster->setLife(0);
            break;

        case SpellSkill::FireBall:
            mMonster->setLife(mMonster->getLife().current - spell->getCapacity());
            break;

        case SpellSkill::HealingHalo:
            mHero->setLife(mHero->getLife().current + spell->getCapacity());
            break;

        case SpellSkill::Poisoning:
            mMonster->applyStatus(Character::eStatus::poisoned, QRandomGenerator::global()->bounded(8, 20));
            break;

        case SpellSkill::PrimitiveShield:
            mHero->applyStatus(Character::eStatus::shield, QRandomGenerator::global()->bounded(2, 5));
            break;

        case SpellSkill::Smoke:
            endFight();
            break;
    }
    mHero->setMana(mHero->getMana().current - spell->getManaCost());
}














void W_AnimatedProgressBar::onStatChanged()
{
    if(mAnimation.blink->isActive())
        mAnimation.blink->stop();
    if(mAnimation.end->isActive())
        mAnimation.end->stop();

    switch(mSetting.entityStatToDisplay)
    {
        case eLife:
            mStatBar.max = mEntity->getLife().maximum;
            mStatBar.current = mEntity->getLife().current;
            break;

        case eMana:
            mStatBar.max = mEntity->getMana().maximum;
            mStatBar.current = mEntity->getMana().current;
            break;

        case eStamina:
            mStatBar.max = mEntity->getStamina().maximum;
            mStatBar.current = mEntity->getStamina().current;
            break;
    }

    mAnimation.end->start(2000);
    mAnimation.blink->start(500);
}

void W_AnimatedProgressBar::onNotEnoughtStat()
{
    if(mNotEnoughtStat)
        return;

    mNotEnoughtStat = true;
    update();
    QTimer::singleShot(800, this, [&]{ mNotEnoughtStat = false; update(); });
}

void W_AnimatedProgressBar::onAnimate()
{
    mAnimation.blinkStatus = !mAnimation.blinkStatus;
    update();
}

void W_AnimatedProgressBar::onEndAnimation()
{
    mAnimation.blink->stop();
    mAnimation.end->stop();

    mStatBar.lost = mStatBar.current;
    update();
}

void W_AnimatedProgressBar::setupUi(Character * entity, Settings settings)
{
    mSetting = settings;
    mEntity = entity;

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setMinimumHeight(mSetting.height);
    setMaximumHeight(mSetting.height);

    switch(settings.entityStatToDisplay)
    {
        case eLife:
            mStatBar.max = mEntity->getLife().maximum;
            mStatBar.current = mEntity->getLife().current;
            connect(entity, &Character::sig_lifeChanged, this, &W_AnimatedProgressBar::onStatChanged);
            break;

        case eMana:
            mStatBar.max = mEntity->getMana().maximum;
            mStatBar.current = mEntity->getMana().current;
            connect(entity, &Character::sig_manaChanged, this, &W_AnimatedProgressBar::onStatChanged);
            break;

        case eStamina:
            mStatBar.max = mEntity->getStamina().maximum;
            mStatBar.current = mEntity->getStamina().current;
            connect(entity, &Character::sig_staminaMaxChanged, this, &W_AnimatedProgressBar::onStatChanged);
            break;
    }
    mStatBar.lost = mStatBar.current;
}

void W_AnimatedProgressBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(!mEntity)
        return;

    QRect borders(0,0,width(),height());
    QRect field(BORDER_SIZE, BORDER_SIZE, width()-2*BORDER_SIZE, height()-2*BORDER_SIZE);

    QRect statBackground(field);
    QRect statBlink(field.x(),field.y(),field.width()*(static_cast<qreal>(mStatBar.lost)/static_cast<qreal>(mStatBar.max)),field.height());
    QRect statFilling(field.x(),field.y(),field.width()*(static_cast<qreal>(mStatBar.current)/static_cast<qreal>(mStatBar.max)),field.height());

    painter.setPen(QPen(QBrush("#00FFFFFF"), 0));

    painter.setBrush(QBrush("#FFFFFF"));
    painter.drawRoundedRect(borders, CORNER_ROUNDED, CORNER_ROUNDED);

    painter.setBrush(mSetting.colorBackground);
    painter.drawRoundedRect(statBackground, CORNER_ROUNDED, CORNER_ROUNDED);

    painter.setBrush(mAnimation.blinkStatus ? mSetting.colorBlink1 : mSetting.colorBlink2);
    painter.drawRoundedRect(statBlink, CORNER_ROUNDED, CORNER_ROUNDED);

    painter.setBrush(mSetting.colorFilling);
    painter.drawRoundedRect(statFilling, CORNER_ROUNDED, CORNER_ROUNDED);

    if(mNotEnoughtStat)
    {
        painter.setOpacity(0.8);
        painter.setBrush(QBrush("#A93226"));
        painter.drawRoundedRect(statBackground, CORNER_ROUNDED, CORNER_ROUNDED);
    }
}

void W_StatusBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(!mEntity)
        return;

    int offset = mRightAligned ? -(height() + 5) : height() + 5;
    QRect logoArea(mRightAligned ? width() - height() - 5 : 5, 0, height(), height());

    if(mEntity->isApplied(Character::eStatus::benediction))
    {
        painter.drawPixmap(logoArea, QPixmap(":/icons/logo_benediction.png"));
        logoArea.setRect(logoArea.x() + offset, logoArea.y(), logoArea.height(), logoArea.height());
    }
    if(mEntity->isApplied(Character::eStatus::confused))
    {
        painter.drawPixmap(logoArea, QPixmap(":/icons/logo_confused.png"));
        logoArea.setRect(logoArea.x() + offset, logoArea.y(), logoArea.height(), logoArea.height());
    }
    if(mEntity->isApplied(Character::eStatus::heal))
    {
        painter.drawPixmap(logoArea, QPixmap(":/icons/logo_heal.png"));
        logoArea.setRect(logoArea.x() + offset, logoArea.y(), logoArea.height(), logoArea.height());
    }
    if(mEntity->isApplied(Character::eStatus::poisoned))
    {
        painter.drawPixmap(logoArea, QPixmap(":/icons/logo_poisoned.png"));
        logoArea.setRect(logoArea.x() + offset, logoArea.y(), logoArea.height(), logoArea.height());
    }
    if(mEntity->isApplied(Character::eStatus::shield))
    {
        painter.drawPixmap(logoArea, QPixmap(":/icons/logo_shield.png"));
        logoArea.setRect(logoArea.x() + offset, logoArea.y(), logoArea.height(), logoArea.height());
    }
}
