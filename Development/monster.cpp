#include "monster.h"

#include "monsterfightview.h"

Monster::Monster(QGraphicsView * view):
    Character (),
    mView(view),
    mIsInView(false),
    mHover(0),
    mHoverCacheFrame(-1),
    mDamage(0),
    mThreatLevel(0),
    mDescription(QString()),
    mAction(Action::stand),
    mMove(MovementHandler()),
    mSprites(SpriteHandler()),
    mCurrentSprite(nullptr),
    mSkin(0),
    mItems(QList<Item*>()),
    mFightView(nullptr)
{
    setZValue(Z_MONSTERS);

    mCurrentSprite = &mSprites.stand;

    connect(&t_movement, &QTimer::timeout, this, &Monster::onNextFrame);
    setAcceptHoverEvents(true);

    t_isWalking.setSingleShot(true);
    t_isRunning.setSingleShot(true);

    connect(&t_fight, &QTimer::timeout, this, &Monster::onStaminaRecovery);

    mMana = Gauge{0,0};
    mStamina = Gauge{100,100};
    mCoin = 0;

    setAction(Action::stand);
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
}

void Monster::applyZigzagStep()
{
    constexpr float ZIGZAG_OFFSET = 45.0f;
    float sideAngleDeg;

    if(qAbs(mMove.netDirection.y()) > qAbs(mMove.netDirection.x()))
    {
        // Direction verticale : alterne entre côté DROIT et côté GAUCHE
        // Les deux angles résultants restent toujours à ±45° de l'horizontal
        // vertSign > 0 = vers le bas, < 0 = vers le haut
        float vertSign = (mMove.netDirection.y() >= 0.0f) ? 1.0f : -1.0f;

        if(!mMove.zigzagSide)
            sideAngleDeg = vertSign * ZIGZAG_OFFSET;              // ex: +45° (bas-droite) ou -45° (haut-droite)
        else
            sideAngleDeg = 180.0f - vertSign * ZIGZAG_OFFSET;     // ex: 135° (bas-gauche) ou 225° (haut-gauche)
    }
    else
    {
        // Direction horizontale : ligne droite, pas de zigzag
        sideAngleDeg = qRadiansToDegrees(qAtan2(mMove.netDirection.y(), mMove.netDirection.x()));
    }

    float rad = qDegreesToRadians(sideAngleDeg);
    mMove.stepDirection = QVector2D(qCos(rad), qSin(rad));

    int visualAngle = (static_cast<int>(sideAngleDeg) % 360 + 360) % 360;
    setAngle(visualAngle);
}

