#include "frag_interface_itemsorter.h"
#include "ui_frag_interface_itemsorter.h"

Frag_Interface_ItemSorter::Frag_Interface_ItemSorter(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Frag_interface_itemSorter)
{
    ui->setupUi(this);

    mScene = new QGraphicsScene(this);
    //mScene->setBackgroundBrush(QPixmap(":/graphicItems/sorter_interface.png"));
    mScene->setSceneRect(QRect(0,0,1000,120));
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->centerOn(0,0);
    ui->graphicsView->setStyleSheet("background-color:rgba(0,0,0,20);");

    mBinItem = new Bin(QPixmap(":/graphicItems/throwBin.png"), QPointF(width()-100,0));
    mScene->addItem(mBinItem);
}

void Frag_Interface_ItemSorter::setSceneDeep(int deep)
{
    mScene->setSceneRect(mScene->sceneRect().x(), mScene->sceneRect().y(), mScene->sceneRect().width(), deep);
}

void Frag_Interface_ItemSorter::addItem(Item * item)
{
    ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
    mItems.append(w_item);
    mScene->addItem(w_item);
    int vOffset = 10, hOffset = 10;
    w_item->setPos(vOffset, hOffset);
    connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(itemClicked(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)));
    connect(w_item, SIGNAL(sig_itemMoved(ItemQuickDisplayer*)), this, SLOT(itemMoved(ItemQuickDisplayer*)));

    while(!w_item->collidingItems().isEmpty())
    {
        hOffset += w_item->boundingRect().width() + 10;
        if(hOffset > 700){
            hOffset = 10;
            vOffset += w_item->boundingRect().height() + 10;
            if(vOffset > mScene->sceneRect().height()-10)
            {
                setSceneDeep(static_cast<int>(vOffset + w_item->boundingRect().height() + 10));
                ui->graphicsView->setSceneRect(mScene->sceneRect());
            }
        }
        w_item->setPos(hOffset, vOffset);
    }
    w_item->setInitialPosition(w_item->pos());
}

void Frag_Interface_ItemSorter::removeQuickItemDisplayers()
{
    for(ItemQuickDisplayer * w_item : mItems)
    {
        mItems.removeOne(w_item);
        delete w_item;
    }
}

void Frag_Interface_ItemSorter::removeQuickItemDisplayer(Item * item)
{
    for(ItemQuickDisplayer * w_item : mItems)
    {
        if(w_item->getItem() == item)
        {
            mItems.removeOne(w_item);
            delete w_item;
        }
    }
    refreshItemsPosition();
}

void Frag_Interface_ItemSorter::refreshItemsPosition()
{
    QList<ItemQuickDisplayer*> newList;
    for(ItemQuickDisplayer * w_item : mItems)
    {
        newList.append(new ItemQuickDisplayer(w_item->getItem()));
        mScene->removeItem(w_item);
        mItems.removeOne(w_item);
        delete w_item;
    }
    for(ItemQuickDisplayer * w_item : newList)
    {
        addItem(w_item->getItem());
    }
}

void Frag_Interface_ItemSorter::unselectItems()
{
    for(ItemQuickDisplayer * item : mItems)
    {
        item->setItemSelected(false);
    }
}

ItemQuickDisplayer * Frag_Interface_ItemSorter::getSelectedItem()
{
    for(ItemQuickDisplayer * item : mItems)
    {
        if(item->isItemSelected())
            return item;
    }

    return nullptr;
}

void Frag_Interface_ItemSorter::itemMoved(ItemQuickDisplayer * w_item)
{
    w_item->setZValue(0);
    if(w_item->collidesWithItem(mBinItem)){
        Item * item = w_item->getItem();
        emit sig_itemThrown(item);
        emit sig_itemClicked(nullptr);
        mItems.removeOne(w_item);
        w_item->deleteLater();
        refreshItemsPosition();
    }else{
        w_item->setPos(w_item->getInitialPosition());
    }
}

void Frag_Interface_ItemSorter::itemClicked(ItemQuickDisplayer * w_item)
{
    w_item->setZValue(1);
    emit sig_itemClicked(w_item);
}

Frag_Interface_ItemSorter::~Frag_Interface_ItemSorter()
{
    delete ui;
    while(!mItems.isEmpty())
        delete mItems.takeLast();
}



Bin::Bin(QPixmap pixmap, QPointF pos):
    QGraphicsPixmapItem(pixmap)
{
    setPos(pos);
}

Bin::~Bin()
{

}

QRectF Bin::boundingRect() const
{
    return QRectF(0,0,100,700);
}

