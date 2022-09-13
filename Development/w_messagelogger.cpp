#include "w_messagelogger.h"
#include "ui_w_messagelogger.h"

#define LOG_MESSAGE_NUMBER_MAX  5

W_MessageLogger::W_MessageLogger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_MessageLogger)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    QTimer * t_timer = new QTimer(this);
    t_timer->setInterval(200);
    connect(t_timer, &QTimer::timeout, this, &W_MessageLogger::onTryToLogPendingMessage);
    t_timer->start();
}

W_MessageLogger::~W_MessageLogger()
{
    delete ui;
}

void W_MessageLogger::log(LogMessage * message)
{
    connect(message, SIGNAL(sig_timeout()), this, SLOT(onRemoveMessage()));
    if(ui->layout->count() < LOG_MESSAGE_NUMBER_MAX)
    {
        message->setVisible(true);
        ui->layout->addWidget(message, 0, Qt::AlignRight);
    }
    else
    {
        message->setVisible(false);
        mWaitingList << message;
    }
}

void W_MessageLogger::setVisible(bool toggle)
{
    for(int i = 0 ; i < ui->layout->count(); i++)
    {
        LogMessage * message = static_cast<LogMessage*>(ui->layout->itemAt(0)->widget());
        message->setVisible(toggle);
    }
    QWidget::setVisible(toggle);
}


void W_MessageLogger::onTryToLogPendingMessage()
{
    if(mWaitingList.isEmpty())
        return;

    if(ui->layout->count() < LOG_MESSAGE_NUMBER_MAX)
    {
        LogMessage * message = mWaitingList.takeFirst();
        ui->layout->addWidget(message, 0, Qt::AlignRight);
        message->setVisible(true);
    }
#ifdef DISABLE_WAITING_LIST
    else
    {
        LogMessage * message = dynamic_cast<LogMessage*>(ui->layout->itemAt(0)->widget());
        message->setVisible(false);
        ui->layout->removeWidget(message);
        if(message)
            delete message;

        message = mWaitingList.takeFirst();
        ui->layout->addWidget(message, 0, Qt::AlignRight);
        message->setVisible(true);
    }
#endif

    if(!ui->layout->count())
        emit sig_closeWindow();
}

void W_MessageLogger::onRemoveMessage()
{
    LogMessage * message = static_cast<LogMessage*>(sender());

    ui->layout->removeWidget(message);
    if(message)
        delete message;
}
