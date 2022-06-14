#include "monster.h"

Monster::Monster(QGraphicsView * view):
    Character (),
    mView(view),
    mIsInView(true),
    mHover(0),
    mStatus(none),
    mAction(Action::stand)
{
    setZValue(Z_MONSTERS);

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(setNextFrame()));
    setAcceptHoverEvents(true);

    t_isWalking = new QTimer(this);
    t_isWalking->setSingleShot(true);

    mNextFrame = 0;
    mNumberFrame = 1;

    mMove.posBeforeCollision = QPointF(-1,-1);
    mMove.collision << false << false;
    mMove.currentangleOnMap = 90;
    mMove.angleOnMap = 90;
}

void Monster::setAngle(qreal a)
{
    mMove.currentangleOnMap = static_cast<int>(a);
    if(mMove.currentangleOnMap < 180)
    {
        if(mMove.currentangleOnMap > 90)
            mMove.angleUseToMove = (180 - mMove.currentangleOnMap);
        else
            mMove.angleUseToMove = mMove.currentangleOnMap + 2*(90 - mMove.currentangleOnMap);

        QTransform matrice(1, 0, 0, -1, 0, mSize);
        setTransform(matrice);

    }else
    {
        mMove.angleUseToMove = mMove.currentangleOnMap;
        setTransform(QTransform());
    }
    setRotation(mMove.angleUseToMove);
}

void Monster::rotateSymetry(qreal oldAngle, qreal newAngle)
{
    if(!(static_cast<int>(oldAngle) < 180 && static_cast<int>(newAngle) < 180) && !(static_cast<int>(oldAngle) > 180 && static_cast<int>(newAngle) > 180))
    {
        mMove.currentangleOnMap = (static_cast<int>(oldAngle) + 180) % 360;
        setAngle(mMove.currentangleOnMap);
    }
}

bool Monster::isStatus(quint32 status)
{
    return ((mStatus & status) == status) ? true : false ;
}

bool Monster::isSkinned()
{
    if(mAction == Action::skinned)
        return true;
    else{
        return false;
    }
}

bool Monster::isDead()
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return true;
    else {
        return false;
    }
}

void Monster::killMonster()
{
    mAction = Action::dead;
    mCurrentPixmap = mPixmap.dead;
    mNumberFrame = getNumberFrame();
}

int Monster::getDamage()
{
    return mDamage;
}

quint32 Monster::getStatus()
{
    return mStatus;
}

qreal Monster::getAngle()
{
    return mMove.angleOnMap;
}

int Monster::getThreatLevel()
{
    return mThreatLevel;
}

Monster::Action Monster::getAction()
{
    return mAction;
}

int Monster::getExperience()
{
    return 5*mThreatLevel;
}

QPixmap Monster::getFightImage(int which)
{
    if(which == 0){
        return mPixmap.fightImage_1;
    }else{
        return mPixmap.fightImage_2;
    }
}

QPixmap Monster::getHeavyAttackAnimation()
{
    return mPixmap.heavyAttack;
}

QPixmap Monster::getLightAttackAnimation()
{
    return mPixmap.lightAttack;
}

int Monster::getSoundIndexFor(int index)
{
    return mSounds[index];
}

Monster::ImageHandler Monster::getImageHandler()
{
    return mPixmap;
}

void Monster::setDamage(int damage)
{
    mDamage = damage;
}

void Monster::addStatus(quint32 status)
{
    mStatus |= status;
}

void Monster::removeStatus(quint32 status)
{
    mStatus &= ~status;
}

void Monster::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && mAction!=Action::skinned)
    {
       emit sig_showMonsterData(this);
       event->accept();
    }else{
        event->ignore();
    }
}

void Monster::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverEnter == QGraphicsSceneHoverEvent::HoverEnter)
    {
        mHover = 1;
        update();
        event->accept();
    }
}

void Monster::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    if(event->HoverLeave == QGraphicsSceneHoverEvent::HoverLeave)
    {
        mHover = 0;
        update();
        event->accept();
    }
}


