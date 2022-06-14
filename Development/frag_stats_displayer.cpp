#include "frag_stats_displayer.h"
#include "ui_frag_stats_displayer.h"

Frag_Stats_Displayer::Frag_Stats_Displayer(QWidget *parent, QString name, QPixmap image, int current, int maximum, int mode) :
    QWidget(parent),
    ui(new Ui::Frag_Stats_Displayer)
{
    ui->setupUi(this);
    ui->data_image->setScaledContents(true);
    setStats(name, image, current, maximum);
    if(mode == 1){
        ui->data_max->hide();
        ui->label_3->hide();
    }
}

Frag_Stats_Displayer::~Frag_Stats_Displayer()
{
    delete ui;
}

void Frag_Stats_Displayer::setStats(QString name, QPixmap image, int current, int maximum)
{
    QString buff = "";
    ui->data_name->setText(name);
    ui->data_image->setPixmap(image);
    ui->data_current->setText(buff.asprintf("%d", current));
    ui->data_max->setText(buff.asprintf("%d", maximum));
    update();
}

void Frag_Stats_Displayer::changeStats(int current, int maximum)
{
    QString buff = "";
    ui->data_current->setText(buff.asprintf("%d", current));
    ui->data_max->setText(buff.asprintf("%d", maximum));
    update();
}

void Frag_Stats_Displayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#FFFFFF"));
    painter.setPen(QPen(QBrush("#000000"), 4));
    painter.drawRect(ui->data_image->x()+ui->data_image->width()/2, ui->data_name->y(), ui->data_name->x()-ui->data_image->x()-ui->data_image->width()/2, ui->data_name->height()+2);
    painter.drawRect(ui->data_name->x()-8, ui->data_name->y(), ui->data_name->width()+16, ui->data_name->height()+2);

    painter.setBrush(QBrush("#CCCCCC"));
    painter.setPen(QPen(QBrush("#000000"), 4));
    painter.drawEllipse(QRect(ui->data_image->x()-4,ui->data_image->y()-4,ui->data_image->width()+8,ui->data_image->height()+8));
}
