#include "monster.h"

#include <QVector2D>
#include "monsterfightview.h"

int mSounds[6];
QTimer * t_isWalking;
int mSkin;

QList<Item*> mItems;

Monster::Monster(QGraphicsView * view):
    Character (),
    mView(view),
    mIsInView(true),
    mHover(0),
    mDamage(0),
    mThreatLevel(0),
    mExperience(0),
    mDescription(QString()),
    mSpeed(0),
    mAction(Action::stand),
    mFrames(FramesAvailable()),
    mMove(MovementHandler()),
    mPixmap(ImageHandler()),
    t_isWalking(nullptr),
    mSkin(0),
    mItems(QList<Item*>()),
    mFightView(nullptr)
{
    setZValue(Z_MONSTERS);

    mNextFrame = 0;
    mNumberFrame = 1;

    t_animation = new QTimer(this);
    connect(t_animation, SIGNAL(timeout()), this, SLOT(setNextFrame()));
    setAcceptHoverEvents(true);

    t_isWalking = new QTimer(this);
    t_isWalking->setSingleShot(true);

    connect(&t_fight, &QTimer::timeout, this, &Monster::onStaminaRecovery);

    mMana = Gauge{0,0};
    mStamina = Gauge{100,100};
}

void Monster::setAngle(int angle)
{
    if(angle > 90 && angle < 270)
    {
        QTransform matrice(1, 0, 0, -1, 0, mBoundingRect.height());
        setTransform(matrice);
    }
    else
    {
        setTransform(QTransform());
    }

    setRotation(static_cast<qreal>(ToolFunctions::correct(angle)));
    mMove.currentAngle = angle;
}

void Monster::setTargetAngle(int angle)
{
    mMove.targetAngle = angle;
    if(ToolFunctions::isOppositeDirection(mMove.currentAngle, mMove.targetAngle))
    {
        setAngle((mMove.currentAngle + 180) % 360);
    }
}

void Monster::setBoundingRect(QRectF bounding)
{
    mBoundingRect.setRect(bounding.x(), bounding.y(), bounding.width(), bounding.height());
    setTransformOriginPoint(boundingRect().center());
    mShape.addEllipse(0,0,boundingRect().width(), boundingRect().height());
}

bool Monster::isSkinned()
{
    if(mAction == Action::skinned)
        return true;
    else
        return false;
}

bool Monster::isDead()
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return true;
    else
        return false;
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

QString Monster::getDescription()
{
    return mDescription;
}

Monster::ImageHandler Monster::getImageHandler()
{
    return mPixmap;
}

IMonsterFightView * Monster::getFightView()
{
    return mFightView;
}

void Monster::setDamage(int damage)
{
    mDamage = damage;
}

QList<Item *> Monster::skinMonster()
{
    QList<Item*> itemList;
    while(!mItems.isEmpty())
        itemList.append(mItems.takeLast());

    setZValue(Z_MONSTER_BACKGROUND);
    mAction = Action::skinned;
    mCurrentPixmap = mPixmap.skinned;
    mNumberFrame = Monster::getNumberFrame();
    return itemList;
}

