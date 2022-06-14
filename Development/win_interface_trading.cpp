#include "win_interface_trading.h"
#include "ui_win_chest.h"
#include "ui_win_blacksmith.h"
#include "ui_win_merchant.h"
#include "ui_win_alchemist.h"

Win_Interface_Trading::Win_Interface_Trading(QWidget *parent, Hero * hero) :
    QWidget(parent)
{
    mHero = hero;
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





Win_Chest::Win_Chest(QWidget *parent, Hero * hero, ChestEvent * chest) :
    Win_Interface_Trading(parent, hero),
    mChest(chest),
    ui(new Ui::Win_chest)
{
    mItemTrader = new Frag_Interface_ItemTrader(this);
    ui->setupUi(this);
    mChest->openChest(true);
    initInterface();
    sig_playSound(SOUND_OPENCHEST);
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
    sig_playSound(SOUND_CLOSECHEST);
}

Win_Chest::~Win_Chest()
{
    delete ui;
}



Win_BlackSmith::Win_BlackSmith(QWidget * parent, Hero * hero, Blacksmith * blacksmith):
    Win_Interface_Trading(parent, hero),
    mBlacksmith(blacksmith),
    ui(new Ui::Win_Blacksmith)
{
    mItemTrader = new Frag_Interface_ItemTrader(this, QPixmap(":/images/bag.png"), QPixmap(":/images/anvil.png"));
    ui->setupUi(this);
    initInterface();
    connect(mBlacksmith, SIGNAL(sig_replenish(QObject*)), this, SLOT(closeWindow()));
}

Win_BlackSmith::~Win_BlackSmith()
{
    delete ui;
    if(itemToDisplay!=nullptr)
        delete itemToDisplay;
}

void Win_BlackSmith::initInterface()
{
    ui->scrollArea->hide();
    ui->button_validate->hide();

    ui->scrollArea->setAutoFillBackground(false);
    ui->scrollAreaWidgetContents->setAutoFillBackground(false);

    ui->button_validate->setStyleSheet("QPushButton{border-image:url(:/icons/button/button_blacksmith.png);border:no border;border-radius:50px;color:#3c3c3c;}"
                                       "QPushButton::hover{border-image:url(:/icons/button/button_blacksmith_h.png);}"
                                       "QPushButton::pressed{border-image:url(:/icons/button/button_blacksmith_p.png);}");

    itemToDisplay = nullptr;
    mEquipmentToForgeSelected = nullptr;
    w_panel = nullptr;

    ui->layout_item_trader->addWidget(mItemTrader);
    QList<Material*> list = mHero->getBag()->getMaterials();
    QList<Item*> list_cast;
    for(Material * m : list)
    {
        list_cast.append(m);
    }
    mItemTrader->addItemsLeftSide(list_cast);

    mScene = new QGraphicsScene(this);
    mScene->setSceneRect(QRect(0,0,120,500));
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setStyleSheet("background-color:rgba(0,0,0,0);border:no border;");
    ui->graphicsView->centerOn(0,0);

    for(EquipmentToForge * e : mBlacksmith->getEquipmentsToForge())
    {
        addItemToForge(e->getEquipment());
    }
    ui->data_coin->setValue(mHero->getCoin());
    t_redBorders = new QTimer(this);
    t_redBorders->setSingleShot(true);
    connect(t_redBorders, SIGNAL(timeout()), this, SLOT(endRedBorders()));
}

void Win_BlackSmith::addItemToForge(Item * item)
{
    ItemQuickDisplayer * w_item = new ItemQuickDisplayer(item);
    w_item->setMovable(false);
    mEquipmentsToForge.append(w_item);
    mScene->addItem(w_item);

    int vOffset = 10, hOffset = 10;
    w_item->setPos(hOffset, vOffset);
    connect(w_item, SIGNAL(sig_itemClicked(ItemQuickDisplayer*)), this, SLOT(showItem(ItemQuickDisplayer*)));

    while(!w_item->collidingItems().isEmpty())
    {
        vOffset += static_cast<int>(w_item->boundingRect().height()) + 5;
        w_item->setPos(hOffset, vOffset);
    }
}

bool Win_BlackSmith::materialListsMatch()
{
    QList<Material*> list;
    for(Item * item : mItemTrader->getItemsRightSide()){
        Material * m = dynamic_cast<Material*>(item);
        list.append(m);
    }

    int isFound = 0;
    for(int i=0;i<mEquipmentToForgeSelected->getLootList().size();i++)
    {
        isFound = 0;
        for(Material * m : list)
        {
            if(m->getIdentifier() == mEquipmentToForgeSelected->getLootList()[i].material->getIdentifier()){
                isFound++;
            }
        }
        if(isFound < mEquipmentToForgeSelected->getLootList()[i].quantities){
            return false;
        }
    }
    return true;
}

void Win_BlackSmith::removeMaterialsNeeded()
{
    QList<Material*> list;
    for(Item * item : mItemTrader->getItemsRightSide()){
        Material * m = dynamic_cast<Material*>(item);
        list.append(m);
    }

    int isFound = 0;
    for(int i=0;i<mEquipmentToForgeSelected->getLootList().size();i++)
    {
        isFound = 0;
        for(Material * m : list)
        {
            if(m->getIdentifier() == mEquipmentToForgeSelected->getLootList()[i].material->getIdentifier()){
                list.removeOne(m);
                mItemTrader->removeItemRightSide(dynamic_cast<Item*>(m));
                mHero->getBag()->removeItem(dynamic_cast<Item*>(m));
                isFound++;
            }
            if(isFound >= mEquipmentToForgeSelected->getLootList()[i].quantities){
                break;
            }
        }
    }
}

void Win_BlackSmith::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#393939"));
    painter.setOpacity(0.7);
    painter.drawRect(0,0,width(),height());
    painter.setOpacity(1);
    painter.drawPixmap(QRect(100,50,width()-150,height()-150), QPixmap(":/graphicItems/background_window_1.png"));

    painter.drawPixmap(QRect(width()-480, height()-600, 480, 600), QPixmap(":/images/blacksmith_character.png"));

    painter.setPen(QPen(QBrush("#FFFFFF"),1));
    painter.setFont(QFont("Lucide Calligraphy", 14));
    painter.drawText(QRect(ui->graphicsView->x()-30, ui->graphicsView->y()-80, 60+ui->graphicsView->width(), 80), Qt::AlignCenter, "Équipements\nà forger");

    painter.setBrush(QBrush("#DDDDDD"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon[6] = {
        QPointF(0,0),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width(),height()-50),
        QPointF(width(),0)
    };
    painter.drawPolygon(polygon, 6);

    painter.setBrush(QBrush("#CCCCCC"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon2[6] = {
        QPointF(25,30),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width()-30,height()-70),
        QPointF(width()-30,30)
    };
    painter.drawPolygon(polygon2, 6);
}

void Win_BlackSmith::on_button_validate_clicked()
{
    if(mEquipmentToForgeSelected)
    {
        if(materialListsMatch())
        {
            if(mHero->getCoin() >= mEquipmentToForgeSelected->getEquipment()->getPrice())
            {
                emit sig_playSound(SOUND_BUY);
                removeMaterialsNeeded();
                mHero->removeCoin(mEquipmentToForgeSelected->getEquipment()->getPrice());
                ui->data_coin->setValue(mHero->getCoin());
                mBlacksmith->buyEquipment(mHero, mEquipmentToForgeSelected);

                W_Animation_Forge * animation = new W_Animation_Forge(this);
                animation->setGeometry(0,0,width(),height());
                connect(animation, SIGNAL(sig_playSound(int)), this, SIGNAL(sig_playSound(int)));

                QTimer * timer = new QTimer(this);
                timer->setSingleShot(true);
                connect(timer, SIGNAL(timeout()), this, SLOT(displayItemForged()));
                timer->start(8000);
            }else{
                ui->data_coin->setStyleSheet("QSpinBox{padding:2px;border:3px solid #A61919;border-radius:20px;background-color: qlineargradient(x1: 0, y1: 1, x2: 1, y2: 0,stop: 0 #696969, stop: 1 #3A3A3A);color:A61919;}");
                t_redBorders->start(500);
            }
        }else
        {
            for(Frag_loot_displayer * box : lootsDisplayed)
            {
                box->redBorders = true;
                box->update();
            }
            t_redBorders->start(500);
        }
    }
}

void Win_BlackSmith::showItem(ItemQuickDisplayer * item)
{
    ui->scrollArea->show();
    ui->button_validate->show();

    ui->button_validate->show();
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

    while(lootsDisplayed.length()!=0){
        delete lootsDisplayed.takeLast();
    }
    for(EquipmentToForge * e : mBlacksmith->getEquipmentsToForge())
    {
        if(e->getEquipment() == item->getItem())
        {
            mEquipmentToForgeSelected = e;
            for(int i=0;i<e->getLootList().size();i++)
            {
                Frag_loot_displayer * loot = new Frag_loot_displayer(this, e->getLootList()[i]);
                lootsDisplayed.append(loot);
                ui->layout_loots->addWidget(loot);
            }
        }
    }
    update();
}

void Win_BlackSmith::displayItemForged()
{
    W_ShowEquipmentForged * show = new W_ShowEquipmentForged(this, mEquipmentToForgeSelected->getEquipment());
    show->setGeometry(width()*3/4, height()-350, 300, 300);
    connect(show, SIGNAL(sig_removeWidget()), this, SLOT(removeCurrentPanel()));
    w_panel = show;
    show->showWidget();

    ui->scrollArea->hide();
    ui->button_validate->hide();

    if(itemToDisplay!=nullptr)
        delete itemToDisplay;
    itemToDisplay = nullptr;
}

void Win_BlackSmith::displayBagFull()
{
    W_ShowBagFull * show = new W_ShowBagFull(this);
    show->setGeometry(width()*3/4, height()-300, 300, 300);
    connect(show, SIGNAL(sig_removeWidget()), this, SLOT(removeCurrentPanel()));
    w_panel = show;
    show->showWidget();

    ui->scrollArea->hide();
    ui->button_validate->hide();

    if(itemToDisplay!=nullptr)
        delete itemToDisplay;
    itemToDisplay = nullptr;
}

void Win_BlackSmith::removeCurrentPanel()
{
    if(w_panel != nullptr)
        delete w_panel;
    w_panel = nullptr;
}

void Win_BlackSmith::endRedBorders()
{
    for(Frag_loot_displayer * box : lootsDisplayed)
    {
        box->redBorders = false;
        box->update();
    }
    ui->data_coin->setStyleSheet("QSpinBox { color:#efef93; border: no border; background-color:rgba(0,0,0,0); }");

}

void Win_BlackSmith::closeWindow()
{
    close();
}















Win_Merchant::Win_Merchant(QWidget * parent, Hero * hero, Merchant * merchant):
    Win_Interface_Trading(parent, hero),
    mMerchant(merchant),
    ui(new Ui::Win_Merchant)
{
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


    painter.setBrush(QBrush("#DDDDDD"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon[6] = {
        QPointF(0,0),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width(),height()-50),
        QPointF(width(),0)
    };
    painter.drawPolygon(polygon, 6);

    painter.setBrush(QBrush("#CCCCCC"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon2[6] = {
        QPointF(25,30),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width()-30,height()-70),
        QPointF(width()-30,30)
    };
    painter.drawPolygon(polygon2, 6);
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
        close();
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
    if(itemToDisplay->getUseButton()!=nullptr)
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
    close();
}



















Win_Alchemist::Win_Alchemist(QWidget * parent, Hero * hero, Alchemist * alchemist):
    Win_Interface_Trading(parent, hero),
    mAlchemist(alchemist),
    ui(new Ui::Win_Alchemist)
{
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

    painter.setBrush(QBrush("#DDDDDD"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon[6] = {
        QPointF(0,0),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width(),height()-50),
        QPointF(width(),0)
    };
    painter.drawPolygon(polygon, 6);

    painter.setBrush(QBrush("#CCCCCC"));
    painter.setPen(QPen(QBrush("#111111"), 4, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
    const QPointF polygon2[6] = {
        QPointF(25,30),
        QPointF(50,50),
        QPointF(width()-50,50),
        QPointF(width()-50,height()-100),
        QPointF(width()-30,height()-70),
        QPointF(width()-30,30)
    };
    painter.drawPolygon(polygon2, 6);

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
        close();
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
    close();
}









Win_HeroChest::Win_HeroChest(QWidget *parent, Hero * hero, HeroChest * chest) :
    Win_Interface_Trading(parent, hero),
    mChest(chest),
    ui(new Ui::Win_chest)
{
    mItemTrader = new Frag_Interface_ItemTrader(this, QPixmap(":/images/bag.png"), QPixmap(":/images/chest.png"));
    ui->setupUi(this);
    mChest->openChest(true);
    initInterface();
    sig_playSound(SOUND_OPENCHEST);
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
    sig_playSound(SOUND_CLOSECHEST);
}

Win_HeroChest::~Win_HeroChest()
{
    delete ui;
}
