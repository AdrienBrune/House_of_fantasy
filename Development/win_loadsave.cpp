#include "win_loadsave.h"
#include "ui_win_loadsave.h"
#include "frag_save.h"

#include <QDir>

Win_LoadSave::Win_LoadSave(QWidget *parent) :
    QDialog(parent),
    mSaveSelected(nullptr),
    ui(new Ui::Win_LoadSave)
{
    ui->setupUi(this);
    ui->scrollAreaWidgetContents->setAutoFillBackground(false);
    setAutoFillBackground(false);
    setWindowFlags(Qt::FramelessWindowHint);

    /* Load saves */
    QDir saveDir(QDir::currentPath()+"/"+FILE_SAVE);
    if(!saveDir.exists())
        saveDir.mkdir(QDir::currentPath()+"/"+FILE_SAVE);

    while(!mSaves.isEmpty())
        delete mSaves.takeLast();

    /* Get saves */
    QStringList fileList = saveDir.entryList(QDir::Files);
    while(!fileList.isEmpty())
    {
        QFile file(QDir::currentPath()+"/"+FILE_SAVE+"/"+ fileList[0]);
        file.open(QIODevice::ReadOnly);
        QDataStream stream(&file);

        Save * save = new Save();
        stream >> *save;

        mSaves.append(save);

        fileList.removeFirst();
    }

    /* Display saves */
    for(Save * save : qAsConst(mSaves))
    {
        Frag_Save * saveDisplayer = new Frag_Save(this, save);
        connect(saveDisplayer, SIGNAL(sig_selectSave(Save*)), this, SLOT(onFocusSave(Save*)));
        connect(saveDisplayer, SIGNAL(sig_unselectSave()), this, SLOT(onUnfocusSave()));
        connect(this, SIGNAL(sig_unFocusAllExcept(Save*)), saveDisplayer, SLOT(onUnfocusExcept(Save*)));
        ui->layoutSaves->addWidget(saveDisplayer);
    }
}

Win_LoadSave::~Win_LoadSave()
{
    while(!mSaves.isEmpty())
    {
        Save * saveToDelete = mSaves.takeLast();
        if(mSaveSelected != saveToDelete)
            delete saveToDelete;
    }
    delete ui;
}

void Win_LoadSave::onFocusSave(Save * save)
{
    emit sig_unFocusAllExcept(save);
    mSaveSelected = save;
}

void Win_LoadSave::onUnfocusSave()
{
    mSaveSelected = nullptr;
}

void Win_LoadSave::on_buttonValid_clicked()
{
    emit sig_selectSave(mSaveSelected);
    close();
}

void Win_LoadSave::on_buttonCancel_clicked()
{
    emit sig_selectSave(nullptr);
    close();
}

void Win_LoadSave::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush("#EEEEEE"));
    painter.drawRect(0,0,width(),height());

    painter.setBrush(QBrush("#272727"));
    painter.drawRect(2,2,width()-4,height()-4);

}