void Monster::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPixmap(0,0, mCurrentPixmap, static_cast<int>(mNextFrame*boundingRect().width()), static_cast<int>(mHover*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void Monster::chooseAction(Hero * hero)
{
    qreal angle = ToolFunctions::getAngleWithHero(hero, this);
    qreal distanceWithHero = ToolFunctions::heroDistanceWith(hero, this);
    if(distanceWithHero < DISTANCE_AGGRO && ToolFunctions::isAllowedAngle(angle) && !hero->isInVillage())
    {
        t_isWalking->stop();

        emit sig_monsterSound(getSoundIndexFor(AGGRO));

        // Set angle only if the monster is not colliding with the new angle
        qreal save = getAngle();
        mMove.angleOnMap = angle;

        mAction = Action::aggro;

        QList<QGraphicsItem*> collisionBefore = this->collidingItems();
        rotateSymetry(save, mMove.angleOnMap);

        if(static_cast<int>(save) != static_cast<int>(mMove.angleOnMap))
        {
            QList<QGraphicsItem*> collisionAfter = this->collidingItems();
            if(ToolFunctions::getNumberObstacles(collisionBefore) < ToolFunctions::getNumberObstacles(collisionAfter))
            {
                mMove.angleOnMap = save;
                setAngle(save);
            }
        }

    }else
    {
        if(t_isWalking->isActive())
            return;

        if(QRandomGenerator::global()->bounded(3) == 0)
        {
            // Set angle only if the monster is not colliding with the new angle
            qreal save = getAngle();
            mMove.angleOnMap = ToolFunctions::getRandomAngle();

            mAction = Action::moving;
            t_isWalking->start(QRandomGenerator::global()->bounded(MOVING_TIME_MIN, MOVING_TIME_MAX));

            if(distanceWithHero < DISTANCE_SOUND)
                emit sig_monsterSound(getSoundIndexFor(SOUND));

            QList<QGraphicsItem*> collisionBefore = this->collidingItems();
            rotateSymetry(save, mMove.angleOnMap);

            if(static_cast<int>(save) != static_cast<int>(mMove.angleOnMap))
            {
                QList<QGraphicsItem*> collisionAfter = this->collidingItems();
                if(ToolFunctions::getNumberObstacles(collisionBefore) < ToolFunctions::getNumberObstacles(collisionAfter))
                {
                    mMove.angleOnMap = save;
                    setAngle(save);
                }
            }

        }else
        {
            mAction = Monster::Action::stand;
            setZValue(Z_MONSTERS);
        }
    }
}

void Monster::advance(int phase)
{
    if(!phase)
        return;

    // SetZValue if monster is in action stand
    if(mAction == Action::stand && mIsInView)
    {
        QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);
        for(QGraphicsItem * i : itemsColliding)
        {
            Monster * monster = dynamic_cast<Monster*>(i);
            if(monster){
                if(pos().y()+boundingRect().height() > monster->y()+monster->boundingRect().height()){
                    setZValue(Z_MONSTER_FOREGROUND);
                }else {
                    setZValue(Z_MONSTER_BACKGROUND);
                }
            }
        }
        return;
    }

    if(mAction == Action::dead || mAction == Action::skinned || (mAction == Action::stand && !mIsInView))
        return;


    // Set new angle
    if(abs(mMove.currentangleOnMap - mMove.angleOnMap) <= 1)
    {
        mMove.currentangleOnMap = mMove.angleOnMap;

    }else
    {
        if(mMove.currentangleOnMap < mMove.angleOnMap)
            mMove.currentangleOnMap++;
        else
            mMove.currentangleOnMap--;
    }
    setAngle(mMove.currentangleOnMap);


    // Start movement
    qreal arcLength = qSqrt(2.0)/2.0*mSize;
    qreal dy = mSpeed*(qCos(qDegreesToRadians(static_cast<qreal>(static_cast<int>(mMove.angleUseToMove)%90))) + qSin(qDegreesToRadians(static_cast<qreal>(static_cast<int>(mMove.angleUseToMove)%90))));

    if(mMove.angleUseToMove <= 135){
        setPos(mapToParent( arcLength*(qSqrt(2.0)/2.0 + qSin(qDegreesToRadians(mMove.angleUseToMove-45.0))),
                           arcLength*(qSqrt(2.0)/2.0 + qCos(qDegreesToRadians(mMove.angleUseToMove-45.0))) - dy
                           ));
    }else{
        setPos(mapToParent(arcLength*(qSqrt(2.0)/2.0 - qSin(qDegreesToRadians(static_cast<qreal>(-mMove.angleUseToMove+135.0)))),
                           arcLength*(qSqrt(2.0)/2.0 + qCos(qDegreesToRadians(static_cast<qreal>(-mMove.angleUseToMove+135.0)))) - dy
                           ));
    }

    doCollision();
}


void Monster::doCollision()
{
    // Refresh data
    mMove.collision.first() = mMove.collision.last();
    mMove.collision.last() = false;
    bool collisionWithMonster = false;

    // Check for collision with specific items
    QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);
    for(QGraphicsItem * item : itemsColliding)
    {
        Village * village = dynamic_cast<Village*>(item);
        if(village){
            if(!mMove.collision.last()){
                mMove.collision.last() = true;
                // Specific collision with village
                mAction = Action::moving;
                mCurrentPixmap = mPixmap.walk;
                mSpeed = getSpeed();
                mNumberFrame = getNumberFrame();
                t_animation->stop();
                t_animation->start(200);
            }
            continue;
        }
        Monster * monster = dynamic_cast<Monster*>(item);
        if(monster){
            // Set Z value to establish depth
            collisionWithMonster = true;
            if(pos().y()+boundingRect().height() > monster->y()+monster->boundingRect().height())
                setZValue(Z_MONSTER_FOREGROUND);
            else
                setZValue(Z_MONSTER_BACKGROUND);

            continue;
        }
    }

    if(!collisionWithMonster)
        setZValue(Z_MONSTERS);

    if(mThreatLevel >= 10)
    {
        // MapItem destruction
        for(QGraphicsItem * item : itemsColliding)
        {
            MapItem * mapItem = dynamic_cast<MapItem*>(item);
            if(mapItem)
            {
                if(mapItem->isObstacle())
                {
                    Tree * tree = dynamic_cast<Tree*>(mapItem);
                    if(tree)
                    {
                        tree->destructIt();
                        continue;
                    }
                    Rock * rock = dynamic_cast<Rock*>(mapItem);
                    if(rock)
                    {
                        rock->destructIt();
                        continue;
                    }
                }
            }
        }
    }

    // Check for collision with items
    if(itemsColliding.length() != 0)
    {
        if(ToolFunctions::getNumberObstacles(itemsColliding) != 0)
            mMove.collision.last() = true;
    }

    // If collision detected - Avoid next collision
    if(!mMove.collision.last())
    {
        mMove.posBeforeCollision = pos();

    }else if(mMove.collision.first() == false && mMove.collision.last() == true)
    {
        setAngle((static_cast<int>(mMove.currentangleOnMap)+180)%360);
        mMove.angleOnMap = mMove.currentangleOnMap;
        if(mMove.posBeforeCollision != QPointF(-1,-1))
            setPos(mMove.posBeforeCollision);
    }

    // Process map items which are not obstacles
    for(QGraphicsItem * i : itemsColliding)
    {
        MapItem * item = dynamic_cast<MapItem*>(i);
        if(item){
            if(!item->isObstacle())
            {
                Bush * bush = dynamic_cast<Bush*>(item);
                BushEventCoin * bushEventCoin = dynamic_cast<BushEventCoin*>(item);
                BushEventEquipment * bushEventEquipment = dynamic_cast<BushEventEquipment*>(item);
                if(bush){
                    if(isInView()){
                        if(y()+boundingRect().height() < item->y()+item->boundingRect().height()*3/4){
                            item->setZValue(Z_MONSTER_FOREGROUND+1);
                        }else {
                            item->setZValue(Z_BUSH);
                        }
                        if(!bush->isAnimated()){
                            emit sig_movedInBush(bush);
                        }
                        continue;
                    }
                }else if(bushEventCoin){
                    if(isInView()){
                        if(y()+boundingRect().height() < item->y()+item->boundingRect().height()*3/4){
                            item->setZValue(Z_MONSTER_FOREGROUND+1);
                        }else {
                            item->setZValue(Z_BUSH);
                        }
                        if(!bushEventCoin->isAnimated()){
                            emit sig_movedInBushEvent(bushEventCoin);
                        }
                        continue;
                    }
                }else if(bushEventEquipment){
                    if(isInView()){
                        if(y()+boundingRect().height() < item->y()+item->boundingRect().height()*3/4){
                            item->setZValue(Z_MONSTER_FOREGROUND+1);
                        }else {
                            item->setZValue(Z_BUSH);
                        }
                        if(!bushEventEquipment->isAnimated()){
                            emit sig_movedInBushEvent(bushEventEquipment);
                        }
                        continue;
                    }
                }
            }
        }
    }
}

