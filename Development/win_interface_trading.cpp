#include "win_interface_trading.h"
#include "ui_win_chest.h"
#include "ui_win_blacksmith.h"
#include "ui_win_merchant.h"
#include "ui_win_alchemist.h"
#include "qcustom_widget.h"
#include "village.h"
#include "w_itemdisplayer.h"
#include "entitieshandler.h"

Win_Interface_Trading::Win_Interface_Trading(QWidget *parent) :
    QWidget(parent),
    mHero(nullptr)
{
    mHero = EntitesHandler::getInstance().getHero();
}

Win_Interface_Trading::~Win_Interface_Trading()
{

}

void Win_Interface_Trading::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,QPixmap(":/graphicItems/screenBackground.png"));

    painter.setOpacity(0.5);
    painter.drawPixmap(QRect(mItemTrader->x(), mItemTrader->y(), mItemTrader->width(), mItemTrader->height()),
                       QPixmap(":/graphicItems/itemTrader.png"));
}





Win_Chest::Win_Chest(QWidget *parent, ChestEvent * chest) :
    Win_Interface_Trading(parent),
    mChest(chest),
    ui(new Ui::Win_chest)
{
    mItemTrader = new Frag_Interface_ItemTrader(this);
    ui->setupUi(this);
    mChest->openChest(true);
    initInterface();
    emit sig_playSound(SOUND_OPENCHEST);
}

void Win_Chest::initInterface()
{
    ui->layout_itemTrader->addWidget(mItemTrader, 0, Qt::AlignCenter);
    mItemTrader->addItemsRightSide(mChest->getItems());
}

void Win_Chest::closeChest()
{
    QList<Item*> list = mItemTrader->getItemsLeftSide();
    for(Item * item : list)
    {
        mHero->takeItem(mChest->takeItem(item));
    }
    mChest->openChest(false);
    emit sig_playSound(SOUND_CLOSECHEST);
}

Win_Chest::~Win_Chest()
{
    delete ui;
}


#define WINDOW_OFFSET   50

Win_BlackSmith::Win_BlackSmith(QWidget * parent):
    Win_Interface_Trading(parent),
    mBlacksmith(nullptr),
    mItemSold(nullptr),
    mItemDisplayer(nullptr),
    mHoverPayment(false),
    mHoverUnknownItem(false),
    ui(new Ui::Win_Blacksmith)
{
    mBlacksmith = EntitesHandler::getInstance().getMap()->getVillage()->getBlacksmith();

    ui->setupUi(this);
    connect(ui->ItemCenter, SIGNAL(sig_hoverIn()), this, SLOT(showItem()));
    connect(ui->ItemCenter, SIGNAL(sig_hoverOut()), this, SLOT(hideItem()));
    connect(ui->button_validate, SIGNAL(clicked()), this, SLOT(on_button_validate_clicked()));
    connect(ui->button_validate, SIGNAL(sig_hoverIn()), this, SLOT(paymentHoverIn()));
    connect(ui->button_validate, SIGNAL(sig_hoverOut()), this, SLOT(paymentHoverOut()));
    connect(mBlacksmith, SIGNAL(sig_replenish(QObject*)), this, SLOT(updateWindow()));

    /* Resplenish timer display */
    ui->chronoResplenish->display(mBlacksmith->getTimeBeforeResplenish());
    t_chronoResplenish = new QTimer(this);
    connect(t_chronoResplenish, &QTimer::timeout, this, [&](){
        ui->chronoResplenish->display(mBlacksmith->getTimeBeforeResplenish());
    } );
    t_chronoResplenish->start(1000);
}

Win_BlackSmith::~Win_BlackSmith()
{
    delete ui;
}

