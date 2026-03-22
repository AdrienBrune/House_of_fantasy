#ifndef W_GAMEOVER_H
#define W_GAMEOVER_H

#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class W_GameOver : public QWidget
{
    Q_OBJECT

public:
    explicit W_GameOver(QWidget* parent = nullptr);
    ~W_GameOver() {}

signals:
    void sig_returnToMenu();

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QPushButton* mBtnMenu;
    qreal        mOverlayOpacity;
    QTimer       mFadeTimer;
};

#endif // W_GAMEOVER_H