bool Monster::isInView()
{
    QRectF area = ToolFunctions::getVisibleView(mView);
    QList<QGraphicsItem*> list = mView->scene()->items(QRectF(area.x(),area.y(),area.width(),area.height()));
    for(QGraphicsItem * item : list)
    {
        if(dynamic_cast<Monster*>(item) == this)
        {
            return true;
        }
    }
    return false;
}


bool Monster::isInBiggerView()
{
    QRectF area = ToolFunctions::getBiggerView(mView);
    QList<QGraphicsItem*> list = mView->scene()->items(QRectF(area.x()-area.width()/2,area.y()-area.height()/2,area.width()*2,area.height()*2));
    for(QGraphicsItem * item : list)
    {
        if(dynamic_cast<Monster*>(item) == this)
        {
            mIsInView = true;
            return true;
        }
    }
    mIsInView = false;
    return false;
}

void Monster::enableMonsterAnimation(bool toggle)
{
    if(toggle){
        t_animation->start();
    }else{
        t_animation->stop();
    }
}

int Monster::getNumberFrame()
{
    switch(mAction)
    {
    case Action::moving :
        return mFrames.move;
    case Action::aggro :
        return mFrames.run;
    case Action::stand :
        return mFrames.stand;
    case Action::dead :
        return mFrames.dead;
    case Action::skinned :
        return mFrames.stand;
    default:
        return 1;
    }
}

