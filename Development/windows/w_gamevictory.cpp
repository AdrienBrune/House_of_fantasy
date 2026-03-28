#include "w_gamevictory.h"

W_GameVictory::W_GameVictory(QWidget* parent)
    : QWidget(parent)
    , mOverlayOpacity(0.0)
    , mRemainingMs(TOTAL_MS)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    if(parent)
        setGeometry(parent->rect());

    // Fade-in sur 1.2 secondes
    connect(&mFadeTimer, &QTimer::timeout, this, [this]() {
        mOverlayOpacity += 0.015;
        if(mOverlayOpacity >= 1.0) {
            mOverlayOpacity = 1.0;
            mFadeTimer.stop();
        }
        update();
    });
    mFadeTimer.start(16);

    // Compte à rebours : tick toutes les 100ms pour une barre fluide
    connect(&mCountdownTimer, &QTimer::timeout, this, [this]() {
        mRemainingMs -= 100;
        if(mRemainingMs <= 0) {
            mRemainingMs = 0;
            mCountdownTimer.stop();
            emit sig_quit();
        }
        update();
    });
    mCountdownTimer.start(100);
}

void W_GameVictory::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fond sombre bleu nuit
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor( 0,  5, 20, static_cast<int>(230 * mOverlayOpacity)));
    bg.setColorAt(0.5, QColor( 5, 10, 35, static_cast<int>(215 * mOverlayOpacity)));
    bg.setColorAt(1.0, QColor( 0,  5, 20, static_cast<int>(230 * mOverlayOpacity)));
    painter.fillRect(rect(), bg);

    if(mOverlayOpacity < 0.4)
        return;

    qreal textOpacity = qMin(1.0, (mOverlayOpacity - 0.4) / 0.6);
    painter.setOpacity(textOpacity);

    // Titre "JEU TERMINÉ"
    QFont titleFont("Sitka Small", 72, QFont::Bold);
    painter.setFont(titleFont);

    // Ombre
    painter.setPen(QColor(60, 40, 0));
    painter.drawText(QRect(3, height() / 3 + 3, width(), 130), Qt::AlignCenter, "JEU TERMINÉ");

    // Texte doré
    painter.setPen(QColor(220, 165, 25));
    painter.drawText(QRect(0, height() / 3, width(), 130), Qt::AlignCenter, "JEU TERMINÉ");

    // Sous-titre
    QFont subFont("Sitka Small", 18);
    painter.setFont(subFont);
    painter.setPen(QColor(200, 175, 100));
    painter.drawText(QRect(0, height() / 3 + 130, width(), 60), Qt::AlignCenter,
                     "Le Lao Shan Lung est terrassé !");

    // --- Barre de compte à rebours ---
    int totalSec = mRemainingMs / 1000;
    int minutes  = totalSec / 60;
    int seconds  = totalSec % 60;
    double ratio = static_cast<double>(mRemainingMs) / TOTAL_MS;

    int barW = width() * 2 / 3;
    int barH = 18;
    int barX = (width() - barW) / 2;
    int barY = height() * 2 / 3 + 20;

    // Fond de la barre
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(15, 15, 35, 200));
    painter.drawRoundedRect(barX, barY, barW, barH, 9, 9);

    // Remplissage : or → rouge au fur et à mesure que le temps passe
    int r = static_cast<int>(30  + (220 - 30)  * ratio);
    int g = static_cast<int>(20  + (155 - 20)  * ratio);
    int b = static_cast<int>(10  + (20  - 10)  * ratio);
    painter.setBrush(QColor(r, g, b, 220));
    painter.drawRoundedRect(barX, barY, static_cast<int>(barW * ratio), barH, 9, 9);

    // Bordure dorée
    painter.setPen(QPen(QColor(180, 140, 40), 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(barX, barY, barW, barH, 9, 9);

    // Texte du compte à rebours
    QFont timerFont("Sitka Small", 13);
    painter.setFont(timerFont);
    painter.setPen(QColor(195, 175, 115));
    painter.drawText(QRect(0, barY + barH + 8, width(), 40), Qt::AlignCenter,
                     QString("Le jeu se fermera dans %1:%2")
                         .arg(minutes)
                         .arg(seconds, 2, 10, QChar('0')));

    painter.setOpacity(1.0);
}
