#include "w_dialogpannel.h"
#include "ui_w_showmonsterdata.h"
#include "ui_w_showpotentialaction.h"
#include "ui_w_showinformation.h"

W_DialogPannel::W_DialogPannel(QWidget *p) :
    QWidget(p)
{

}

void W_DialogPannel::hideWidget()
{
    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(sig_removeWidget()));
    timer->start(1000);

    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(100);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
    b->setDuration(400);
    b->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    b->setEndValue(QRect(this->x(),this->y()+this->height(),this->width(),this->width()));
    b->start(QPropertyAnimation::DeleteWhenStopped);
}

void W_DialogPannel::showWidget()
{
    QTimer * timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(hideWidget()));
    timer->start(5000);

    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(500);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    QPropertyAnimation *b = new QPropertyAnimation(this,"geometry");
    b->setDuration(600);
    b->setStartValue(QRect(this->x(),this->y()+this->height(),this->width(),this->height()));
    b->setEndValue(QRect(this->x(),this->y(),this->width(),this->width()));
    b->start(QPropertyAnimation::DeleteWhenStopped);

    show();
}

void W_DialogPannel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0,width(),height(), QPixmap(":/graphicItems/pannel.png"));
}

W_DialogPannel::~W_DialogPannel()
{

}




W_ShowMonsterData::W_ShowMonsterData(QWidget *parent, Monster * monster):
    W_DialogPannel (parent),
    ui(new Ui::W_showMonsterData)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_menaceLv->setScaledContents(true);

    QString buff = "";
    ui->data_name->setText(monster->getName());
    ui->data_image->setPixmap(monster->getImage());
    ui->data_threatLabel->setText(buff.asprintf("Menace : %d", monster->getThreatLevel()));
    ui->data_menaceLv->setPixmap(QPixmap(":/images/threatLevel.png"));
    ui->data_life->setMaximum(monster->getLife().maxLife);
    ui->data_life->setValue(monster->getLife().curLife);
}

W_ShowMonsterData::~W_ShowMonsterData()
{
    delete ui;
}





W_ShowInformation::W_ShowInformation(QWidget *parent):
    W_DialogPannel (parent),
    ui(new Ui::W_showInformation)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);
}

W_ShowInformation::~W_ShowInformation()
{
    delete ui;
}

W_ShowBagFull::W_ShowBagFull(QWidget *parent):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(":/images/bag.png"));
    ui->data_textDisplayed->setText("Votre sac est plein\nObjet non ramassé");
}

W_ShowBagFull::~W_ShowBagFull()
{

}


W_ShowEquipmentTook::W_ShowEquipmentTook(QWidget *parent, Item * item):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(item->getImage());
    ui->data_textDisplayed->setText("Équipement ramassé :\n" + item->getName() + "\nObjet envoyé dans l'inventaire");
}

W_ShowEquipmentTook::~W_ShowEquipmentTook()
{

}


W_ShowItemTook::W_ShowItemTook(QWidget * parent, Item * item):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    BagCoin * coins = dynamic_cast<BagCoin*>(item);
    if(coins){
        ui->data_image->setPixmap(QPixmap(":/images/coinBag.png"));
        QString buff = "";
        ui->data_textDisplayed->setText(buff.asprintf("Vous avez trouvé un sac de pièce\nLe nombre de pièce s'éléve à %d", item->getPrice()));
    }else{
        ui->data_image->setPixmap(item->getImage());
        ui->data_textDisplayed->setText("objet ramassé :\n" + item->getName());
    }
}

W_ShowItemTook::~W_ShowItemTook()
{

}


W_ShowEquipmentForged::W_ShowEquipmentForged(QWidget *parent, Item *item):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(item->getImage());
    ui->data_textDisplayed->setText("Équipement forgé :\n" + item->getName() + "\nObjet envoyé dans le sac");
}

W_ShowEquipmentForged::~W_ShowEquipmentForged()
{

}

W_ShowChestFound::W_ShowChestFound(QWidget *parent):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(":/images/chest.png"));
    ui->data_textDisplayed->setText("un coffre a été trouvé");
}

W_ShowChestFound::~W_ShowChestFound()
{

}

W_ShowChestBurried::W_ShowChestBurried(QWidget *parent):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(":/images/chest_burried.png"));
    ui->data_textDisplayed->setText("Quelque chose semble enterré par ici");
}

W_ShowChestBurried::~W_ShowChestBurried()
{

}



W_ShowLevelUp::W_ShowLevelUp(QWidget *parent, int level):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    QString buff = "";
    ui->data_image->setPixmap(QPixmap(":/images/level_up.png"));
    ui->data_textDisplayed->setText(buff.asprintf("Vous avez gagné un niveau\nVous êtes maintenant niveau %d", level));
}