Monster::~Monster()
{

}












Wolf::Wolf(QGraphicsView * view):
    Monster(view)
{
    mName = "Loup";
    mLife = Life{600,600};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 15;
    mAction = Action::stand;
    mThreatLevel = 2;
    mImage = QPixmap(":/monsters/wolf/wolf_logo.png");
    mSpeed = getSpeed();

    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(25,0,boundingRect().width()-50, boundingRect().height());

    mFrames.run = 14;
    mFrames.dead = 1;
    mFrames.move = 13;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/wolf/wolf_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/wolf/wolf_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/wolf/wolf_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/wolf/wolf_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/wolf/wolf_move.png");
    mPixmap.run = QPixmap(":/monsters/wolf/wolf_run.png");
    mPixmap.stand = QPixmap(":/monsters/wolf/wolf_stand.png");
    mPixmap.dead = QPixmap(":/monsters/wolf/wolf_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/wolf/wolf_skinned.png");

    mSounds[0] = SOUND_WOLF_HEAVYATTACK;
    mSounds[1] = SOUND_WOLF_LIGHTATTACK;
    mSounds[2] = SOUND_WOLF_ROAR;
    mSounds[3] = SOUND_WOLF_DIE;
    mSounds[4] = SOUND_WOLF;
    mSounds[5] = SOUND_WOLF_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> Wolf::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void Wolf::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new WolfPelt);
        mItems.append(new WolfMeat);
        mItems.append(new WolfFang);
    }
}

void Wolf::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }

    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(150);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(60);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int Wolf::getSpeed()
{
    return SPEED_WOLF;
}

int Wolf::getBoostedSpeed()
{
    return SPEEDBOOST_WOLF;
}

QList<Item *> Wolf::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new WolfMeat);
    loots.append(new WolfFang);
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new WolfPelt);
    }
    while(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new WolfFang);
    }
    return loots;
}

Wolf::~Wolf()
{

}

