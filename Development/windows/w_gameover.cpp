#include "w_gameover.h"

W_GameOver::W_GameOver(QWidget* parent)
    : QWidget(parent)
    , mOverlayOpacity(0.0)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    if(parent)
        setGeometry(parent->rect());

    // Fade-in the overlay over 1.2 seconds
    connect(&mFadeTimer, &QTimer::timeout, this, [this]() {
        mOverlayOpacity += 0.015;
        if(mOverlayOpacity >= 1.0)
        {
            mOverlayOpacity = 1.0;
            mFadeTimer.stop();
            mBtnMenu->show();
        }
        update();
    });
    mFadeTimer.start(16);

    mBtnMenu = new QPushButton("Retour au menu", this);
    mBtnMenu->setFixedSize(220, 50);
    mBtnMenu->move((width() - mBtnMenu->width()) / 2, height() * 2 / 3 + 60);
    mBtnMenu->setStyleSheet(
        "QPushButton { background-color: rgba(180,20,20,200); color: white; "
        "              border: 2px solid #880000; border-radius: 8px; "
        "              font-size: 16px; font-weight: bold; }"
        "QPushButton:hover   { background-color: rgba(220,40,40,220); }"
        "QPushButton:pressed { background-color: rgba(140,10,10,240); }"
    );
    mBtnMenu->hide(); // shown once fade-in is complete
    connect(mBtnMenu, &QPushButton::clicked, this, &W_GameOver::sig_returnToMenu);
}

void W_GameOver::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dark red vignette background
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor(10,  0,  0, static_cast<int>(220 * mOverlayOpacity)));
    bg.setColorAt(0.5, QColor(40,  0,  0, static_cast<int>(200 * mOverlayOpacity)));
    bg.setColorAt(1.0, QColor(10,  0,  0, static_cast<int>(220 * mOverlayOpacity)));
    painter.fillRect(rect(), bg);

    if(mOverlayOpacity < 0.4)
        return;

    qreal textOpacity = qMin(1.0, (mOverlayOpacity - 0.4) / 0.6);
    painter.setOpacity(textOpacity);

    // "GAME OVER"
    QFont titleFont("Sitka Small", 72, QFont::Bold);
    painter.setFont(titleFont);

    // Shadow
    painter.setPen(QColor(80, 0, 0));
    painter.drawText(QRect(3, height() / 3 + 3, width(), 110), Qt::AlignCenter, "GAME OVER");

    // Main text
    painter.setPen(QColor(200, 20, 20));
    painter.drawText(QRect(0, height() / 3, width(), 110), Qt::AlignCenter, "GAME OVER");

    // Subtitle
    QFont subFont("Sitka Small", 18);
    painter.setFont(subFont);
    painter.setPen(QColor(180, 100, 100));
    painter.drawText(QRect(0, height() / 3 + 110, width(), 40), Qt::AlignCenter, "Le héros est mort...");

    painter.setOpacity(1.0);
}
