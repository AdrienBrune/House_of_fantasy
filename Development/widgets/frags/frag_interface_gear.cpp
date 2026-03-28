#include "frag_interface_gear.h"
#include "ui_frag_interface_gear.h"
#include "entitieshandler.h"
#include <cmath>

Frag_Interface_Gear::Frag_Interface_Gear(QWidget *parent) :
    QWidget(parent),
    xPosSplitter(485),
    ui(new Ui::Frag_interface_gear)
{
    ui->setupUi(this);
    mScene = new QGraphicsScene(this);

    setStyleSheet("Frag_Interface_Gear { background: transparent; border: none; }");
    ui->graphicsView->setStyleSheet("QGraphicsView { background: transparent; border: none; }");
    ui->graphicsView->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    ui->graphicsView->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    ui->graphicsView->setContentsMargins(0, 0, 0, 0);

    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setAlignment(Qt::AlignCenter);
    mScene->setSceneRect(ui->graphicsView->rect());

    int offset = 10;
    int space = 20;
    int itemSize = 100;
    
    mHelmetPos = QPointF(xPosSplitter+50 +      5 + offset+space+itemSize,          offset);
    mBreastplatePos = QPointF(xPosSplitter+50 + offset+space+itemSize,          offset+space+itemSize);
    mGlovesPos = QPointF(xPosSplitter+50 +      offset+2*(itemSize+space),      offset+space+itemSize*3/2);
    mPantPos = QPointF(xPosSplitter+50 +        5 + offset+space+itemSize,          offset+2*(itemSize+space));
    mFootwearsPos = QPointF(xPosSplitter+50 +   offset+space+itemSize,          offset+3*(itemSize+space));
    mAmuletPos = QPointF(xPosSplitter+50 +      offset+2*(itemSize+space),      offset);
    mWeaponPos = QPointF(xPosSplitter+50 +      offset,                         offset+2*(space+itemSize));

    // Animation
    static const float kPhases[7] = {0.0f, 1.1f, 2.3f, 3.5f, 4.7f, 5.9f, 0.7f};
    for(int i = 0; i < 7; ++i)
        mAnimPhases[i] = kPhases[i];
    mAnimTime = 0.0;
    mAnimTimer = new QTimer(this);
    connect(mAnimTimer, &QTimer::timeout, this, &Frag_Interface_Gear::onAnimTick);
    mAnimTimer->start(30);

    initEquipmentRightSide();
    addItemsLeftSide();
}