void Monster::setBoundingRect(QRectF bounding)
{
    mBoundingRect.setRect(bounding.x(), bounding.y(), bounding.width(), bounding.height());
    setTransformOriginPoint(boundingRect().center());
    mShape = QPainterPath();
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
    setAction(Action::dead);
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

    setAction(Action::skinned);
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
    Q_UNUSED(widget)
    Q_UNUSED(option)

    if (!mCurrentSprite)
        return;

    QSize fs = mCurrentSprite->grid.frameSize;
    int col = mCurrentSprite->index % mCurrentSprite->grid.col;
    int row = mCurrentSprite->index / mCurrentSprite->grid.col;
    const QRect spriteRect(col * fs.width(), row * fs.height(), fs.width(), fs.height());

    if(mHover)
    {
        // Rebuild silhouette only when the animation frame changes
        if(mCurrentSprite->index != mHoverCacheFrame || mHoverSilhouette.isNull())
        {
            const QSize sz = boundingRect().size().toSize();
            mHoverSilhouette = QPixmap(sz);
            mHoverSilhouette.fill(Qt::transparent);
            QPainter sp(&mHoverSilhouette);
            sp.drawPixmap(mHoverSilhouette.rect(), mCurrentSprite->image, spriteRect);
            sp.setCompositionMode(QPainter::CompositionMode_SourceIn);
            sp.fillRect(mHoverSilhouette.rect(), QColor(30, 30, 30, 70));
            mHoverCacheFrame = mCurrentSprite->index;
        }

        // Draw silhouette shifted in 8 directions → filled red contour
        const int N = 2;
        for(int dx = -N; dx <= N; dx += N)
            for(int dy = -N; dy <= N; dy += N)
                if(dx || dy)
                    painter->drawPixmap(boundingRect().translated(dx, dy).toRect(),
                                        mHoverSilhouette);
    }

    painter->drawPixmap(
        boundingRect().toRect(),
        mCurrentSprite->image,
        spriteRect
    );


    // /// DEBUG
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

    // // Collision avoidance vector
    // if (!mCollisionVector.isNull())
    // {
    //     QPointF center = boundingRect().center();
    //     // Convert scene-space direction to item-space (handles rotation + flip)
    //     QPointF sceneOrigin = mapToScene(center);
    //     QPointF sceneTip = sceneOrigin + QPointF(mCollisionVector.x(), mCollisionVector.y()) * 60.0f;
    //     QPointF localTip = mapFromScene(sceneTip);
    //     QPen vectorPen(QColor(255, 140, 0), 3);
    //     painter->setPen(vectorPen);
    //     painter->setBrush(QColor(255, 140, 0));
    //     painter->drawLine(center, localTip);
    //     // Arrowhead
    //     double arrowAngle = qAtan2(localTip.y() - center.y(), localTip.x() - center.x());
    //     QPointF arrowP1 = localTip - QPointF(qCos(arrowAngle + M_PI / 6) * 10, qSin(arrowAngle + M_PI / 6) * 10);
    //     QPointF arrowP2 = localTip - QPointF(qCos(arrowAngle - M_PI / 6) * 10, qSin(arrowAngle - M_PI / 6) * 10);
    //     QPolygonF arrowTip;
    //     arrowTip << localTip << arrowP1 << arrowP2;
    //     painter->drawPolygon(arrowTip);
    // }
    // /// END DEBUG
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

void Monster::onNextFrame()
{
    if (!mCurrentSprite)
        return;

    mCurrentSprite->incrementIndex();
    update();
}

void Monster::nextAction(Hero * hero, DayNightCycle * daynightCycle)
{
    if(mAction == Action::dead || mAction == Action::skinned)
        return;

    if(!isInBiggerView())
    {
        if(mAction != Action::stand)
        {
            setAction(Action::stand);
        }
        return;
    }

    chooseAction(hero, daynightCycle);
}

void Monster::setAction(Action action)
{
    // if (action == mAction)
    //     return;

    t_movement.stop();
    switch(action)
    {
        case Monster::Action::moving:
            mCurrentSprite = &mSprites.walk;
            break;
        case Monster::Action::aggro:
            mCurrentSprite = &mSprites.run;
            break;
        case Monster::Action::stand:
            mCurrentSprite = &mSprites.stand;
            break;
        case Monster::Action::dead:
            mCurrentSprite = &mSprites.dead;
            break;
        case Monster::Action::skinned:
            mCurrentSprite = &mSprites.skinned;
            break;
        default:
            break;
    }

    if (action == Monster::Action::moving || action == Monster::Action::aggro)
    {
        t_movement.start(mCurrentSprite->timerElapseMs);
    }

    mAction = action;
    mCurrentSprite->index = 0;
    update();
}

void Monster::chooseAction(Hero * hero, DayNightCycle * daynightCycle)
{
    int angle = ToolFunctions::getAngleBetween(hero, this);
    int distanceWithHero = ToolFunctions::getDistanceBeetween(hero, this);

    if(t_isRunning.isActive())
    {
        emit sig_monsterSound(getSoundIndexFor(AGGRO));
        if(mMove.obstacles.isEmpty())
        {
            float rad = qDegreesToRadians(static_cast<float>(angle));
            mMove.netDirection = QVector2D(qCos(rad), qSin(rad));
            applyZigzagStep();
        }
        return;
    }

    if(distanceWithHero < DISTANCE_AGGRO && !hero->isInVillage())
    {
        bool aggro = false;
        switch(daynightCycle->getDayTime())
        {
            case DayNightCycle::eDayTime::noon:
                if(QRandomGenerator::global()->bounded(100) < 5)
                    aggro = true;
                break;
            case DayNightCycle::eDayTime::dusk:
            case DayNightCycle::eDayTime::dawn:
                if(QRandomGenerator::global()->bounded(100) < 20)
                    aggro = true;
                break;
            case DayNightCycle::eDayTime::night:
            default:
                aggro = true;
                break;
        }

        if(aggro)
        {
            t_isWalking.stop();
            t_isRunning.start(QRandomGenerator::global()->bounded(RUNNING_TIME_MIN, RUNNING_TIME_MAX));
            setAction(Action::aggro);

            emit sig_monsterSound(getSoundIndexFor(AGGRO));
            if(mMove.obstacles.isEmpty())
            {
                float rad = qDegreesToRadians(static_cast<float>(angle));
                mMove.netDirection = QVector2D(qCos(rad), qSin(rad));
                mMove.zigzagSide = false;
                mMove.zigzagCounter = 0;
                applyZigzagStep();
            }
            return;
        }
    }

    if(t_isWalking.isActive())
        return; // Action in progress

    // Monster choose new action (walk, stand, ...)
    switch(QRandomGenerator::global()->bounded(3))
    {
        case 0: // 33% — déplacement
        {
            setAction(Action::moving);
            t_isWalking.start(QRandomGenerator::global()->bounded(MOVING_TIME_MIN, MOVING_TIME_MAX));
            if(distanceWithHero < DISTANCE_SOUND)
                emit sig_monsterSound(getSoundIndexFor(SOUND));

            if(mMove.obstacles.isEmpty())
            {
                int netAngle = QRandomGenerator::global()->bounded(360);
                float rad = qDegreesToRadians(static_cast<float>(netAngle));
                mMove.netDirection = QVector2D(qCos(rad), qSin(rad));
                mMove.zigzagSide = false;
                mMove.zigzagCounter = 0;
                applyZigzagStep();
            }
            break;
        }
        default: // 66% — immobile
            setAction(Action::stand);
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

    doCollision();

    if(mAction == Action::dead || mAction == Action::skinned || mAction == Action::stand)
        return;

    if((mAction == Action::moving || mAction == Action::aggro) && qAbs(mMove.netDirection.y()) > qAbs(mMove.netDirection.x()))
    {
        // switch direction (45° to -45°) every ZIGZAG_FRAMES ~1sec
        constexpr int ZIGZAG_FRAMES = 45;
        if(++mMove.zigzagCounter >= ZIGZAG_FRAMES)
        {
            mMove.zigzagCounter = 0;
            mMove.zigzagSide = !mMove.zigzagSide;
            applyZigzagStep();
        }
    }

    // Move along current zigzag step direction
    int speed = mCurrentSprite ? mCurrentSprite->pixSpeedRatio : 0;
    qreal dx = static_cast<qreal>(speed) * static_cast<qreal>(mMove.stepDirection.x());
    qreal dy = static_cast<qreal>(speed) * static_cast<qreal>(mMove.stepDirection.y());

    dx = (dx > 0) ? ceil(dx) : -ceil(qAbs(dx));
    dy = (dy > 0) ? ceil(dy) : -ceil(qAbs(dy));

    Character::setPosition(x() + dx, y() + dy);
}


void Monster::doCollision()
{
    int zOffset;
    QList<QGraphicsItem*> itemsColliding = scene()->collidingItems(this);

    // Set Z value when monster interaction
    zOffset = itemsColliding.isEmpty() ? Z_MONSTERS : Z_GROUND_FOREGROUND ;
    for(QGraphicsItem * monster : qAsConst(itemsColliding))
    {
        if(monster->type() == eQGraphicItemType::monster)
        {
            if(pos().y()+boundingRect().height() > monster->y()+monster->boundingRect().height())
                zOffset = Z_MONSTER_FOREGROUND;
            else
                zOffset = Z_MONSTER_BACKGROUND;
        }
    }

    // Process map items which are not obstacles
    for(QGraphicsItem* item : qAsConst(itemsColliding))
    {
        if(!IsMapitemTypeOrDerived(item))
        {
            continue;
        }

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

                if(isInView())
                {
                    if (mapitem->type() == eQGraphicItemType::bush)
                    {
                        Bush * bush = dynamic_cast<Bush*>(mapitem);
                        if(bush)
                        {
                            if(!bush->isAnimated())
                                emit sig_movedInBush(bush);

                            continue;
                        }
                    }
                    if (mapitem->type() == eQGraphicItemType::bushcoin)
                    {
                        BushEventCoin * bushEventCoin = dynamic_cast<BushEventCoin*>(mapitem);
                        if(bushEventCoin)
                        {
                            if(!bushEventCoin->isAnimated())
                                emit sig_movedInBushEvent(bushEventCoin);

                            continue;
                        }
                    }
                    if (mapitem->type() == eQGraphicItemType::bushequipment)
                    {
                        BushEventEquipment * bushEventEquipment = dynamic_cast<BushEventEquipment*>(mapitem);
                        if(bushEventEquipment)
                        {
                            if(!bushEventEquipment->isAnimated())
                                emit sig_movedInBushEvent(bushEventEquipment);

                            continue;
                        }
                    }
                }
            }
        }
    }

    // Create obstacle list
    QList<QGraphicsItem*> obstacles;
    for(QGraphicsItem * item : qAsConst(itemsColliding))
    {
        if (IsMapitemTypeOrDerived(item))
        {
            MapItem* mapitem = dynamic_cast<MapItem*>(item);
            if(mapitem)
            {
                if(mapitem->isObstacle())
                {
                    obstacles.append(mapitem);
                }
            }
        }
        else if (item->type() == eQGraphicItemType::village)
        {
            Village * village = dynamic_cast<Village*>(item);
            if(village)
            {
                obstacles.append(village);
                t_isRunning.stop();
                t_isWalking.start(1000);
                setAction(Action::moving);
            }
        }
    }

    // Remove old obstacles
    for(Obstacle& obstacle : mMove.obstacles)
    {
        for(QGraphicsItem * currentObstacle : qAsConst(obstacles))
        {
            if(currentObstacle == obstacle.item)
            {
                obstacle.vanishing = VANISHING_COUNTERS; // Reset disparition counters
                continue;
            }
        }
        if(--obstacle.vanishing < 0)
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
    mCollisionVector = QVector2D(0, 0);
    if(!mMove.obstacles.isEmpty())
    {
        int angle;
        for(Obstacle & obstacle : mMove.obstacles)
        {
            angle = ToolFunctions::getAngleBetween(this, obstacle.item);
            QVector2D vectorObstacle( qCos(qDegreesToRadians(static_cast<double>(angle))),
                                      qSin(qDegreesToRadians(static_cast<double>(angle))));
            mCollisionVector += vectorObstacle;
        }

        mCollisionVector.normalize();

        // Obstacle : on redirige la trajectoire nette et on recalcule le step zigzag
        mMove.netDirection = mCollisionVector;
        mMove.zigzagSide = false;
        applyZigzagStep();
    }

    setZValue(zOffset);
    update();
}

bool Monster::isInView()
{
    return ToolFunctions::getVisibleView(mView).intersects(sceneBoundingRect());
}


bool Monster::isInBiggerView()
{
    QRectF area = ToolFunctions::getBiggerView(mView);
    QRectF bigArea(area.x() - area.width() / 2, area.y() - area.height() / 2, area.width() * 2, area.height() * 2);
    mIsInView = bigArea.intersects(sceneBoundingRect());
    return mIsInView;
}

void Monster::enableMonsterAnimation(bool toggle)
{
    if(toggle){
        t_movement.start();
    }else{
        t_movement.stop();
    }
}

Monster::~Monster()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();
    if(mFightView)
        delete mFightView;
}