W_ShowLevelUp::~W_ShowLevelUp()
{

}






ShowPotentialAction::ShowPotentialAction(QWidget *parent):
    W_DialogPannel (parent)
{

}

ShowPotentialAction::~ShowPotentialAction()
{

}






W_ShowItemOnGround::W_ShowItemOnGround(QWidget * parent, Item * itemToShow):
    W_DialogPannel (parent)
{
    W_ItemDisplayer * item = new W_ItemDisplayer(this, itemToShow);
    setGeometry(0,0,300,300);
    item->setGeometry(0, 0, width(), height());
    item->hideUseButton();
    connect(itemToShow, SIGNAL(destroyed(QObject*)), this, SLOT(itemDestroyed()));
}

W_ShowItemOnGround::~W_ShowItemOnGround()
{

}

void W_ShowItemOnGround::itemDestroyed()
{
    close();
}









W_ShowOreSpot::W_ShowOreSpot(QWidget * parent, OreSpot * ore):
    W_ShowInformation(parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    QString buff = "";
    ui->data_image->setPixmap(QPixmap(":/images/oreSpotFound.png"));
    ui->data_textDisplayed->setText("Vous avez trouvé un filon de minerai :\n" + ore->getItems().first()->getName());
}

W_ShowOreSpot::~W_ShowOreSpot()
{

}

W_ShowOreTook::W_ShowOreTook(QWidget * parent, Item * ore):
    W_ShowInformation(parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(ore->getImage());
    ui->data_textDisplayed->setText("Vous avez récolté :\n\t" + ore->getName());
}

W_ShowOreTook::~W_ShowOreTook()
{

}

W_ShowFishSpot::W_ShowFishSpot(QWidget * parent):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(":/images/fishSpot.png"));
    ui->data_textDisplayed->setText("Les poissons semblent s'ammasser par ici");
}

W_ShowFishSpot::~W_ShowFishSpot()
{

}

W_ShowFishTook::W_ShowFishTook(QWidget * parent, Fish * fish):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(fish->getImage());
    ui->data_textDisplayed->setText("Vous avez pêché :\n" + fish->getName());
}

W_ShowFishTook::~W_ShowFishTook()
{

}


W_ShowReplenish::W_ShowReplenish(QWidget *parent, QObject *shop):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    Merchant * merchant = dynamic_cast<Merchant*>(shop);
    if(merchant){
        ui->data_image->setPixmap(QPixmap(":/images/ship.png"));
        ui->data_textDisplayed->setText("Le marchand a été réaprovisionné, passez le voir");
    }
    Alchemist * alchemist = dynamic_cast<Alchemist*>(shop);
    if(alchemist){
        ui->data_image->setPixmap(QPixmap(":/images/alchemist.png"));
        ui->data_textDisplayed->setText("L'alchimiste a concocté une nouvelle potion, passez le voir");
    }
}

W_ShowReplenish::~W_ShowReplenish()
{

}

W_ShowConsumableTook::W_ShowConsumableTook(QWidget * parent, Consumable * item):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(item->getImage());
    ui->data_textDisplayed->setText("Vous avez ramassé :\n" + item->getName() + "\nObjet envoyé dans l'inventaire");
}

W_ShowConsumableTook::~W_ShowConsumableTook()
{

}






W_ShowVilageInfo::W_ShowVilageInfo(QWidget * parent, QGraphicsItem * interaction):
    W_ShowInformation (parent)
{
    House * house = dynamic_cast<House*>(interaction);
    if(house){
        ui->data_image->setPixmap(house->getImage());
        ui->data_textDisplayed->setText(house->getInformation());
    }
}

W_ShowVilageInfo::~W_ShowVilageInfo()
{

}




W_ShowCarcass::W_ShowCarcass(QWidget * parent, Monster * monster):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(":/graphicItems/carcass.png"));
    ui->data_textDisplayed->setText("Carcasse : " + monster->getName() + "\nDépecez le corps à l'aide d'un couteau");
}

W_ShowCarcass::~W_ShowCarcass()
{

}





W_ShowCalamitySpawned::W_ShowCalamitySpawned(QWidget * parent):
    W_ShowInformation (parent)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    ui->data_textDisplayed->setWordWrap(true);

    ui->data_image->setPixmap(QPixmap(""));
    ui->data_textDisplayed->setText("Une calamité est apparue");
}

W_ShowCalamitySpawned::~W_ShowCalamitySpawned()
{

}
