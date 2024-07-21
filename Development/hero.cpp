#include "hero.h"
#include "monster.h"

bool gEnableMovementWithMouseClic = false;

Hero::Hero():
    Character (),
    mIsInVillage(false),
    mFreeze(false),
    mSkillPoints(0),
    mIsInMapEvent(false)
{    
    setZValue(Z_HERO);
    mNextFrame = 0;
    mMoveHandler.t_move = new QTimer(this);
    connect(mMoveHandler.t_move, SIGNAL(timeout()), this, SLOT(move()));
    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(setNextFrame()));
    setTransformOriginPoint(boundingRect().center());

    t_movement = new QTimer(this);
    connect(t_movement, SIGNAL(timeout()), this, SLOT(nextMovement()));
    t_movement->setInterval(100);
}

void Hero::nextMovement()
{
    int directionMask = mMoveHandler.movementMask, xMov = 0, yMov = 0;

    if(!mMoveHandler.movementMask)
        return;

    if((mMoveHandler.movementMask & KEY_MASK_Z) && (mMoveHandler.movementMask & KEY_MASK_S))
        directionMask &= (~(KEY_MASK_Z|KEY_MASK_S)); // Incompatible directions
    if((mMoveHandler.movementMask & KEY_MASK_Q) && (mMoveHandler.movementMask & KEY_MASK_D))
        directionMask &= (~(KEY_MASK_Q|KEY_MASK_D)); // Incompatible directions

    if(directionMask)
    {
        if(directionMask & KEY_MASK_Z)
        {
            yMov = 100;
            if(directionMask & KEY_MASK_Q)
            {
                mMoveHandler.angle = 320;
            }
            else if(directionMask & KEY_MASK_D)
            {
                mMoveHandler.angle = 45;
            }
            else
            {
                mMoveHandler.angle = 0;
            }
        }
        if(directionMask & KEY_MASK_S)
        {
            yMov = -100;
            if(directionMask & KEY_MASK_Q)
            {
                mMoveHandler.angle = 225;
            }
            else if(directionMask & KEY_MASK_D)
            {
                mMoveHandler.angle = 135;
            }
            else
            {
                mMoveHandler.angle = 180;
            }
        }
        if(directionMask & KEY_MASK_Q)
        {
            xMov = -100;
            if(directionMask & KEY_MASK_Z)
            {
                mMoveHandler.angle = 315;
            }
            else if(directionMask & KEY_MASK_S)
            {
                mMoveHandler.angle = 225;
            }
            else
            {
                mMoveHandler.angle = 270;
            }
        }
        if(directionMask & KEY_MASK_D)
        {
            xMov = 100;
            if(directionMask & KEY_MASK_Z)
            {
                mMoveHandler.angle = 45;
            }
            else if(directionMask & KEY_MASK_S)
            {
                mMoveHandler.angle = 135;
            }
            else
            {
                mMoveHandler.angle = 90;
            }
        }

        mMoveHandler.destPos = QPointF(pos().x()+xMov,pos().y()+yMov);
        mMoveHandler.lastPos = pos();
    }


    if(mMoveHandler.angle > 180 && mMoveHandler.angle < 320)
    {
        setTransform(QTransform(-1, 0, 0, 1, boundingRect().width(), 0));
    }
    else
    {
        setTransform(QTransform());
    }

    if(mMoveHandler.angle < 40){
        mImageSelected = HERO_TOP;
    }else if(mMoveHandler.angle < 70){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 110){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 140){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 220){
        mImageSelected = HERO_BOT;
    }else if(mMoveHandler.angle < 250){
        mImageSelected = HERO_LEFT;
    }else if(mMoveHandler.angle < 290){
        mImageSelected = HERO_LEFT;
    }else if(mMoveHandler.angle < 320){
        mImageSelected = HERO_LEFT;
    }else{
        mImageSelected = HERO_TOP;
    }
}