void Win_BlackSmith::on_button_validate_clicked()
{
    /* Check if the item is already solds */
    if(!mBlacksmith->getEquipmentToCraft()->getEquipment())
        return;

    /* Check payment */
    for(int i = 0 ; i < mBlacksmith->getEquipmentToCraft()->getLootList().size(); i++)
    {
        if(mBlacksmith->getEquipmentToCraft()->getLootList()[i].quantities > mHero->getBag()->getQuantityOf(mBlacksmith->getEquipmentToCraft()->getLootList()[i].material))
            return;
    }

    W_Animation_Forge * forgeAnimation = new W_Animation_Forge(this);
    connect(forgeAnimation, SIGNAL(sig_playSound(int)), this, SIGNAL(sig_playSound(int)));
    forgeAnimation->setGeometry(0, 0, width(), height());
    forgeAnimation->show();

    /* Get payment */
    for(int i = 0 ; i < mBlacksmith->getEquipmentToCraft()->getLootList().size(); i++)
    {
        for(int j = 0; j < mBlacksmith->getEquipmentToCraft()->getLootList()[i].quantities; j++)
        {
            Item * item = mHero->getBag()->takeItemWithIdentifier(mBlacksmith->getEquipmentToCraft()->getLootList()[i].material->getIdentifier());
            if(item)
            {
                delete item;
            }
            else
            {
                DEBUG_ERR("Erreur paiement de l'équipement !");
            }
        }
    }
    mHero->setCoin(mHero->getCoin() - mBlacksmith->getEquipmentToCraft()->getCraftingPrice());

    mItemSold = mBlacksmith->getEquipmentToCraft()->takeEquipment();
    if(!mHero->getBag()->addItem(mItemSold))
    {
        emit sig_itemThrown(mItemSold);
    }
    update();
}

void Win_BlackSmith::showItem()
{
#define ITEM_DISPLAYER_SIZE 200
    if(mItemSold)
    {
        mItemDisplayer = new W_ItemDisplayer(this, mItemSold);
        mItemDisplayer->setGeometry(100, (height()-2*WINDOW_OFFSET)/2-ITEM_DISPLAYER_SIZE/2, ITEM_DISPLAYER_SIZE, ITEM_DISPLAYER_SIZE);
        mItemDisplayer->setMinimumSize(QSize(300,300));
        mItemDisplayer->setMaximumSize(QSize(300,300));
        mItemDisplayer->show();
        mItemDisplayer->hideUseButton();
    }
    update();
}

void Win_BlackSmith::hideItem()
{
    if(mItemDisplayer)
    {
        delete mItemDisplayer;
    }
    update();
}

void Win_BlackSmith::paymentHoverIn()
{
    mHoverPayment = true;
    update();
}

void Win_BlackSmith::paymentHoverOut()
{
    mHoverPayment = false;
    update();
}

#define SETUP_MATERIAL_DISPLAY(index)    textColor = 0; \
                                         iconMaterialSize = SIZE_MATERIAL_NORMAL; \
                                         mHoverPayment ? ((equipment->getLootList()[index].quantities <= mHero->getBag()->getQuantityOf(equipment->getLootList()[index].material)) ? iconMaterialSize = SIZE_MATERIAL_GROWN : textColor = 1) : iconMaterialSize = SIZE_MATERIAL_NORMAL ; \
                                         textColor ? painter.setPen(QPen(Qt::red, 1)) : painter.setPen(QPen(Qt::white, 1));
#define SETUP_COIN_DISPLAY()             textColor = 0; \
                                         iconMaterialSize = SIZE_MATERIAL_NORMAL; \
                                         mHoverPayment ? ((equipment->getCraftingPrice() <= mHero->getCoin()) ? iconMaterialSize = SIZE_MATERIAL_GROWN : textColor = 1) : iconMaterialSize = SIZE_MATERIAL_NORMAL ; \
                                         textColor ? painter.setPen(QPen(Qt::red, 1)) : painter.setPen(QPen(Qt::white, 1));

