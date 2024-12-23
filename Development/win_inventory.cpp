#include "win_inventory.h"
#include "ui_win_inventory.h"

#include "entitieshandler.h"

Win_Inventory::Win_Inventory(QWidget *parent) :
    QWidget(parent),
    mItemShownRect(QRect(0,0,0,0)),
    mItemToDisplay(nullptr),
    mItemSelected(0),
    mShowItemDescription(0),
    ui(new Ui::Win_inventory)
{
    ui->setupUi(this);
    ui->item_info->setWordWrap(true);

    mItemsContainer = new Frag_Interface_ItemSorter(this);
    connect(mItemsContainer, SIGNAL(sig_itemThrown(Item*)), this, SIGNAL(sig_itemThrown(Item*)));
    connect(mItemsContainer, SIGNAL(sig_itemThrown(Item*)), this, SLOT(onItemThrown(Item*)));
    connect(mItemsContainer, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(showItem(ItemQuickDisplayer*)));
    connect(mItemsContainer, SIGNAL(sig_itemDoubleClicked(ItemQuickDisplayer*)), this, SLOT(useItem(ItemQuickDisplayer*)));
    connect(mItemsContainer, SIGNAL(sig_itemHoverIn(ItemQuickDisplayer*)), this, SLOT(showItemHover(ItemQuickDisplayer*)));
    connect(mItemsContainer, SIGNAL(sig_itemHoverOut(ItemQuickDisplayer*)), this, SLOT(hideItemHover(ItemQuickDisplayer*)));
    ui->layoutItems->addWidget(mItemsContainer, 0, Qt::AlignCenter);

    Hero * hero = EntitiesHandler::getInstance().getHero();
    Bag * bag = hero->getBag();
    QList<Weapon*> weapons = bag->getItemList<Weapon*>();
    for(Item * item : bag->getItemList<Item*>())
    {
        mItemsContainer->addItem(item);
    }

    hide();
}

void Win_Inventory::showItem(ItemQuickDisplayer * item)
{
    if(item == nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
        ui->item_info->setStyleSheet("QLabel{}");
        ui->item_info->setText("");
        return;
    }

    ItemQuickDisplayer * lastItemSelected = mItemsContainer->getSelectedItem();

    mItemsContainer->unselectItems();

    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }

    mItemToDisplay = new W_ItemDisplayer(this, item->getItem());
    mItemToDisplay->setGeometry(mItemShownRect);
    connect(mItemToDisplay, SIGNAL(sig_itemUsed(Item*)), this, SLOT(useItem(Item*)));
    mItemToDisplay->show();

    ui->item_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->item_info->setText(item->getItem()->getInformation());

    if(lastItemSelected != nullptr && lastItemSelected == item)
    {
        mItemSelected = 0;
        item->setItemSelected(false);

    }else{
        mItemSelected = 1;
        item->setItemSelected(true);
    }
}

void Win_Inventory::showItemHover(ItemQuickDisplayer * item)
{
    if(item == nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
        ui->item_info->setStyleSheet("QLabel{}");
        ui->item_info->setText("");
        mShowItemDescription = 0;
        return;
    }

    if(mItemSelected != 0)
        return;

    if(mItemToDisplay != nullptr)
        delete mItemToDisplay;

    mItemToDisplay = new W_ItemDisplayer(this, item->getItem());
    mItemToDisplay->setGeometry(mItemShownRect);

    mItemToDisplay->show();

    ui->item_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->item_info->setText(item->getItem()->getInformation());
    mShowItemDescription = 1;
}

void Win_Inventory::hideItemHover(ItemQuickDisplayer * item)
{
    if(item == nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
        ui->item_info->setStyleSheet("QLabel{}");
        ui->item_info->setText("");
        mShowItemDescription = 0;
        return;
    }

    if(mItemSelected != 0)
        return;

    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }
    ui->item_info->setStyleSheet("QLabel{}");
    ui->item_info->setText("");
    mShowItemDescription = 0;
}