void Hero::startMovingTo(int x, int y)
{
    mMoveHandler.t_move->start(TIMER_MOVE);
    mMoveHandler.destPos = QPointF(x,y);
    mMoveHandler.lastPos = pos();

    if(mMoveHandler.angle > 180 && mMoveHandler.angle < 320){
        setTransform(QTransform(-1, 0, 0, 1, boundingRect().width(), 0));
    }else{
        setTransform(QTransform());
    }

    if(mMoveHandler.angle < 40){
        mImageSelected = HERO_TOP;
    }else if(mMoveHandler.angle < 70){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 110){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 140){
        mImageSelected = HERO_RIGHT;
    }else if(mMoveHandler.angle < 220){
        mImageSelected = HERO_BOT;
    }else if(mMoveHandler.angle < 250){
        mImageSelected = HERO_LEFT;
    }else if(mMoveHandler.angle < 290){
        mImageSelected = HERO_LEFT;
    }else if(mMoveHandler.angle < 320){
        mImageSelected = HERO_LEFT;
    }else{
        mImageSelected = HERO_TOP;
    }
}

void Hero::stopMoving()
{
    mMoveHandler.t_move->stop();
    mMoveHandler.destPos = QPointF(x(),y());
    mImageSelected = HERO_STAND;
    mNextFrame = 0;
    update();
}

void Hero::interactWithPNG(bool toggle)
{
    mInteractionPNG = toggle;
}

bool Hero::isInteractingWithPNG()
{
    return mInteractionPNG;
}

bool Hero::isInVillage()
{
    return mIsInVillage;
}

bool Hero::isDead()
{
    if(mLife.curLife <= 0){
        return true;
    }else{
        return false;
    }
}

void Hero::freeze(bool toggle)
{
    if(toggle)
    {
        mFreeze = true;
        mMoveHandler.destPos = mMoveHandler.lastPos;
    }else
        mFreeze = false;
}

bool Hero::isFreeze()
{
    return mFreeze;
}

Hero *Hero::getInstance(Hero::HeroClasses hero)
{
    switch(hero)
    {
    case eSwordman:
        return new SwordMan("");

    case eArcher:
        return new Archer("");

    case eWizard:
        return new Wizard("");

    default:
        return new SwordMan("");
    }
}

void Hero::move()
{
    bool mapEventFound = false;
    double dx = static_cast<double>(SPEED_HERO)*qSin(qDegreesToRadians(mMoveHandler.angle));
    double dy = -static_cast<double>(SPEED_HERO)*qCos(qDegreesToRadians(mMoveHandler.angle));
    setPos(x()+dx, y()+dy);

    if(gEnableMovementWithMouseClic)
        if( (abs(mMoveHandler.destPos.x() - x()) < 1)  || (abs(mMoveHandler.destPos.y() - y()) < 1) )
        {
            stopMoving(); // Not working with keys
        }

    emit sig_heroMoved();

    QList<QGraphicsItem*> list = collidingItems();
    for(QGraphicsItem * item : qAsConst(list))
    {
        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem)
        {
            if(mapItem->isObstacle()){
                setPos(mMoveHandler.lastPos);
                stopMoving();
                return;
            }
            Bush * bush = dynamic_cast<Bush*>(item);
            if(bush)
            {
                if(y()+this->boundingRect().height() < mapItem->y()+mapItem->boundingRect().height()*3/4){
                    mapItem->setZValue(Z_HERO+1);
                }else {
                    mapItem->setZValue(Z_BUSH);
                }
                if(!bush->isAnimated()){
                    emit sig_movedInBush(bush);
                }
                continue;
            }
            BushEventCoin * bushCoinEvent = dynamic_cast<BushEventCoin*>(item);
            if(bushCoinEvent)
            {
                if(y()+this->boundingRect().height() < mapItem->y()+mapItem->boundingRect().height()*3/4){
                    mapItem->setZValue(Z_HERO+1);
                }else {
                    mapItem->setZValue(Z_BUSH);
                }
                if(!bushCoinEvent->isAnimated()){
                    emit sig_movedInBushEvent(bushCoinEvent);
                }
                continue;
            }
            BushEventEquipment * bushEquipmentEvent = dynamic_cast<BushEventEquipment*>(item);
            if(bushEquipmentEvent)
            {
                if(y()+this->boundingRect().height() < mapItem->y()+mapItem->boundingRect().height()*3/4){
                    mapItem->setZValue(Z_HERO+1);
                }else {
                    mapItem->setZValue(Z_BUSH);
                }
                if(!bushEquipmentEvent->isAnimated()){
                    emit sig_movedInBushEvent(bushEquipmentEvent);
                }
                continue;
            }
        }
        MapEvent* mapEvent = dynamic_cast<MapEvent*>(item);
        if(mapEvent)
        {
            if(mapEvent->eventIsActive())
                mapEventFound = true;
            if(!mIsInMapEvent && mapEvent->eventIsActive())
            {
                Tool * tool = nullptr;

                ChestEvent * chestEvent = dynamic_cast<ChestEvent*>(mapEvent);
                if(chestEvent)
                {
                    tool = dynamic_cast<Tool*>(getBag()->getShovel());
                }

                OreSpot * oreSpot = dynamic_cast<OreSpot*>(mapEvent);
                if(oreSpot)
                {
                    tool = dynamic_cast<Tool*>(getBag()->getPickaxe());
                }

                FishingEvent * fishes = dynamic_cast<FishingEvent*>(mapEvent);
                if(fishes)
                {
                    tool = dynamic_cast<Tool*>(getBag()->getFishingrod());
                }

                if(tool)
                    emit sig_enterMapEvent(tool);
            }
        }
        Monster * monsterDead = dynamic_cast<Monster*>(item);
        if(monsterDead)
        {
            if(monsterDead->isDead())
                mapEventFound = true;
            if(!mIsInMapEvent && monsterDead->isDead() && !monsterDead->isSkinned())
            {
                Tool * tool  = dynamic_cast<Tool*>(getBag()->getKnife());
                if(tool)
                    emit sig_enterMapEvent(tool);
            }
        }
    }

    if(mapEventFound)
    {
        mIsInMapEvent = true;
    }
    else
    {
        if(mIsInMapEvent)
        {
            emit sig_leaveMapEvent();
        }
        mIsInMapEvent = false;
    }

    mMoveHandler.lastPos = pos();
}