void Win_BlackSmith::paintEvent(QPaintEvent *)
{
#define SIZE_MATERIAL_NORMAL    100
#define SIZE_MATERIAL_GROWN     140
    QPainter painter(this);
    QRect boundingRect;
    int iconMaterialSize = 100, textColor = 0;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(WINDOW_OFFSET,WINDOW_OFFSET,width()-2*WINDOW_OFFSET,height()-2*WINDOW_OFFSET), QPixmap(":/graphicItems/background_window_1.png"));

    painter.drawPixmap(QRect(width()-480, height()-600, 480, 600), QPixmap(":/images/blacksmith_character.png"));

    /* Forge area perimeter definition */
    quint16 diameter = 0.75*height();
    QRect forgeArea(QRect((width()-diameter)/2, (height()-diameter)/2, diameter, diameter));

    /* First circle */
    painter.setPen(QPen(Qt::white, 12));
    painter.setBrush(QBrush());
    painter.drawEllipse(QRect(ui->ItemCenter->x(), ui->ItemCenter->y(), ui->ItemCenter->width(), ui->ItemCenter->height()));

    /* Second circle */
    painter.setPen(QPen(Qt::white, 1));
    painter.drawEllipse(forgeArea);

    if(!mBlacksmith->getEquipmentToCraft())
        return;

    /* Display item at center */
    QRect itemCenterRect(ui->ItemCenter->x(), ui->ItemCenter->y(), ui->ItemCenter->width(), ui->ItemCenter->height());
    if(mBlacksmith->getEquipmentToCraft()->getEquipment())
    {
        /* Item not sold already */
        switch(mBlacksmith->getEquipmentToCraft()->getEquipment()->getIdentifier())
        {
            case ARMOR_HELMET:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_helmet.png"));
                break;

            case ARMOR_BREASTPLATE:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_breastplate.png"));
                break;

            case ARMOR_FOOTWEARS:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_footwears.png"));
                break;

            case ARMOR_GLOVES:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_gloves.png"));
                break;

            case ARMOR_AMULET:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_amulet.png"));
                break;

            case ARMOR_PANT:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_pant.png"));
                break;

            case WEAPON_BOW:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_bow.png"));
                break;

            case WEAPON_STAFF:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_staff.png"));
                break;

            case WEAPON_SWORD:
                painter.drawPixmap(itemCenterRect, QPixmap(":/images/forge_sword.png"));
                break;
        }
    }
    else
    {
        if(mItemSold)
        {
            /* Item sold to display */
            painter.drawPixmap(itemCenterRect, QPixmap(mItemSold->getImage()));
        }
        else
        {
            /* Shop empty */
            painter.setPen(QPen(Qt::white, 1));
            painter.setFont(QFont("Sitka Small", 12));
            painter.drawText(itemCenterRect, Qt::AlignCenter, "En attente de\nréaprovisionnement ...", &boundingRect);
        }
    }

    if(mItemSold || !mBlacksmith->getEquipmentToCraft()->getEquipment())
        return;

    EquipmentToForge * equipment = mBlacksmith->getEquipmentToCraft();
    if(!equipment)
        return;

    painter.setFont(QFont("Sitka Small", 8));

