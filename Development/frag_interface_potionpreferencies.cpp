#include "frag_interface_potionpreferencies.h"
#include "ui_frag_interface_potionpreferencies.h"


Custom_ComboBox::Custom_ComboBox(QWidget *parent, int currentIndex):
    QComboBox (parent)
{
    addItem(PotionLife().getImage(), PotionLife().getName());
    addItem(PotionMana().getImage(), PotionMana().getName());
    addItem(PotionStamina().getImage(), PotionStamina().getName());
    addItem(PotionStrenght().getImage(), PotionStrenght().getName());
    addItem(PotionKnowledge().getImage(), PotionKnowledge().getName());

    mLastIndex = currentIndex;
    setCurrentIndex(currentIndex);

    setMinimumHeight(40);
    setIconSize(QSize(40,40));
    setFont(QFont("Pristina", 12));
    setStyleSheet("Custom_ComboBox{color:white;background-color:rgba(255,255,255,40);border:0px solid white;border-radius:5px;}");
}

Custom_ComboBox::~Custom_ComboBox()
{

}

int Custom_ComboBox::getLastIndex()
{
    return mLastIndex;
}

void Custom_ComboBox::setLastIndex(int index)
{
    mLastIndex = index;
}

void Custom_ComboBox::setIndex(int index)
{
    mLastIndex = currentIndex();
    setCurrentIndex(index);
}



Frag_Interface_PotionPreferencies::Frag_Interface_PotionPreferencies(QWidget *parent, QList<Consumable*> preferencies):
    QWidget(parent),
    mList({0,0,0}),
    ui(new Ui::Frag_Interface_PotionPreferencies)
{
    ui->setupUi(this);

    int index = 0, listIndex = 0;
    for(Consumable * c : preferencies)
    {
        PotionLife * pLife = dynamic_cast<PotionLife*>(c);
        if(pLife){
            index = 0;
            mList[listIndex] = 0;
        }
        PotionMana * pMana = dynamic_cast<PotionMana*>(c);
        if(pMana){
            index = 1;
            mList[listIndex] = 1;
        }
        PotionStamina * pStamina = dynamic_cast<PotionStamina*>(c);
        if(pStamina){
            index = 2;
            mList[listIndex] = 2;
        }
        PotionStrenght * pStrenght = dynamic_cast<PotionStrenght*>(c);
        if(pStrenght){
            index = 3;
            mList[listIndex] = 3;
        }
        PotionKnowledge * pKnowledge = dynamic_cast<PotionKnowledge*>(c);
        if(pKnowledge){
            index = 4;
            mList[listIndex] = 4;
        }
        Custom_ComboBox * cb = new Custom_ComboBox(this, index);
        mComboBoxList.append(cb);
        ui->mainLayout->addWidget(cb);

        listIndex++;
    }
    connect(mComboBoxList[0], SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSelection_1(int)));
    connect(mComboBoxList[1], SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSelection_2(int)));
    connect(mComboBoxList[2], SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSelection_3(int)));
}

Frag_Interface_PotionPreferencies::~Frag_Interface_PotionPreferencies()
{
    delete ui;
}

void Frag_Interface_PotionPreferencies::currentIndexChangedSelection_1(int index)
{
    for(Custom_ComboBox * c : mComboBoxList)
    {
        if(c!=mComboBoxList[0]){
            if(c->currentIndex() == index){
                mComboBoxList[0]->setIndex(mComboBoxList[0]->getLastIndex());
                mComboBoxList[0]->setLastIndex(mComboBoxList[0]->currentIndex());
                return;
            }
        }
    }
    mComboBoxList[0]->setIndex(index);
    mList[0] = index;
    emit sig_potionPreferenciesChanged();
}

void Frag_Interface_PotionPreferencies::currentIndexChangedSelection_2(int index)
{
    for(Custom_ComboBox * c : mComboBoxList)
    {
        if(c!=mComboBoxList[1]){
            if(c->currentIndex() == index){
                mComboBoxList[1]->setIndex(mComboBoxList[1]->getLastIndex());
                mComboBoxList[1]->setLastIndex(mComboBoxList[1]->currentIndex());
                return;
            }
        }
    }
    mComboBoxList[1]->setIndex(index);
    mList[1] = index;
    emit sig_potionPreferenciesChanged();
}

void Frag_Interface_PotionPreferencies::currentIndexChangedSelection_3(int index)
{
    for(Custom_ComboBox * c : mComboBoxList)
    {
        if(c!=mComboBoxList[2]){
            if(c->currentIndex() == index){
                mComboBoxList[2]->setIndex(mComboBoxList[2]->getLastIndex());
                mComboBoxList[2]->setLastIndex(mComboBoxList[2]->currentIndex());
                return;
            }
        }
    }
    mComboBoxList[2]->setIndex(index);
    mList[2] = index;
    emit sig_potionPreferenciesChanged();
}

QList<int> Frag_Interface_PotionPreferencies::getPotionPreferencies()
{
    return mList;
}

void Frag_Interface_PotionPreferencies::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(QBrush(Qt::white), 2));
    painter.drawLine(QPointF(ui->label->x()-20, ui->label->y()+ui->label->height()/2), QPointF(width()/4, ui->label->y()+ui->label->height()/2));
    painter.drawLine(QPointF(ui->label->x()+ui->label->width()+20, ui->label->y()+ui->label->height()/2), QPointF(width()*3/4, ui->label->y()+ui->label->height()/2));
}
