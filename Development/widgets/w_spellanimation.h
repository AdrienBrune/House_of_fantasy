/********************************************************************************
** Form generated from reading UI file 'w_spellanimation.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_W_SPELLANIMATION_H
#define UI_W_SPELLANIMATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_W_SpellAnimation
{
public:

    void setupUi(QWidget *W_SpellAnimation)
    {
        if (W_SpellAnimation->objectName().isEmpty())
            W_SpellAnimation->setObjectName(QString::fromUtf8("W_SpellAnimation"));
        W_SpellAnimation->resize(700, 700);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(W_SpellAnimation->sizePolicy().hasHeightForWidth());
        W_SpellAnimation->setSizePolicy(sizePolicy);

        retranslateUi(W_SpellAnimation);

        QMetaObject::connectSlotsByName(W_SpellAnimation);
    } // setupUi

    void retranslateUi(QWidget *W_SpellAnimation)
    {
        W_SpellAnimation->setWindowTitle(QCoreApplication::translate("W_SpellAnimation", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class W_SpellAnimation: public Ui_W_SpellAnimation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_W_SPELLANIMATION_H
