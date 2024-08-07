#include "w_itemdisplayer.h"

Hero * gSelectedHero = nullptr;

ItemQuickDisplayer::ItemQuickDisplayer(Item * item):
    Item(item->getName(), item->getImage(), item->getWeight(), item->getPrice()),
    mItem(item),
    mSelected(false),
    mSelectedHero(nullptr)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    isMovable = true;
    mHover = false;
    item->setZValue(0);
    setSelectedHero();
    connect(mItem, SIGNAL(sig_itemStatsChanged()), this, SLOT(updateStats()));
}

QPointF ItemQuickDisplayer::getInitialPosition()
{
    return mPosition;
}

Item *ItemQuickDisplayer::getItem()
{
    return mItem;
}

void ItemQuickDisplayer::setInitialPosition(QPointF point)
{
    mPosition = point;
}

void ItemQuickDisplayer::setMovable(bool toggle)
{
    isMovable = toggle;
    setFlag(QGraphicsItem::ItemIsMovable, toggle);
}

void ItemQuickDisplayer::setItemSelected(bool toggle)
{
    mSelected = toggle;
    update();
}

bool ItemQuickDisplayer::isItemSelected()
{
    return mSelected;
}

Item::Feature ItemQuickDisplayer::getFirstCaracteristic()
{
    return Feature{-1,QPixmap()};
}

Item::Feature ItemQuickDisplayer::getSecondCaracteristic()
{
    return Feature{-1,QPixmap()};
}

Item::Feature ItemQuickDisplayer::getThirdCaracteristic()
{
    return Feature{-1,QPixmap()};
}

Item::Feature ItemQuickDisplayer::getFourthCaracteristic()
{
    return Feature{-1,QPixmap()};
}

void ItemQuickDisplayer::setSelectedHero()
{
    mSelectedHero = gSelectedHero;
}

QPainterPath ItemQuickDisplayer::shape() const
{
    QPainterPath path;
    path.addEllipse(QRect(0,0,static_cast<int>(boundingRect().width()),static_cast<int>(boundingRect().height())));
    return path;
}

QRectF ItemQuickDisplayer::boundingRect() const
{
    return QRect(0,0,100,100);
}

void ItemQuickDisplayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString buff = "";

    // DRAW Background
    if(!mSelected)
        painter->setOpacity(0.5);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush("#FFFFFF"));
    painter->setPen(QPen(QColor("#3A3A3A"),1));
    painter->drawRoundedRect(boundingRect(), 10, 10);
    painter->setOpacity(1);

    // DRAW Item picture
    painter->drawPixmap(0, 0, 60, 60, mItem->getImage());

    // DRAW Item name
    QRect boundingRect;
    painter->setFont(QFont("Sitka Small", 6));
    painter->drawText(QRect(0,60,100,40),Qt::AlignCenter,mItem->getName(), &boundingRect);

    // DRAW Top feature
    QFont font2("Papyrus", 10);
    font2.setBold(true);
    painter->setFont(font2);
    QRectF whiteCircleTop(75-15,2,30,30);
    QRectF textAreaTop(75-15,2,30,30);

    painter->drawEllipse(whiteCircleTop);
    painter->drawText(textAreaTop,Qt::AlignCenter, buff.asprintf("%d", mItem->getFirstCaracteristic().value));
    painter->drawPixmap(QRect(80,17,20,20), mItem->getFirstCaracteristic().logo);

    // DRAW Bot feature if exist
    if(mItem->getSecondCaracteristic().value != -1)
    {
        QFont font("Papyrus", 6);
        font.setBold(true);
        painter->setFont(font);
        QRectF whiteCircleBot(75-10,35+2,20,20);
        QRectF textAreaBot(75-10,35+2,20,20);

        painter->drawEllipse(whiteCircleBot);
        painter->drawText(textAreaBot,Qt::AlignCenter, buff.asprintf("%d", mItem->getSecondCaracteristic().value));
        painter->drawPixmap(QRect(80,45,15,15), mItem->getSecondCaracteristic().logo);
    }

    if(mSelectedHero)
    {
        Equipment * equipment = dynamic_cast<Equipment*>(mItem);
        if(equipment)
        {
            if(!IS_ABLE(mSelectedHero->getHeroClass(), equipment->getUsable()))
            {
                painter->setBrush(QBrush());
                painter->setPen(QPen(QBrush("#AAFF4A4A"), 8));
                switch(mSelectedHero->getHeroClass())
                {
                    case Hero::eSwordman:
                        painter->drawEllipse(QRect(5,5,50,50));
                        painter->drawLine(QLine(25-0.7*25+5, 25-0.7*25+5, 25+0.7*25, 25+0.7*25));
                        break;

                    case Hero::eArcher:
                        painter->drawEllipse(QRect(5,5,50,50));
                        painter->drawLine(QLine(25-0.7*25+5, 25-0.7*25+5, 25+0.7*25, 25+0.7*25));
                        break;

                    case Hero::eWizard:
                        painter->drawEllipse(QRect(5,5,50,50));
                        painter->drawLine(QLine(25-0.7*25+5, 25-0.7*25+5, 25+0.7*25, 25+0.7*25));
                        break;
                }
            }
        }
    }

    Q_UNUSED(widget)
    Q_UNUSED(option)
}