Monster* Monster::Factory(QString name, QGraphicsView* view)
{
    if (name == Spider::Name())
    {
        return new Spider(view);
    }
    if (name == Goblin::Name())
    {
        return new Goblin(view);
    }
    if (name == Troll::Name())
    {
        return new Troll(view);
    }
    if (name == Wolf::Name())
    {
        return new Wolf(view);
    }
    if (name == WolfAlpha::Name())
    {
        return new WolfAlpha(view);
    }
    if (name == Bear::Name())
    {
        return new Bear(view);
    }
    if (name == Oggre::Name())
    {
        return new Oggre(view);
    }
    if (name == LaoShanLung::Name())
    {
        return new LaoShanLung(view);
    }
    assert(false);
    return nullptr;
}










struct SpiderPixmapCache {
    QPixmap logo        {":/monsters/spider/Ressources/spider_logo.png"};
    QPixmap heavyAttack {":/monsters/spider/Ressources/spider_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/spider/Ressources/spider_lightAttack.png"};
    QPixmap move        {":/monsters/spider/Ressources/spider_move.png"};
    QPixmap stand       {":/monsters/spider/Ressources/spider_stand.png"};
    QPixmap dead        {":/monsters/spider/Ressources/spider_dead.png"};
    QPixmap skinned     {":/monsters/spider/Ressources/spider_skinned.png"};
};
static const SpiderPixmapCache& sSpiderPx() { static SpiderPixmapCache c; return c; }

