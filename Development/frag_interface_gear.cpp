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
    mScene->setSceneRect(QRect(0,0,855,420));
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->centerOn(0,0);
    ui->graphicsView->setStyleSheet("background-color:rgba(0,0,0,0);");

    helmetPos = QPointF(xPosSplitter+50+110, 10);
    breastplatePos = QPointF(xPosSplitter+50+110, 110);
    glovesPos = QPointF(xPosSplitter+50+210, 110);
    pantPos = QPointF(xPosSplitter+50+110, 210);
    footwearsPos = QPointF(xPosSplitter+50+110, 310);
    amuletPos = QPointF(xPosSplitter+50+210, 10);
    weaponPos = QPointF(xPosSplitter+50+10, 110);

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
                    area.setRect(static_cast<int>(weaponPos.x()), static_cast<int>(weaponPos.y()), 100,100);
                }
                Helmet * helmet = dynamic_cast<Helmet*>(item->getItem());
                if(helmet)
                {
                    area.setRect(static_cast<int>(helmetPos.x()), static_cast<int>(helmetPos.y()), 100,100);
                }
                Breastplate * breastplate = dynamic_cast<Breastplate*>(item->getItem());
                if(breastplate)
                {
                    area.setRect(static_cast<int>(breastplatePos.x()), static_cast<int>(breastplatePos.y()), 100,100);
                }
                Gloves * gloves = dynamic_cast<Gloves*>(item->getItem());
                if(gloves)
                {
                    area.setRect(static_cast<int>(glovesPos.x()), static_cast<int>(glovesPos.y()), 100,100);
                }
                Pant * pant = dynamic_cast<Pant*>(item->getItem());
                if(pant)
                {
                    area.setRect(static_cast<int>(pantPos.x()), static_cast<int>(pantPos.y()), 100,100);
                }
                Footwears * footwears = dynamic_cast<Footwears*>(item->getItem());
                if(footwears)
                {
                    area.setRect(static_cast<int>(footwearsPos.x()), static_cast<int>(footwearsPos.y()), 100,100);
                }
                Amulet * amulet = dynamic_cast<Amulet*>(item->getItem());
                if(amulet)
                {
                    area.setRect(static_cast<int>(amuletPos.x()), static_cast<int>(amuletPos.y()), 100,100);
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
        w_helmet->setInitialPosition(helmetPos);
        w_helmet->setPos(helmetPos);
        equipmentRightSide.append(w_helmet);
        connect(w_helmet, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_helmet, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getBreastplate()!=nullptr){
        ItemQuickDisplayer * w_breasplate = new ItemQuickDisplayer(hero->getGear()->getBreastplate());
        mScene->addItem(w_breasplate);
        w_breasplate->setInitialPosition(breastplatePos);
        w_breasplate->setPos(breastplatePos);
        equipmentRightSide.append(w_breasplate);
        connect(w_breasplate, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_breasplate, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getGloves()!=nullptr){
        ItemQuickDisplayer * w_gloves = new ItemQuickDisplayer(hero->getGear()->getGloves());
        mScene->addItem(w_gloves);
        w_gloves->setInitialPosition(glovesPos);
        w_gloves->setPos(glovesPos);
        equipmentRightSide.append(w_gloves);
        connect(w_gloves, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_gloves, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getPant()!=nullptr){
        ItemQuickDisplayer * w_pant = new ItemQuickDisplayer(hero->getGear()->getPant());
        mScene->addItem(w_pant);
        w_pant->setInitialPosition(pantPos);
        w_pant->setPos(pantPos);
        equipmentRightSide.append(w_pant);
        connect(w_pant, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_pant, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getFootWears()!=nullptr){
        ItemQuickDisplayer * w_footwears = new ItemQuickDisplayer(hero->getGear()->getFootWears());
        mScene->addItem(w_footwears);
        w_footwears->setInitialPosition(footwearsPos);
        w_footwears->setPos(footwearsPos);
        equipmentRightSide.append(w_footwears);
        connect(w_footwears, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_footwears, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getAmulet()!=nullptr){
        ItemQuickDisplayer * w_amulet = new ItemQuickDisplayer(hero->getGear()->getAmulet());
        mScene->addItem(w_amulet);
        w_amulet->setInitialPosition(amuletPos);
        w_amulet->setPos(amuletPos);
        equipmentRightSide.append(w_amulet);
        connect(w_amulet, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
        connect(w_amulet, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    }
    if(hero->getGear()->getWeapon()!=nullptr){
        ItemQuickDisplayer * w_weapon = new ItemQuickDisplayer(hero->getGear()->getWeapon());
        mScene->addItem(w_weapon);
        w_weapon->setInitialPosition(weaponPos);
        w_weapon->setPos(weaponPos);
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

    painter.drawPixmap(QRect(600, 45, 180, 370), QPixmap(":/graphicItems/gear_hero.png"));

    painter.setPen(QPen(QBrush("#FFFFFF"), 1));
    painter.drawRect(QRect(0,0,width(),height()));
}

Frag_Interface_Gear::~Frag_Interface_Gear()
{
    delete ui;
}


