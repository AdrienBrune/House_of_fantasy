#ifndef FRAG_STATS_DISPLAYER_H
#define FRAG_STATS_DISPLAYER_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class Frag_Stats_Displayer;
}

class Frag_Stats_Displayer : public QWidget
{
public:
    explicit Frag_Stats_Displayer(QWidget *parent = nullptr, QString name = "", QPixmap image = QPixmap(), int current = 0, int maximum = 0, int mode = 0);
    ~Frag_Stats_Displayer();
public:
    void setStats(QString name, QPixmap image, int current, int maximum);
    void changeStats(int current, int maximum);
protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::Frag_Stats_Displayer *ui;
};

#endif // FRAG_STATS_DISPLAYER_H