Spider::Spider(QGraphicsView * view):
    Monster(view)
{
    mName = Spider::Name();
    mLife = Gauge{100,100};
    mDamage = 30;
    mThreatLevel = 1;
    mImage = sSpiderPx().logo;
    mDescription = "Les araignées sont dangereuses mais aussi fragiles";
    mSkin = QRandomGenerator::global()->bounded(SPIDER_SKIN_NUM);

    setBoundingRect(QRectF(0,0,100,80));

    mPixmap.heavyAttack = sSpiderPx().heavyAttack;
    mPixmap.lightAttack = sSpiderPx().lightAttack;

    mSprites.walk.set(sSpiderPx().move,       8, 1, 8, 150, SPEED_SPIDER,      QSize(800, 80));
    mSprites.run.set(sSpiderPx().move,        8, 1, 8, 100, SPEEDBOOST_SPIDER, QSize(800, 80));
    mSprites.stand.set(sSpiderPx().stand,     1, 1, 1,   0, 0,                 QSize(100, 80));
    mSprites.dead.set(sSpiderPx().dead,       1, 1, 1,   0, 0,                 QSize(100, 80));
    mSprites.skinned.set(sSpiderPx().skinned, 1, 1, 1,   0, 0,                 QSize(100, 80));

    mSounds[0] = SOUND_SPIDER_HEAVYATTACK;
    mSounds[1] = SOUND_SPIDER_LIGHTATTACK;
    mSounds[2] = SOUND_SPIDER_ROAR;
    mSounds[3] = SOUND_SPIDER_DIE;
    mSounds[4] = SOUND_SPIDER;
    mSounds[5] = SOUND_SPIDER_AGGRO;

    mFightView = new SpiderFightView();

    Spider::generateRandomLoots();
}

void Spider::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new PoisonPouch);
        mItems.append(new PoisonPouch);
    }
}