void Monster::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && mAction!=Action::skinned)
    {
       emit sig_showMonsterData(this);
       event->accept();
    }
    else
        event->ignore();
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
    painter->drawPixmap(0,0, mCurrentPixmap, static_cast<int>(mNextFrame*boundingRect().width()), static_cast<int>((2*(isSkinned() ? 0 : mSkin)+mHover)*boundingRect().height()), static_cast<int>(boundingRect().width()), static_cast<int>(boundingRect().height()));
    Q_UNUSED(widget)
    Q_UNUSED(option)

    // DEBUG - to comment

    // painter->setBrush(QBrush("#7700FF00"));
    // painter->drawPath(mShape);

    // if(mCollisionShape)
    // {
    //     painter->setBrush(QBrush("#77FF0000"));
    //     painter->drawPath(mCollisionShape->shape());
    // }

    // QRectF rect = boundingRect();
    // QPen pen(Qt::black, 2);
    // QBrush brush(Qt::black);

    // painter->setPen(pen);
    // painter->drawRect(rect);
    // painter->setBrush(brush);

    // QPointF p1(rect.left(), rect.center().y());
    // QPointF p2(rect.right(), rect.center().y());
    // QPointF arrowLeft(p2.x() - 10, p2.y() - 10);
    // QPointF arrowRight(p2.x() - 10, p2.y() + 10);
    // painter->drawLine(p1, p2);
    // QPolygonF arrowHead;
    // arrowHead << p2 << arrowLeft << arrowRight;
    // painter->drawPolygon(arrowHead);
    // QPointF p3(rect.center().x(), rect.bottom());
    // QPointF p4(rect.center().x(), rect.top());
    // QPointF arrowTopLeft(p4.x() - 10, p4.y() + 10);
    // QPointF arrowTopRight(p4.x() + 10, p4.y() + 10);

    // QPen pen2(Qt::red, 2);
    // QBrush brush2(Qt::red);
    // painter->setPen(pen2);
    // painter->setBrush(brush2);
    // painter->drawLine(p3, p4);
    // QPolygonF arrowHeadUp;
    // arrowHeadUp << p4 << arrowTopLeft << arrowTopRight;
    // painter->drawPolygon(arrowHeadUp);

    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void Monster::onStaminaRecovery()
{
#define HEAVY_ATTACK 0
#define LIGHT_ATTACK 1

    setStamina(mStamina.current + 5);
    if(mStamina.current == mStamina.maximum)
    {
        quint8 action = QRandomGenerator::global()->bounded(3);
        switch(action)
        {
            case HEAVY_ATTACK:
            case LIGHT_ATTACK:
                setStamina(action == HEAVY_ATTACK ? mStamina.current - 80 : mStamina.current - 50);
                if(isApplied(eStatus::confused))
                {
                    mStatus[eStatus::confused] = mStatus[eStatus::confused].toInt() - 1;
                    if(mStatus[eStatus::confused].toInt() < 1)
                        removeStatus(eStatus::confused);
                    
                    if(!QRandomGenerator().global()->bounded(3)) // 33% chance to waste the attack
                        return;
                }
                action == HEAVY_ATTACK ? emit sig_heavyAttack() : emit sig_lightAttack();
            break;

            default:
                setStamina(mStamina.current - 30);
                emit sig_monsterSound(getSoundIndexFor(ROAR));
            break;
        }
    }
}

void Monster::chooseAction(Hero * hero)
{
    int angle = ToolFunctions::getAngleBetween(hero, this);
    int distanceWithHero = ToolFunctions::getDistanceBeetween(hero, this);

    // Monster rush on Hero
    if(distanceWithHero < DISTANCE_AGGRO && ToolFunctions::isAllowedAngle(angle) && !hero->isInVillage())
    {
        t_isWalking->stop(); // Cut walking action to set running action
        
        mAction = Action::aggro;
        emit sig_monsterSound(getSoundIndexFor(AGGRO));

        setTargetAngle(angle);
        return;
    }

    if(t_isWalking->isActive())
        return; // Action in progress

    // Monster choose new action (walk, stand, ...)
    switch(QRandomGenerator::global()->bounded(3))
    {
        case 0: // 33%
            mAction = Action::moving;
            t_isWalking->start(QRandomGenerator::global()->bounded(MOVING_TIME_MIN, MOVING_TIME_MAX));
            if(distanceWithHero < DISTANCE_SOUND)
                emit sig_monsterSound(getSoundIndexFor(SOUND));

            angle = ToolFunctions::getRandomAngle();
            setTargetAngle(angle);
        break;

        default:// 66%
            mAction = Monster::Action::stand;
            setZValue(Z_MONSTERS);
        break;
    }
}

