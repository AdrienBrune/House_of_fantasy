#include "hero.h"
#include "monster.h"

bool gEnableMovementWithMouseClic = false;

Hero::Hero():
    Character (),
    mBag(nullptr),
    mGear(nullptr),
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

    mAdventurerMap.unlocked = false;
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
    mMoveHandler.movementMask = 0;
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
    if(mLife.current <= 0){
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

void Hero::checkMapInteractions()
{
    int zOffset;
    bool mapEventFound = false;
    QList<QGraphicsItem*> list;

    list = mCollisionShape->collidingItems();
    for(QGraphicsItem * item : qAsConst(list))
    {
        CollisionShape * obstacle = dynamic_cast<CollisionShape*>(item);
        if(obstacle)
        {
            setPos(mMoveHandler.lastPos);
            stopMoving();
            break;
        }
    }

    list = collidingItems();
    zOffset = list.isEmpty() ? Z_HERO : Z_GROUND_FOREGROUND ;
    for(QGraphicsItem * item : qAsConst(list))
    {
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
            zOffset = Z_HERO;
        }

        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem)
        {
            Bush * bush = dynamic_cast<Bush*>(item);
            if(bush)
            {
                if(!bush->isAnimated())
                {
                    emit sig_movedInBush(bush);
                }
            }
            BushEventCoin * bushCoinEvent = dynamic_cast<BushEventCoin*>(item);
            if(bushCoinEvent)
            {
                if(!bushCoinEvent->isAnimated())
                {
                    emit sig_movedInBushEvent(bushCoinEvent);
                }
            }
            BushEventEquipment * bushEquipmentEvent = dynamic_cast<BushEventEquipment*>(item);
            if(bushEquipmentEvent)
            {
                if(!bushEquipmentEvent->isAnimated())
                {
                    emit sig_movedInBushEvent(bushEquipmentEvent);
                }
            }

            // Check map event
            MapEvent* mapEvent = dynamic_cast<MapEvent*>(mapItem);
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

            // Update Z position
            if(y() + this->boundingRect().height() < mapItem->y() + mapItem->getZOffset())
                zOffset = (zOffset < mapItem->zValue() - 1) ? mapItem->zValue() - 1 : zOffset ;
            else
                zOffset = (zOffset < mapItem->zValue() + 1) ? mapItem->zValue() + 1 : zOffset ;
        }
    }

    setZValue(zOffset);
    update();

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
    double dx = static_cast<double>(SPEED_HERO)*qSin(qDegreesToRadians(mMoveHandler.angle));
    double dy = -static_cast<double>(SPEED_HERO)*qCos(qDegreesToRadians(mMoveHandler.angle));
    setPos(x()+dx, y()+dy);

    if(gEnableMovementWithMouseClic)
        if( (abs(mMoveHandler.destPos.x() - x()) < 1)  || (abs(mMoveHandler.destPos.y() - y()) < 1) )
        {
            stopMoving(); // Not working with keys
        }

    emit sig_heroMoved();

    checkMapInteractions();

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
        setLife(getLife().current+potionLife->getCapacity());
    }
    PotionMana * potionMana = dynamic_cast<PotionMana*>(item);
    if(potionMana)
    {
        setMana(getMana().current+potionMana->getCapacity());
    }
    PotionStamina * potionStamina = dynamic_cast<PotionStamina*>(item);
    if(potionStamina)
    {
        setStaminaMax(getStamina().maximum+potionStamina->getCapacity());
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
        setLife(getLife().current+redFish->getCapacity());
    }
    CommonFish * fish = dynamic_cast<CommonFish*>(item);
    if(fish)
    {
        setLife(getLife().current+fish->getCapacity());
    }
    Yellowfish * yellowFish = dynamic_cast<Yellowfish*>(item);
    if(yellowFish)
    {
        setLife(getLife().current+yellowFish->getCapacity());    }
    BlueFish * blueFish = dynamic_cast<BlueFish*>(item);
    if(blueFish)
    {
        setMana(getMana().current+blueFish->getCapacity());
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
    float innateRobustnessCoef = 1.0;

    // Benediction : if counters remain, not damages applied
    if(isApplied(eStatus::benediction))
    {
        if(mStatus[eStatus::benediction].toInt() > 0)
        {
            mStatus[eStatus::benediction] = mStatus[eStatus::benediction].toInt() - 1;
            return;
        }
        else
            removeStatus(eStatus::benediction);
    }

    // Innate Robustness : -15% damage reduction if skill obtained
    if(getSkillList()[PassiveSkill::InnateRobustness]->isUnlock())
        innateRobustnessCoef = 0.85; // -15% dmg

    // Primitive Shield : -20% damage reduction if counter remains
    if(isApplied(eStatus::shield))
    {
        if(mStatus[eStatus::shield].toInt() > 0)
        {
            mStatus[eStatus::shield] = mStatus[eStatus::shield].toInt() - 1;
            innateRobustnessCoef *= 0.8; // -20% dmg
        }
        else
            removeStatus(eStatus::shield);
    }

    damage *= innateRobustnessCoef;

    if(QRandomGenerator::global()->bounded(100) > mGear->dodgingStat())
    {
        setLife(getLife().current - static_cast<int>((1.0 - static_cast<qreal>(mGear->defenseStat())/HERO_DEFENSE_MAX)*static_cast<qreal>(damage)));
    }
    else
        emit sig_dodge();
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
        case life:
            setLifeMax(mLife.maximum + mHeroList[mClass].steps.life);
            setLife(mLife.current + mHeroList[mClass].steps.life);
            break;

        case mana:
            setManaMax(mMana.maximum + mHeroList[mClass].steps.mana);
            setMana(mMana.current + mHeroList[mClass].steps.mana);
            break;

        case stamina:
            setStaminaMax(mStamina.maximum + mHeroList[mClass].steps.stamina);
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
    mShape.addRect(QRect(mBoundingRect.width()/4, 0, mBoundingRect.width()/2, mBoundingRect.height()));
    QPainterPath collidingShape;
    collidingShape.addRect(QRect(mBoundingRect.width()/3, mBoundingRect.height() -10, mBoundingRect.width()/3, 5));
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

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
    mShape.addRect(QRect(mBoundingRect.width()/4, 0, mBoundingRect.width()/2, mBoundingRect.height()));
    QPainterPath collidingShape;
    collidingShape.addRect(QRect(mBoundingRect.width()/3, mBoundingRect.height() -10, mBoundingRect.width()/3, 5));
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

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
    mShape.addRect(QRect(mBoundingRect.width()/4, 0, mBoundingRect.width()/2, mBoundingRect.height()));
    QPainterPath collidingShape;
    collidingShape.addRect(QRect(mBoundingRect.width()/3, mBoundingRect.height() -10, mBoundingRect.width()/3, 5));
    mCollisionShape = new CollisionShape(this, mBoundingRect, collidingShape);

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

