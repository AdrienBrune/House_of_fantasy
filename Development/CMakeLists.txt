cmake_minimum_required(VERSION 3.16)

project(House_of_fantasy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_UNITY_BUILD ON) # turbo build !

set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019_64")
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64")

# --- Qt automatique
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
)

# main sources path
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(UI_DIR  ${CMAKE_CURRENT_SOURCE_DIR})
set(QRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Ressources)

if(NOT EXISTS "${RES_DIR}")
    message(FATAL_ERROR
        "Ressource folder not found : ${RES_DIR}\n"
        "Please visit https://github.com/AdrienBrune/House_of_fantasy and get google drive link to download the annexe ressource folder"
    )
endif()

# =========================
# Sources
# =========================
set(HEADERS
    ${SRC_DIR}/daynightcycle.h
    ${SRC_DIR}/bag.h
    ${SRC_DIR}/character.h
    ${SRC_DIR}/consumable.h
    ${SRC_DIR}/ctrwindow.h
    ${SRC_DIR}/entitieshandler.h
    ${SRC_DIR}/equipment.h
    ${SRC_DIR}/widgets/frags/frag_interface_gear.h
    ${SRC_DIR}/widgets/frags/frag_interface_itemsorter.h
    ${SRC_DIR}/widgets/frags/frag_interface_itemtrader.h
    ${SRC_DIR}/widgets/frags/frag_speel.h
    ${SRC_DIR}/widgets/frags/frag_save.h
    ${SRC_DIR}/widgets/frags/frag_stats_displayer.h
    ${SRC_DIR}/hero.h
    ${SRC_DIR}/item.h
    ${SRC_DIR}/itemgenerator.h
    ${SRC_DIR}/map.h
    ${SRC_DIR}/mapevent.h
    ${SRC_DIR}/mapitem.h
    ${SRC_DIR}/material.h
    ${SRC_DIR}/monster.h
    ${SRC_DIR}/monsterfightview.h
    ${SRC_DIR}/skill.h
    ${SRC_DIR}/soundmanager.h
    ${SRC_DIR}/toolfunctions.h
    ${SRC_DIR}/village.h
    ${SRC_DIR}/village_goblin.h
    ${SRC_DIR}/widgets/w_potioncookingslot.h
    ${SRC_DIR}/widgets/w_animation_exploration.h
    ${SRC_DIR}/widgets/w_animation_fight.h
    ${SRC_DIR}/widgets/w_animation_forge.h
    ${SRC_DIR}/widgets/w_animation_night.h
    ${SRC_DIR}/widgets/w_animationspell.h
    ${SRC_DIR}/widgets/w_itemdisplayer.h
    ${SRC_DIR}/widgets/w_messagelogger.h
    ${SRC_DIR}/widgets/w_quicktooldrawer.h
    ${SRC_DIR}/widgets/w_skill.h
    ${SRC_DIR}/widgets/w_spelllistselection.h
    ${SRC_DIR}/widgets/w_usetool.h
    ${SRC_DIR}/widgets/w_herostats.h
    ${SRC_DIR}/widgets/w_daynightcycle.h
    ${SRC_DIR}/windows/win_fight.h
    ${SRC_DIR}/windows/win_gear.h
    ${SRC_DIR}/windows/win_heroselection.h
    ${SRC_DIR}/windows/win_interface_trading.h
    ${SRC_DIR}/windows/win_inventory.h
    ${SRC_DIR}/windows/win_loadinggamescreen.h
    ${SRC_DIR}/windows/win_loadsave.h
    ${SRC_DIR}/windows/win_skillinfo.h
    ${SRC_DIR}/windows/win_skills.h
    ${SRC_DIR}/windows/win_menu.h
    ${SRC_DIR}/save.h
    ${SRC_DIR}/qcustom_widget.h
    ${SRC_DIR}/widgets/w_spellanimation.h
    ${SRC_DIR}/constants.h
)

