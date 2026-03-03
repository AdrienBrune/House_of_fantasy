#include "frag_interface_itemtrader.h"
#include "ui_frag_interface_itemtrader.h"

Frag_Interface_ItemTrader::Frag_Interface_ItemTrader(QWidget *parent, QPixmap pictureLeft, QPixmap pictureRight) :
    QWidget(parent),
    mPictureLeft(pictureLeft),
    mPictureRight(pictureRight),
    ui(new Ui::Frag_interface_itemTrader)
{
    ui->setupUi(this);

    mSizeData.horizontalOffset = 20;
    mSizeData.verticalOffset = 0;
    mSizeData.numberRows = 4;
    mSizeData.numberLines = 4;
    mSizeData.deep = mSizeData.numberLines*100+2*mSizeData.verticalOffset;
    mSizeData.xPosSplitter = 2 * mSizeData.horizontalOffset + mSizeData.numberRows * (100 + 5) - 5 + 50;
    
    mScene = new QGraphicsScene(this);
    mScene->setSceneRect(QRect(0, 0, 2*mSizeData.xPosSplitter, mSizeData.deep));
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setSceneRect(mScene->sceneRect());
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->graphicsView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    ui->graphicsView->setMaximumHeight(mSizeData.numberLines*100+(mSizeData.numberLines-1)*5+2*mSizeData.verticalOffset);
    ui->graphicsView->setMinimumHeight(mSizeData.numberLines*100+(mSizeData.numberLines-1)*5+2*mSizeData.verticalOffset);
    ui->graphicsView->setMaximumWidth(2*mSizeData.xPosSplitter);
    ui->graphicsView->setMinimumWidth(2*mSizeData.xPosSplitter);

    ui->graphicsView->setStyleSheet("background-color:rgba(0,0,0,0);border:no border;");
}

void Frag_Interface_ItemTrader::onItemMovedHandler(ItemQuickDisplayer * item)
{
    item->setZValue(0);
    if(mScene->sceneRect().contains(item->boundingRect()))
    {
        if(static_cast<int>(item->getInitialPosition().x()) < mSizeData.xPosSplitter)
        {
               if(item->x() > mSizeData.xPosSplitter)
               {
                   moveItemLeftToRight(item);
                   emit sig_itemSwipedToRight(item);
               }else
                   item->setPos(item->getInitialPosition());
        }
        else
        {
            if(item->x() < mSizeData.xPosSplitter)
            {
                moveItemRightToLeft(item);
                emit sig_itemSwipedToLeft(item);
            }else
                item->setPos(item->getInitialPosition());
        }
    }
    else
        item->setPos(item->getInitialPosition());
}

void Frag_Interface_ItemTrader::onItemSelected(ItemQuickDisplayer * item)
{
    item->setZValue(1);
    emit sig_itemClicked(item);
}

void Frag_Interface_ItemTrader::onItemRightClicked(ItemQuickDisplayer* itemDisplayer)
{
    QList<Item*> itemsLeft = getItemsLeftSide();
    QList<Item*> itemsRight = getItemsRightSide();

    for(Item *item : qAsConst(itemsLeft))
    {
        if(item == itemDisplayer->getItem())
        {
            moveItemLeftToRight(itemDisplayer);
            emit sig_itemSwipedToRight(itemDisplayer);
        }
    }
    for(Item *item : qAsConst(itemsRight))
    {
        if(item == itemDisplayer->getItem())
        {
            moveItemRightToLeft(itemDisplayer);
            emit sig_itemSwipedToLeft(itemDisplayer);
        }
    }
}

void Frag_Interface_ItemTrader::setSceneDeep(int deep)
{
    mScene->setSceneRect(mScene->sceneRect().x(), mScene->sceneRect().y(), mScene->sceneRect().width(), deep);
    update();
}

