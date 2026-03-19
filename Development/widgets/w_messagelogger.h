#ifndef W_MESSAGELOGGER_H
#define W_MESSAGELOGGER_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "item.h"
#include "w_itemdisplayer.h"

#define TIMEOUT_MESSAGE_MS      10000
#define VISIBLE_MESSAGES        3
#define HEADER_HEIGHT_PX        26
#define ITEM_DISPLAY_SIZE_PX    200

#define COLOR_BLACK_LIGHT   QColor(40,40,40)
#define COLOR_HEADER_BG     QColor(30,30,38)

/* List all messages */
#define LM_SHOW_MERCHANT_RESPLENISH()   new LogMessageType_2(this, QString("Le marchand a reçu une nouvelle cargaison de fournitures\nPassez le voir !"), QPixmap(":/images/Ressources/ship.png"))
#define LM_SHOW_BLACKSMITH_RESPLENISH() new LogMessageType_2(this, QString("Le forgeron propose de forger un nouvel équipement\nPassez le voir !"), QPixmap(":/images/Ressources/anvil.png"))
#define LM_SHOW_ALCHEMIST_RESPLENISH()  new LogMessageType_2(this, QString("L'alchemist a concocté une nouvelle potion\nPassez la voir !"), QPixmap(":/images/Ressources/alchemist.png"))
#define ML_SHOW_MONSTER(monster)        new LogMessageType_2(this, QString("%1 - Menace %2\n%3").arg(monster->getName()).arg(monster->getThreatLevel()).arg(monster->getDescription()), QPixmap(monster->getImage()))
#define ML_SHOW_CALAMITY()              new LogMessageType_2(this, QString("Un Lao Shan Lung est apparu\nCette créature mythique est entourée de nombreux mystères et ses apparitions sont toujours synomyme de catastrophes"), QPixmap(":/graphicItems/Ressources/Lao.png"))
#define ML_SHOW_LEVEL_UP(hero)          new LogMessageType_1(this, QString("Niveau supérieur atteint\nNiveau actuel : %1\nPoints de compétence gagnés : 1").arg(hero->getExperience().level))
#define ML_SHOW_BAG_COIN(coinNumber)    new LogMessageType_2(this, QString("Un sac contenant %1 pièces a été rammassé").arg(coinNumber), QPixmap(":/images/Ressources/coinBag.png"))
#define ML_SHOW_ITEM_TOOK(item)         new LogMessageType_3(this, item)
#define ML_SHOW_CHEST_DUG_UP()          new LogMessageType_2(this, QString("Un coffre a été déterré\nIl ne semble pas fermé à clef"), QPixmap(":/images/Ressources/chest.png"))
#define ML_SHOW_CHEST_BURRIED()         new LogMessageType_2(this, QString("Quelque chose semble enterré ici, elle pourrait être déterrée avec le bon outil"), QPixmap(":/images/Ressources/chest_burried.png"))
#define ML_SHOW_ORE_SPOT(oreSpot)       new LogMessageType_2(this, QString("Vous avez trouvé\n%1").arg(oreSpot->getItems().first()->getName()), QPixmap(":/images/Ressources/oreSpotFound.png"))
#define ML_SHOW_ORE_TOOK(ore)           new LogMessageType_2(this, QString("%1 récolté").arg(ore->getName()), QPixmap(ore->getImage()))
#define ML_SHOW_NO_MORE_TO_HARVEST()    new LogMessageType_1(this, QString("Il n'y a plus rien ici"))
#define ML_SHOW_FISH_TOOK(fish)         new LogMessageType_2(this, QString("Vous venez de pécher un\n%1").arg(fish->getName()), fish->getImage())
#define ML_SHOW_FISH_SPOT()             new LogMessageType_2(this, QString("Les poissons semblent s'ammasser par ici, ils pourraient être péchés avec le bon outil"), QPixmap(":/images/Ressources/fishSpot.png"))
#define ML_SHOW_CARCASS()               new LogMessageType_2(this, QString("Ce cadavre pourrait être depecé avec le bon outil"), QPixmap(":/graphicItems/Ressources/carcass.png"))
#define ML_SHOW_MONSTER_SKINNED()       new LogMessageType_1(this, QString("Cadavre déjà dépecé"))
#define ML_SHOW_LOOT_NUMBER(number)     new LogMessageType_1(this, number ? ( number > 1 ? QString("%1 matériaux de monstre ont pu être extraits").arg(number) : QString("1 matériau de monstre a pu être extrait") ) : QString("Aucun matériau n'a pu être extrait"))
#define SHOW_BAG_FULL(bag)              new LogMessageType_2(this, QString("Votre inventaire est plein, augmentez la capacité de chargement via les potions\nCharge actuelle %1/%2").arg(bag->getPayload().maximum).arg(bag->getPayload().current), QPixmap(":/images/Ressources/bag.png"))
#define ML_SHOW_ITEM_ON_GROUND(item)    new LogMessageType_3(this, item)
#define ML_SHOW_PNG_INFO(house)         new LogMessageType_2(this, QString(house->getInformation()), QPixmap(house->getImage()))
#define ML_SHOW_SLEEP_IMPOSSIBLE(house) new LogMessageType_2(this, QString("Vous ne pouvez dormir que pendant le nuit"), QPixmap(house->getImage()))

