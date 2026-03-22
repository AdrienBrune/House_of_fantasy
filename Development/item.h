#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include <QtDebug>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QRandomGenerator>
#include <QPainterPath>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "constants.h"
#include "common.h"

class Item : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    struct Feature{
        int value;
        QPixmap logo;
    };

public:
    Item(QString name, QString imagePath, int weight, int price);
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
    QString getImagePath();
    int getPrice();
    QString getInformation();
    void setShape();
    void setName(QString);
    void setweight(int);
    void setImage(QString);
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
    inline virtual void toJson(QJsonObject &json) const
    {
        json["name"] = mName;
        json["type"] = (double)mIdentifier;
    }
    inline virtual void fromJson(const QJsonObject &json)
    {
        if (json.contains("name") && json["name"].isString())
        {
            mName = json["name"].toString();
        }
        if (json.contains("type") && json["type"].isDouble())
        {
            mIdentifier = json["type"].toInt();
        }
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
public:
    static Item * Factory(quint32);
    // static quint32 getNbInstances();
protected:
    quint32 mIdentifier;
    QString mName;
    QPixmap mImage;
    QString mImagePath;
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
    inline virtual void toJson(QJsonObject &json) const
    {
        Item::toJson(json);

        json["coins"] = mPrice;
    }
    inline virtual void fromJson(const QJsonObject &json)
    {
        Item::fromJson(json);

        if (json.contains("coins") && json["coins"].isDouble())
        {
            mPrice = json["coins"].toInt();
        }
    }
public:
    Feature getFirstCaracteristic();
    Feature getSecondCaracteristic();
    Feature getThirdCaracteristic();
    Feature getFourthCaracteristic();
};




class Scroll : public Item
{
public:
    Scroll(QString name, QString imagePath, int weight, int price);
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
    Tool(QString name, QString imagePath, int weight, int price);
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
    virtual int type() const override { return eQGraphicItemType::shovel; }
};

class Pickaxe : public Tool
{
public:
    Pickaxe();
    ~Pickaxe();
public :
    bool use();
    virtual int type() const override { return eQGraphicItemType::pickaxe; }
};

class FishingRod : public Tool
{
public:
    FishingRod();
    ~FishingRod();
public :
    bool use();
    virtual int type() const override { return eQGraphicItemType::fishingrod; }
};

class Compass : public Tool
{
public:
    Compass();
    ~Compass();
public :
    bool use();
    virtual int type() const override { return eQGraphicItemType::compass; }
};

class Knife : public Tool
{
public:
    Knife();
    ~Knife();
public :
    bool use();
    virtual int type() const override { return eQGraphicItemType::knife; }
};

class Monster;
class MapItem;

class MapScroll : public Tool
{
    Q_OBJECT
public:
    typedef struct
    {
        QTimer timer;
        QVector<QVector<bool>> fogMatrix;
        QList<Monster*> monsters;
        QList<MapItem*> mapItem;
    }Map_Discovery;
public:
    MapScroll();
    ~MapScroll();
signals:
    void sig_update();
public slots:
    void init();
private slots:
    void updateDiscovery();
public :
    bool use();
    void discoveryMode(bool enable) { enable ? mMapDiscovery.timer.start() : mMapDiscovery.timer.stop(); }
    const Map_Discovery & getDiscoveryData() const { return mMapDiscovery; }
    bool getFilter(QString key) { return mFilter[key]; }
    void setFilter(QString key, bool enable) { mFilter[key] = enable; }
private:
    Map_Discovery mMapDiscovery;
    QMap<QString, bool> mFilter;
};

#endif // ITEM_H