#define TEXT_HEIGHT 35
    /* Coins always at bottom */
    SETUP_COIN_DISPLAY();
    QRect IconCoinRect = QRect(forgeArea.x()+forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()+forgeArea.height()-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
    painter.drawPixmap(IconCoinRect, QPixmap(":/images/coinBag.png"));
    painter.drawText(QRect(IconCoinRect.x()-IconCoinRect.width()/2,IconCoinRect.y()-TEXT_HEIGHT,IconCoinRect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, "Coin", &boundingRect);
    painter.drawText(QRect(IconCoinRect.x()-IconCoinRect.width()/2,IconCoinRect.y()+IconCoinRect.height(),IconCoinRect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getCoin()).arg(equipment->getCraftingPrice()), &boundingRect);

    /* Materials required */
    QRect Icon1Rect, Icon2Rect, Icon3Rect, Icon4Rect;
    switch(equipment->getLootList().size())
    {
    case 1: /* π/2 */
        SETUP_MATERIAL_DISPLAY(0);
        Icon1Rect = QRect(forgeArea.x()+forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon1Rect, equipment->getLootList()[0].material->getImage());
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()-TEXT_HEIGHT,Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[0].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()+Icon1Rect.height(),Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[0].material)).arg(equipment->getLootList()[0].quantities), &boundingRect);
        break;

     case 2: /* π/6 - 5π/6 */
        SETUP_MATERIAL_DISPLAY(0);
        Icon1Rect = QRect(forgeArea.x()+(forgeArea.width()/2-(forgeArea.width()/2*sqrt(3)/2))-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/4-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon1Rect, equipment->getLootList()[0].material->getImage());
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()-TEXT_HEIGHT,Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[0].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()+Icon1Rect.height(),Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[0].material)).arg(equipment->getLootList()[0].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(1);
        Icon2Rect = QRect(forgeArea.x()+forgeArea.width()-(forgeArea.width()/2-(forgeArea.width()/2*sqrt(3)/2))-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/4-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon2Rect, equipment->getLootList()[1].material->getImage());
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()-TEXT_HEIGHT,Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[1].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()+Icon2Rect.height(),Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[1].material)).arg(equipment->getLootList()[1].quantities), &boundingRect);
        break;

     case 3: /* 2π - π/2 - π */
        SETUP_MATERIAL_DISPLAY(0);
        Icon1Rect = QRect(forgeArea.x()-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon1Rect, equipment->getLootList()[0].material->getImage());
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()-TEXT_HEIGHT,Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[0].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()+Icon1Rect.height(),Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[0].material)).arg(equipment->getLootList()[0].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(1);
        Icon2Rect = QRect(forgeArea.x()+forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon2Rect, equipment->getLootList()[1].material->getImage());
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()-TEXT_HEIGHT,Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[1].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()+Icon2Rect.height(),Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[1].material)).arg(equipment->getLootList()[1].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(2);
        Icon3Rect = QRect(forgeArea.x()+forgeArea.width()-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon3Rect, equipment->getLootList()[2].material->getImage());
        painter.drawText(QRect(Icon3Rect.x()-Icon3Rect.width()/2,Icon3Rect.y()-TEXT_HEIGHT,Icon3Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[2].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon3Rect.x()-Icon3Rect.width()/2,Icon3Rect.y()+Icon3Rect.height(),Icon3Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[2].material)).arg(equipment->getLootList()[2].quantities), &boundingRect);
        break;

     case 4: /* 3π/2 - 3π/2 + n*2π/5 */
        SETUP_MATERIAL_DISPLAY(0);
        Icon1Rect = QRect(forgeArea.x()+forgeArea.width()/2+cos(90-72)*forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2+forgeArea.height()/2*sin(90-72)-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon1Rect, equipment->getLootList()[0].material->getImage());
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()-TEXT_HEIGHT,Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[0].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon1Rect.x()-Icon1Rect.width()/2,Icon1Rect.y()+Icon1Rect.height(),Icon1Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[0].material)).arg(equipment->getLootList()[0].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(1);
        Icon2Rect = QRect(forgeArea.x()+forgeArea.width()/2+cos(54)*forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2-forgeArea.height()/2*sin(54)-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon2Rect, equipment->getLootList()[1].material->getImage());
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()-TEXT_HEIGHT,Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[1].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon2Rect.x()-Icon2Rect.width()/2,Icon2Rect.y()+Icon2Rect.height(),Icon2Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[1].material)).arg(equipment->getLootList()[1].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(2);
        Icon3Rect = QRect(forgeArea.x()+forgeArea.width()/2-cos(54)*forgeArea.width()/2-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2-forgeArea.height()/2*sin(54)-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon3Rect, equipment->getLootList()[2].material->getImage());
        painter.drawText(QRect(Icon3Rect.x()-Icon3Rect.width()/2,Icon3Rect.y()-TEXT_HEIGHT,Icon3Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[2].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon3Rect.x()-Icon3Rect.width()/2,Icon3Rect.y()+Icon3Rect.height(),Icon3Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[2].material)).arg(equipment->getLootList()[2].quantities), &boundingRect);

        SETUP_MATERIAL_DISPLAY(3);
        Icon4Rect = QRect(forgeArea.x()+forgeArea.width()/2-forgeArea.width()/2*cos(18)-iconMaterialSize/2, forgeArea.y()+forgeArea.height()/2+forgeArea.height()/2*sin(90-72)-iconMaterialSize/2, iconMaterialSize, iconMaterialSize);
        painter.drawPixmap(Icon4Rect, equipment->getLootList()[3].material->getImage());
        painter.drawText(QRect(Icon4Rect.x()-Icon4Rect.width()/2,Icon4Rect.y()-TEXT_HEIGHT,Icon4Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, equipment->getLootList()[3].material->getName(), &boundingRect);
        painter.drawText(QRect(Icon4Rect.x()-Icon4Rect.width()/2,Icon4Rect.y()+Icon4Rect.height(),Icon4Rect.width()*2,TEXT_HEIGHT), Qt::AlignCenter, QString("%1/%2").arg(mHero->getBag()->getQuantityOf(equipment->getLootList()[3].material)).arg(equipment->getLootList()[3].quantities), &boundingRect);
        break;
    }
}

void Win_BlackSmith::updateWindow()
{
    update();
}

void Win_BlackSmith::closeWindow()
{
    mHero->freeze(false);
    close();
}















