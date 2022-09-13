#include "win_fight.h"
#include "ui_win_fight.h"

#include "frag_speel.h"

Win_Fight::Win_Fight(QWidget *parent, Hero * hero, Monster * monster) :
    QWidget(parent),
    mHero(hero),
    mMonster(monster),
    mDodgeSucces(false),
    mFleeFail(false),
    mBreak(false),
    w_fightAnimation(nullptr),
    w_spellAnimation(nullptr),
    w_spellList(nullptr),
    ui(new Ui::Win_Fight)
{
    ui->setupUi(this);
    if(parent)
        setGeometry(parent->x(), parent->y(), parent->width(), parent->height());
    hide();
    initInterface();

    t_restoreMonsterImage = new QTimer(this);
    t_restoreMonsterImage->setSingleShot(true);
    connect(t_restoreMonsterImage, SIGNAL(timeout()), this, SLOT(restoreMonsterImage()));

    t_fightEvent = new QTimer(this);
    connect(t_fightEvent, SIGNAL(timeout()), this, SLOT(onFightEvent()));
    t_fightEvent->start(10000);

    hero->stopMoving();

    if(hero->getHeroClass() == Hero::eSwordman)
        ui->button_useSpell->hide();

    connect(ui->button_heavyAttack, &QPushButton::clicked, this, &Win_Fight::onButtonHeavyAttackClicked);
    connect(ui->button_lightAttack, &QPushButton::clicked, this, &Win_Fight::onButtonLightAttackClicked);
    connect(ui->button_useObject, &QPushButton::clicked, this, &Win_Fight::onButtonUseObjectClicked);
    connect(ui->button_flee, &QPushButton::clicked, this, &Win_Fight::onButtonFleeClicked);
    connect(ui->button_useSpell, &QPushButton::clicked, this, &Win_Fight::onButtonUseSpellClicked);
}

Win_Fight::~Win_Fight()
{
    delete ui;
    if(w_fightAnimation)
        delete w_fightAnimation;
    if(w_spellAnimation)
        delete w_spellAnimation;
    if(w_spellList)
        delete w_spellList;
}

void Win_Fight::displayInterface()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(500);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
    b->setDuration(600);
    b->setStartValue(QRect(this->x()-this->width(),this->y(),this->width(),this->height()));
    b->setEndValue(QRect(this->x(),this->y(),this->width(),this->height()));
    b->start(QPropertyAnimation::DeleteWhenStopped);

    showFullScreen();
}

void Win_Fight::hideInterface()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(600);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_closeWindow()));
    timer->start(1000);
}

