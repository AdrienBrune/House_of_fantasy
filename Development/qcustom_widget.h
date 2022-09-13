#ifndef QCUSTOM_WIDGET_H
#define QCUSTOM_WIDGET_H

#include <QLabel>
#include <QPushButton>

class QLabel_Hover : public QLabel
{
    Q_OBJECT

public:
    explicit QLabel_Hover(QWidget *parent = nullptr):QLabel(parent){};
    ~QLabel_Hover(){};
signals:
    void sig_hoverIn();
    void sig_hoverOut();
public:
    void setImage(QPixmap image)
    {
        mPixmap = image;
        setShowImage(true);
    };
    void setShowImage(bool toggle)
    {
        toggle ? setPixmap(mPixmap) : setPixmap(QPixmap());
    };
protected:
    void enterEvent(QEvent*) override
    {
        emit sig_hoverIn();
    };
    void leaveEvent(QEvent*) override
    {
        emit sig_hoverOut();
    };
private:
    QPixmap mPixmap;
};

class QPushButton_Hover : public QPushButton
{
    Q_OBJECT

public:
    explicit QPushButton_Hover(QWidget *parent = nullptr):QPushButton(parent){};
    ~QPushButton_Hover(){};
signals:
    void sig_hoverIn();
    void sig_hoverOut();
protected:
    void enterEvent(QEvent*) override
    {
        emit sig_hoverIn();
    };
    void leaveEvent(QEvent*) override
    {
        emit sig_hoverOut();
    };
};

#endif // QCUSTOM_WIDGET_H