void Frag_Interface_Gear::onItemMovedHandler(ItemQuickDisplayer * item)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    item->setZValue(0);
    if(!mScene->sceneRect().contains(item->boundingRect()))
    {
        item->setPos(item->getInitialPosition());
    }else
    {
        if(static_cast<int>(item->getInitialPosition().x()) < xPosSplitter)
        {
            if(item->x() < xPosSplitter)
            {
                item->setPos(item->getInitialPosition());
            }
            else
            {
                Equipment * equipment = dynamic_cast<Equipment*>(item->getItem());
                if(equipment)
                {
                    if(!IS_ABLE(hero->getHeroClass(), equipment->getUsable()))
                    {
                        item->setPos(item->getInitialPosition());
                        return;
                    }
                }
                else
                {
                    item->setPos(item->getInitialPosition());
                    return;
                }

                QRect area;
                Weapon * weapon = dynamic_cast<Weapon*>(item->getItem());
                if(weapon)
                {
                    area.setRect(static_cast<int>(mWeaponPos.x()), static_cast<int>(mWeaponPos.y()), 100,100);
                }
                Helmet * helmet = dynamic_cast<Helmet*>(item->getItem());
                if(helmet)
                {
                    area.setRect(static_cast<int>(mHelmetPos.x()), static_cast<int>(mHelmetPos.y()), 100,100);
                }
                Breastplate * breastplate = dynamic_cast<Breastplate*>(item->getItem());
                if(breastplate)
                {
                    area.setRect(static_cast<int>(mBreastplatePos.x()), static_cast<int>(mBreastplatePos.y()), 100,100);
                }
                Gloves * gloves = dynamic_cast<Gloves*>(item->getItem());
                if(gloves)
                {
                    area.setRect(static_cast<int>(mGlovesPos.x()), static_cast<int>(mGlovesPos.y()), 100,100);
                }
                Pant * pant = dynamic_cast<Pant*>(item->getItem());
                if(pant)
                {
                    area.setRect(static_cast<int>(mPantPos.x()), static_cast<int>(mPantPos.y()), 100,100);
                }
                Footwears * footwears = dynamic_cast<Footwears*>(item->getItem());
                if(footwears)
                {
                    area.setRect(static_cast<int>(mFootwearsPos.x()), static_cast<int>(mFootwearsPos.y()), 100,100);
                }
                Amulet * amulet = dynamic_cast<Amulet*>(item->getItem());
                if(amulet)
                {
                    area.setRect(static_cast<int>(mAmuletPos.x()), static_cast<int>(mAmuletPos.y()), 100,100);
                }

                ItemQuickDisplayer * equipmentArea = new ItemQuickDisplayer(new Helmet("", "", 0, 0, 0, 0, "")); // dummy item for collision check
                mScene->addItem(equipmentArea);
                equipmentArea->setPos(area.x(), area.y());

                if(equipmentArea->collidesWithItem(item))
                {
                    item->setPos(equipmentArea->pos());
                    item->setInitialPosition(equipmentArea->pos());
                    mScene->removeItem(equipmentArea);
                    delete equipmentArea;

                    QList<QGraphicsItem*> collision = mScene->collidingItems(item);
                    if(!collision.isEmpty())
                    {
                        for(QGraphicsItem * itemFound : collision)
                        {
                            ItemQuickDisplayer * itemToMove = static_cast<ItemQuickDisplayer*>(itemFound);
                            swapItemFromRightToLeft(itemToMove);
                            emit sig_unequipItem(itemToMove->getItem());
                            emit sig_equipItem(item->getItem());
                        }
                    }
                    else
                    {
                        equipmentRightSide.append(item);
                        mItemsLeftSide.removeOne(item);
                        emit sig_equipItem(item->getItem());
                    }
                }
                else
                {
                    item->setPos(item->getInitialPosition());
                    mScene->removeItem(equipmentArea);
                    delete equipmentArea;
                }
            }
        }else
        {
            if(item->x() < xPosSplitter)
            {
                swapItemFromRightToLeft(item);
                emit sig_unequipItem(item->getItem());
            }else
            {
                item->setPos(item->getInitialPosition());
            }
        }
    }
}

void Frag_Interface_Gear::onItemSelected(ItemQuickDisplayer * item)
{
    emit sig_itemClicked(item);
    item->setZValue(1);
}