Win_Merchant::Win_Merchant(QWidget * parent):
    Win_Interface_Trading(parent),
    mMerchant(nullptr),
    ui(new Ui::Win_Merchant)
{
    mMerchant = EntitesHandler::getInstance().getMap()->getVillage()->getMerchant();
    mItemTrader = new Frag_Interface_ItemTrader(this, QPixmap(":/images/bag.png"), QPixmap(":/images/ship.png"));
    ui->setupUi(this);
    initInterface();
    connect(mMerchant, SIGNAL(sig_replenish(QObject*)), this, SLOT(closeWindow()));
}

Win_Merchant::~Win_Merchant()
{
    delete ui;
    if(itemToDisplay!=nullptr)
        delete itemToDisplay;
}

void Win_Merchant::virtuallyBuyItemFromSeller(ItemQuickDisplayer * w_item)
{
    ui->data_coin_virtual->setValue(ui->data_coin_virtual->value() - w_item->getItem()->getPrice());
}

void Win_Merchant::virtuallySellItemToSeller(ItemQuickDisplayer * w_item)
{
   ui->data_coin_virtual->setValue(ui->data_coin_virtual->value() + w_item->getItem()->getPrice());
}

void Win_Merchant::initInterface()
{
    itemToDisplay = nullptr;
    ui->layout_item_trader->addWidget(mItemTrader, 0, Qt::AlignCenter);

    ui->button_validate->setStyleSheet("QPushButton{border-image:url(:/icons/button/button_merchant.png);border:no border;border-radius:50px;color:#3c3c3c;}"
                                       "QPushButton::hover{border-image:url(:/icons/button/button_merchant_h.png);}"
                                       "QPushButton::pressed{border-image:url(:/icons/button/button_merchant_p.png);}");

    ui->data_info->setWordWrap(true);
    ui->data_info->setStyleSheet("QLabel{}");
    ui->data_info->setText("");

    mItemTrader->addItemsRightSide(mMerchant->getItemsToSell());
    mItemTrader->addItemsLeftSide(mHero->getBag()->getItems());

    ui->data_coin->setValue(mHero->getCoin());
    ui->data_coin_virtual->setValue(mHero->getCoin());

    t_redBorders = new QTimer(this);
    t_redBorders->setSingleShot(true);
    connect(t_redBorders, SIGNAL(timeout()), this, SLOT(endRedBorders()));

    connect(mItemTrader, SIGNAL(sig_itemSwipedToLeft(ItemQuickDisplayer*)), this, SLOT(virtuallyBuyItemFromSeller(ItemQuickDisplayer*)));
    connect(mItemTrader, SIGNAL(sig_itemSwipedToRight(ItemQuickDisplayer*)), this, SLOT(virtuallySellItemToSeller(ItemQuickDisplayer*)));
    connect(mItemTrader, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(showItem(ItemQuickDisplayer*)));
}

void Win_Merchant::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(100,50,width()-150,height()-150), QPixmap(":/graphicItems/background_window_1.png"));

    painter.drawPixmap(QRect(width()-430, height()-630, 480, 600), QPixmap(":/images/merchant_character.png"));


    painter.setPen(QPen(QBrush("#FFF894"),5));
    int offset = 20;
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20, ui->titreCoins->y()+ui->titreCoins->height()/2),
                     QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->titreCoins->y()+ui->titreCoins->height()/2));
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->titreCoins->y()+ui->titreCoins->height()/2),
                     QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset));
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset),
                     QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset));
    painter.drawLine(QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset),
                     QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->titreCoins->y()+ui->titreCoins->height()+10));
}

void Win_Merchant::on_button_validate_clicked()
{
    bool interactionOccured = false;
    if(ui->data_coin_virtual->value() >= 0)
    {
        for(Item * item : mItemTrader->getItemsRightSide())
        {
            if(!mMerchant->itemIsInShop(item))
            {
                mMerchant->sellItem(mHero, item);
                interactionOccured = true;
            }
        }
        for(Item * item : mItemTrader->getItemsLeftSide())
        {
            if(!mHero->getBag()->itemIsInBag(item))
            {
                mMerchant->buyItem(mHero, item);
                interactionOccured = true;
            }
        }     
        ui->data_coin->setValue(mHero->getCoin());
        if(interactionOccured)
            emit sig_playSound(SOUND_BUY);
        closeWindow();
    }else{
        ui->data_coin->setStyleSheet("QSpinBox{color:#A61919;border: no border;background-color:rgba(0,0,0,0);}");
        t_redBorders->start(500);
    }
}