void Win_Fight::initInterface()
{    
    ui->button_useSpell->setEnabled(false);
    ui->statusConfused->hide();
    ui->statusPoisoned->hide();
    ui->statusHeroGodBenediction->hide();

    if(mHero->getSkillList()[PassiveSkill::GodBenediction]->isUnlock())
        ui->statusHeroGodBenediction->show();

    if(mHero->getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
        ui->button_useSpell->setEnabled(true);

    mLifeMonster = new AnimatedProgressBar(this, mMonster, {25, AnimatedProgressBar::eLife, QBrush("#ABEBC6"), QBrush("#196F3D"), QBrush("#CCCCCC"), QBrush("#E74C3C")});
    mLifeHero = new AnimatedProgressBar(this, mHero, {25, AnimatedProgressBar::eLife, QBrush("#ABEBC6"), QBrush("#196F3D"), QBrush("#CCCCCC"), QBrush("#E74C3C")});
    mManaHero = new AnimatedProgressBar(this, mHero, {12, AnimatedProgressBar::eMana, QBrush("#A9CCE3"), QBrush("#154360"), QBrush("#CCCCCC"), QBrush("#A6ACAF")});

    t_heroStaminaRecovery = new QTimer(this);
    t_monsterStaminaRecovery = new QTimer(this);

    loadFightAnimationsPixmap();
    ui->image_fight->setScaledContents(true);
    ui->image_fight->setPixmap(mMonster->getFightImage(0));

    ui->layout_monsterLife->addWidget(mLifeMonster);
    ui->layout_heroLife->addWidget(mLifeHero);
    ui->layout_heroMana->addWidget(mManaHero);
    ui->data_playerStamina->setMaximum(mHero->getStamina().maxStamina);
    ui->data_playerStamina->setValue(mHero->getStamina().curStamina);

    mBenedictionCounter = 0;
    mPrimitiveShield = false;

    connect(mHero, &Hero::sig_lifeChanged, this, [&]{ update(); });
    connect(mHero, &Hero::sig_manaChanged, this, [&]{ update(); });
    connect(mHero, &Hero::sig_dodge, this, [&]{
        mDodgeSucces = true;
        update();
        QTimer::singleShot(800, this, [&]{ mDodgeSucces = false; update(); });
    });

    connect(t_heroStaminaRecovery, SIGNAL(timeout()), this, SLOT(heroStaminaRecovery()));
    connect(t_monsterStaminaRecovery, SIGNAL(timeout()), this, SLOT(monsterStaminaRecovery()));

    mScenePotion = new QGraphicsScene(this);
    //ui->graphicsView->setStyleSheet(STYLE_SHEET_GRAPHICSVIEW(0));
    ui->graphicsView->setScene(mScenePotion);

    addConsumablesOnScreen();

    ui->img_player->setPixmap(mHero->getImage());
    ui->img_player->setScaledContents(true);
    ui->img_monster->setPixmap(mMonster->getImage());
    ui->img_monster->setScaledContents(true);

    monsterUseStamina(QRandomGenerator::global()->bounded(30, 70));

    w_spellList = new W_SpellListSelection(this, mHero);
    w_spellList->setWindowFlags(Qt::FramelessWindowHint);
    connect(w_spellList, SIGNAL(sig_spellClicked(Skill*)), this, SLOT(onUseSpell(Skill*)));
    w_spellList->setGeometry(ui->button_useSpell->x() - w_spellList->width(), ui->button_useSpell->y(), w_spellList->width(), w_spellList->height());
    w_spellList->hide();

    t_heroStaminaRecovery->start(static_cast<int>(300.0*(static_cast<qreal>(abs(mHero->getAttackSpeed()-11.0))/5.0)));
}

void Win_Fight::loadFightAnimationsPixmap()
{
    pMonsterLightAttack = mMonster->getLightAttackAnimation();
    pMonsterHeavyAttack = mMonster->getHeavyAttackAnimation();
}

bool Win_Fight::heroUseStamina(int loss)
{
    if(mHero->getStamina().curStamina - loss < 0)
        return false;
    mHero->setStamina(mHero->getStamina().curStamina - loss);
    if(t_heroStaminaRecovery->isActive())
        t_heroStaminaRecovery->stop();
    t_heroStaminaRecovery->start(static_cast<int>(300.0*(static_cast<qreal>(abs(mHero->getAttackSpeed()-11.0))/5.0)));
    return true;
}

void Win_Fight::monsterUseStamina(int loss)
{
    mMonster->setStamina(mMonster->getStamina().curStamina - loss);
    t_monsterStaminaRecovery->start(100);
}

void Win_Fight::heroStaminaRecovery()
{
    mHero->setStamina(mHero->getStamina().curStamina+5);
    ui->data_playerStamina->setValue(mHero->getStamina().curStamina);
    if(mHero->getStamina().curStamina == mHero->getStamina().maxStamina)
        heroStaminaRecovered();
    update();
}

void Win_Fight::monsterStaminaRecovery()
{
    if(!mMonster->isStatus(Monster::confused) || QRandomGenerator::global()->bounded(3))
    {
        mMonster->setStamina(mMonster->getStamina().curStamina+1);
    }

    if(mMonster->getStamina().curStamina >= mMonster->getStamina().maxStamina)
    {
        monsterStaminaRecovered();
    }
}

void Win_Fight::monsterStaminaRecovered()
{
    float innateRobustnessCoef = 1.0;
    t_monsterStaminaRecovery->stop();

    if(mHero->getSkillList()[PassiveSkill::InnateRobustness]->isUnlock())
    {
        // 30% chance -20% dmg
        if(QRandomGenerator::global()->bounded(100) < 30)
            innateRobustnessCoef = 0.8;
    }

    if(mPrimitiveShield)
    {
        // -10% dmg
        innateRobustnessCoef *= 0.9;
    }

    switch(QRandomGenerator::global()->bounded(3))
    {
        case 0 :
            emit sig_playSound(mMonster->getSoundIndexFor(HEAVY_ATTACK));
            if(w_fightAnimation)
                delete w_fightAnimation;
            w_fightAnimation = new W_Animation_Fight(this, pMonsterHeavyAttack);
            connect(w_fightAnimation, SIGNAL(sig_hideAnimation()), this, SLOT(hideAnimation()));
            w_fightAnimation->setGeometry((width()-700)/2,(height()-700)/2,700,700);

            monsterUseStamina(80);
            if(!mBenedictionCounter)
            {
                mHero->takeDamage(static_cast<int>(mMonster->getDamage()*1.5*innateRobustnessCoef));
            }
            else
                mBenedictionCounter--;
            break;

        case 1 :
            emit sig_playSound(mMonster->getSoundIndexFor(LIGHT_ATTACK));
            if(w_fightAnimation)
                delete w_fightAnimation;
            w_fightAnimation = new W_Animation_Fight(this, pMonsterLightAttack);
            connect(w_fightAnimation, SIGNAL(sig_hideAnimation()), this, SLOT(hideAnimation()));
            w_fightAnimation->setGeometry((width()-700)/2,(height()-700)/2,700,700);

            monsterUseStamina(50);
            if(!mBenedictionCounter)
            {
                mHero->takeDamage(static_cast<int>(mMonster->getDamage()*1.0*innateRobustnessCoef));
            }
            else
                mBenedictionCounter--;
            break;

        case 2 :
            emit sig_playSound(mMonster->getSoundIndexFor(ROAR));
            monsterUseStamina(30);
    }
    checkFightIssue();
}

void Win_Fight::useConsumable(ItemQuickDisplayer * w_item)
{
    enableItemsUtilisation(false);
    Consumable * item = dynamic_cast<Consumable*>(w_item->getItem());
    while(!mConsumables.isEmpty())
    {
        mScenePotion->removeItem(mConsumables.last());
        mConsumables.takeLast()->deleteLater();
    }
    mHero->useConsumable(item);
    addConsumablesOnScreen();
}

void Win_Fight::heroStaminaRecovered()
{
    t_heroStaminaRecovery->stop();
}

void Win_Fight::enableButtons(bool toggle)
{
    ui->button_flee->setEnabled(toggle);
    ui->button_useObject->setEnabled(toggle);
    ui->button_heavyAttack->setEnabled(toggle);
    ui->button_lightAttack->setEnabled(toggle);
}

void Win_Fight::enableItemsUtilisation(bool toggle)
{
    if(toggle)
    {
        //ui->graphicsView->setStyleSheet(STYLE_SHEET_GRAPHICSVIEW(1));
        for(ItemQuickDisplayer * w_item : mConsumables)
        {
            connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(useConsumable(ItemQuickDisplayer*)));
        }
    }else
    {
        //ui->graphicsView->setStyleSheet(STYLE_SHEET_GRAPHICSVIEW(0));
        for(ItemQuickDisplayer * w_item : mConsumables)
        {
            disconnect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(useConsumable(ItemQuickDisplayer*)));
        }
    }
}

