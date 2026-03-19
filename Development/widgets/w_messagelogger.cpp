#include "w_messagelogger.h"
#include "ui_w_messagelogger.h"

W_MessageLogger::W_MessageLogger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_MessageLogger)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

W_MessageLogger::~W_MessageLogger()
{
    delete ui;
}

void W_MessageLogger::log(LogMessage * message)
{
    message->setParent(this);
    connect(message, SIGNAL(sig_timeout()), this, SLOT(onRemoveMessage()));
    mMessages.append(message);
    updateDisplay();
}

void W_MessageLogger::setVisible(bool toggle)
{
    for (LogMessage* msg : mMessages)
        msg->setVisible(toggle);
    QWidget::setVisible(toggle);
}

void W_MessageLogger::onRemoveMessage()
{
    LogMessage* message = static_cast<LogMessage*>(sender());
    mMessages.removeOne(message);
    message->deleteLater();

    if (mMessages.isEmpty())
        emit sig_closeWindow();
    else
        updateDisplay();
}

void W_MessageLogger::updateDisplay()
{
    const int spacing = 5;
    int yBottom = height();

    for (int i = 0; i < mMessages.size(); i++)
    {
        LogMessage* msg = mMessages[i];

        msg->setFixedWidth(width());

        if (i < VISIBLE_MESSAGES)
        {
            msg->setHeaderVisible(false);
            msg->setBodyVisible(true);

            msg->setMinimumHeight(0);
            msg->setMaximumHeight(QWIDGETSIZE_MAX);
            msg->adjustSize();

            yBottom -= msg->height();
            msg->move(0, yBottom);
            yBottom -= spacing;
            msg->raise();
        }
        else
        {
            msg->setHeaderVisible(true);
            msg->setBodyVisible(false);

            msg->setFixedHeight(HEADER_HEIGHT_PX);

            LogMessage* lastVisible = mMessages[VISIBLE_MESSAGES - 1];
            int peekIndex = i - VISIBLE_MESSAGES + 1;
            msg->move(0, lastVisible->y() - peekIndex * HEADER_HEIGHT_PX);
            msg->lower();
        }
        msg->setVisible(true);
    }
}