void Win_Inventory::useItem(ItemQuickDisplayer * itemDisplayer)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    /* Check usable item */
    if(!itemDisplayer->getItem()->isUsable())
        return;

    /* Hide item shown */
    mItemsContainer->unselectItems();
    mItemSelected = 0;
    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
    }
    mItemToDisplay = nullptr;
    ui->item_info->setStyleSheet("QLabel{}");
    ui->item_info->setText("");
    mShowItemDescription = 0;

    /* Use item */
    Item * item = itemDisplayer->getItem();
    Consumable * consumable = dynamic_cast<Consumable*>(item);
    if(consumable)
    {
        mItemsContainer->removeQuickItemDisplayer(item);
        hero->useConsumable(consumable);
        return;
    }
    Tool * tool = dynamic_cast<Tool*>(item);
    if(tool)
    {
        emit sig_useTool(tool);
        closeInventory();
        return;
    }
    Scroll * scroll = dynamic_cast<Scroll*>(item);
    if(scroll)
    {
        mItemsContainer->removeQuickItemDisplayer(item);
        emit sig_useScroll(scroll);
        closeInventory();
        return;
    }
}

void Win_Inventory::useItem(Item * item)
{
    Hero * hero = EntitiesHandler::getInstance().getHero();

    mItemsContainer->unselectItems();
    mItemSelected = 0;

    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }
    ui->item_info->setStyleSheet("QLabel{}");
    ui->item_info->setText("");
    mShowItemDescription = 0;

    Consumable * consumable = dynamic_cast<Consumable*>(item);
    if(consumable)
    {
        mItemsContainer->removeQuickItemDisplayer(item);
        hero->useConsumable(consumable);
        return;
    }
    Tool * tool = dynamic_cast<Tool*>(item);
    if(tool)
    {
        emit sig_useTool(tool);
        closeInventory();
        return;
    }
    Scroll * scroll = dynamic_cast<Scroll*>(item);
    if(scroll)
    {
        mItemsContainer->removeQuickItemDisplayer(item);
        emit sig_useScroll(scroll);
        closeInventory();
        return;
    }
}

void Win_Inventory::onItemThrown(Item *)
{
    mItemsContainer->unselectItems();
    mItemSelected = 0;

    if(mItemToDisplay != nullptr)
    {
        delete mItemToDisplay;
        mItemToDisplay = nullptr;
    }
    ui->item_info->setStyleSheet("QLabel{}");
    ui->item_info->setText("");
    mShowItemDescription = 0;
}

void Win_Inventory::diplayInventory()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    show();

    mItemShownRect = QRect(ui->itemArea->x(), ui->itemArea->y(), ui->itemArea->rect().width(), ui->itemArea->height());
}

void Win_Inventory::closeInventory()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(400);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_closeWindow()));
    timer->start(1000);
}

void Win_Inventory::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setOpacity(0.95);
    painter.drawPixmap(QRect(50, ui->title->y(), width()-100, ui->itemArea->y()-40-ui->title->y()), QPixmap(":/graphicItems/background_window_2.png"));

    painter.setPen(QPen(QBrush(Qt::white), 2));
    painter.drawRect(QRect(50, ui->title->y(), width()-100, ui->itemArea->y()-40-ui->title->y()));

    painter.setOpacity(0.2);
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(QPen());
    painter.drawRect(QRect(ui->title->x(), ui->title->y(), ui->title->width(), ui->title->height()));

    painter.setOpacity(1);

    uint16_t sizeTitle = ui->title->height();
    painter.drawPixmap(QRect(ui->title->x()-0.4*sizeTitle, ui->title->y()-sizeTitle*0.5, sizeTitle*2, sizeTitle*2),QPixmap(":/images/bag.png"));

    painter.setBrush(QBrush());
    painter.setPen(QPen(QBrush(Qt::white), 3));
    painter.drawRect(QRect(mItemsContainer->x(), mItemsContainer->y(), mItemsContainer->width(), mItemsContainer->height()));

    if(mShowItemDescription)
    {
        painter.drawPixmap(QRect(ui->item_info->x(), ui->item_info->y(), 100, 200), QPixmap(":/graphicItems/background_itemInfo_left.png"));
        painter.drawPixmap(QRect(ui->item_info->x()+100, ui->item_info->y(), ui->item_info->width()-200, 200), QPixmap(":/graphicItems/background_itemInfo_middle.png"));
        painter.drawPixmap(QRect(ui->item_info->x()+ui->item_info->width()-100, ui->item_info->y(), 100, 200), QPixmap(":/graphicItems/background_itemInfo_right.png"));
    }
}

Win_Inventory::~Win_Inventory()
{
    delete ui;
}