void ItemQuickDisplayer::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit sig_itemClicked(this);
        QGraphicsItem::mousePressEvent(event);
        event->accept();
    }
    else if(event->button() == Qt::RightButton){
        emit sig_itemRightClicked(this);
        QGraphicsItem::mousePressEvent(event);
        event->accept();
    }
    else{
        event->ignore();
    }   
}

void ItemQuickDisplayer::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if(isMovable){
        emit sig_itemMoved(this);
        QGraphicsItem::mouseReleaseEvent(event);
        event->accept();
    }
    else
        event->ignore();
}

void ItemQuickDisplayer::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    mHover = true;
    emit sig_itemHoverIn(this);
    QGraphicsItem::hoverEnterEvent(event);
}

void ItemQuickDisplayer::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    mHover = false;
    emit sig_itemHoverOut(this);
    QGraphicsItem::hoverLeaveEvent(event);
}

void ItemQuickDisplayer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    emit sig_itemDoubleClicked(this);
}

ItemQuickDisplayer::~ItemQuickDisplayer()
{

}

void ItemQuickDisplayer::updateStats()
{
    update();
}





W_ItemDisplayer::W_ItemDisplayer(QWidget * parent, Item * item):
    QWidget(parent),
    mItem(item),
    mButtonUse(nullptr)
{
    Consumable * consumable = dynamic_cast<Consumable*>(mItem);
    Tool * tool = dynamic_cast<Tool*>(mItem);
    Scroll * scroll = dynamic_cast<Scroll*>(mItem);
    if(consumable || tool || scroll)
    {
        mButtonUse = new QPushButton(this);
        if(tool)
            mButtonUse->setText("équiper");
        else
            mButtonUse->setText("utiliser");

        mButtonUse->setFont(QFont("Papyrus"));
        mButtonUse->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
        mButtonUse->setStyleSheet("QPushButton{padding:2px;background-color:#181818;color:white;border:1px solid #404040;}"
                                  "QPushButton::hover{background-color:#252525;}");
        connect(mButtonUse, SIGNAL(clicked()), this, SLOT(useItem()));
    }
}