WolfAlpha::WolfAlpha(QGraphicsView * view):
    Wolf(view)
{
    mName = "Loup Alpha";
    mLife = Life{1000,1000};
    mDamage = 18;
    mThreatLevel = 3;
    mImage = QPixmap(":/monsters/wolf/wolfAlpha_logo.png");

    QPainterPath path;
    mBoundingRect = QRect(0,0,120,120);
    path.addEllipse(25,0,boundingRect().width()-50, boundingRect().height());
    mShape = path;

    mPixmap.fightImage_1 = QPixmap(":/monsters/wolf/wolfAlpha_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/wolf/wolfAlpha_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/wolf/wolfAlpha_move.png");
    mPixmap.run = QPixmap(":/monsters/wolf/wolfAlpha_run.png");
    mPixmap.stand = QPixmap(":/monsters/wolf/wolfAlpha_stand.png");
    mPixmap.dead = QPixmap(":/monsters/wolf/wolfAlpha_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/wolf/wolfAlpha_skinned.png");

    mCurrentPixmap = mPixmap.stand;

    while(!mItems.isEmpty())
        delete mItems.takeLast();
    generateRandomLoots();

    update();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

WolfAlpha::~WolfAlpha()
{

}

int WolfAlpha::getBoostedSpeed()
{
    return SPEEDBOOST_WOLF+1;
}

QList<Item *> WolfAlpha::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new WolfMeat);
    loots.append(new WolfFang);
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new WolfAlphaPelt);
    }
    while(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new WolfFang);
    }
    return loots;
}









Goblin::Goblin(QGraphicsView * view):
    Monster(view)
{
    mName = "Gobelin";
    mLife = Life{400,400};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 8;
    mAction = Action::stand;
    mThreatLevel = 1;
    mImage = QPixmap(":/monsters/goblin/goblin_logo.png");

    mBoundingRect = QRect(0,0,60,60);
    mShape.addEllipse(0,0,boundingRect().width(), boundingRect().height());

    mFrames.run = 6;
    mFrames.dead = 1;
    mFrames.move = 6;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/ressources/goblin/goblin_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/goblin/goblin_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/goblin/goblin_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/goblin/goblin_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/goblin/goblin_move.png");
    mPixmap.run = QPixmap(":/monsters/goblin/goblin_run.png");
    mPixmap.stand = QPixmap(":/monsters/goblin/goblin_stand.png");
    mPixmap.dead = QPixmap(":/monsters/goblin/goblin_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/goblin/goblin_skinned.png");

    mSounds[0] = SOUND_GOBLIN_HEAVYATTACK;
    mSounds[1] = SOUND_GOBLIN_LIGHTATTACK;
    mSounds[2] = SOUND_GOBLIN_ROAR;
    mSounds[3] = SOUND_GOBLIN_DIE;
    mSounds[4] = SOUND_GOBLIN;
    mSounds[5] = SOUND_GOBLIN_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> Goblin::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void Goblin::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new GoblinEar);
        mItems.append(new GoblinBones);
    }
}

void Goblin::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }

    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(160);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(80);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int Goblin::getSpeed()
{
    return SPEED_GOBLIN;
}

int Goblin::getBoostedSpeed()
{
    return SPEEDBOOST_GOBLIN;
}

QList<Item *> Goblin::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new GoblinEar);
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new GoblinEar);
    }
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new GoblinBones);
    }
    if(QRandomGenerator::global()->bounded(10) == 0){
        loots.append(new Amulet("Amulette\nde shaman",QPixmap(":/equipment/amulet_7.png"),8,2,5,8,"Amulette mystérieuse confectionnée par un gobelin."));
    }
    return loots;
}

Goblin::~Goblin()
{

}











Bear::Bear(QGraphicsView * view):
    Monster(view)
{
    mName = "Ours";
    mLife = Life{1300,1300};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 20;
    mAction = Action::stand;
    mThreatLevel = 3;
    mImage = QPixmap(":/monsters/bear/bear_logo.png");
    mSpeed = getSpeed();

    mBoundingRect = QRect(0,0,200,200);
    mShape.addEllipse(40,0,boundingRect().width()-80, boundingRect().height());

    mFrames.run = 12;
    mFrames.dead = 1;
    mFrames.move = 8;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/bear/bear_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/bear/bear_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/bear/bear_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/bear/bear_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/bear/bear_move.png");
    mPixmap.run = QPixmap(":/monsters/bear/bear_run.png");
    mPixmap.stand = QPixmap(":/monsters/bear/bear_stand.png");
    mPixmap.dead = QPixmap(":/monsters/bear/bear_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/bear/bear_skinned.png");

    mSounds[0] = SOUND_BEAR_HEAVYATTACK;
    mSounds[1] = SOUND_BEAR_LIGHTATTACK;
    mSounds[2] = SOUND_BEAR_ROAR;
    mSounds[3] = SOUND_BEAR_DIE;
    mSounds[4] = SOUND_BEAR;
    mSounds[5] = SOUND_BEAR_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> Bear::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void Bear::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new BearClaw);
        mItems.append(new BearMeat);
        mItems.append(new BearPelt);
    }
}

