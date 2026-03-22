#include "win_loadinggamescreen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QResizeEvent>
#include <QRandomGenerator>

#define NUMBER_IMAGES            6
#define TIME_BEFORE_NEXT_PAGE_MS 6000

const QStringList Win_LoadingGameScreen::sTips = {
    "Récoltez des matériaux en tuant des monstres pour forger de puissants équipements chez le forgeron.",
    "Pensez à choisir auprès de l'alchemiste quel type de potion doit être concocté, elle ne travaillera pas sans vous.",
    "L'épeiste est une classe simple et complète efficace pour le combat.",
    "L'archer est une classe spécialisé dans l'endurance, lui permettant d'assainir des coups surpuissants.",
    "Le mage est une classe spécialisé dans les sorts, parmis eux, les sorts mineurs et les sorts majeurs.",
    "Les amats de terre peuvent être creusés avec une pelle pour y déterrer des coffres enfouis.",
    "Les minerais peuvent être recoltés avec une pioche pour obtenir des matériaux.",
    "Des poissons peuvent être pêché avec une canne à pêche au bord des lacs.",
    "Passer dans les buissons pour découvrir des surprises, ou mieux, laissez les monstres s'en charger à votre place.",
    "Déplacez certains élement avec votre souris pour y découvrir des objets cachés.",
    "L'Archimage permet d'apprendre des sorts puissants réservés au Mage, il est couteux mais en vaut la chandelle",
    "Vendez les objets dont vous n'avez plus besoin au marchand pour accumuler des pièces d'or.",
    "Les monstres, une fois chassés, doivent être dépecé pour obtenir des materiaux.",
    "Le héros peut obtenir des points de compétence avec une augmentation de niveau ou la consommation d'une potions anciennes",
};

Win_LoadingGameScreen::Win_LoadingGameScreen(QWidget* parent)
    : QWidget(parent),
      mImageIndex(0),
      mCurrentTip(0)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);

    // ── Tip card (centre) ──────────────────────────────────────────────────

    mTipCard = new QWidget(this);
    mTipCard->setStyleSheet(R"(
        QWidget {
            background-color: rgba(30, 30, 30, 215);
            border-radius: 16px;
        }
    )");

    mTipTitle = new QLabel("Le saviez-vous ?", mTipCard);
    mTipTitle->setStyleSheet(
        "color: rgba(255, 200, 70, 255);"
        "font-size: 20px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
    );
    mTipTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFrame* separator = new QFrame(mTipCard);
    separator->setFrameShape(QFrame::HLine);
    separator->setFixedHeight(1);
    separator->setStyleSheet("background-color: rgba(255, 195, 70, 55); border: none;");

    mTipText = new QLabel(sTips[0], mTipCard);
    mTipText->setStyleSheet(
        "color: rgba(230, 230, 230, 230);"
        "font-size: 16px;"
        "background: transparent;"
        "border: none;"
    );
    mTipText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mTipText->setWordWrap(true);

    QVBoxLayout* cardLayout = new QVBoxLayout(mTipCard);
    cardLayout->setContentsMargins(32, 26, 32, 26);
    cardLayout->setSpacing(14);
    cardLayout->addWidget(mTipTitle);
    cardLayout->addWidget(separator);
    cardLayout->addWidget(mTipText);

    // ── Barre de chargement (bas droite) ──────────────────────────────────

    mLoadingRow = new QWidget(this);
    mLoadingRow->setAttribute(Qt::WA_TranslucentBackground, true);

    mStepLabel = new QLabel(mLoadingRow);
    mStepLabel->setStyleSheet(
        "color: rgba(255, 255, 255, 120);"
        "font-style: italic;"
        "font-size: 11px;"
        "background: transparent;"
    );
    mStepLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mProgressBar = new QProgressBar(mLoadingRow);
    mProgressBar->setFixedSize(180, 6);
    mProgressBar->setTextVisible(false);
    mProgressBar->setValue(0);
    mProgressBar->setStyleSheet(R"(
        QProgressBar {
            background-color: rgba(255, 255, 255, 35);
            border: none;
            border-radius: 3px;
        }
        QProgressBar::chunk {
            background-color: rgba(255, 255, 255, 210);
            border-radius: 3px;
        }
    )");

    QHBoxLayout* rowLayout = new QHBoxLayout(mLoadingRow);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(10);
    rowLayout->addWidget(mStepLabel);
    rowLayout->addWidget(mProgressBar, 0, Qt::AlignVCenter);
    mLoadingRow->adjustSize();

    // ── Images ────────────────────────────────────────────────────────────

    for (int index = 1; index <= NUMBER_IMAGES; index++)
        mImages.append(QPixmap(QString(":/images/Ressources/loading_%1.png").arg(index)));

    connect(&t_image, &QTimer::timeout, this, &Win_LoadingGameScreen::onUpdatePage);
}