QPointF Hero::getLocation()
{
    return mMoveHandler.lastPos;
}

Bag *Hero::getBag()
{
    return mBag;
}

Gear * Hero::getGear()
{
    return mGear;
}

int Hero::getCoin()
{
    return mCoin;
}

Hero::Experience Hero::getExperience()
{
    return mExperience;
}

int Hero::getSkillPoints()
{
    return mSkillPoints;
}

Skill * Hero::getPassiveSkill(int index)
{
    if(index >= PassiveSkill::NbSkills)
    {
        DEBUG_ERR("Skill asked doesn't exist");
        return mSkillList[0];
    }
    return mSkillList[index];
}

Skill * Hero::getSpellSkill(int index)
{
    if(index >= SpellSkill::NbSpells)
    {
        DEBUG_ERR("Skill asked doesn't exist");
        return mSpellList[0];
    }

    return mSpellList[index];
}

PassiveSkill **Hero::getSkillList()
{
    return mSkillList;
}

SpellSkill **Hero::getSpellList()
{
    return mSpellList;
}

void Hero::setLocation(QPointF location)
{
    mMoveHandler.lastPos = location;
    mMoveHandler.destPos = location;
}

void Hero::setGear(Gear * gear)
{
    mGear = gear;
}

void Hero::setCoin(int coin)
{
    mCoin = coin;
    emit sig_coinChanged();
}

void Hero::setIsInVillage(bool toggle)
{
    mIsInVillage = toggle;
}

void Hero::setSkillPoints(int points)
{
    mSkillPoints = points;
    emit sig_skillPointsChanged();
}