# =========================
# Sources
# =========================
set(SOURCES
    ${SRC_DIR}/daynightcycle.cpp
    ${SRC_DIR}/bag.cpp
    ${SRC_DIR}/character.cpp
    ${SRC_DIR}/consumable.cpp
    ${SRC_DIR}/ctrwindow.cpp
    ${SRC_DIR}/entitieshandler.cpp
    ${SRC_DIR}/equipment.cpp
    ${SRC_DIR}/widgets/frags/frag_interface_gear.cpp
    ${SRC_DIR}/widgets/frags/frag_interface_itemsorter.cpp
    ${SRC_DIR}/widgets/frags/frag_interface_itemtrader.cpp
    ${SRC_DIR}/widgets/frags/frag_speel.cpp
    ${SRC_DIR}/widgets/frags/frag_save.cpp
    ${SRC_DIR}/widgets/frags/frag_stats_displayer.cpp
    ${SRC_DIR}/hero.cpp
    ${SRC_DIR}/item.cpp
    ${SRC_DIR}/itemgenerator.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/map.cpp
    ${SRC_DIR}/mapevent.cpp
    ${SRC_DIR}/mapitem.cpp
    ${SRC_DIR}/material.cpp
    ${SRC_DIR}/monster.cpp
    ${SRC_DIR}/monsterfightview.cpp
    ${SRC_DIR}/skill.cpp
    ${SRC_DIR}/soundmanager.cpp
    ${SRC_DIR}/toolfunctions.cpp
    ${SRC_DIR}/village.cpp
    ${SRC_DIR}/village_goblin.cpp
    ${SRC_DIR}/widgets/w_potioncookingslot.cpp
    ${SRC_DIR}/widgets/w_animation_exploration.cpp
    ${SRC_DIR}/widgets/w_animation_fight.cpp
    ${SRC_DIR}/widgets/w_animation_forge.cpp
    ${SRC_DIR}/widgets/w_animation_night.cpp
    ${SRC_DIR}/widgets/w_animationspell.cpp
    ${SRC_DIR}/widgets/w_itemdisplayer.cpp
    ${SRC_DIR}/widgets/w_messagelogger.cpp
    ${SRC_DIR}/widgets/w_quicktooldrawer.cpp
    ${SRC_DIR}/widgets/w_skill.cpp
    ${SRC_DIR}/widgets/w_spelllistselection.cpp
    ${SRC_DIR}/widgets/w_usetool.cpp
    ${SRC_DIR}/widgets/w_herostats.cpp
    ${SRC_DIR}/widgets/w_daynightcycle.cpp
    ${SRC_DIR}/windows/win_fight.cpp
    ${SRC_DIR}/windows/win_gear.cpp
    ${SRC_DIR}/windows/win_heroselection.cpp
    ${SRC_DIR}/windows/win_interface_trading.cpp
    ${SRC_DIR}/windows/win_inventory.cpp
    ${SRC_DIR}/windows/win_loadinggamescreen.cpp
    ${SRC_DIR}/windows/win_loadsave.cpp
    ${SRC_DIR}/windows/win_skillinfo.cpp
    ${SRC_DIR}/windows/win_skills.cpp
    ${SRC_DIR}/windows/win_menu.cpp
    ${SRC_DIR}/save.cpp
)

# =========================
# UI files
# =========================
set(FORMS
    ${UI_DIR}/ctrwindow.ui
    ${UI_DIR}/widgets/frags/frag_interface_gear.ui
    ${UI_DIR}/widgets/frags/frag_interface_itemsorter.ui
    ${UI_DIR}/widgets/frags/frag_interface_itemtrader.ui
    ${UI_DIR}/widgets/frags/frag_speel.ui
    ${UI_DIR}/widgets/frags/frag_stats_displayer.ui
    ${UI_DIR}/widgets/frags/frag_save.ui
    ${UI_DIR}/widgets/w_animationspell.ui
    ${UI_DIR}/widgets/w_messagelogger.ui
    ${UI_DIR}/widgets/w_showinformation.ui
    ${UI_DIR}/widgets/w_showmonsterdata.ui
    ${UI_DIR}/widgets/w_showpotentialaction.ui
    ${UI_DIR}/widgets/w_skill.ui
    ${UI_DIR}/widgets/w_spelllistselection.ui
    ${UI_DIR}/windows/win_alchemist.ui
    ${UI_DIR}/windows/win_blacksmith.ui
    ${UI_DIR}/windows/win_chest.ui
    ${UI_DIR}/windows/win_fight.ui
    ${UI_DIR}/windows/win_gear.ui
    ${UI_DIR}/windows/win_heroselection.ui
    ${UI_DIR}/windows/win_inventory.ui
    ${UI_DIR}/windows/win_loadinggamescreen.ui
    ${UI_DIR}/windows/win_loadsave.ui
    ${UI_DIR}/windows/win_merchant.ui
    ${UI_DIR}/windows/win_skillinfo.ui
    ${UI_DIR}/windows/win_skills.ui
    ${UI_DIR}/windows/win_menu.ui
)

# =========================
# Resources
# =========================
set(RESOURCES 
    ${QRC_DIR}/res_background.qrc
    ${QRC_DIR}/res_mapitems.qrc
    ${QRC_DIR}/res_equipment.qrc
    ${QRC_DIR}/res_items.qrc
    ${QRC_DIR}/res_monsters.qrc
    ${QRC_DIR}/res_audio.qrc
    ${QRC_DIR}/res_graphics.qrc
    ${QRC_DIR}/res_music_village.qrc
    ${QRC_DIR}/res_music_fight.qrc
    ${QRC_DIR}/res_music_event.qrc
    ${QRC_DIR}/res_music_world.qrc
)

# =========================
# Executable
# =========================
add_executable(House_of_fantasy
    ${HEADERS}
    ${SOURCES}
    ${FORMS}
    ${RESOURCES}
)
target_include_directories(House_of_fantasy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(House_of_fantasy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/windows)
target_include_directories(House_of_fantasy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/widgets)
target_include_directories(House_of_fantasy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/widgets/frags)

# =========================
# Link Qt
# =========================
target_link_libraries(House_of_fantasy PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
)

# =========================
# Defines
# =========================
target_compile_definitions(House_of_fantasy PRIVATE QT_DEPRECATED_WARNINGS)

# =========================
# Windows deployment
# =========================
if(WIN32)
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    set(DEPLOY_TOOL "${_qt_bin_dir}/windeployqt.exe")

    add_custom_command(TARGET House_of_fantasy POST_BUILD
        COMMAND "${DEPLOY_TOOL}" $<TARGET_FILE:House_of_fantasy>
        COMMENT "Running windeployqt..."
        VERBATIM
    )

    set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Application")

    add_custom_command(TARGET House_of_fantasy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<TARGET_FILE_DIR:House_of_fantasy>"
                "${OUTPUT_DIR}"
        COMMENT "Deploying Qt and copying runtime..."
        VERBATIM
    )
endif()

# build command
#cmake --build . --config Release