namespace Ui {
class W_MessageLogger;
}

class QTimerCustom : public QTimer
{
    Q_OBJECT
public:
    explicit QTimerCustom(QWidget *parent = nullptr):QTimer(parent), mRemainingTime(TIMEOUT_MESSAGE_MS){ setSingleShot(true); setInterval(TIMEOUT_MESSAGE_MS); };
    ~QTimerCustom(){};
public:
    int getRemainingTime()const{ return mRemainingTime; }
    void pause(){ if(isActive()){ mRemainingTime = remainingTime(); stop();} }
    void resume(){ if(!isActive()) start(mRemainingTime); }
private:
    int mRemainingTime;
};



class LogMessage : public QWidget
{
    Q_OBJECT
public:
    explicit LogMessage(QWidget *parent = nullptr, QString message = QString()):
        QWidget(parent),
        mMessage(message),
        t_timer(nullptr),
        mAccentColor(Qt::gray),
        mHeaderVisible(true),
        mHeader(nullptr),
        mHeaderLabel(nullptr),
        mBody(nullptr)
    {
        t_timer = new QTimerCustom(this);
        t_timer->setSingleShot(true);
        connect(t_timer, SIGNAL(timeout()), this, SIGNAL(sig_timeout()));
        setVisible(false);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

        QVBoxLayout * outerLayout = new QVBoxLayout(this);
        outerLayout->setContentsMargins(0, 0, 0, 0);
        outerLayout->setSpacing(0);
        setLayout(outerLayout);

        // Header
        mHeader = new QWidget(this);
        mHeader->setFixedHeight(HEADER_HEIGHT_PX);
        mHeader->setAttribute(Qt::WA_TransparentForMouseEvents);
        {
            QHBoxLayout * hl = new QHBoxLayout(mHeader);
            hl->setContentsMargins(8, 0, 4, 0);
            hl->setSpacing(0);
            mHeaderLabel = new QLabel(mHeader);
            mHeaderLabel->setStyleSheet("QLabel{ color: rgba(255,255,255,200); }");
            mHeaderLabel->setFont(QFont("Poor Richard", 10));
            mHeaderLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            hl->addWidget(mHeaderLabel);
        }
        outerLayout->addWidget(mHeader);

        // Content area
        mBody = new QWidget(this);
        mBody->setAttribute(Qt::WA_TransparentForMouseEvents);
        outerLayout->addWidget(mBody);
    }
    ~LogMessage(){};
signals:
    void sig_timeout();
public:
    void setVisible(bool toggle) override
    {
        toggle ? t_timer->resume() : t_timer->pause();
        QWidget::setVisible(toggle);
    }
    void setHeaderVisible(bool visible)
    {
        mHeaderVisible = visible;
        mHeader->setVisible(visible);
        mHeader->setFixedHeight(visible ? HEADER_HEIGHT_PX : 0);
        update();
    }
    void setBodyVisible(bool visible)
    {
        mBody->setVisible(visible);
    }
protected:
    void setupHeader(const QString & title, QColor accentColor)
    {
        mAccentColor = accentColor;
        mHeaderLabel->setText(title.section('\n', 0, 0)); // first raw only
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setPen(Qt::NoPen);

        if (mHeaderVisible)
        {
            painter.setOpacity(0.97);
            painter.setBrush(COLOR_HEADER_BG);
            painter.drawRect(0, 0, width(), HEADER_HEIGHT_PX);

            // color bar on the left
            painter.setOpacity(1.0);
            painter.setBrush(mAccentColor);
            painter.drawRect(0, 0, 4, HEADER_HEIGHT_PX);
        }

        painter.setOpacity(0.90);
        painter.setBrush(COLOR_BLACK_LIGHT);
        painter.drawRect(0, mHeaderVisible ? HEADER_HEIGHT_PX : 0, width(), height() - (mHeaderVisible ? HEADER_HEIGHT_PX : 0));
    }
protected:
    QString       mMessage;
    QTimerCustom* t_timer;
    QColor        mAccentColor;
    bool          mHeaderVisible;
    QWidget*      mHeader;
    QLabel*       mHeaderLabel;
    QWidget*      mBody;
};