void Spider::onHitEffect(Character* target)
{
    if(QRandomGenerator::global()->bounded(100) < 30)
    {
        target->applyStatus(Character::eStatus::poisoned, QRandomGenerator::global()->bounded(3, 7));
    }
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










struct WolfPixmapCache {
    QPixmap logo        {":/monsters/wolf/Ressources/wolf_logo.png"};
    QPixmap heavyAttack {":/monsters/wolf/Ressources/wolf_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/wolf/Ressources/wolf_lightAttack.png"};
    QPixmap move        {":/monsters/wolf/Ressources/wolf_move.png"};
    QPixmap run         {":/monsters/wolf/Ressources/wolf_run.png"};
    QPixmap stand       {":/monsters/wolf/Ressources/wolf_stand.png"};
    QPixmap dead        {":/monsters/wolf/Ressources/wolf_dead.png"};
    QPixmap skinned     {":/monsters/wolf/Ressources/wolf_skinned.png"};
};
static const WolfPixmapCache& sWolfPx() { static WolfPixmapCache c; return c; }

Wolf::Wolf(QGraphicsView * view):
    Monster(view)
{
    mName = Wolf::Name();
    mLife = Gauge{600,600};
    mDamage = 15;
    mThreatLevel = 2;
    mImage = sWolfPx().logo;
    mDescription = "Les loups sont des chasseurs agressifs chassant exclusivement en meute";

    setBoundingRect(QRectF(0, 0, 120, 70));

    mPixmap.heavyAttack = sWolfPx().heavyAttack;
    mPixmap.lightAttack = sWolfPx().lightAttack;

    mSprites.walk.set(sWolfPx().move,       4, 7, 25, 60, SPEED_WOLF,      QSize(1024, 1050));
    mSprites.run.set(sWolfPx().run,         4, 7, 28, 25, SPEEDBOOST_WOLF, QSize(1024, 1050));
    mSprites.stand.set(sWolfPx().stand,     1, 1,  1,  0, 0,               QSize(246, 150));
    mSprites.dead.set(sWolfPx().dead,       1, 1,  1,  0, 0,               QSize(246, 150));
    mSprites.skinned.set(sWolfPx().skinned, 1, 1,  1,  0, 0,               QSize(100, 70));

    mSounds[0] = SOUND_WOLF_HEAVYATTACK;
    mSounds[1] = SOUND_WOLF_LIGHTATTACK;
    mSounds[2] = SOUND_WOLF_ROAR;
    mSounds[3] = SOUND_WOLF_DIE;
    mSounds[4] = SOUND_WOLF;
    mSounds[5] = SOUND_WOLF_AGGRO;

    mFightView = new WolfFightView();

    Wolf::generateRandomLoots();
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

struct WolfAlphaPixmapCache {
    QPixmap logo        {":/monsters/wolfAlpha/Ressources/wolfAlpha_logo.png"};
    QPixmap move        {":/monsters/wolfAlpha/Ressources/wolfAlpha_move.png"};
    QPixmap run         {":/monsters/wolfAlpha/Ressources/wolfAlpha_run.png"};
    QPixmap stand       {":/monsters/wolfAlpha/Ressources/wolfAlpha_stand.png"};
    QPixmap dead        {":/monsters/wolfAlpha/Ressources/wolfAlpha_dead.png"};
    QPixmap skinned     {":/monsters/wolfAlpha/Ressources/wolfAlpha_skinned.png"};
};
static const WolfAlphaPixmapCache& sWolfAlphaPx() { static WolfAlphaPixmapCache c; return c; }

WolfAlpha::WolfAlpha(QGraphicsView * view):
    Wolf(view)
{
    mName = WolfAlpha::Name();
    mLife = Gauge{1000,1000};
    mDamage = 18;
    mThreatLevel = 3;
    mImage = sWolfAlphaPx().logo;
    mDescription = "Les loups alpha sont des meneurs par nature, s'ils ont pu se hisser à la tête de leur meute, c'est bien par leur férocité sans égale";

    setBoundingRect(QRectF(0,0,145,85));

    mSprites.walk.set(sWolfAlphaPx().move,       4, 7, 25, 75, SPEED_WOLF,      QSize(1024, 1050));
    mSprites.run.set(sWolfAlphaPx().run,         4, 7, 28, 25, SPEEDBOOST_WOLF, QSize(1024, 1050));
    mSprites.stand.set(sWolfAlphaPx().stand,      1, 1,  1,   0, 0,               QSize(246, 150));
    mSprites.dead.set(sWolfAlphaPx().dead,        1, 1,  1,   0, 0,               QSize(246, 150));
    mSprites.skinned.set(sWolfAlphaPx().skinned,  1, 1,  1,   0, 0,               QSize(120, 80));

    mFightView = new WolfAlphaFightView();

    WolfAlpha::generateRandomLoots();
}

WolfAlpha::~WolfAlpha()
{

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

void WolfAlpha::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new WolfAlphaPelt);
        mItems.append(new WolfMeat);
        mItems.append(new WolfFang);
    }
}