void Win_Fight::addConsumablesOnScreen()
{
    mScenePotion->setSceneRect(0,0,ui->graphicsView->width(), ui->graphicsView->height());
    QList<Consumable*> list = mHero->getBag()->getConsumables();
    for(Consumable * item : qAsConst(list))
    {
        ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
        w_item->setMovable(false);
        ui->graphicsView->scene()->addItem(w_item);
        mConsumables.append(w_item);
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

void Win_Fight::checkFightIssue()
{
    if(mHero->getLife().curLife <= 0){
        emit sig_endFight(mHero);
        hideInterface();
    }else if(mMonster->getLife().curLife <= 0){
        emit sig_endFight(mMonster);
        hideInterface();
    }
}

#define ACTION_COST_HEAVY_ATTACK    ((mHero->getStamina().maxStamina > 400) ? 400 : mHero->getStamina().maxStamina)
#define ACTION_COST_LIGHT_ATTACK    100
#define ACTION_COST_USE_ITEM        80
#define ACTION_COST_FLEE            ((mHero->getStamina().maxStamina > 200) ? 200 : mHero->getStamina().maxStamina)

void Win_Fight::onButtonHeavyAttackClicked()
{
    if(!heroUseStamina(ACTION_COST_HEAVY_ATTACK))
        return;

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    float foreOfNatureCoef = 1.0;

    if(mHero->getSkillList()[PassiveSkill::ForceOfNature]->isUnlock())
    {
        // 30% chance +20% dmg
        if(QRandomGenerator::global()->bounded(100) < 30)
            foreOfNatureCoef = 1.2;
    }

    emit sig_playSound(SOUND_HERO_ATTACK);
    mMonster->setLife(mMonster->getLife().curLife-static_cast<int>(mHero->calculateDamage()*foreOfNatureCoef*(ACTION_COST_HEAVY_ATTACK/100.0)));
    ui->image_fight->setPixmap(mMonster->getFightImage(1));
    t_restoreMonsterImage->start(600);

    checkFightIssue();
}

void Win_Fight::onButtonUseObjectClicked()
{
    if(mConsumables.isEmpty())
        return;
    if(!heroUseStamina(ACTION_COST_USE_ITEM))
        return;

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    enableItemsUtilisation(true);

    checkFightIssue();
}

void Win_Fight::onButtonLightAttackClicked()
{
    if(!heroUseStamina(ACTION_COST_LIGHT_ATTACK))
        return;

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    emit sig_playSound(SOUND_HERO_ATTACK);
    mMonster->setLife(mMonster->getLife().curLife-static_cast<int>(mHero->calculateDamage()));
    ui->image_fight->setPixmap(mMonster->getFightImage(1));
    t_restoreMonsterImage->start(1000);

    checkFightIssue();
}

void Win_Fight::onButtonFleeClicked()
{
    if(!heroUseStamina(ACTION_COST_FLEE))
        return;

    enableButtons(false);
    QTimer::singleShot(1500, this, [&]{enableButtons(true);});

    if(QRandomGenerator::global()->bounded(3))
    {
        emit sig_endFight(nullptr);
        hideInterface();
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

void Win_Fight::paintEvent(QPaintEvent *)
{
    QPainter painter(this);    
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(100,50,width()-200,height()-150), QPixmap(":/graphicItems/background_window_1.png"));


    painter.drawPixmap(QRect(ui->button_useObject->x(), ui->button_heavyAttack->y(), ui->button_useObject->width()*3, ui->button_heavyAttack->height()*3),
                       QPixmap(":/graphicItems/buttons_background.png"));

    painter.setBrush(QBrush("#434343"));
    painter.setPen(QPen(QBrush("#FFFFFF"), 3));

    painter.drawLine(ui->img_monster->x()+ui->img_monster->width()/2, mLifeMonster->y()-10,
                     mLifeMonster->x()+mLifeMonster->width()+50, mLifeMonster->y()-10);

    painter.drawEllipse(QRect(ui->img_monster->x()-15, ui->img_monster->y()-15, ui->img_monster->width()+30, ui->img_monster->height()+30));

    painter.setPen(QPen(QBrush("#ffd800"),1));

    QString buff = "";
    float stamina = static_cast<float>(ui->data_playerStamina->value());
    painter.drawText(QRect(ui->data_playerStamina->x()-10, ui->data_playerStamina->y()+ui->data_playerStamina->height(), 30, 20), Qt::AlignCenter,  buff.asprintf("%d", static_cast<int>(stamina)));
    if(stamina < mHero->getStamina().maxStamina){
        int heightReach = static_cast<int>(ui->data_playerStamina->y()+ui->data_playerStamina->height() - static_cast<float>(stamina/mHero->getStamina().maxStamina * ui->data_playerStamina->height()));
        painter.drawLine(ui->data_playerStamina->x()-10, heightReach, ui->data_playerStamina->x()+ui->data_playerStamina->width()+10, heightReach);
    }
    painter.drawText(QPoint(ui->button_useObject->x()+ui->button_useObject->width()+5, ui->button_useObject->y()+ui->button_useObject->height()/2+2), QString("%1").arg(ACTION_COST_USE_ITEM));
    painter.drawText(QPoint(ui->button_heavyAttack->x()+ui->button_heavyAttack->width()/2-10, ui->button_heavyAttack->y()+ui->button_heavyAttack->height()+15), QString("%1").arg(ACTION_COST_HEAVY_ATTACK));
    painter.drawText(QPoint(ui->button_lightAttack->x()-18, ui->button_lightAttack->y()+ui->button_lightAttack->height()/2+2), QString("%1").arg(ACTION_COST_LIGHT_ATTACK));
    painter.drawText(QPoint(ui->button_flee->x()+ui->button_flee->width()/2-10, ui->button_flee->y()-5), QString("%1").arg(ACTION_COST_FLEE));

    QFont font;
    font.setPixelSize(20);
    painter.setFont(font);
    painter.setPen(QPen(QBrush("#FFFFFF"),1));

    painter.drawText(QPoint(ui->img_monster->x()+ui->img_monster->width()+40, mLifeMonster->y()+mLifeMonster->height()+25), buff.asprintf("%d/%d", mMonster->getLife().curLife, mMonster->getLife().maxLife));

    painter.setPen(QPen("#2944AD"));
    painter.drawText(QPoint(mManaHero->x()+10, mManaHero->y()+mManaHero->height()+25), buff.asprintf("%d/%d", mHero->getMana().curMana, mHero->getMana().maxMana));
    painter.setPen(QPen("#308D32"));
    painter.drawText(mLifeHero->x()+10,  mLifeHero->y()-10, buff.asprintf("%d/%d", mHero->getLife().curLife, mHero->getLife().maxLife));
    painter.setPen(QPen(QBrush("#FFFFFF"), 1));

    if(mDodgeSucces)
    {
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(mLifeHero->x()+mLifeHero->width()*2/3, mLifeHero->y()-10, "Esquive");
    }
    if(mFleeFail)
    {
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(mLifeHero->x()+mLifeHero->width()*2/3, mLifeHero->y()-10, "Échec");
    }

    if(mBreak){
        painter.setFont(QFont("Sitka Small", 15));
        painter.drawText(ui->buttonPause->x()-290, ui->buttonPause->y()+ui->buttonPause->height()-13, "Combat mis en Pause");
    }
}

void Win_Fight::on_buttonPause_clicked()
{
    if(mBreak){
        mBreak = false;
        t_heroStaminaRecovery->start(static_cast<int>(300.0*(static_cast<qreal>(abs(mHero->getAttackSpeed()-11.0))/5.0)));
        t_monsterStaminaRecovery->start(100);
        enableButtons(true);
    }else{
        mBreak = true;
        t_heroStaminaRecovery->stop();
        t_monsterStaminaRecovery->stop();
        enableButtons(false);
    }
    update();
}

void Win_Fight::hideAnimation()
{
    if(w_fightAnimation)
        delete w_fightAnimation;
    w_fightAnimation = nullptr;
}

void Win_Fight::hideAnimationSpell()
{
    if(w_spellAnimation)
        delete w_spellAnimation;
    w_spellAnimation = nullptr;
}

void Win_Fight::restoreMonsterImage()
{
    ui->image_fight->setPixmap(mMonster->getFightImage(0));
}

void Win_Fight::onFightEvent()
{
    if(mHero->getSkillList()[PassiveSkill::GodBenediction]->isUnlock())
    {
        if(QRandomGenerator::global()->bounded(3) == 0)
        {
            // TODO : add healing sound
            mHero->setLife(mHero->getLife().curLife + mHero->getLife().maxLife/20);
        }
    }

    /* Poisoned monster */
    if(mMonster->isStatus(Monster::poisoned))
    {
        mMonster->setLife(mMonster->getLife().curLife - mMonster->getLife().maxLife*0.08);
        if(!QRandomGenerator::global()->bounded(10))
        {
            mMonster->removeStatus(Monster::poisoned);
            ui->statusPoisoned->hide();
        }
    }
}

void Win_Fight::onUseSpell(Skill * skill)
{
    SpellSkill * spell = dynamic_cast<SpellSkill*>(skill);
    if(mHero->getMana().curMana < spell->getManaCost())
    {
        mManaHero->onNotEnoughtStat();
        return;
    }

    enableButtons(false);
    w_spellList->hide();

    w_spellAnimation = new W_AnimationSpell(this, spell->getIndex());
    connect(w_spellAnimation, SIGNAL(sig_hideAnimation()), this, SLOT(hideAnimationSpell()));
    w_spellAnimation->setGeometry((width()-700)/2, (height()-700)/2, 700, 700);
    emit sig_playSound(SOUND_SPELL_0+skill->getIndex());

    w_spellList->enable(false);
    QTimer::singleShot(3000, this, [&]{w_spellList->enable(true);w_spellList->show();enableButtons(true);});

    /* Spell can be cast */
    switch(spell->getIndex())
    {
        case SpellSkill::Benediction:
            mBenedictionCounter = 3;
            break;

        case SpellSkill::Confusion:
            mMonster->addStatus(Monster::confused);
            ui->statusConfused->show();
            break;

        case SpellSkill::DeathTouch:
            if(QRandomGenerator::global()->bounded(100) < spell->getCapacity())
            {
                mMonster->setLife(0);
            }
            break;

        case SpellSkill::FireBall:
            mMonster->setLife(mMonster->getLife().curLife - spell->getCapacity());
            break;

        case SpellSkill::HealingHalo:
            mHero->setLife(mHero->getLife().curLife + spell->getCapacity());
            break;

        case SpellSkill::Poisoning:
            mMonster->addStatus(Monster::poisoned);
            ui->statusPoisoned->show();
            break;

        case SpellSkill::PrimitiveShield:
            mPrimitiveShield = true;
            break;

        case SpellSkill::Smoke:
            emit sig_endFight(nullptr);
            hideInterface();
            break;
    }
    mHero->setMana(mHero->getMana().curMana - spell->getManaCost());

    checkFightIssue();
}














AnimatedProgressBar::AnimatedProgressBar(QWidget * parent, Character * entity, Settings settings):
    QWidget(parent),
    mEntity(entity),
    mSetting(settings),
    mNotEnoughtStat(false)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setMinimumHeight(mSetting.height);
    setMaximumHeight(mSetting.height);

    switch(settings.entityStatToDisplay)
    {
        case eLife:
            mStatBar.max = mEntity->getLife().maxLife;
            mStatBar.current = mEntity->getLife().curLife;
            connect(entity, &Character::sig_lifeChanged, this, &AnimatedProgressBar::onStatChanged);
            break;

        case eMana:
            mStatBar.max = mEntity->getMana().maxMana;
            mStatBar.current = mEntity->getMana().curMana;
            connect(entity, &Character::sig_manaChanged, this, &AnimatedProgressBar::onStatChanged);
            break;

        case eStamina:
            mStatBar.max = mEntity->getStamina().maxStamina;
            mStatBar.current = mEntity->getStamina().curStamina;
            connect(entity, &Character::sig_staminaMaxChanged, this, &AnimatedProgressBar::onStatChanged);
            break;
    }
    mStatBar.lost = mStatBar.current;

    mAnimation.blinkStatus = 0;
    mAnimation.blink = new QTimer(this);
    connect(mAnimation.blink, &QTimer::timeout, this, &AnimatedProgressBar::onAnimate);
    mAnimation.end = new QTimer(this);
    connect(mAnimation.end, &QTimer::timeout, this, &AnimatedProgressBar::onEndAnimation);
}

AnimatedProgressBar::~AnimatedProgressBar()
{

}

void AnimatedProgressBar::onStatChanged()
{
    if(mAnimation.blink->isActive())
        mAnimation.blink->stop();
    if(mAnimation.end->isActive())
        mAnimation.end->stop();

    switch(mSetting.entityStatToDisplay)
    {
        case eLife:
            mStatBar.max = mEntity->getLife().maxLife;
            mStatBar.current = mEntity->getLife().curLife;
            break;

        case eMana:
            mStatBar.max = mEntity->getMana().maxMana;
            mStatBar.current = mEntity->getMana().curMana;
            break;

        case eStamina:
            mStatBar.max = mEntity->getStamina().maxStamina;
            mStatBar.current = mEntity->getStamina().curStamina;
            break;
    }

    mAnimation.end->start(2000);
    mAnimation.blink->start(500);
}

void AnimatedProgressBar::onNotEnoughtStat()
{
    if(mNotEnoughtStat)
        return;

    mNotEnoughtStat = true;
    update();
    QTimer::singleShot(800, this, [&]{ mNotEnoughtStat = false; update(); });
}

void AnimatedProgressBar::onAnimate()
{
    mAnimation.blinkStatus = !mAnimation.blinkStatus;
    update();
}

void AnimatedProgressBar::onEndAnimation()
{
    mAnimation.blink->stop();
    mAnimation.end->stop();

    mStatBar.lost = mStatBar.current;
    update();
}

void AnimatedProgressBar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

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
