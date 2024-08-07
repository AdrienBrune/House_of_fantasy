#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include <QtDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include "constants.h"

class Item : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    struct Feature{
        int value;
        QPixmap logo;
    };

public:
    Item(QString name,QPixmap image, int weight, int price);
    virtual ~Item();
signals:
    void sig_itemStatsChanged();
    void sig_itemClicked(Item*);
    void sig_showItemInfo(Item*);
private slots:
    void showItemInfo();
public:
    quint32 getIdentifier();
    QString getName();
    int getWeight();
    QPixmap getImage();
    int getPrice();
    QString getInformation();
    void setShape();
    void setName(QString);
    void setweight(int);
    void setPrice(int);
    void setInformation(QString);
    void setHover(bool toggle);
    bool isUsable();
public:
    QPainterPath shape() const;
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public:
    virtual Feature getFirstCaracteristic()=0;
    virtual Feature getSecondCaracteristic()=0;
    virtual Feature getThirdCaracteristic()=0;
    virtual Feature getFourthCaracteristic()=0;
public:
    virtual void serialize(QDataStream& stream)const
    {
        stream << mIdentifier << mName << mImage << mWeight << mPrice << mInformation << mHover /*<< mShape*/;
    }
    virtual void deserialize(QDataStream& stream)
    {
        stream >> mIdentifier >> mName >> mImage >> mWeight >> mPrice >> mInformation >> mHover /*>> mShape*/;
    }
    friend QDataStream& operator<<(QDataStream& stream, const Item& object)
    {
        object.serialize(stream);
        return stream;
    }
    friend QDataStream& operator>>(QDataStream& stream, Item& object)
    {
        object.deserialize(stream);
        return stream;
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
public:
    static Item * getInstance(quint32);
    static quint32 getNbInstances();
protected:
    quint32 mIdentifier;
    QString mName;
    QPixmap mImage;
    int mWeight;
    int mPrice;
    QString mInformation;
    bool mIsUsable;
    int mHover;
    QPainterPath mShape;
    QTimer * t_delayHover;
protected:
    static quint32 sNbInstances;
};

inline QDebug operator<< (QDebug flux, Item &item) {
    flux << "ITEM :\n";
    flux << "  Identifier(" << item.getIdentifier() << ")\n";
    flux << "  Name      (" << item.getName() << ")\n";
    flux << "  Info      (" << item.getInformation() << ")\n";
    flux << "  Prix      (" << item.getPrice() << ")\n";
    return flux;
};

class BagCoin : public Item
{
public:
    BagCoin(int);
    ~BagCoin();
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
};




class Scroll : public Item
{
public:
    Scroll(QString name, QPixmap image, int weight, int price);
    ~Scroll();
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
};

class Scroll_X : public Scroll
{
public:
    Scroll_X();
    ~Scroll_X();
};




class Tool : public Item
{
public:
    Tool(QString name, QPixmap image, int weight, int price);
    virtual ~Tool();
public:
    bool use();
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
protected:
    int mDurability;
};

class Shovel : public Tool
{
public:
    Shovel();
    ~Shovel();
public :
    bool use();
};

class Pickaxe : public Tool
{
public:
    Pickaxe();
    ~Pickaxe();
public :
    bool use();
};

class FishingRod : public Tool
{
public:
    FishingRod();
    ~FishingRod();
public :
    bool use();
};

class Compass : public Tool
{
public:
    Compass();
    ~Compass();
public :
    bool use();
};

class Knife : public Tool
{
public:
    Knife();
    ~Knife();
public :
    bool use();
};

#endif // ITEM_H