struct GoblinPixmapCache {
    QPixmap logo        {":/monsters/goblin/Ressources/goblin_logo.png"};
    QPixmap heavyAttack {":/monsters/goblin/Ressources/goblin_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/goblin/Ressources/goblin_lightAttack.png"};
    QPixmap move        {":/monsters/goblin/Ressources/goblin_move.png"};
    QPixmap run         {":/monsters/goblin/Ressources/goblin_run.png"};
    QPixmap stand       {":/monsters/goblin/Ressources/goblin_stand.png"};
    QPixmap dead        {":/monsters/goblin/Ressources/goblin_dead.png"};
    QPixmap skinned     {":/monsters/goblin/Ressources/goblin_skinned.png"};
};
static const GoblinPixmapCache& sGoblinPx() { static GoblinPixmapCache c; return c; }

Goblin::Goblin(QGraphicsView * view):
    Monster(view)
{
    mName = Goblin::Name();
    mLife = Gauge{400,400};
    mDamage = 8;
    mThreatLevel = 1;
    mImage = sGoblinPx().logo;
    mDescription = "Le gobelin est un être incompris et sournois qui cherchera à vous faire du mal par tous les moyens";
    mSkin = QRandomGenerator::global()->bounded(GOBLIN_SKIN_NUM);

    setBoundingRect(QRectF(0,0,60,60));

    mPixmap.heavyAttack = sGoblinPx().heavyAttack;
    mPixmap.lightAttack = sGoblinPx().lightAttack;

    mSprites.walk.set(sGoblinPx().move,       6, 1, 6, 150, SPEED_GOBLIN,      QSize(360, 60));
    mSprites.run.set(sGoblinPx().run,         7, 1, 6, 100, SPEEDBOOST_GOBLIN, QSize(420, 60));
    mSprites.stand.set(sGoblinPx().stand,     1, 1, 1,   0, 0,                 QSize(60, 60));
    mSprites.dead.set(sGoblinPx().dead,       1, 1, 1,   0, 0,                 QSize(60, 60));
    mSprites.skinned.set(sGoblinPx().skinned, 1, 1, 1,   0, 0,                 QSize(60, 60));

    mSounds[0] = SOUND_GOBLIN_HEAVYATTACK;
    mSounds[1] = SOUND_GOBLIN_LIGHTATTACK;
    mSounds[2] = SOUND_GOBLIN_ROAR;
    mSounds[3] = SOUND_GOBLIN_DIE;
    mSounds[4] = SOUND_GOBLIN;
    mSounds[5] = SOUND_GOBLIN_AGGRO;

    mFightView = new GobelinFightView();

    Goblin::generateRandomLoots();

    mCoin = QRandomGenerator::global()->bounded(5);
}

void Goblin::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new GoblinEar);
        mItems.append(new GoblinBones);
    }
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
        mItems.append(new Amulet("Amulette\nde shaman", QString(":/equipment/Ressources/amulet_7.png"),8,2,5,8,"Amulette mystérieuse confectionnée par un gobelin."));

}

Goblin::~Goblin()
{

}











struct BearPixmapCache {
    QPixmap logo        {":/monsters/bear/Ressources/bear_logo.png"};
    QPixmap heavyAttack {":/monsters/bear/Ressources/bear_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/bear/Ressources/bear_lightAttack.png"};
    QPixmap move        {":/monsters/bear/Ressources/bear_move.png"};
    QPixmap run         {":/monsters/bear/Ressources/bear_run.png"};
    QPixmap stand       {":/monsters/bear/Ressources/bear_stand.png"};
    QPixmap dead        {":/monsters/bear/Ressources/bear_dead.png"};
    QPixmap skinned     {":/monsters/bear/Ressources/bear_skinned.png"};
};
static const BearPixmapCache& sBearPx() { static BearPixmapCache c; return c; }