void Win_LoadingGameScreen::show()
{
    mImageIndex  = 0;
    mCurrentTip  = 0;
    mTipText->setText(sTips[0]);
    mProgressBar->setValue(0);
    onUpdatePage();
    t_image.start(TIME_BEFORE_NEXT_PAGE_MS);
    QWidget::showFullScreen();
}

void Win_LoadingGameScreen::hide()
{
    t_image.stop();
    QWidget::hide();
}

void Win_LoadingGameScreen::onUpdatePage()
{
    int nextImage = mImageIndex;
    while (nextImage == mImageIndex)
        nextImage = static_cast<int>(QRandomGenerator::global()->bounded(NUMBER_IMAGES));
    mImageIndex = nextImage;

    int nextTip = mCurrentTip;
    while (nextTip == mCurrentTip)
        nextTip = static_cast<int>(QRandomGenerator::global()->bounded(sTips.size()));
    mCurrentTip = nextTip;
    mTipText->setText(sTips[mCurrentTip]);

    update();
}

void Win_LoadingGameScreen::updateLoadingProgress(quint8 value)
{
    mProgressBar->setValue(value);
}

void Win_LoadingGameScreen::updateLoadingStep(const QString& text)
{
    mStepLabel->setText(text);
    mLoadingRow->adjustSize();
    repositionWidgets();
}

void Win_LoadingGameScreen::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    repositionWidgets();
}

void Win_LoadingGameScreen::repositionWidgets()
{
    const int bottomReserve = 130;
    const int diameter      = qMin(width(), height() - bottomReserve) * 3 / 4;
    const int circleBottom  = (height() - bottomReserve - diameter) / 2 + diameter;

    int cardW = qMin(diameter, width() - 60);
    mTipCard->setFixedWidth(cardW);
    mTipCard->adjustSize();
    mTipCard->move((width() - mTipCard->width()) / 2, circleBottom + 12);

    mLoadingRow->adjustSize();
    mLoadingRow->move(width() - mLoadingRow->width() - 20, height() - mLoadingRow->height() - 20);
}

void Win_LoadingGameScreen::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.fillRect(rect(), QColor("#272727"));

    const int bottomReserve = 130;
    const int diameter = qMin(width(), height() - bottomReserve) * 3 / 4;
    const int circleX  = (width()  - diameter) / 2;
    const int circleY  = (height() - bottomReserve - diameter) / 2;
    const QRect circleRect(circleX, circleY, diameter, diameter);

    if (diameter != mCachedDiameter)
    {
        mCachedDiameter = diameter;
        mCachedMask = QPixmap(diameter, diameter);
        mCachedMask.fill(Qt::transparent);

        QPainter mp(&mCachedMask);
        mp.setRenderHint(QPainter::Antialiasing);

        QRadialGradient gradient(diameter / 2.0, diameter / 2.0, diameter / 2.0);
        gradient.setColorAt(0.00, QColor(0, 0, 0, 255));
        gradient.setColorAt(0.50, QColor(0, 0, 0, 255));
        gradient.setColorAt(0.68, QColor(0, 0, 0, 200));
        gradient.setColorAt(0.80, QColor(0, 0, 0, 100));
        gradient.setColorAt(0.90, QColor(0, 0, 0, 30));
        gradient.setColorAt(1.00, QColor(0, 0, 0, 0));

        mp.setBrush(gradient);
        mp.setPen(Qt::NoPen);
        mp.drawEllipse(0, 0, diameter, diameter);
    }

    if (!mImages.isEmpty())
    {
        QPixmap frame(diameter, diameter);
        frame.fill(Qt::transparent);
        {
            QPainter fp(&frame);
            fp.setRenderHint(QPainter::SmoothPixmapTransform);
            fp.drawPixmap(frame.rect(), mImages.at(mImageIndex));
        }
        {
            QPainter fp(&frame);
            fp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            fp.drawPixmap(0, 0, mCachedMask);
        }
        painter.drawPixmap(circleRect, frame);
    }
}