void Win_Merchant::showItem(ItemQuickDisplayer * item)
{
    if(itemToDisplay != nullptr)
         delete itemToDisplay;
    itemToDisplay = new W_ItemDisplayer(this, item->getItem());
    itemToDisplay->setGeometry(0,0,300,300);
    itemToDisplay->setMinimumSize(QSize(300,300));
    itemToDisplay->setMaximumSize(QSize(300,300));
    ui->layout_displayer->addWidget(itemToDisplay, 0, Qt::AlignCenter);
    if(itemToDisplay->getUseButton() != nullptr)
        itemToDisplay->hideUseButton();
    itemToDisplay->show();

    ui->data_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->data_info->setText(item->getItem()->getInformation());

    update();
}

void Win_Merchant::endRedBorders()
{
    ui->data_coin->setStyleSheet("QSpinBox{color:#efef93;border: no border;background-color:rgba(0,0,0,0);}");
}

void Win_Merchant::closeWindow()
{
    mHero->freeze(false);
    close();
}



















Win_Alchemist::Win_Alchemist(QWidget * parent):
    Win_Interface_Trading(parent),
    mAlchemist(nullptr),
    ui(new Ui::Win_Alchemist)
{
    mAlchemist = EntitesHandler::getInstance().getMap()->getVillage()->getAlchemist();
    mItemTrader = new Frag_Interface_ItemTrader(this, QPixmap(":/images/bag.png"), QPixmap(":/images/alchemist.png"));
    ui->setupUi(this);
    initInterface();
    connect(mAlchemist, SIGNAL(sig_replenish(QObject*)), this, SLOT(closeWindow()));
}

Win_Alchemist::~Win_Alchemist()
{
    delete ui;
    if(itemToDisplay!=nullptr)
        delete itemToDisplay;
}

void Win_Alchemist::virtuallyBuyItemFromSeller(ItemQuickDisplayer * w_item)
{
    ui->data_coin_virtual->setValue(ui->data_coin_virtual->value() - w_item->getItem()->getPrice());
}

void Win_Alchemist::virtuallySellItemToSeller(ItemQuickDisplayer * w_item)
{
   ui->data_coin_virtual->setValue(ui->data_coin_virtual->value() + w_item->getItem()->getPrice());
}

void Win_Alchemist::initInterface()
{
    itemToDisplay = nullptr;
    ui->layout_item_trader->addWidget(mItemTrader, 0, Qt::AlignCenter);

    ui->button_validate->setStyleSheet("QPushButton{border-image:url(:/icons/button/button_buyer.png);border:no border;border-radius:50px;color:#3c3c3c;}"
                                       "QPushButton::hover{border-image:url(:/icons/button/button_buyer_h.png);}"
                                       "QPushButton::pressed{border-image:url(:/icons/button/button_buyer_p.png);}");

    ui->data_info->setWordWrap(true);
    ui->data_info->setStyleSheet("QLabel{}");
    ui->data_info->setText("");

    mItemTrader->addItemsRightSide(mAlchemist->getItemsToSell());

    ui->data_coin->setValue(mHero->getCoin());
    ui->data_coin_virtual->setValue(mHero->getCoin());

    t_redBorders = new QTimer(this);
    t_redBorders->setSingleShot(true);
    connect(t_redBorders, SIGNAL(timeout()), this, SLOT(endRedBorders()));

    connect(mItemTrader, SIGNAL(sig_itemSwipedToLeft(ItemQuickDisplayer*)), this, SLOT(virtuallyBuyItemFromSeller(ItemQuickDisplayer*)));
    connect(mItemTrader, SIGNAL(sig_itemSwipedToRight(ItemQuickDisplayer*)), this, SLOT(virtuallySellItemToSeller(ItemQuickDisplayer*)));
    connect(mItemTrader, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(showItem(ItemQuickDisplayer*)));

    addPotionPreferenciesInterface();
}