void Monster::advance(int phase)
{
    if(!phase)
        return;
    
    if(!mIsInView)
        return;

    // SetZValue if monster is standing
    if(mAction == Action::stand)
    {
        QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);
        for(QGraphicsItem * i : qAsConst(itemsColliding))
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
    int angle = (mMove.currentAngle + 90) % 360;
    int targetAngle = (mMove.targetAngle + 90) % 360;

    if(angle != targetAngle)
        angle < targetAngle ? angle ++ : angle--;

    setAngle((angle + 270) % 360);

    // Compute collision
    doCollision();

    // Start movement
    qreal dx = static_cast<qreal>(mSpeed) * qCos(qDegreesToRadians(static_cast<double>(mMove.currentAngle)));
    qreal dy = static_cast<qreal>(mSpeed) * qSin(qDegreesToRadians(static_cast<double>(mMove.currentAngle)));

    dx = (dx > 0) ? ceil(dx) : -ceil(abs(dx));
    dy = (dy > 0) ? ceil(dy) : -ceil(abs(dy));

    setPos(x() + dx, y() + dy);
}


void Monster::doCollision()
{
    int zOffset;
    QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);

    // Set Z value when monster interaction
    zOffset = itemsColliding.isEmpty() ? Z_MONSTERS : Z_GROUND_FOREGROUND ;
    for(QGraphicsItem * item : qAsConst(itemsColliding))
    {
        Monster * monster = dynamic_cast<Monster*>(item);
        if(monster)
        {
            if(pos().y()+boundingRect().height() > monster->y()+monster->boundingRect().height())
                zOffset = Z_MONSTER_FOREGROUND;
            else
                zOffset = Z_MONSTER_BACKGROUND;
        }
    }

    // Process map items which are not obstacles
    for(QGraphicsItem * item : qAsConst(itemsColliding))
    {
        MapItem * mapitem = dynamic_cast<MapItem*>(item);
        if(mapitem)
        {
            if(!mapitem->isObstacle())
            {
                // Update Z position
                if(y() + this->boundingRect().height() < mapitem->y() + mapitem->getZOffset())
                    zOffset = (zOffset < mapitem->zValue() - 1) ? mapitem->zValue() - 1 : zOffset ;
                else
                    zOffset = (zOffset < mapitem->zValue() + 1) ? mapitem->zValue() + 1 : zOffset ;

                Bush * bush = dynamic_cast<Bush*>(mapitem);
                if(bush)
                {
                    if(isInView())
                    {
                        if(!bush->isAnimated())
                            emit sig_movedInBush(bush);

                        continue;
                    }
                }
                BushEventCoin * bushEventCoin = dynamic_cast<BushEventCoin*>(mapitem);
                if(bushEventCoin)
                {
                    if(isInView())
                    {
                        if(!bushEventCoin->isAnimated())
                            emit sig_movedInBushEvent(bushEventCoin);

                        continue;
                    }
                }
                BushEventEquipment * bushEventEquipment = dynamic_cast<BushEventEquipment*>(mapitem);
                if(bushEventEquipment)
                {
                    if(isInView())
                    {
                        if(!bushEventEquipment->isAnimated())
                            emit sig_movedInBushEvent(bushEventEquipment);

                        continue;
                    }
                }
            }
        }
    }

    // Create obstacle list
    QList<QGraphicsItem*> obstacles;
    for(QGraphicsItem * item : qAsConst(itemsColliding))
    {
        MapItem * mapitem = dynamic_cast<MapItem*>(item);
        if(mapitem)
        {
            if(mapitem->isObstacle())
                obstacles.append(mapitem);
        }
        Village * village = dynamic_cast<Village*>(item);
        if(village)
        {
            obstacles.append(village);
        }
    }

    // Remove old obstacles
    for(Obstacle & obstacle : mMove.obstacles)
    {
        for(QGraphicsItem * currentObstacle : qAsConst(obstacles))
        {
            if(currentObstacle == obstacle.item)
            {
                obstacle.vanishing = VANISHING_COUNTERS; // Reset disparition counters
                continue;
            }
        }
        if(!obstacle.vanishing--)
            mMove.obstacles.removeOne(obstacle);
    }

    // Add new obstacles
    bool isNew;
    for(QGraphicsItem * currentObstacle : qAsConst(obstacles))
    {
        isNew = true;
        for(Obstacle & obstacle : mMove.obstacles)
        {
            if(currentObstacle == obstacle.item)
            {
                isNew = false;
                break;
            }
        }

        if(isNew)
        {
            Obstacle add;
            add.vanishing = VANISHING_COUNTERS;
            add.item = currentObstacle;
            mMove.obstacles.append(add);
        }
    }

    // Try to avoid collisions
    if(!mMove.obstacles.isEmpty())
    {
        int angle;
        QVector2D vectorSum(0, 0);
        for(Obstacle & obstacle : mMove.obstacles)
        {
            angle = ToolFunctions::getAngleBetween(this, obstacle.item);
            QVector2D vectorObstacle( qCos(qDegreesToRadians(static_cast<double>(angle))),
                                      qSin(qDegreesToRadians(static_cast<double>(angle))));
            vectorSum += vectorObstacle;
        }

        vectorSum.normalize();

        angle = static_cast<int>(qRadiansToDegrees(qAtan2(vectorSum.y(), vectorSum.x())));
        if(angle < 0)
            angle += 360;

        setTargetAngle(angle);
    }

    setZValue(zOffset);
    update();
}

