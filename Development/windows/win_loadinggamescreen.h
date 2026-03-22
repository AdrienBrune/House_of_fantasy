#ifndef WIN_LOADINGGAMESCREEN_H
#define WIN_LOADINGGAMESCREEN_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

class Win_LoadingGameScreen : public QWidget
{
    Q_OBJECT

public:
    explicit Win_LoadingGameScreen(QWidget *parent = nullptr);
    ~Win_LoadingGameScreen() = default;

public:
    void show();
    void hide();

private slots:
    void onUpdatePage();

public slots:
    void updateLoadingProgress(quint8);
    void updateLoadingStep(const QString&);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    void repositionWidgets();

private:
    int            mImageIndex;
    int            mCurrentTip;
    QList<QPixmap> mImages;
    QTimer         t_image;

    // Centre — tip card
    QWidget* mTipCard;
    QLabel*  mTipTitle;
    QLabel*  mTipText;

    // Bas droite — barre de chargement
    QWidget*      mLoadingRow;
    QLabel*       mStepLabel;
    QProgressBar* mProgressBar;

    QPixmap mCachedMask;
    int     mCachedDiameter = -1;

    static const QStringList sTips;
};

#endif // WIN_LOADINGGAMESCREEN_H