Bear::Bear(QGraphicsView * view):
    Monster(view)
{
    mName = Bear::Name();
    mLife = Gauge{1300,1300};
    mDamage = 20;
    mThreatLevel = 3;
    mImage = sBearPx().logo;
    mDescription = "L'ours est un prédator puissant est dangereux, il hiberne pendant la saison hivernale";
    mSkin = QRandomGenerator::global()->bounded(BEAR_SKIN_NUM);

    setBoundingRect(QRectF(0,0,300,200));

    mPixmap.heavyAttack = sBearPx().heavyAttack;
    mPixmap.lightAttack = sBearPx().lightAttack;

    mSprites.walk.set(sBearPx().move,       8,  1,  8, 150, SPEED_BEAR,      QSize(2400, 200));
    mSprites.run.set(sBearPx().run,         12, 1, 12, 100, SPEEDBOOST_BEAR, QSize(3600, 200));
    mSprites.stand.set(sBearPx().stand,     1,  1,  1,   0, 0,               QSize(300, 200));
    mSprites.dead.set(sBearPx().dead,       1,  1,  1,   0, 0,               QSize(300, 200));
    mSprites.skinned.set(sBearPx().skinned, 1,  1,  1,   0, 0,               QSize(300, 200));

    mSounds[0] = SOUND_BEAR_HEAVYATTACK;
    mSounds[1] = SOUND_BEAR_LIGHTATTACK;
    mSounds[2] = SOUND_BEAR_ROAR;
    mSounds[3] = SOUND_BEAR_DIE;
    mSounds[4] = SOUND_BEAR;
    mSounds[5] = SOUND_BEAR_AGGRO;

    mFightView = new BearFightView();

    Bear::generateRandomLoots();
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










struct TrollPixmapCache {
    QPixmap logo        {":/monsters/troll/Ressources/troll_logo.png"};
    QPixmap heavyAttack {":/monsters/troll/Ressources/troll_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/troll/Ressources/troll_lightAttack.png"};
    QPixmap move        {":/monsters/troll/Ressources/troll_move.png"};
    QPixmap run         {":/monsters/troll/Ressources/troll_run.png"};
    QPixmap stand       {":/monsters/troll/Ressources/troll_stand.png"};
    QPixmap dead        {":/monsters/troll/Ressources/troll_die.png"};
    QPixmap skinned     {":/monsters/troll/Ressources/troll_skinned.png"};
};
static const TrollPixmapCache& sTrollPx() { static TrollPixmapCache c; return c; }

Troll::Troll(QGraphicsView * view):
    Monster(view)
{
    mName = Troll::Name();
    mLife = Gauge{800,800};
    mDamage = 14;
    mThreatLevel = 2;
    mImage = sTrollPx().logo;
    mDescription = "Le troll est avare et cherche à dérober les biens des humains et ce par tous les moyens";
    mSkin = QRandomGenerator::global()->bounded(TROLL_SKIN_NUM);

    setBoundingRect(QRectF(0,0,100,100));

    mPixmap.heavyAttack = sTrollPx().heavyAttack;
    mPixmap.lightAttack = sTrollPx().lightAttack;

    mSprites.walk.set(sTrollPx().move,       7, 1, 7, 150, SPEED_TROLL,      QSize(700, 100));
    mSprites.run.set(sTrollPx().run,         7, 1, 7, 100, SPEEDBOOST_TROLL, QSize(700, 100));
    mSprites.stand.set(sTrollPx().stand,     1, 1, 1,   0, 0,                QSize(100, 100));
    mSprites.dead.set(sTrollPx().dead,       1, 1, 1,   0, 0,                QSize(100, 100));
    mSprites.skinned.set(sTrollPx().skinned, 1, 1, 1,   0, 0,                QSize(100, 100));

    mSounds[0] = SOUND_TROLL_HEAVYATTACK;
    mSounds[1] = SOUND_TROLL_LIGHTATTACK;
    mSounds[2] = SOUND_TROLL_ROAR;
    mSounds[3] = SOUND_TROLL_DIE;
    mSounds[4] = SOUND_TROLL;
    mSounds[5] = SOUND_TROLL_AGGRO;

    mFightView = new TrollFightView();

    Troll::generateRandomLoots();
}

void Troll::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new TrollMeat);
        mItems.append(new TrollSkull);
    }
}

void Troll::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    mItems.append(new TrollMeat);
    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new TrollSkull);

    if(QRandomGenerator::global()->bounded(6) == 0)
        mItems.append(new Sword("Gourdin", QString(":/equipment/Ressources/sword_19.png"), 220, 1, 20, 8, "Gourdin extremement lourd et très dur à manipuler."));

}


Troll::~Troll()
{

}













struct OggrePixmapCache {
    QPixmap logo        {":/monsters/oggre/Ressources/oggre_logo.png"};
    QPixmap heavyAttack {":/monsters/oggre/Ressources/oggre_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/oggre/Ressources/oggre_lightAttack.png"};
    QPixmap move        {":/monsters/oggre/Ressources/oggre_move.png"};
    QPixmap run         {":/monsters/oggre/Ressources/oggre_run.png"};
    QPixmap stand       {":/monsters/oggre/Ressources/oggre_stand.png"};
    QPixmap dead        {":/monsters/oggre/Ressources/oggre_die.png"};
    QPixmap skinned     {":/monsters/oggre/Ressources/oggre_skinned.png"};
};
static const OggrePixmapCache& sOggrePx() { static OggrePixmapCache c; return c; }