/* Simple text log message */
class LogMessageType_1 : public LogMessage
{
    Q_OBJECT
public:
    explicit LogMessageType_1(QWidget *parent = nullptr, QString message = QString()):LogMessage(parent, message)
    {
        setupHeader(mMessage, QColor(100, 120, 210));

        QLabel * label = new QLabel(mBody);
        label->setWordWrap(true);
        label->setText(mMessage);
        label->setStyleSheet("QLabel{color:white;}");
        label->setFont(QFont("Poor Richard", 12));
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

        QVBoxLayout * layout = new QVBoxLayout(mBody);
        mBody->setLayout(layout);
        layout->addWidget(label);
    }
    ~LogMessageType_1(){};
};

/* Image text log message */
class LogMessageType_2 : public LogMessage
{
    Q_OBJECT
public:
    explicit LogMessageType_2(QWidget *parent = nullptr, QString message = QString(), QPixmap pixmap = QPixmap()):LogMessage(parent, message)
    {
        setupHeader(mMessage, QColor(80, 170, 100));

        QLabel * label = new QLabel(mBody);
        label->setWordWrap(true);
        label->setText(mMessage);
        label->setStyleSheet("QLabel{color:white;}");
        label->setFont(QFont("Poor Richard", 12));
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        label->setAlignment(Qt::AlignCenter);

        QLabel * image = new QLabel(mBody);
        image->setScaledContents(true);
        image->setPixmap(pixmap);
        image->setMinimumSize(QSize(80, 80));
        image->setMaximumSize(QSize(80, 80));

        QHBoxLayout * layout = new QHBoxLayout(mBody);
        mBody->setLayout(layout);
        layout->addWidget(image);
        layout->addWidget(label);
    }
    ~LogMessageType_2(){};
};

/* Item description log message */
class LogMessageType_3 : public LogMessage
{
    Q_OBJECT
public:
    explicit LogMessageType_3(QWidget *parent = nullptr, Item * item = nullptr):LogMessage(parent, QString()), mWItem(nullptr)
    {
        setupHeader(item ? item->getName() : QString("Objet"), QColor(200, 170, 50));

        mWItem = new W_ItemDisplayer(mBody, item);
        mWItem->setFixedSize(ITEM_DISPLAY_SIZE_PX, ITEM_DISPLAY_SIZE_PX);
        mWItem->hideUseButton();
        connect(item, SIGNAL(destroyed(QObject*)), this, SIGNAL(sig_timeout()));

        QVBoxLayout * layout = new QVBoxLayout(mBody);
        layout->setMargin(0);
        layout->setAlignment(Qt::AlignCenter);
        mBody->setLayout(layout);
        layout->addWidget(mWItem);
    }
    ~LogMessageType_3(){};
private:
    W_ItemDisplayer * mWItem;
};



class W_MessageLogger : public QWidget
{
    Q_OBJECT
public:
    explicit W_MessageLogger(QWidget *parent = nullptr);
    ~W_MessageLogger();
signals:
    void sig_closeWindow();
public:
    void log(LogMessage*);
    void setVisible(bool)override;
private slots:
    void onRemoveMessage();
private:
    void updateDisplay();
private:
    QList<LogMessage*> mMessages;
private:
    Ui::W_MessageLogger *ui;
};

#endif // W_MESSAGELOGGER_H