bool Monster::isInView()
{
    QRectF area = ToolFunctions::getVisibleView(mView);
    QList<QGraphicsItem*> list = mView->scene()->items(QRectF(area.x(),area.y(),area.width(),area.height()));
    for(QGraphicsItem * item : qAsConst(list))
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
    for(QGraphicsItem * item : qAsConst(list))
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
    while(!mItems.isEmpty())
        delete mItems.takeLast();
    if(mFightView)
        delete mFightView;
}











Spider::Spider(QGraphicsView * view):
    Monster(view)
{
    mName = "Araignée";
    mLife = Gauge{100,100};
    mDamage = 30;
    mAction = Action::stand;
    mThreatLevel = 1;
    mImage = QPixmap(":/monsters/spider/spider_logo.png");
    mDescription = "Les araignées sont dangereuses mais aussi fragiles";
    mSkin = QRandomGenerator::global()->bounded(SPIDER_SKIN_NUM);

    setBoundingRect(QRectF(0,0,100,80));

    mFrames.run = 8;
    mFrames.dead = 1;
    mFrames.move = 8;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/spider/spider_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/spider/spider_lightAttack.png");
    mPixmap.walk = QPixmap(":/monsters/spider/spider_move.png");
    mPixmap.run = QPixmap(":/monsters/spider/spider_move.png");
    mPixmap.stand = QPixmap(":/monsters/spider/spider_stand.png");
    mPixmap.dead = QPixmap(":/monsters/spider/spider_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/spider/spider_skinned.png");

    mSounds[0] = SOUND_SPIDER_HEAVYATTACK;
    mSounds[1] = SOUND_SPIDER_LIGHTATTACK;
    mSounds[2] = SOUND_SPIDER_ROAR;
    mSounds[3] = SOUND_SPIDER_DIE;
    mSounds[4] = SOUND_SPIDER;
    mSounds[5] = SOUND_SPIDER_AGGRO;

    mFightView = new SpiderFightView();

    Spider::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
}

void Spider::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new PoisonPouch);
        mItems.append(new PoisonPouch);
    }
}

void Spider::nextAction(Hero * hero)
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

int Spider::getSpeed()
{
    return SPEED_SPIDER;
}

int Spider::getBoostedSpeed()
{
    return SPEEDBOOST_SPIDER;
}

void Spider::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new Mandibles);
    if(QRandomGenerator::global()->bounded(8) == 0)
        mItems.append(new PoisonPouch);
}

Spider::~Spider()
{

}