void Bear::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }

    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(150);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(50);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int Bear::getSpeed()
{
    return SPEED_BEAR;
}

int Bear::getBoostedSpeed()
{
    return SPEEDBOOST_BEAR;
}

QList<Item *> Bear::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new BearMeat);
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new BearPelt);
    }
    while(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new BearClaw);
    }
    return loots;
}


Bear::~Bear()
{

}










Troll::Troll(QGraphicsView * view):
    Monster(view)
{
    mName = "Troll";
    mLife = Life{800,800};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 14;
    mAction = Action::stand;
    mThreatLevel = 2;
    mImage = QPixmap(":/monsters/troll/troll_logo.png");
    mSpeed = getSpeed();

    mBoundingRect = QRect(0,0,100,100);
    mShape.addEllipse(0,0,boundingRect().width()-25, boundingRect().height());

    mFrames.run = 7;
    mFrames.dead = 1;
    mFrames.move = 7;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/troll/troll_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/troll/troll_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/troll/troll_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/troll/troll_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/troll/troll_move.png");
    mPixmap.run = QPixmap(":/monsters/troll/troll_run.png");
    mPixmap.stand = QPixmap(":/monsters/troll/troll_stand.png");
    mPixmap.dead = QPixmap(":/monsters/troll/troll_die.png");
    mPixmap.skinned = QPixmap(":/monsters/troll/troll_skinned.png");

    mSounds[0] = SOUND_TROLL_HEAVYATTACK;
    mSounds[1] = SOUND_TROLL_LIGHTATTACK;
    mSounds[2] = SOUND_TROLL_ROAR;
    mSounds[3] = SOUND_TROLL_DIE;
    mSounds[4] = SOUND_TROLL;
    mSounds[5] = SOUND_TROLL_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> Troll::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void Troll::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new TrollMeat);
        mItems.append(new TrollSkull);
    }
}

void Troll::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }


    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(150);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(100);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int Troll::getSpeed()
{
    return SPEED_TROLL;
}

int Troll::getBoostedSpeed()
{
    return SPEEDBOOST_TROLL;
}

QList<Item *> Troll::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new TrollMeat);
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new TrollSkull);
    }
    if(QRandomGenerator::global()->bounded(6) == 0){
        loots.append(new Sword("Gourdin", QPixmap(":/equipment/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));
    }
    return loots;
}


Troll::~Troll()
{

}













Oggre::Oggre(QGraphicsView * view):
    Monster(view)
{
    mName = "Ogre";
    mLife = Life{2500,2500};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 30;
    mAction = Action::stand;
    mThreatLevel = 4;
    mImage = QPixmap(":/monsters/oggre/oggre_logo.png");
    mSpeed = getSpeed();

    mBoundingRect = QRect(0,0,350,350);
    mShape.addEllipse(QRect(100, 50, static_cast<int>(boundingRect().width()-200), static_cast<int>(boundingRect().height()-100)));

    mFrames.run = 8;
    mFrames.dead = 1;
    mFrames.move = 10;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/oggre/oggre_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/oggre/oggre_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/oggre/oggre_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/oggre/oggre_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/oggre/oggre_move.png");
    mPixmap.run = QPixmap(":/monsters/oggre/oggre_run.png");
    mPixmap.stand = QPixmap(":/monsters/oggre/oggre_stand.png");
    mPixmap.dead = QPixmap(":/monsters/oggre/oggre_die.png");
    mPixmap.skinned = QPixmap(":/monsters/oggre/oggre_skinned.png");

    mSounds[0] = SOUND_OGGRE_HEAVYATTACK;
    mSounds[1] = SOUND_OGGRE_LIGHTATTACK;
    mSounds[2] = SOUND_OGGRE_ROAR;
    mSounds[3] = SOUND_OGGRE_DIE;
    mSounds[4] = SOUND_OGGRE;
    mSounds[5] = SOUND_OGGRE_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> Oggre::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void Oggre::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new OggreSkull);
    }
}

void Oggre::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }

    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(150);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(110);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int Oggre::getSpeed()
{
    return SPEED_OGGRE;
}