void W_ItemDisplayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush("#ECECEC"));
    painter.setPen(QPen(QBrush("#FFFFFF"), 5));
    painter.setFont(QFont("Sitka Small", 10));
    QString buff = "";
    int margin = width()/9;

    if(mItem != nullptr){

        // DRAW Background
        //painter.drawRoundedRect(QRect(0,0,width(),height()), 20, 20);
        painter.drawPixmap(QRect(0,0,width(),height()), QPixmap(":/graphicItems/itemBackground.png"));

        // DRAW Item name
        QRect nameField(0, height()*2/3-10, width(), height()/3);

        // DRAW Item picture
        painter.drawPixmap(QRect(0,0,width()*2/3, height()*2/3), mItem->getImage());
        painter.drawText(nameField, Qt::AlignCenter, mItem->getName());

        QFont font("Papyrus",8);
        font.setBold(true);
        painter.setFont(font);

        // DRAW First feature
        QRect fieldImg_1(width()*2/3+5, margin, width()/6-5, width()/6-5);
        QRect fieldTxt_1(fieldImg_1.x()+fieldImg_1.width()+2, fieldImg_1.y()+2, fieldImg_1.width(), fieldImg_1.height());
        painter.drawText(fieldTxt_1, buff.asprintf("%d", mItem->getFirstCaracteristic().value));
        painter.drawPixmap(fieldImg_1, mItem->getFirstCaracteristic().logo);

        if(mItem->getSecondCaracteristic().value!=-1)
        {
            // DRAW Second feature if exist
            QRect fieldImg_2(fieldImg_1.x(), fieldImg_1.y()+fieldImg_1.height(), fieldImg_1.width(), fieldImg_1.height());
            QRect fieldTxt_2(fieldTxt_1.x(), fieldTxt_1.y()+fieldTxt_1.height(), fieldTxt_1.width(), fieldTxt_1.height());
            painter.drawText(fieldTxt_2, buff.asprintf("%d", mItem->getSecondCaracteristic().value));
            painter.drawPixmap(fieldImg_2, mItem->getSecondCaracteristic().logo);

            if(mItem->getThirdCaracteristic().value!=-1)
            {
                // DRAW Third feature if exist
                QRect fieldImg_3(fieldImg_2.x(), fieldImg_2.y()+fieldImg_2.height(), fieldImg_2.width(), fieldImg_2.height());
                QRect fieldTxt_3(fieldTxt_2.x(), fieldTxt_2.y()+fieldTxt_2.height(), fieldTxt_2.width(), fieldTxt_2.height());
                painter.drawText(fieldTxt_3, buff.asprintf("%d", mItem->getThirdCaracteristic().value));
                painter.drawPixmap(fieldImg_3, mItem->getThirdCaracteristic().logo);

                if(mItem->getFourthCaracteristic().value!=-1)
                {
                    // DRAW Fourth feature if exist
                    QRect fieldImg_4(fieldImg_3.x(), fieldImg_3.y()+fieldImg_3.height(), fieldImg_3.width(), fieldImg_3.height());
                    QRect fieldTxt_4(fieldTxt_3.x(), fieldTxt_3.y()+fieldTxt_3.height(), fieldTxt_3.width(), fieldTxt_3.height());
                    painter.drawText(fieldTxt_4, buff.asprintf("%d", mItem->getFourthCaracteristic().value));
                    painter.drawPixmap(fieldImg_4, mItem->getFourthCaracteristic().logo);
                }
            }
        }
    }
}

void W_ItemDisplayer::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    if(mButtonUse != nullptr)
    {
        mButtonUse->setGeometry(width()/2-mButtonUse->width()/2, height()-mButtonUse->height(), mButtonUse->width(), mButtonUse->height());
    }
}

void W_ItemDisplayer::useItem()
{
    emit sig_itemUsed(mItem);
}

Item * W_ItemDisplayer::getItem()
{
    return mItem;
}

QPushButton *W_ItemDisplayer::getUseButton()
{
    if(mButtonUse!=nullptr){
        return mButtonUse;
    }else{
        return nullptr;
    }
}

void W_ItemDisplayer::hideUseButton()
{
    if(mButtonUse != nullptr)
        mButtonUse->hide();
}

W_ItemDisplayer::~W_ItemDisplayer()
{

}



