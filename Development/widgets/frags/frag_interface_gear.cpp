#include "frag_interface_gear.h"
#include "ui_frag_interface_gear.h"
#include "entitieshandler.h"

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

                ItemQuickDisplayer * equipmentArea = new ItemQuickDisplayer(new Helmet("", QPixmap(""), 0, 0, 0, 0, "")); // dummy item
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

void Frag_Interface_Gear::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // --- Définition des tailles de ronds ---
    float ratio = 1.5f;
    int sizeHelmet      = int(ratio * 20);
    int sizeBreastplate = int(ratio * 30);
    int sizeGloves      = int(ratio * 20);
    int sizePants       = int(ratio * 25);
    int sizeFootwear    = int(ratio * 20);
    int sizeAmulet      = int(ratio * 15);
    int sizeWeapon      = int(ratio * 25);

    // Taille des emplacements (100x100) et demi-slot
    const int slotSize = 100;
    const qreal slotHalf = slotSize / 2.0;

    // --- Conversion coordonnées scène → widget (centrées) ---
    // on ajoute slotHalf pour obtenir le centre du slot en scène, puis mapFromScene
    QPoint helmetPt      = ui->graphicsView->mapFromScene(mHelmetPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint breastplatePt = ui->graphicsView->mapFromScene(mBreastplatePos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint glovesPt      = ui->graphicsView->mapFromScene(mGlovesPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint pantsPt       = ui->graphicsView->mapFromScene(mPantPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint footwearPt    = ui->graphicsView->mapFromScene(mFootwearsPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint amuletPt      = ui->graphicsView->mapFromScene(mAmuletPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));
    QPoint weaponPt      = ui->graphicsView->mapFromScene(mWeaponPos + QPointF(slotHalf, slotHalf) + QPoint(ui->graphicsView->x(), ui->graphicsView->y()));

    // Convertir en QPointF pour calculs flottants
    QPointF helmetC      = QPointF(helmetPt);
    QPointF breastplateC = QPointF(breastplatePt);
    QPointF glovesC      = QPointF(glovesPt);
    QPointF pantsC       = QPointF(pantsPt);
    QPointF footwearC    = QPointF(footwearPt);
    QPointF amuletC      = QPointF(amuletPt);
    QPointF weaponC      = QPointF(weaponPt);

    // rayons
    qreal rHelmet      = sizeHelmet / 2.0;
    qreal rBreastplate = sizeBreastplate / 2.0;
    qreal rGloves      = sizeGloves / 2.0;
    qreal rPants       = sizePants / 2.0;
    qreal rFootwear    = sizeFootwear / 2.0;
    qreal rAmulet      = sizeAmulet / 2.0;
    qreal rWeapon      = sizeWeapon / 2.0;

    // --- Dessin des connexions (traits) ---
    painter.setPen(QPen(QBrush("#FFFFFF"), 2, Qt::SolidLine, Qt::RoundCap));

    // lambda pour dessiner une ligne qui s'arrête au bord des cercles (esthétique)
    auto drawLink = [&](const QPointF &a, qreal ra, const QPointF &b, qreal rb) {
        qreal dx = b.x() - a.x();
        qreal dy = b.y() - a.y();
        qreal len = std::hypot(dx, dy);
        if (len <= 0.0001) {
            // Points confondus -> rien à dessiner
            return;
        }
        qreal ux = dx / len;
        qreal uy = dy / len;
        QPointF p1(a.x() + ux * ra, a.y() + uy * ra); // départ au bord du 1er cercle
        QPointF p2(b.x() - ux * rb, b.y() - uy * rb); // fin au bord du 2ème cercle
        painter.drawLine(p1, p2);
    };

    // connexions demandées
    drawLink(helmetC, rHelmet,      breastplateC, rBreastplate);
    drawLink(breastplateC, rBreastplate, pantsC, rPants);
    drawLink(breastplateC, rBreastplate, weaponC, rWeapon);
    drawLink(breastplateC, rBreastplate, amuletC, rAmulet);
    drawLink(breastplateC, rBreastplate, glovesC, rGloves);
    drawLink(pantsC, rPants, footwearC, rFootwear);

    // --- Dessin des ronds blancs (au-dessus des lignes) ---
    painter.setBrush(QBrush("#FFFFFF"));
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(helmetC, rHelmet, rHelmet);
    painter.drawEllipse(glovesC, rGloves, rGloves);
    painter.drawEllipse(pantsC, rPants, rPants);
    painter.drawEllipse(footwearC, rFootwear, rFootwear);
    painter.drawEllipse(amuletC, rAmulet, rAmulet);
    painter.drawEllipse(weaponC, rWeapon, rWeapon);
    painter.drawEllipse(breastplateC, rBreastplate, rBreastplate);
}


Frag_Interface_Gear::~Frag_Interface_Gear()
{
    delete ui;
}