int Oggre::getBoostedSpeed()
{
    return SPEEDBOOST_OGGRE;
}

QList<Item *> Oggre::generateRandomLoots()
{
    QList<Item*> loots;
    if(QRandomGenerator::global()->bounded(2) == 0){
        loots.append(new OggreSkull);
    }
    if(QRandomGenerator::global()->bounded(3) == 0){
        loots.append(new Sword("Gourdin", QPixmap(":/equipment/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));
    }
    return loots;
}


Oggre::~Oggre()
{

}










LaoShanLung::LaoShanLung(QGraphicsView * view):
    Monster(view)
{
    mName = "Lao Shan Lung";
    mLife = Life{10000,10000};
    mMana = Mana{0,0};
    mStamina = Stamina{100,100};
    mDamage = 50;
    mAction = Action::stand;
    mThreatLevel = 10;
    mImage = QPixmap(":/monsters/laoshanlung/laoshanlung_logo.png");
    mSpeed = getSpeed();

    mBoundingRect = QRect(0,0,700,700);
    mShape.addEllipse(QRect(250, 100, static_cast<int>(boundingRect().width())-500, static_cast<int>(boundingRect().height())-200));

    mFrames.run = 18;
    mFrames.dead = 1;
    mFrames.move = 18;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/laoshanlung/laoshanlung_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/laoshanlung/laoshanlung_lightAttack.png");
    mPixmap.fightImage_1 = QPixmap(":/monsters/laoshanlung/laoshanlung_fight.png");
    mPixmap.fightImage_2 = QPixmap(":/monsters/laoshanlung/laoshanlung_fight_attacked.png");
    mPixmap.walk = QPixmap(":/monsters/laoshanlung/laoshanlung_move.png");
    mPixmap.run = mPixmap.walk;
    mPixmap.stand = QPixmap(":/monsters/laoshanlung/laoshanlung_stand.png");
    mPixmap.dead = QPixmap(":/monsters/laoshanlung/laoshanlung_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/laoshanlung/laoshanlung_skinned.png");

    mSounds[0] = SOUND_LAOSHANLUNG_HEAVYATTACK;
    mSounds[1] = SOUND_LAOSHANLUNG_LIGHTATTACK;
    mSounds[2] = SOUND_LAOSHANLUNG_ROAR;
    mSounds[3] = SOUND_LAOSHANLUNG_DIE;
    mSounds[4] = SOUND_LAOSHANLUNG;
    mSounds[5] = SOUND_LAOSHANLUNG_AGGRO;

    mCurrentPixmap = mPixmap.stand;

    mItems = generateRandomLoots();

    mSize = boundingRect().height();
    setTransformOriginPoint(boundingRect().center());
    setAngle(90);
}

QList<Item *> LaoShanLung::skinMonster()
{
    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return mItems;
}

void LaoShanLung::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new LaoshanlungHeart);
    }
}

void LaoShanLung::nextAction(Hero * hero)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView()){
        if(mAction!=Action::stand){
            mAction = Action::stand;
            mCurrentPixmap = mPixmap.stand;
            t_animation->stop();
            mNextFrame = 0;
            mNumberFrame = Monster::getNumberFrame();
        }
        return;
    }

    Monster::chooseAction(hero);

    // Action behaviour
    switch(mAction)
    {
    case Action::moving:
        mCurrentPixmap = mPixmap.walk;
        mSpeed = getSpeed();
        t_animation->stop();
        t_animation->start(150);
        break;
    case Action::aggro:
        mCurrentPixmap = mPixmap.run;
        mSpeed = getBoostedSpeed();
        t_animation->stop();
        t_animation->start(80);
        break;
    case Action::stand:
        mCurrentPixmap = mPixmap.stand;
        t_animation->stop();
        break;
    default:
        break;
    }
    mNextFrame = 0;
    mNumberFrame = Monster::getNumberFrame();
    update();
}

int LaoShanLung::getSpeed()
{
    return SPEED_LAOSHANLUNG;
}

int LaoShanLung::getBoostedSpeed()
{
    return SPEEDBOOST_LAOSHANLUNG;
}

QList<Item *> LaoShanLung::generateRandomLoots()
{
    QList<Item*> loots;
    loots.append(new LaoshanlungHeart);
    return loots;
}

LaoShanLung::~LaoShanLung()
{

}
