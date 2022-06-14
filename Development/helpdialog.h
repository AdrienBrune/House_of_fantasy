#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();
private slots:
    void on_buttonReturn_clicked();
    void closeWindow();
public:
    void displayWindow();
    void hideWindow();
private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