void Hero::useConsumable(Consumable * item)
{
    emit sig_playSound(SOUND_DRINK);
    PotionLife * potionLife = dynamic_cast<PotionLife*>(item);
    if(potionLife)
    {
        setLife(getLife().curLife+potionLife->getCapacity());
    }
    PotionMana * potionMana = dynamic_cast<PotionMana*>(item);
    if(potionMana)
    {
        setMana(getMana().curMana+potionMana->getCapacity());
    }
    PotionStamina * potionStamina = dynamic_cast<PotionStamina*>(item);
    if(potionStamina)
    {
        setStaminaMax(getStamina().maxStamina+potionStamina->getCapacity());
    }
    PotionStrenght * potionStrength = dynamic_cast<PotionStrenght*>(item);
    if(potionStrength)
    {
        getBag()->setMaximumPayload(getBag()->getPayload().max+item->getCapacity());
    }
    PotionKnowledge * potionKnowledge = dynamic_cast<PotionKnowledge*>(item);
    if(potionKnowledge)
    {
        setSkillPoints(getSkillPoints()+potionKnowledge->getCapacity());
    }
    RedFish * redFish = dynamic_cast<RedFish*>(item);
    if(redFish)
    {
        setLife(getLife().curLife+redFish->getCapacity());
    }
    CommonFish * fish = dynamic_cast<CommonFish*>(item);
    if(fish)
    {
        setLife(getLife().curLife+fish->getCapacity());
    }
    Yellowfish * yellowFish = dynamic_cast<Yellowfish*>(item);
    if(yellowFish)
    {
        setLife(getLife().curLife+yellowFish->getCapacity());    }
    BlueFish * blueFish = dynamic_cast<BlueFish*>(item);
    if(blueFish)
    {
        setMana(getMana().curMana+blueFish->getCapacity());
    }

    mBag->removeItem(item);
}

void Hero::useScroll(Scroll * item)
{
    mBag->removeItem(item);
}

bool Hero::takeItem(Item * item)
{
    BagCoin * coins = dynamic_cast<BagCoin*>(item);
    if(coins){
        emit sig_playSound(SOUND_PICK_OBJECT);
        addCoin(item->getPrice());
        return true;
    }else{
        if(!mBag->addItem(item))
        {
            emit sig_ThrowItem(item);
            return false;
        }else{
            emit sig_playSound(SOUND_PICK_OBJECT);
            return true;
        }
    }
}

void Hero::throwItem(Item * item)
{
    mBag->removeItem(item);
}

qreal Hero::calculateDamage()
{
    return (1+(mExperience.level*0.05))*mGear->damageStat();
}

int Hero::getAttackSpeed()
{
    if(getGear()->getWeapon()!=nullptr){
        return getGear()->getWeapon()->getSpeed();
    }else{
        return 10;
    }
}

void Hero::takeDamage(int damage)
{
    if(QRandomGenerator::global()->bounded(100) > mGear->dodgingStat()){
        setLife(getLife().curLife - static_cast<int>((1.0 - static_cast<qreal>(mGear->defenseStat())/HERO_DEFENSE_MAX)*static_cast<qreal>(damage)));
    }else{
        emit sig_dodge();
    }
}

void Hero::addCoin(int coin)
{
    mCoin += coin;
    emit sig_coinChanged();
}

bool Hero::removeCoin(int coin)
{
    if(mCoin >= coin){
        mCoin -= coin;
        emit sig_coinChanged();
        return true;
    }
    return false;
}

bool Hero::addExperience(int exp)
{
    if( (mExperience.points += exp) >= mExperience.pointsToLevelUp){
        mExperience.level++;
        mExperience.points -= mExperience.pointsToLevelUp;
        mExperience.pointsToLevelUp += mExperience.pointsToLevelUp/4;
        levelUpHero();
        emit sig_experienceChanged();
        return true;
    }
    emit sig_experienceChanged();
    return false;
}

