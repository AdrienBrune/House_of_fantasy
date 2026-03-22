#include "w_fightresult.h"

static const int PANEL_W        = 580;
static const int PANEL_H        = 430;
static const int RING_SIZE      = 180;
static const int RING_THICKNESS = 16;

W_FightResult::W_FightResult(QWidget* parent, int level, int expBefore, int expGained, int expToLevelUp, int coinGained)
    : QWidget(parent)
    , mLevel(level)
    , mExpBefore(expBefore)
    , mExpGained(expGained)
    , mExpToLevelUp(expToLevelUp > 0 ? expToLevelUp : 1)
    , mCoinGained(coinGained)
    , mLevelUp(expBefore + expGained >= expToLevelUp)
    , mClosing(false)
{
    // Cover the full parent
    if(parent)
        resize(parent->size());

    // Panel centered in this widget
    mPanel = QRect((width() - PANEL_W) / 2, (height() - PANEL_H) / 2, PANEL_W, PANEL_H);

    // XP arc angles
    mPreDeg     = (static_cast<qreal>(mExpBefore) / mExpToLevelUp) * 360.0;
    mCurrentDeg = mPreDeg;
    mTargetDeg  = qMin((static_cast<qreal>(mExpBefore + mExpGained) / mExpToLevelUp) * 360.0, 360.0);
    qreal travel = mTargetDeg - mPreDeg;
    mStep = (travel > 0.0) ? travel / 90.0 : 0.0;

    connect(&mAnimTimer, &QTimer::timeout, this, &W_FightResult::onAnimate);
    mAnimTimer.start(16);
}

W_FightResult::~W_FightResult()
{
    mAnimTimer.stop();
}

void W_FightResult::onAnimate()
{
    if(mCurrentDeg < mTargetDeg)
    {
        mCurrentDeg = qMin(mCurrentDeg + mStep, mTargetDeg);
        update();
    }
    else
    {
        if(mClosing)
            return;

        mClosing = true;
        mAnimTimer.stop();
        update();
        QTimer::singleShot(1000, this, [this]{ emit sig_closed(); });
    }
}

void W_FightResult::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ── Full-screen dark overlay ──────────────────────────────────────────
    painter.fillRect(rect(), QColor(0, 0, 0, 204)); // 0.8 * 255 ≈ 204

    // ── Panel background ─────────────────────────────────────────────────
    painter.setBrush(QBrush(QColor(25, 25, 25, 245)));
    painter.setPen(QPen(QColor(200, 120, 0), 2));
    painter.drawRoundedRect(mPanel, 14, 14);

    // ── Circular XP ring ──────────────────────────────────────────────────
    int ringX = mPanel.x() + (mPanel.width()  - RING_SIZE) / 2;
    int ringY = mPanel.y() + 30;
    QRect ringRect(ringX, ringY, RING_SIZE, RING_SIZE);

    // Background ring
    QPen bgPen(QColor(55, 55, 55), RING_THICKNESS);
    bgPen.setCapStyle(Qt::FlatCap);
    painter.setPen(bgPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(ringRect);

    // Pre-existing XP arc (dark orange)
    if(mPreDeg > 0.5)
    {
        QPen prePen(QColor(160, 80, 0), RING_THICKNESS);
        prePen.setCapStyle(Qt::FlatCap);
        painter.setPen(prePen);
        painter.drawArc(ringRect, 90 * 16, -static_cast<int>(mPreDeg * 16.0));
    }

    // Animated new XP arc (bright orange)
    if(mCurrentDeg > mPreDeg + 0.1)
    {
        QPen newPen(QColor(255, 145, 0), RING_THICKNESS);
        newPen.setCapStyle(Qt::RoundCap);
        painter.setPen(newPen);
        int startAngle = static_cast<int>((90.0 - mPreDeg) * 16.0);
        int spanAngle  = -static_cast<int>((mCurrentDeg - mPreDeg) * 16.0);
        painter.drawArc(ringRect, startAngle, spanAngle);
    }

    // Level text inside ring
    painter.setPen(QColor(220, 220, 220));
    painter.setFont(QFont("Sitka Small", 14, QFont::Bold));
    painter.drawText(ringRect, Qt::AlignCenter, QString("Niv. %1").arg(mLevel));

    // ── Stats text ────────────────────────────────────────────────────────
    int textY = ringRect.bottom() + 16;

    painter.setFont(QFont("Sitka Small", 14));
    painter.setPen(QColor(255, 165, 0));
    painter.drawText(QRect(mPanel.x(), textY, mPanel.width(), 34), Qt::AlignCenter, QString("+ %1 XP").arg(mExpGained));

    if(mCoinGained > 0)
    {
        painter.setPen(QColor(255, 215, 0));
        painter.drawText(QRect(mPanel.x(), textY + 34, mPanel.width(), 34), Qt::AlignCenter, QString("+ %1 Pièce%2").arg(mCoinGained).arg(mCoinGained > 1 ? "s" : ""));
    }

    // Level-up notice
    if(mLevelUp && mCurrentDeg >= mTargetDeg)
    {
        painter.setFont(QFont("Sitka Small", 15, QFont::Bold));
        painter.setPen(QColor(120, 220, 255));
        int noticeY = (mCoinGained > 0) ? textY + 68 : textY + 34;
        painter.drawText(QRect(mPanel.x(), noticeY, mPanel.width(), 34), Qt::AlignCenter, "+1 point de compétence");
    }
}