Oggre::Oggre(QGraphicsView * view):
    Monster(view)
{
    mName = Oggre::Name();
    mLife = Gauge{2500,2500};
    mDamage = 30;
    mThreatLevel = 4;
    mImage = sOggrePx().logo;
    mDescription = "L'ogre est massif et terriblement dangereux, il vaut mieux ne pas croiser son chemin si l'on y est pas préparé";
    mSkin = QRandomGenerator::global()->bounded(OGGRE_SKIN_NUM);

    setBoundingRect(QRectF(0,0,300,300));

    mPixmap.heavyAttack = sOggrePx().heavyAttack;
    mPixmap.lightAttack = sOggrePx().lightAttack;

    mSprites.walk.set(sOggrePx().move,       10, 1, 10, 150, SPEED_OGGRE,      QSize(3000, 300));
    mSprites.run.set(sOggrePx().run,          8, 1,  8, 100, SPEEDBOOST_OGGRE, QSize(2400, 300));
    mSprites.stand.set(sOggrePx().stand,      1, 1,  1,   0, 0,                QSize(300, 300));
    mSprites.dead.set(sOggrePx().dead,        1, 1,  1,   0, 0,                QSize(300, 300));
    mSprites.skinned.set(sOggrePx().skinned,  1, 1,  1,   0, 0,                QSize(300, 300));

    mSounds[0] = SOUND_OGGRE_HEAVYATTACK;
    mSounds[1] = SOUND_OGGRE_LIGHTATTACK;
    mSounds[2] = SOUND_OGGRE_ROAR;
    mSounds[3] = SOUND_OGGRE_DIE;
    mSounds[4] = SOUND_OGGRE;
    mSounds[5] = SOUND_OGGRE_AGGRO;

    mFightView = new OggreFightView();

    Oggre::generateRandomLoots();
}

void Oggre::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
        mItems.append(new OggreSkull);
}

void Oggre::generateRandomLoots()
{
    while(!mItems.isEmpty())
        delete mItems.takeLast();

    if(QRandomGenerator::global()->bounded(2) == 0)
        mItems.append(new OggreSkull);

    if(QRandomGenerator::global()->bounded(3) == 0)
        mItems.append(new Sword("Gourdin", QString(":/equipment/Ressources/sword_19.png"), 220, 1, 20, 8, "Gourdin extrèmement lourd et très dur à manipuler."));
}


Oggre::~Oggre()
{

}










struct LaoShanLungPixmapCache {
    QPixmap logo        {":/monsters/laoshanlung/Ressources/laoshanlung_logo.png"};
    QPixmap heavyAttack {":/monsters/laoshanlung/Ressources/laoshanlung_heavyAttack.png"};
    QPixmap lightAttack {":/monsters/laoshanlung/Ressources/laoshanlung_lightAttack.png"};
    QPixmap move        {":/monsters/laoshanlung/Ressources/laoshanlung_move.png"};
    QPixmap stand       {":/monsters/laoshanlung/Ressources/laoshanlung_stand.png"};
    QPixmap dead        {":/monsters/laoshanlung/Ressources/laoshanlung_dead.png"};
    QPixmap skinned     {":/monsters/laoshanlung/Ressources/laoshanlung_skinned.png"};
};
static const LaoShanLungPixmapCache& sLaoPx() { static LaoShanLungPixmapCache c; return c; }

LaoShanLung::LaoShanLung(QGraphicsView * view):
    Monster(view)
{
    mName = LaoShanLung::Name();
    mLife = Gauge{10000,10000};
    mDamage = 50;
    mThreatLevel = 10;
    mImage = sLaoPx().logo;
    mDescription = "Créature mythique, le Lao Shun Lung est un dragon de terre colossale qui écume les plaines depuis des centaines d'années";

    setBoundingRect(QRectF(0,0,700,450));

    mPixmap.heavyAttack = sLaoPx().heavyAttack;
    mPixmap.lightAttack = sLaoPx().lightAttack;

    mSprites.walk.set(sLaoPx().move,       18, 1, 18, 150, SPEED_LAOSHANLUNG,      QSize(12600, 450));
    mSprites.run.set(sLaoPx().move,        18, 1, 18, 100, SPEEDBOOST_LAOSHANLUNG, QSize(12600, 450));
    mSprites.stand.set(sLaoPx().stand,      1, 1,  1,   0, 0,                      QSize(700, 450));
    mSprites.dead.set(sLaoPx().dead,        1, 1,  1,   0, 0,                      QSize(700, 450));
    mSprites.skinned.set(sLaoPx().skinned,  1, 1,  1,   0, 0,                      QSize(700, 450));

    mSounds[0] = SOUND_LAOSHANLUNG_HEAVYATTACK;
    mSounds[1] = SOUND_LAOSHANLUNG_LIGHTATTACK;
    mSounds[2] = SOUND_LAOSHANLUNG_ROAR;
    mSounds[3] = SOUND_LAOSHANLUNG_DIE;
    mSounds[4] = SOUND_LAOSHANLUNG;
    mSounds[5] = SOUND_LAOSHANLUNG_AGGRO;

    mFightView = new LaoShanLungFightView();

    LaoShanLung::generateRandomLoots();
}

void LaoShanLung::addExtraLoots()
{
    if(QRandomGenerator::global()->bounded(4) == 0)
    {
        mItems.append(new LaoshanlungHeart);
    }
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