bool Hero::learnSkill(Skill * skillToLearn)
{
    PassiveSkill * skill = dynamic_cast<PassiveSkill*>(skillToLearn);
    if(skill)
    {
        if(getSkillList()[skill->getIndex()]->getUnlockPoints() > mSkillPoints)
            return false;

        /* Mage apprentice skill mandatory to learn Archmage*/
        if(!getSkillList()[PassiveSkill::MageApprentice]->isUnlock() && skillToLearn->getIndex() == PassiveSkill::Archmage)
            return false;

        setSkillPoints(mSkillPoints - getSkillList()[skill->getIndex()]->getUnlockPoints());
        skill->unlockSkill();
        return true;
    }

    SpellSkill * spell = dynamic_cast<SpellSkill*>(skillToLearn);
    if(spell)
    {
        if(getSpellList()[spell->getIndex()]->getUnlockPoints() > mSkillPoints)
            return false;

        /* Mage apprentice skill mandatory */
        if(!getSkillList()[PassiveSkill::MageApprentice]->isUnlock())
            return false;

        /* Archmage skill mandatory for superior spells */
        if(!getSkillList()[PassiveSkill::Archmage]->isUnlock() && spell->getIndex() > SpellSkill::PrimitiveShield)
            return false;

        setSkillPoints(mSkillPoints - getSpellList()[spell->getIndex()]->getUnlockPoints());
        spell->unlockSkill();
        return true;
    }

    return false;
}

bool Hero::learnPassiveSkill(int index)
{
    if(mSkillPoints < 1)
        return false;

    switch(index)
    {
        case Life:
            setLifeMax(mLife.maxLife + mHeroList[mClass].steps.life);
            setLife(mLife.curLife + mHeroList[mClass].steps.life);
            break;

        case Mana:
            setManaMax(mMana.maxMana + mHeroList[mClass].steps.mana);
            setMana(mMana.curMana + mHeroList[mClass].steps.mana);
            break;

        case Stamina:
            setStaminaMax(mStamina.maxStamina + mHeroList[mClass].steps.stamina);
            break;

        case Strength:
            mBag->setMaximumPayload(mBag->getPayload().max + mHeroList[mClass].steps.strength);
            break;

        default:
            return false;
    }

    setSkillPoints(mSkillPoints - 1);

    emit sig_skillUnlocked();
    return true;
}

void Hero::levelUpHero()
{
    mSkillPoints++;
    getBag()->setMaximumPayload(getBag()->getPayload().max + 2);
}

Hero::~Hero()
{
    for(int i = 0; i < Hero::eNbHeroClasses; i++)
    {
        if(mHeroList[i].weapon)
            delete mHeroList[i].weapon;
    }
}









SwordMan::SwordMan(QString name):
    Hero ()
{
    mName = name;
    mLife = {mHeroList[eSwordman].life, mHeroList[eSwordman].life};
    mMana = {mHeroList[eSwordman].mana, mHeroList[eSwordman].mana};
    mStamina = {mHeroList[eSwordman].stamina, mHeroList[eSwordman].stamina};
    mSkillPoints = mHeroList[eSwordman].skillPoints;

    mClass = eSwordman;

    mBoundingRect = QRect(0,0,100,100);
    QPolygon polygon;
    static const int points[] = {
        50, 100,
        25, 90,
        30, 50,
        30, 20,
        70, 20,
        70, 50,
        80, 90
    };
    polygon.setPoints(7, points);
    mShape.addPolygon(polygon);

    mGear = new Gear();
    connect(mGear, SIGNAL(sig_equipmentSet()), this, SIGNAL(sig_equipmentChanged()));
    mBag = new Bag();
    connect(mBag, SIGNAL(sig_bagEvent()), this, SIGNAL(sig_bagEvent()));
    connect(mBag, SIGNAL(sig_bagFull()), this, SIGNAL(sig_bagFull()));
    mCoin = mHeroList[eSwordman].coin;
    mExperience = Experience{0,1,10};
    mImage = QPixmap(":/heros/swordman_logo.png");
    mCurrentPixmap = QPixmap(":/heros/swordman.png");
    mNumberFrame = 4;
    mImageSelected = HERO_STAND;
    t_animation->start(150);
}