void Frag_Interface_Gear::initEquipmentRightSide()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    if(hero->getGear()->getHelmet()!=nullptr){
        ItemQuickDisplayer * w_helmet = new ItemQuickDisplayer(hero->getGear()->getHelmet());
        mScene->addItem(w_helmet);
        w_helmet->setInitialPosition(mHelmetPos);
        w_helmet->setPos(mHelmetPos);
        equipmentRightSide.append(w_helmet);
        connect(w_helmet, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getBreastplate()!=nullptr){
        ItemQuickDisplayer * w_breasplate = new ItemQuickDisplayer(hero->getGear()->getBreastplate());
        mScene->addItem(w_breasplate);
        w_breasplate->setInitialPosition(mBreastplatePos);
        w_breasplate->setPos(mBreastplatePos);
        equipmentRightSide.append(w_breasplate);
        connect(w_breasplate, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getGloves()!=nullptr){
        ItemQuickDisplayer * w_gloves = new ItemQuickDisplayer(hero->getGear()->getGloves());
        mScene->addItem(w_gloves);
        w_gloves->setInitialPosition(mGlovesPos);
        w_gloves->setPos(mGlovesPos);
        equipmentRightSide.append(w_gloves);
        connect(w_gloves, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getPant()!=nullptr){
        ItemQuickDisplayer * w_pant = new ItemQuickDisplayer(hero->getGear()->getPant());
        mScene->addItem(w_pant);
        w_pant->setInitialPosition(mPantPos);
        w_pant->setPos(mPantPos);
        equipmentRightSide.append(w_pant);
        connect(w_pant, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getFootWears()!=nullptr){
        ItemQuickDisplayer * w_footwears = new ItemQuickDisplayer(hero->getGear()->getFootWears());
        mScene->addItem(w_footwears);
        w_footwears->setInitialPosition(mFootwearsPos);
        w_footwears->setPos(mFootwearsPos);
        equipmentRightSide.append(w_footwears);
        connect(w_footwears, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getAmulet()!=nullptr){
        ItemQuickDisplayer * w_amulet = new ItemQuickDisplayer(hero->getGear()->getAmulet());
        mScene->addItem(w_amulet);
        w_amulet->setInitialPosition(mAmuletPos);
        w_amulet->setPos(mAmuletPos);
        equipmentRightSide.append(w_amulet);
        connect(w_amulet, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getWeapon()!=nullptr){
        ItemQuickDisplayer * w_weapon = new ItemQuickDisplayer(hero->getGear()->getWeapon());
        mScene->addItem(w_weapon);
        w_weapon->setInitialPosition(mWeaponPos);
        w_weapon->setPos(mWeaponPos);
        equipmentRightSide.append(w_weapon);
        connect(w_weapon, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_weapon, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_weapon, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_weapon, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
        connect(w_weapon, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));
    }
}

void Frag_Interface_Gear::addItemLeftSide(Item * item)
{
    ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
    mItemsLeftSide.append(w_item);
    mScene->addItem(w_item);

    int vOffset = 10, hOffset = 10;
    w_item->setPos(vOffset, hOffset);
    connect(w_item, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += w_item->boundingRect().width() + 5;
        if(hOffset > xPosSplitter-60)
        {
            hOffset = 10;
            vOffset += w_item->boundingRect().height() + 5;
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());
}

void Frag_Interface_Gear::addItemsLeftSide()
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    QList<Item*> list;
    for(Item * item : hero->getBag()->getItemList<Weapon*>())
    {
        addItemLeftSide(item);
    }
    for(Item * item : hero->getBag()->getItemList<ArmorPiece*>())
    {
        addItemLeftSide(item);
    }
}

QList<Item *> Frag_Interface_Gear::getItemsLeftSide()
{
    QList<Item*> itemsToFind;
    QList<QGraphicsItem*> items = mScene->items();
    foreach(QGraphicsItem * item, items)
    {
        if(item->x() < xPosSplitter)
        {
            ItemQuickDisplayer * leftItem = dynamic_cast<ItemQuickDisplayer*>(item);
            if(leftItem){
                itemsToFind.append(leftItem->getItem());
            }
        }
    }
    return itemsToFind;
}

void Frag_Interface_Gear::unselectItems()
{
    for(ItemQuickDisplayer * item : mItemsLeftSide)
    {
        item->setItemSelected(false);
    }
    for(ItemQuickDisplayer * item : equipmentRightSide)
    {
        item->setItemSelected(false);
    }
}

ItemQuickDisplayer *Frag_Interface_Gear::getSelectedItem()
{
    for(ItemQuickDisplayer * item : mItemsLeftSide)
    {
        if(item->isItemSelected())
            return item;
    }
    for(ItemQuickDisplayer * item : equipmentRightSide)
    {
        if(item->isItemSelected())
            return item;
    }

    return nullptr;
}

void Frag_Interface_Gear::swapItemFromRightToLeft(ItemQuickDisplayer * w_item)
{
    equipmentRightSide.removeOne(w_item);
    mItemsLeftSide.append(w_item);

    int vOffset = 10, hOffset = 10;
    w_item->setPos(vOffset, hOffset);

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += w_item->boundingRect().width() + 5;
        if(hOffset > xPosSplitter-60)
        {
            hOffset = 10;
            vOffset += w_item->boundingRect().height() + 5;
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());
}

void Frag_Interface_Gear::onAnimTick()
{
    mAnimTime += 0.03;
    update();
}

void Frag_Interface_Gear::onItemRightClicked(ItemQuickDisplayer* item)
{
    Hero* hero = EntitiesHandler::getInstance().getHero();

    // Item on the right side (equipped) → unequip it
    if(equipmentRightSide.contains(item))
    {
        swapItemFromRightToLeft(item);
        emit sig_unequipItem(item->getItem());
        return;
    }

    // Item on the left side (inventory) → try to equip it
    Equipment* equipment = dynamic_cast<Equipment*>(item->getItem());
    if(!equipment) return;
    if(!IS_ABLE(hero->getHeroClass(), equipment->getUsable())) return;

    // Determine the target slot based on equipment type
    QPointF targetPos;
    if     (dynamic_cast<Weapon*>     (equipment)) targetPos = mWeaponPos;
    else if(dynamic_cast<Helmet*>     (equipment)) targetPos = mHelmetPos;
    else if(dynamic_cast<Breastplate*>(equipment)) targetPos = mBreastplatePos;
    else if(dynamic_cast<Gloves*>     (equipment)) targetPos = mGlovesPos;
    else if(dynamic_cast<Pant*>       (equipment)) targetPos = mPantPos;
    else if(dynamic_cast<Footwears*>  (equipment)) targetPos = mFootwearsPos;
    else if(dynamic_cast<Amulet*>     (equipment)) targetPos = mAmuletPos;
    else return;

    // If the slot is already occupied, send the existing item back to the left
    for(ItemQuickDisplayer* rightItem : equipmentRightSide)
    {
        if(rightItem->getInitialPosition() == targetPos)
        {
            swapItemFromRightToLeft(rightItem);
            emit sig_unequipItem(rightItem->getItem());
            break;
        }
    }

    // Move the item to the slot and equip it
    item->setPos(targetPos);
    item->setInitialPosition(targetPos);
    equipmentRightSide.append(item);
    mItemsLeftSide.removeOne(item);
    emit sig_equipItem(item->getItem());
}

void Frag_Interface_Gear::paintEvent(QPaintEvent*)
{
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- Slot states ---
    Hero* hero = EntitiesHandler::getInstance().getHero();
    const bool hasHelmet      = hero->getGear()->getHelmet()      != nullptr;
    const bool hasBreastplate = hero->getGear()->getBreastplate() != nullptr;
    const bool hasGloves      = hero->getGear()->getGloves()      != nullptr;
    const bool hasPants       = hero->getGear()->getPant()        != nullptr;
    const bool hasFootwear    = hero->getGear()->getFootWears()   != nullptr;
    const bool hasAmulet      = hero->getGear()->getAmulet()      != nullptr;
    const bool hasWeapon      = hero->getGear()->getWeapon()      != nullptr;

    // --- Base positions (center of each slot) ---
    const qreal slotHalf = 50.0;
    const QPoint vOff(ui->graphicsView->x(), ui->graphicsView->y());
    auto basePos = [&](const QPointF& sp) -> QPointF {
        return QPointF(ui->graphicsView->mapFromScene(sp + QPointF(slotHalf, slotHalf) + vOff));
    };

    // --- Chaotic floating (superposition of 2 movements) ---
    const qreal floatAmp = 4.0;
    auto animOff = [&](int i) -> QPointF {
        return QPointF(
            floatAmp * (0.6 * std::cos(mAnimTime * 0.7  + mAnimPhases[i])
                      + 0.4 * std::cos(mAnimTime * 1.618 + mAnimPhases[i] * 1.4)),
            floatAmp * (0.6 * std::sin(mAnimTime * 0.9  + mAnimPhases[i])
                      + 0.4 * std::sin(mAnimTime * 1.414 + mAnimPhases[i] * 0.7))
        );
    };

    // --- Animated centers ---
    QPointF helmetC      = basePos(mHelmetPos)      + animOff(0);
    QPointF breastplateC = basePos(mBreastplatePos) + animOff(1);
    QPointF glovesC      = basePos(mGlovesPos)      + animOff(2);
    QPointF pantsC       = basePos(mPantPos)        + animOff(3);
    QPointF footwearC    = basePos(mFootwearsPos)   + animOff(4);
    QPointF amuletC      = basePos(mAmuletPos)      + animOff(5);
    QPointF weaponC      = basePos(mWeaponPos)      + animOff(6);

    // --- Dot radii ---
    const float ratio = 1.5f;
    const qreal rHelmet      = ratio * 20 / 2.0;
    const qreal rBreastplate = ratio * 30 / 2.0;
    const qreal rGloves      = ratio * 20 / 2.0;
    const qreal rPants       = ratio * 25 / 2.0;
    const qreal rFootwear    = ratio * 20 / 2.0;
    const qreal rAmulet      = ratio * 15 / 2.0;
    const qreal rWeapon      = ratio * 25 / 2.0;

    // --- Draw chaotic wavy connections ---
    // Each connection receives an index to give it its own character
    auto drawWavyLink = [&](const QPointF& a, qreal ra, bool aOn,
                            const QPointF& b, qreal rb, bool bOn,
                            int connIdx)
    {
        const qreal dx  = b.x() - a.x();
        const qreal dy  = b.y() - a.y();
        const qreal len = std::hypot(dx, dy);
        if(len < 1.0) return;

        const qreal ux = dx / len,  uy = dy / len;
        const qreal px = -uy,       py = ux;

        const bool  on        = aOn && bOn;
        const int   numLines  = on ? 4  : 1;
        const qreal amplitude = on ? 9.0 : 4.0;
        const qreal speed     = on ? 2.2 : 0.45;
        const qreal lineWidth = on ? 1.3 : 0.8;
        const QColor colBase  = on ? QColor(255, 200, 50) : QColor(255, 255, 255);

        const QPointF start(a.x() + ux * ra, a.y() + uy * ra);
        const QPointF end  (b.x() - ux * rb, b.y() - uy * rb);
        const qreal segLen = std::hypot(end.x()-start.x(), end.y()-start.y());

        // Phase unique to this connection
        const qreal connPhase = mAnimPhases[connIdx % 7];

        for(int l = 0; l < numLines; ++l)
        {
            // Each line has its own phase and frequency offset → all different
            const qreal lf = l * 1.3;   // per-line phase offset (irrational)

            // 3 sine components with irrational frequencies → pattern never repeats
            const qreal p1 = mAnimTime * speed           + connPhase + lf;
            const qreal p2 = mAnimTime * speed * 1.618   + connPhase * 1.7 + lf * 0.9;
            const qreal p3 = mAnimTime * speed * 2.414   + connPhase * 0.5 + lf * 2.1;

            // Slightly different amplitude per line
            const qreal amp = amplitude * (0.8 + 0.2 * (l % 2 == 0 ? 1.0 : -0.3));

            // Slightly different opacity per line
            const int alpha = on ? (190 - l * 30) : (110 - l * 20);
            QColor col = colBase;
            col.setAlpha(qMax(40, alpha));
            painter.setPen(QPen(col, lineWidth, Qt::SolidLine, Qt::RoundCap));

            QPainterPath path;
            const int steps = 56;
            for(int step = 0; step <= steps; ++step)
            {
                const qreal t     = step / (qreal)steps;
                const qreal along = t * segLen;

                // Envelope: zeroes at both ends → connects cleanly to the dots
                const qreal envelope = std::sin(M_PI * t);

                // Superposition of 3 sine waves (frequencies φ, √2, ~e — irrational ratios)
                const qreal wave =
                    0.50 * std::sin(2.0   * 2.0 * M_PI * t + p1) +
                    0.30 * std::sin(3.141 * 2.0 * M_PI * t + p2) +
                    0.20 * std::sin(5.0   * 2.0 * M_PI * t + p3);

                const qreal sine = amp * envelope * wave;

                const qreal x = start.x() + along * ux + sine * px;
                const qreal y = start.y() + along * uy + sine * py;
                if(step == 0) path.moveTo(x, y);
                else          path.lineTo(x, y);
            }
            painter.drawPath(path);
        }
    };

    // Network connections — each call has its own index
    drawWavyLink(helmetC,      rHelmet,      hasHelmet,      breastplateC, rBreastplate, hasBreastplate, 0);
    drawWavyLink(breastplateC, rBreastplate, hasBreastplate, pantsC,       rPants,       hasPants,       1);
    drawWavyLink(breastplateC, rBreastplate, hasBreastplate, weaponC,      rWeapon,      hasWeapon,      2);
    drawWavyLink(breastplateC, rBreastplate, hasBreastplate, amuletC,      rAmulet,      hasAmulet,      3);
    drawWavyLink(breastplateC, rBreastplate, hasBreastplate, glovesC,      rGloves,      hasGloves,      4);
    drawWavyLink(pantsC,       rPants,       hasPants,       footwearC,    rFootwear,    hasFootwear,    5);

    // --- Draw dots on top of the lines ---
    painter.setPen(Qt::NoPen);
    auto drawDot = [&](const QPointF& c, qreal r, bool on) {
        painter.setBrush(on ? QColor(255, 200, 50, 230) : QColor(255, 255, 255, 160));
        painter.drawEllipse(c, r, r);
    };
    drawDot(helmetC,      rHelmet,      hasHelmet);
    drawDot(glovesC,      rGloves,      hasGloves);
    drawDot(pantsC,       rPants,       hasPants);
    drawDot(footwearC,    rFootwear,    hasFootwear);
    drawDot(amuletC,      rAmulet,      hasAmulet);
    drawDot(weaponC,      rWeapon,      hasWeapon);
    drawDot(breastplateC, rBreastplate, hasBreastplate);
}


Frag_Interface_Gear::~Frag_Interface_Gear()
{
    delete ui;
}