void Win_Alchemist::addPotionPreferenciesInterface()
{
    w_potionPreferencies = new Frag_Interface_PotionPreferencies(this, mAlchemist->getPotionPreferencies());
    connect(w_potionPreferencies, SIGNAL(sig_potionPreferenciesChanged()), this, SLOT(potionPreferenciesChanged()));
    ui->layout_potion_selection->addWidget(w_potionPreferencies);
}

void Win_Alchemist::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(100,50,width()-150,height()-150), QPixmap(":/graphicItems/background_window_1.png"));

    painter.setPen(QPen(QBrush("#FFF894"),5));
    int offset = 20;
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20, ui->titreCoins->y()+ui->titreCoins->height()/2),
                     QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->titreCoins->y()+ui->titreCoins->height()/2));
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->titreCoins->y()+ui->titreCoins->height()/2),
                     QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset));
    painter.drawLine(QPointF(ui->titreCoins->x()+ui->titreCoins->width()+20+offset, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset),
                     QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset));
    painter.drawLine(QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->data_coin_virtual->y()+ui->data_coin_virtual->height()+offset),
                     QPointF(ui->titreCoins->x()+(ui->image_coin->x()-ui->titreCoins->x())/2, ui->titreCoins->y()+ui->titreCoins->height()+10));

    painter.drawPixmap(QRect(width()-480, height()-630, 480, 600), QPixmap(":/images/alchemist_character.png"));
}

void Win_Alchemist::on_button_validate_clicked()
{
    bool interactionOccured = false;
    if(ui->data_coin_virtual->value() >= 0)
    {
        for(Item * item : mItemTrader->getItemsLeftSide())
        {
            mAlchemist->buyItem(mHero, item);
            mItemTrader->removeItemLeftSide(item);
            interactionOccured = true;
        }
        ui->data_coin->setValue(mHero->getCoin());
        if(interactionOccured)
            emit sig_playSound(SOUND_BUY);
        closeWindow();
    }else{
        ui->data_coin->setStyleSheet("QSpinBox{padding:2px;border:3px solid #A61919;border-radius:20px;background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0,stop: 0 #696969, stop: 1 #3A3A3A);color:#A61919;}");
        t_redBorders->start(500);
    }
}

void Win_Alchemist::showItem(ItemQuickDisplayer * item)
{
    if(itemToDisplay != nullptr)
         delete itemToDisplay;
    itemToDisplay = new W_ItemDisplayer(this, item->getItem());
    itemToDisplay->setGeometry(0,0,300,300);
    itemToDisplay->setMinimumSize(QSize(300,300));
    itemToDisplay->setMaximumSize(QSize(300,300));
    ui->layout_displayer->addWidget(itemToDisplay, 0, Qt::AlignCenter);
    if(itemToDisplay->getUseButton()!=nullptr)
        itemToDisplay->hideUseButton();
    itemToDisplay->show();

    ui->data_info->setStyleSheet("QLabel{border-image:url(:/graphicItems/background_itemInfo.png);padding:20px;color:#FFFFFF;}");
    ui->data_info->setText(item->getItem()->getInformation());

    update();
}

void Win_Alchemist::endRedBorders()
{
    ui->data_coin->setStyleSheet("QSpinBox{padding:2px;border:3px solid #3C3C3C;border-radius:20px;background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0,stop: 0 #696969, stop: 1 #3A3A3A);color:#ffc926;}");
}

void Win_Alchemist::potionPreferenciesChanged()
{
    mAlchemist->setPotionPreferencies(w_potionPreferencies->getPotionPreferencies());
}

void Win_Alchemist::closeWindow()
{
    mHero->freeze(false);
    close();
}









Win_HeroChest::Win_HeroChest(QWidget *parent) :
    Win_Interface_Trading(parent),
    mChest(nullptr),
    ui(new Ui::Win_chest)
{
    mChest = EntitesHandler::getInstance().getMap()->getVillage()->getHeroHouse()->getChest();
    mItemTrader = new Frag_Interface_ItemTrader(this, QPixmap(":/images/bag.png"), QPixmap(":/images/chest.png"));
    ui->setupUi(this);
    mChest->openChest(true);
    initInterface();
    emit sig_playSound(SOUND_OPENCHEST);
}

Win_HeroChest::~Win_HeroChest()
{
    delete ui;
}

void Win_HeroChest::initInterface()
{
    ui->layout_itemTrader->addWidget(mItemTrader, 0, Qt::AlignCenter);
    mItemTrader->addItemsRightSide(mChest->getItems());
    mItemTrader->addItemsLeftSide(mHero->getBag()->getItems());
}