Wolf::Wolf(QGraphicsView * view):
    Monster(view)
{
    mName = "Loup";
    mLife = Gauge{600,600};
    mDamage = 15;
    mAction = Action::stand;
    mThreatLevel = 2;
    mImage = QPixmap(":/monsters/wolf/wolf_logo.png");
    mDescription = "Les loups sont des chasseurs agressifs chassant exclusivement en meute";

    setBoundingRect(QRectF(0,0,100,70));

    mFrames.run = 14;
    mFrames.dead = 1;
    mFrames.move = 13;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/wolf/wolf_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/wolf/wolf_lightAttack.png");
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

    mFightView = new WolfFightView();

    Wolf::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
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

void Wolf::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new WolfMeat);
    mItems.append(new WolfFang);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new WolfPelt);

    while(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new WolfFang);

}

Wolf::~Wolf()
{

}

WolfAlpha::WolfAlpha(QGraphicsView * view):
    Wolf(view)
{
    mName = "Loup Alpha";
    mLife = Gauge{1000,1000};
    mDamage = 18;
    mThreatLevel = 3;
    mImage = QPixmap(":/monsters/wolfAlpha/wolfAlpha_logo.png");
    mDescription = "Les loups alpha sont des meneurs par nature, s'ils ont pu se hisser à la tête de leur meute, c'est bien par leur férocité sans égale";

    setBoundingRect(QRectF(0,0,120,80));

    mPixmap.walk = QPixmap(":/monsters/wolfAlpha/wolfAlpha_move.png");
    mPixmap.run = QPixmap(":/monsters/wolfAlpha/wolfAlpha_run.png");
    mPixmap.stand = QPixmap(":/monsters/wolfAlpha/wolfAlpha_stand.png");
    mPixmap.dead = QPixmap(":/monsters/wolfAlpha/wolfAlpha_dead.png");
    mPixmap.skinned = QPixmap(":/monsters/wolfAlpha/wolfAlpha_skinned.png");

    mFightView = new WolfAlphaFightView();

    WolfAlpha::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
}

WolfAlpha::~WolfAlpha()
{

}

int WolfAlpha::getBoostedSpeed()
{
    return SPEEDBOOST_WOLF+1;
}

void WolfAlpha::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new WolfMeat);
    mItems.append(new WolfFang);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new WolfAlphaPelt);

    while(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new WolfFang);

}









Goblin::Goblin(QGraphicsView * view):
    Monster(view)
{
    mName = "Gobelin";
    mLife = Gauge{400,400};
    mDamage = 8;
    mAction = Action::stand;
    mThreatLevel = 1;
    mImage = QPixmap(":/monsters/goblin/goblin_logo.png");
    mDescription = "Le gobelin est un être incompris et sournois qui cherchera à vous faire du mal par tous les moyens";
    mSkin = QRandomGenerator::global()->bounded(GOBLIN_SKIN_NUM);

    setBoundingRect(QRectF(0,0,60,60));

    mFrames.run = 6;
    mFrames.dead = 1;
    mFrames.move = 6;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/ressources/goblin/goblin_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/goblin/goblin_lightAttack.png");
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

    mFightView = new GobelinFightView();

    Goblin::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
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

void Goblin::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new GoblinEar);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new GoblinEar);

    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new GoblinBones);

    if(QRandomGenerator::global()->bounded(10) == 0)
        mItems.append(new Amulet("Amulette\nde shaman",QPixmap(":/equipment/amulet_7.png"),8,2,5,8,"Amulette mystérieuse confectionnée par un gobelin."));

}

Goblin::~Goblin()
{

}











Bear::Bear(QGraphicsView * view):
    Monster(view)
{
    mName = "Ours";
    mLife = Gauge{1300,1300};
    mDamage = 20;
    mAction = Action::stand;
    mThreatLevel = 3;
    mImage = QPixmap(":/monsters/bear/bear_logo.png");
    mDescription = "L'ours est un prédator puissant est dangereux, il hiberne pendant la saison hivernale";
    mSkin = QRandomGenerator::global()->bounded(BEAR_SKIN_NUM);

    setBoundingRect(QRectF(0,0,300,200));

    mFrames.run = 12;
    mFrames.dead = 1;
    mFrames.move = 8;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/bear/bear_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/bear/bear_lightAttack.png");
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

    mFightView = new BearFightView();

    Bear::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
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

void Bear::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new BearMeat);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new BearPelt);

    while(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new BearClaw);

}


