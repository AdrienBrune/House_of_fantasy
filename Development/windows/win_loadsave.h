#ifndef WIN_LOADSAVE_H
#define WIN_LOADSAVE_H

#include <QDialog>

#include "save.h"

namespace Ui {
class Win_LoadSave;
}

class Win_LoadSave : public QDialog
{
    Q_OBJECT

public:
    explicit Win_LoadSave(QWidget *parent = nullptr);
    ~Win_LoadSave();

signals:
    void sig_unFocusAllExcept(Save*);
    void sig_selectSave(Save*);

private slots:
    void onFocusSave(Save*);
    void onUnfocusSave();
    void on_buttonValid_clicked();
    void on_buttonCancel_clicked();

protected:
    void paintEvent(QPaintEvent*);

private:
    Save * mSaveSelected;
    QList<Save*> mSaves;

private:
    Ui::Win_LoadSave *ui;
};

#endif // WIN_LOADSAVE_H