void Win_HeroChest::closeChest()
{
    QList<Item*> list = mItemTrader->getItemsLeftSide();
    for(Item * item : list)
    {
        if(!mHero->getBag()->itemIsInBag(item))
        {
            if(!mHero->getBag()->addItem(item))
                break;
            mChest->takeItem(item);
        }
    }
    list = mItemTrader->getItemsRightSide();
    for(Item * item : list)
    {
        if(!mChest->itemIsInChest(item))
        {
            mChest->addItem(mHero->getBag()->takeItem(item));
        }
    }
    mChest->openChest(false);
    emit sig_playSound(SOUND_CLOSECHEST);
}











Win_Altar::Win_Altar(QWidget *parent):
    Win_Interface_Trading(parent),
    mAltar(nullptr)
{
#define OFFERING_SIZE       300
#define OFFERINGS_OFFSET    (window.width()*0.15)
#define OFFERINGS_SPACING   ((window.width()-(3*OFFERING_SIZE)-(2*OFFERINGS_OFFSET))/2)

    mAltar = EntitesHandler::getInstance().getMap()->getVillage()->getAltar();

    setGeometry(0,0,parent->width(),parent->height());

    QRect window(QRect(WINDOW_OFFSET, WINDOW_OFFSET, width()-(2*WINDOW_OFFSET), height()-(2*WINDOW_OFFSET)));
    mAreasOffering.append(QRect(window.x()+OFFERINGS_OFFSET, height()/2-OFFERING_SIZE/2, OFFERING_SIZE, OFFERING_SIZE));
    mAreasOffering.append(QRect(window.x()+OFFERINGS_OFFSET+OFFERING_SIZE+OFFERINGS_SPACING, height()/2-OFFERING_SIZE/2, OFFERING_SIZE, OFFERING_SIZE));
    mAreasOffering.append(QRect(window.x()+OFFERINGS_OFFSET+(2*OFFERING_SIZE)+(2*OFFERINGS_SPACING), height()/2-OFFERING_SIZE/2, OFFERING_SIZE, OFFERING_SIZE));

    for(int i = 0; i < mAltar->getOfferings().size(); i++)
    {
        if(mHero->getBag()->getQuantityOf(mAltar->getOfferings().at(i).identifier) && !mAltar->getOfferings().at(i).item)
        {
            QPushButton_Offering * button = new QPushButton_Offering(this, mHero->getBag()->getItem(mAltar->getOfferings().at(i).identifier), i);
            connect(button, SIGNAL(sig_offer(Item*, int)), this, SLOT(onGiveOffering(Item*, int)));
            button->setGeometry(mAreasOffering.at(i).x(), mAreasOffering.at(i).y()+mAreasOffering.at(i).height()+5, mAreasOffering.at(i).width(), button->height());
        }
    }
}

Win_Altar::~Win_Altar()
{

}

void Win_Altar::onGiveOffering(Item * item, int id)
{
    mAltar->setOffering(id, mHero->getBag()->takeItem(item));
    update();

    closeWindow();
}

void Win_Altar::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect centralArea(WINDOW_OFFSET,WINDOW_OFFSET,width()-(2*WINDOW_OFFSET),height()-(2*WINDOW_OFFSET));
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(centralArea, QPixmap(":/graphicItems/background_window_1.png"));

    /* Draw skull Lao Shan Lung */
    painter.setOpacity(0.2);
    painter.drawPixmap(QRect(width()/4, 0, width()/2, height()), QPixmap(":/graphicItems/LaoShanLungSkull.png"));

    /* Draw item areas */
    painter.setOpacity(0.1);
    painter.setBrush(Qt::white);
    for(int i = 0; i < mAreasOffering.size(); i++)
    {
        painter.drawRect(mAreasOffering.at(i));
    }

    /* Draw items */
    for(int i = 0; i < mAreasOffering.size(); i++)
    {
        mAltar->getOfferings().at(i).item ? painter.setOpacity(1) : painter.setOpacity(0.4);
        painter.drawPixmap(mAreasOffering.at(i), mAltar->getOfferingPixmap());
    }
}

void Win_Altar::closeWindow()
{
    mHero->freeze(false);
    close();
}