void Frag_Interface_ItemTrader::addItemLeftSide(Item * item)
{
    ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
    mItemsLeftSide.append(w_item);
    mScene->addItem(w_item);

    int vOffset = mSizeData.verticalOffset, hOffset = mSizeData.horizontalOffset;
    w_item->setPos(hOffset, vOffset);
    connect(w_item, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += static_cast<int>(w_item->boundingRect().width()) + 5;
        if(hOffset >= mSizeData.xPosSplitter - 50 - mSizeData.horizontalOffset)
        {
            hOffset = mSizeData.horizontalOffset;
            vOffset += static_cast<int>(w_item->boundingRect().height()) + 5;
            if(vOffset + 105 > mScene->sceneRect().height())
            {
                setSceneDeep(static_cast<int>(vOffset + w_item->boundingRect().height() + mSizeData.verticalOffset));
                ui->graphicsView->setSceneRect(mScene->sceneRect());
            }
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());
    ui->graphicsView->centerOn(w_item->pos());

    rearangeItems();
}

void Frag_Interface_ItemTrader::addItemRightSide(Item * item)
{
    ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);

    mItemsRightSide.append(w_item);
    mScene->addItem(w_item);

    int vOffset = mSizeData.verticalOffset;
    int hOffset = mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset;
    w_item->setPos(hOffset, vOffset);
    connect(w_item, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(onItemMovedHandler(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(onItemSelected(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemRightClicked(ItemQuickDisplayer*)), this, SLOT(onItemRightClicked(ItemQuickDisplayer*)));

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += static_cast<int>(w_item->boundingRect().width()) + 5;
        if(hOffset >= mSizeData.xPosSplitter + mSizeData.horizontalOffset + mSizeData.numberRows*(100 + 5) - 5)
        {
            hOffset = mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset;
            vOffset += static_cast<int>(w_item->boundingRect().height()) + 5;
            if(vOffset + 105 > mScene->sceneRect().height())
            {
                setSceneDeep(static_cast<int>(vOffset + w_item->boundingRect().height() + mSizeData.verticalOffset));
                ui->graphicsView->setSceneRect(mScene->sceneRect());
            }
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());

    rearangeItems();
}

void Frag_Interface_ItemTrader::addItemsLeftSide(QList<Item *> items)
{
    for(Item * item : items)
    {
        addItemLeftSide(item);
    }
    ui->graphicsView->centerOn(0,0);
}

void Frag_Interface_ItemTrader::addItemsRightSide(QList<Item *> items)
{
    for(Item * item : items)
    {
        addItemRightSide(item);
    }
    ui->graphicsView->centerOn(0,0);
}

QList<Item *> Frag_Interface_ItemTrader::getItemsLeftSide()
{
    QList<Item*> itemsToFind;
    QList<QGraphicsItem*> items = mScene->items();
    foreach(QGraphicsItem * item, items)
    {
        if(item->x() < mSizeData.xPosSplitter)
        {
            ItemQuickDisplayer * leftItem = dynamic_cast<ItemQuickDisplayer*>(item);
            if(leftItem){
                itemsToFind.append(leftItem->getItem());
            }
        }
    }
    return itemsToFind;
}

QList<Item *> Frag_Interface_ItemTrader::getItemsRightSide()
{
    QList<Item*> itemsToFind;
    QList<QGraphicsItem*> items = mScene->items();
    foreach(QGraphicsItem * item, items)
    {
        if(item->x() > mSizeData.xPosSplitter)
        {
            ItemQuickDisplayer * rightItem = dynamic_cast<ItemQuickDisplayer*>(item);
            if(rightItem){
                itemsToFind.append(rightItem->getItem());
            }
        }
    }
    return itemsToFind;
}

void Frag_Interface_ItemTrader::removeItemRightSide(Item * itemToRemove)
{
    for(ItemQuickDisplayer * item : qAsConst(mItemsRightSide))
    {
        if(item->getItem() == itemToRemove)
        {
            mScene->removeItem(item);
            mItemsRightSide.removeOne(item);
            delete item;
        }
    }
    rearangeItems();
}

void Frag_Interface_ItemTrader::removeItemLeftSide(Item * itemToRemove)
{
    for(ItemQuickDisplayer * item : qAsConst(mItemsLeftSide))
    {
        if(item->getItem() == itemToRemove)
        {
            mScene->removeItem(item);
            mItemsLeftSide.removeOne(item);
            delete item;
        }
    }
    rearangeItems();
}

void Frag_Interface_ItemTrader::rearangeItems()
{
    for(ItemQuickDisplayer * item : mItemsLeftSide)
    {
        int vOffset = mSizeData.verticalOffset;
        int hOffset = mSizeData.horizontalOffset;
        item->setPos(hOffset, vOffset);

        while(!item->collidingItems().isEmpty())
        {
            hOffset += static_cast<int>(item->boundingRect().width()) + 5;
            if(hOffset >= mSizeData.horizontalOffset + mSizeData.numberRows*(100 + 5) - 5)
            {
                hOffset = mSizeData.horizontalOffset;
                vOffset += static_cast<int>(item->boundingRect().height()) + 5;
                if(vOffset + 105 > mScene->height())
                {
                    setSceneDeep(static_cast<int>(vOffset + item->boundingRect().height() + mSizeData.verticalOffset));
                    ui->graphicsView->setSceneRect(mScene->sceneRect());
                }
            }
            item->setPos(hOffset, vOffset);
        }
        item->setInitialPosition(item->pos());
    }

    for(ItemQuickDisplayer * item : mItemsRightSide)
    {
        int vOffset = mSizeData.verticalOffset;
        int hOffset = mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset;
        item->setPos(hOffset, vOffset);

        while(!item->collidingItems().isEmpty())
        {
            hOffset += static_cast<int>(item->boundingRect().width()) + 5;
            if(hOffset >= mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset + mSizeData.numberRows*(100 + 5) - 5)
            {
                hOffset = mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset;
                vOffset += static_cast<int>(item->boundingRect().height()) + 5;
                if(vOffset + 105 > mScene->height())
                {
                    setSceneDeep(static_cast<int>(vOffset + item->boundingRect().height() + mSizeData.verticalOffset));
                    ui->graphicsView->setSceneRect(mScene->sceneRect());
                }
            }
            item->setPos(hOffset, vOffset);
        }
        item->setInitialPosition(item->pos());
    }
}

void Frag_Interface_ItemTrader::moveItemLeftToRight(ItemQuickDisplayer * w_item)
{
    mItemsLeftSide.removeOne(w_item);
    mItemsRightSide.append(w_item);

    int vOffset = mSizeData.verticalOffset, hOffset = mSizeData.xPosSplitter+50+mSizeData.horizontalOffset;
    w_item->setPos(hOffset, vOffset);

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += static_cast<int>(w_item->boundingRect().width()) + 5;
        if(hOffset >= mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset + mSizeData.numberRows*(100 + 5) - 5)
        {
            hOffset = mSizeData.xPosSplitter + 50 + mSizeData.horizontalOffset;
            vOffset += static_cast<int>(w_item->boundingRect().height()) + 5;
            if(vOffset+105 > mScene->height())
            {
                setSceneDeep(static_cast<int>(vOffset + w_item->boundingRect().height() + mSizeData.verticalOffset));
                ui->graphicsView->setSceneRect(mScene->sceneRect());
            }
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());

    rearangeItems();
}

void Frag_Interface_ItemTrader::moveItemRightToLeft(ItemQuickDisplayer * w_item)
{
    mItemsRightSide.removeOne(w_item);
    mItemsLeftSide.append(w_item);

    int vOffset = mSizeData.verticalOffset, hOffset = mSizeData.horizontalOffset;
    w_item->setPos(hOffset, vOffset);

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += static_cast<int>(w_item->boundingRect().width()) + 5;
        if(hOffset >= mSizeData.horizontalOffset + mSizeData.numberRows*(100 + 5) - 5)
        {
            hOffset = mSizeData.horizontalOffset;
            vOffset += static_cast<int>(w_item->boundingRect().height()) + 5;
            if(vOffset+105 > mScene->height())
            {
                setSceneDeep(static_cast<int>(vOffset + w_item->boundingRect().height() + mSizeData.verticalOffset));
                ui->graphicsView->setSceneRect(mScene->sceneRect());
            }
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());

    rearangeItems();
}

void Frag_Interface_ItemTrader::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int widthOneSide = width()/2-50;

    painter.drawPixmap(QRect(0, 0, width(), static_cast<int>(50*width()/1000.0)/6), QPixmap(":/graphicItems/whiteLine.png"));
    painter.drawPixmap(QRect(0, height()-static_cast<int>(50*width()/1000.0)/6, width(), static_cast<int>(50*width()/1000.0)/6), QPixmap(":/graphicItems/whiteLine.png"));

    painter.drawPixmap(QRect((widthOneSide-300)/2, (height()-300)/2, 300, 300), mPictureLeft);
    painter.drawPixmap(QRect(widthOneSide+(widthOneSide-300)/2+100, (height()-300)/2, 300, 300), mPictureRight);
}

Frag_Interface_ItemTrader::~Frag_Interface_ItemTrader()
{
    while(!mItemsLeftSide.isEmpty())
    {
        delete mItemsLeftSide.takeLast();
    }
    while(!mItemsRightSide.isEmpty())
    {
        delete mItemsRightSide.takeLast();
    }
}

