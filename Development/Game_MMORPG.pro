#-------------------------------------------------
#
# Project created by QtCreator 2020-11-20T20:21:11
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Game_MMORPG
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        bag.cpp \
        character.cpp \
        consumable.cpp \
        ctrwindow.cpp \
        equipment.cpp \
        frag_interface_gear.cpp \
        frag_interface_itemsorter.cpp \
        frag_interface_itemtrader.cpp \
        frag_interface_potionpreferencies.cpp \
        frag_speel.cpp \
        frag_save.cpp \
        frag_stats_displayer.cpp \
        helpdialog.cpp \
        hero.cpp \
        item.cpp \
        itemgenerator.cpp \
        main.cpp \
        map.cpp \
        mapevent.cpp \
        mapitem.cpp \
        material.cpp \
        monster.cpp \
        skill.cpp \
        soundmanager.cpp \
        toolfunctions.cpp \
        village.cpp \
        village_goblin.cpp \
        w_animation_exploration.cpp \
        w_animation_fight.cpp \
        w_animation_forge.cpp \
        w_animation_night.cpp \
        w_animationspell.cpp \
        w_interface_herostats.cpp \
        w_itemdisplayer.cpp \
        w_messagelogger.cpp \
        w_quicktooldrawer.cpp \
        w_skill.cpp \
        w_spelllistselection.cpp \
        w_usetool.cpp \
        win_fight.cpp \
        win_gear.cpp \
        win_heroselection.cpp \
        win_interface_trading.cpp \
        win_inventory.cpp \
        win_loadinggamescreen.cpp \
        win_loadsave.cpp \
        win_skillinfo.cpp \
        win_skills.cpp \
        win_menu.cpp \
        save.cpp

HEADERS += \
        bag.h \
        character.h \
        constants.h \
        consumable.h \
        ctrwindow.h \
        equipment.h \
        frag_interface_gear.h \
        frag_interface_itemsorter.h \
        frag_interface_itemtrader.h \
        frag_interface_potionpreferencies.h \
        frag_speel.h \
        frag_save.h \
        frag_stats_displayer.h \
        helpdialog.h \
        hero.h \
        item.h \
        itemgenerator.h \
        map.h \
        mapevent.h \
        mapitem.h \
        material.h \
        monster.h \
        qcustom_widget.h \
        skill.h \
        soundmanager.h \
        toolfunctions.h \
        village.h \
        village_goblin.h \
        w_animation_exploration.h \
        w_animation_fight.h \
        w_animation_forge.h \
        w_animation_night.h \
        w_animationspell.h \
        w_interface_herostats.h \
        w_itemdisplayer.h \
        w_messagelogger.h \
        w_quicktooldrawer.h \
        w_skill.h \
        w_spellanimation.h \
        w_spelllistselection.h \
        w_usetool.h \
        win_fight.h \
        win_gear.h \
        win_heroselection.h \
        win_interface_trading.h \
        win_inventory.h \
        win_loadinggamescreen.h \
        win_loadsave.h \
        win_skillinfo.h \
        win_skills.h \
        win_menu.h \
        save.h

FORMS += \
         ctrwindow.ui \
        frag_interface_gear.ui \
        frag_interface_itemsorter.ui \
        frag_interface_itemtrader.ui \
        frag_interface_potionpreferencies.ui \
        frag_speel.ui \
        frag_stats_displayer.ui \
        frag_save.ui \
        helpdialog.ui \
    w_animationspell.ui \
        w_interface_herostats.ui \
    w_messagelogger.ui \
        w_showinformation.ui \
        w_showmonsterdata.ui \
        w_showpotentialaction.ui \
        w_skill.ui \
        w_spelllistselection.ui \
        win_alchemist.ui \
        win_blacksmith.ui \
        win_chest.ui \
        win_fight.ui \
        win_gear.ui \
    win_heroselection.ui \
        win_inventory.ui \
    win_loadinggamescreen.ui \
    win_loadsave.ui \
        win_merchant.ui \
        win_skillinfo.ui \
        win_skills.ui \
        win_menu.ui

RESOURCES += \
    Ressources/res.qrc

CONFIG += resources_big

RC_ICONS = ./Ressources/rpg_game.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
