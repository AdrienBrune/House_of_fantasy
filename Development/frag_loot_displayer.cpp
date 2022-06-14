#include "frag_loot_displayer.h"
#include "ui_frag_loot_displayer.h"

Frag_loot_displayer::Frag_loot_displayer(QWidget *parent, EquipmentToForge::Loot loot) :
    QWidget(parent),
    mLoot(loot),
    ui(new Ui::Frag_loot_displayer)
{
    ui->setupUi(this);
    redBorders = false;

    ui->data_name->setText(mLoot.material->getName());
    ui->data_image->setScaledContents(true);
    ui->data_image->setPixmap(mLoot.material->getImage());
    ui->data_number->setRange(0,100);
    ui->data_number->setValue(mLoot.quantities);
}

Frag_loot_displayer::~Frag_loot_displayer()
{
    delete ui;
}

void Frag_loot_displayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(0.7);
    painter.setBrush(QBrush("#6c6c6c"));
    painter.setPen(QPen(QBrush("#3c3c3c"),2));
    painter.drawRoundedRect(QRect(10,10,width()-20,height()-20), 20, 20, Qt::SizeMode::RelativeSize);

    if(redBorders){
        painter.setBrush(QBrush());
        painter.setPen(QPen(QBrush("#A61919"),5));
        painter.drawRoundedRect(QRect(10,10,width()-20,height()-20), 20, 20, Qt::SizeMode::RelativeSize);
    }
}