void SwordMan::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(0,0, mCurrentPixmap, static_cast<int>(mNextFrame*boundingRect().width()), static_cast<int>(mImageSelected*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

SwordMan::~SwordMan()
{
    if(mGear)
        delete mGear;
    if(mBag)
        delete mBag;
    for(int i=0; i<PassiveSkill::NbSkills; i++)
        delete mSkillList[i];
    for(int i=0; i<SpellSkill::NbSpells; i++)
        delete mSpellList[i];
}

int SwordMan::getHeroClass()
{
    return mClass;
}






Archer::Archer(QString name):
    Hero ()
{
    mName = name;
    mLife = {mHeroList[eArcher].life, mHeroList[eArcher].life};
    mMana = {mHeroList[eArcher].mana, mHeroList[eArcher].mana};
    mStamina = {mHeroList[eArcher].stamina, mHeroList[eArcher].stamina};
    mSkillPoints = mHeroList[eArcher].skillPoints;

    mClass = eArcher;

    mBoundingRect = QRect(0,0,100,100);
    QPolygon polygon;
    static const int points[] = {
        50, 100,
        25, 90,
        30, 50,
        30, 20,
        70, 20,
        70, 50,
        80, 90
    };
    polygon.setPoints(7, points);
    mShape.addPolygon(polygon);

    mGear = new Gear();
    connect(mGear, SIGNAL(sig_equipmentSet()), this, SIGNAL(sig_equipmentChanged()));
    mBag = new Bag();
    connect(mBag, SIGNAL(sig_bagEvent()), this, SIGNAL(sig_bagEvent()));
    connect(mBag, SIGNAL(sig_bagFull()), this, SIGNAL(sig_bagFull()));
    mCoin = mHeroList[eArcher].coin;
    mExperience = Experience{0,1,10};
    mImage = QPixmap(":/heros/archer_logo.png");
    mCurrentPixmap = QPixmap(":/heros/archer.png");
    mNumberFrame = 4;
    mImageSelected = HERO_STAND;
    t_animation->start(150);
}

void Archer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(0,0, mCurrentPixmap, static_cast<int>(mNextFrame*boundingRect().width()), static_cast<int>(mImageSelected*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

Archer::~Archer()
{
    if(mGear)
        delete mGear;
    if(mBag)
        delete mBag;
    for(int i=0; i<PassiveSkill::NbSkills; i++)
        delete mSkillList[i];
    for(int i=0; i<SpellSkill::NbSpells; i++)
        delete mSpellList[i];
}

int Archer::getHeroClass()
{
    return mClass;
}




Wizard::Wizard(QString name):
    Hero ()
{
    mName = name;
    mLife = {mHeroList[eWizard].life, mHeroList[eWizard].life};
    mMana = {mHeroList[eWizard].mana, mHeroList[eWizard].mana};
    mStamina = {mHeroList[eWizard].stamina, mHeroList[eWizard].stamina};
    mSkillPoints = mHeroList[eWizard].skillPoints;

    mClass = eWizard;
    mSkillList[PassiveSkill::MageApprentice]->unlockSkill();

    mBoundingRect = QRect(0,0,100,100);
    QPolygon polygon;
    static const int points[] = {
        50, 100,
        25, 90,
        30, 50,
        30, 20,
        70, 20,
        70, 50,
        80, 90
    };
    polygon.setPoints(7, points);
    mShape.addPolygon(polygon);

    mGear = new Gear();
    connect(mGear, SIGNAL(sig_equipmentSet()), this, SIGNAL(sig_equipmentChanged()));
    mBag = new Bag();
    connect(mBag, SIGNAL(sig_bagEvent()), this, SIGNAL(sig_bagEvent()));
    connect(mBag, SIGNAL(sig_bagFull()), this, SIGNAL(sig_bagFull()));
    mCoin = mHeroList[eWizard].coin;
    mExperience = Experience{0,1,10};
    mImage = QPixmap(":/heros/wizard_logo.png");
    mCurrentPixmap = QPixmap(":/heros/wizard.png");
    mNumberFrame = 4;
    mImageSelected = HERO_STAND;
    t_animation->start(150);
}

void Wizard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(0,0, mCurrentPixmap, static_cast<int>(mNextFrame*boundingRect().width()), static_cast<int>(mImageSelected*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

Wizard::~Wizard()
{
    if(mGear)
        delete mGear;
    if(mBag)
        delete mBag;
    for(int i=0; i<PassiveSkill::NbSkills; i++)
        delete mSkillList[i];
    for(int i=0; i<SpellSkill::NbSpells; i++)
        delete mSpellList[i];
}

int Wizard::getHeroClass()
{
    return mClass;
}