Bear::~Bear()
{

}










Troll::Troll(QGraphicsView * view):
    Monster(view)
{
    mName = "Troll";
    mLife = Gauge{800,800};
    mDamage = 14;
    mAction = Action::stand;
    mThreatLevel = 2;
    mImage = QPixmap(":/monsters/troll/troll_logo.png");
    mDescription = "Le troll est avare et cherche à dérober les biens des humains et ce par tous les moyens";
    mSkin = QRandomGenerator::global()->bounded(TROLL_SKIN_NUM);

    setBoundingRect(QRectF(0,0,100,100));

    mFrames.run = 7;
    mFrames.dead = 1;
    mFrames.move = 7;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/troll/troll_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/troll/troll_lightAttack.png");
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

    mFightView = new TrollFightView();

    Troll::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
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

void Troll::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new TrollMeat);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new TrollSkull);

    if(QRandomGenerator::global()->bounded(6) == 0)
        mItems.append(new Sword("Gourdin", QPixmap(":/equipment/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));

}


Troll::~Troll()
{

}













Oggre::Oggre(QGraphicsView * view):
    Monster(view)
{
    mName = "Ogre";
    mLife = Gauge{2500,2500};
    mDamage = 30;
    mAction = Action::stand;
    mThreatLevel = 4;
    mImage = QPixmap(":/monsters/oggre/oggre_logo.png");
    mDescription = "L'ogre est massif et terriblement dangereux, il vaut mieux ne pas croiser son chemin si l'on y est pas préparé";
    mSkin = QRandomGenerator::global()->bounded(OGGRE_SKIN_NUM);

    setBoundingRect(QRectF(0,0,300,300));

    mFrames.run = 8;
    mFrames.dead = 1;
    mFrames.move = 10;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/oggre/oggre_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/oggre/oggre_lightAttack.png");
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

    mFightView = new OggreFightView();

    Oggre::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
}

void Oggre::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
        mItems.append(new OggreSkull);
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

void Oggre::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new OggreSkull);

    if(QRandomGenerator::global()->bounded(3) == 0)
        mItems.append(new Sword("Gourdin", QPixmap(":/equipment/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));
}


Oggre::~Oggre()
{

}










LaoShanLung::LaoShanLung(QGraphicsView * view):
    Monster(view)
{
    mName = "Lao Shan Lung";
    mLife = Gauge{10000,10000};
    mDamage = 50;
    mAction = Action::stand;
    mThreatLevel = 10;
    mImage = QPixmap(":/monsters/laoshanlung/laoshanlung_logo.png");
    mDescription = "Créature mythique, le Lao Shun Lung est un dragon de terre colossale qui écume les plaines depuis des centaines d'années";

    setBoundingRect(QRectF(0,0,700,450));

    mFrames.run = 18;
    mFrames.dead = 1;
    mFrames.move = 18;
    mFrames.stand = 1;
    mFrames.skinned = 1;

    mPixmap.heavyAttack = QPixmap(":/monsters/laoshanlung/laoshanlung_heavyAttack.png");
    mPixmap.lightAttack = QPixmap(":/monsters/laoshanlung/laoshanlung_lightAttack.png");
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

    mFightView = new LaoShanLungFightView();

    LaoShanLung::generateRandomLoots();

    mCurrentPixmap = mPixmap.stand;
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

void LaoShanLung::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new LaoshanlungHeart);
}

void LaoShanLung::doCollision()
{
    Monster::doCollision();

    // MapItem destruction
    QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);
    for(QGraphicsItem * item : qAsConst(itemsColliding))
    {
        MapItem * mapItem = dynamic_cast<MapItem*>(item);
        if(mapItem)
        {
            if(mapItem->isDestroyed())
                continue;
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

LaoShanLung::~LaoShanLung()
{

}
