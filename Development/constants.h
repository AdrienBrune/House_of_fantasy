#ifndef CONSTANTS_H
#define CONSTANTS_H

//#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define DEBUG(str)      qDebug() << str
#define DEBUG_ERR(str)  qDebug() << str
#else
#define DEBUG(str)
#define DEBUG_ERR(str)  qDebug() << str
#endif

#define LOADING_MAX_STEPS   27
#define UPDATE_STEP(step)   (((++step)*100.0)/LOADING_MAX_STEPS)

#define FILE_SAVE   "saves"

//#define ENABLE_MOVEMENT_BY_MOUSE_CLIC

#define PERIODICAL_EVENTS   1000

// Speed definitions
#define SPEED_HERO              6
#define SPEED_SPIDER            3
#define SPEEDBOOST_SPIDER       6
#define SPEED_WOLF              1
#define SPEEDBOOST_WOLF         6
#define SPEED_GOBLIN            1
#define SPEEDBOOST_GOBLIN       3
#define SPEED_BEAR              2
#define SPEEDBOOST_BEAR         5
#define SPEED_TROLL             1
#define SPEEDBOOST_TROLL        2
#define SPEED_OGGRE             1
#define SPEEDBOOST_OGGRE        4
#define SPEED_LAOSHANLUNG       1
#define SPEEDBOOST_LAOSHANLUNG  3

#define MOVING_TIME_MIN 3000
#define MOVING_TIME_MAX 10000

#define TIMER_MOVE              30 // 33 FPS
#define TIMER_MONSTERS_MOVE     30
#define TIMER_MONSTERS_ACTION   1000
#define TIMER_COLLISION         500
#define TIME_TO_SHOW_ITEM       1400

#define DISTANCE_AGGRO  700
#define DISTANCE_SOUND  700

#define MOVE_RANDOMLY       0
#define MOVE_STOP           1
#define MOVE_AGGRO_PLAYER   2

#define MAP_WIDTH   15000
#define MAP_HEIGHT  15000

#define HERO_DEFENSE_MAX    1500
#define HERO_RAW_DAMAGE     5

#define NUM_WOLFPACK        16
#define NUM_GOBLIN          30
#define NUM_SPIDER          20
#define NUM_BEAR            20
#define NUM_TROLL           20
#define NUM_OGGRE           8

#define DEFAULT_SKIN_NUM    1
#define GOBLIN_SKIN_NUM     3
#define SPIDER_SKIN_NUM     3
#define BEAR_SKIN_NUM       3
#define TROLL_SKIN_NUM      3
#define OGGRE_SKIN_NUM      2

#define NUM_GROUND          220
#define NUM_FLOWERS         20
#define NUM_TREES           250
#define NUM_TREES_FALLEN    80
#define NUM_BUSHES          800
#define NUM_ROCKS           200
#define NUM_PLANK           50
#define NUM_STONE           80
#define NUM_LAKES           5

#define NUM_BUSHES_COIN_EVENT       40
#define NUM_BUSHES_EQUIPMENT_EVENT  20
#define NUM_CHEST_BURRIED_EVENT     8
#define NUM_STONE_ORE               15
#define NUM_IRON_ORE                10
#define NUM_SAPHIR_ORE              4
#define NUM_EMERALD_ORE             3
#define NUM_RUBIS_ORE               2

#define DIRECTION_LEFT  0
#define DIRECTION_RIGHT 1

// Number of different ressources per picture/MapItem
#define RES_TREE        10
#define RES_TREE_FALLEN 5
#define RES_BUSH        6
#define RES_ROCK        5
#define RES_FLOWERS     15
#define RES_GROUND      31
#define RES_PLANK       6
#define RES_STONE       9
#define RES_LAKE        1
#define RES_ORE_SPOT    4

#define Z_GROUND                1
#define Z_GROUND_FOREGROUND     2
#define Z_CHEST_BURRIED         3
#define Z_ORESPOT               3
#define Z_PLANK                 4
#define Z_STONE                 5
#define Z_ROCK                  6
#define Z_BUSH                  7
#define Z_TREE_FALLEN           8
#define Z_TREE                  14
#define Z_ITEM                  Z_GROUND_FOREGROUND
#define Z_ITEM_FOREGROUND       Z_HERO-4
#define Z_LAKE                  8
#define Z_FISH_EVENT            Z_LAKE+1

#define Z_VILLAGE               Z_HERO-4
#define Z_MONSTER_BACKGROUND    10
#define Z_MONSTERS              11
#define Z_MONSTER_FOREGROUND    12
#define Z_HERO                  15

// Sounds
#define SOUND_BUSH_SHAKED                   0
#define SOUND_BUSH_SHAKED_EVENT_EQUIPMENT   1
#define SOUND_BUSH_SHAKED_EVENT_COIN        2
#define SOUND_START_FIGHT                   3
#define SOUND_CLIC_MONSTER_STAT             4
#define SOUND_WOLF                          5
#define SOUND_WOLF_AGGRO                    6
#define SOUND_WOLF_HEAVYATTACK              7
#define SOUND_WOLF_LIGHTATTACK              8
#define SOUND_WOLF_ROAR                     9
#define SOUND_WOLF_DIE                      10
#define SOUND_BEAR                          11
#define SOUND_BEAR_AGGRO                    12
#define SOUND_BEAR_HEAVYATTACK              13
#define SOUND_BEAR_LIGHTATTACK              14
#define SOUND_BEAR_ROAR                     15
#define SOUND_BEAR_DIE                      16
#define SOUND_GOBLIN                        17
#define SOUND_GOBLIN_AGGRO                  18
#define SOUND_GOBLIN_HEAVYATTACK            19
#define SOUND_GOBLIN_LIGHTATTACK            20
#define SOUND_GOBLIN_ROAR                   21
#define SOUND_GOBLIN_DIE                    22
#define SOUND_EQUIP                         23
#define SOUND_MOVE_ROCK                     24
#define SOUND_MOVE_PLANK                    25
#define SOUND_HERO_ATTACK                   26
#define SOUND_BUY                           27
#define SOUND_FORGE                         28
#define SOUND_OPENCHEST                     29
#define SOUND_CLOSECHEST                    30
#define SOUND_TROLL                         31
#define SOUND_TROLL_AGGRO                   32
#define SOUND_TROLL_HEAVYATTACK             33
#define SOUND_TROLL_LIGHTATTACK             34
#define SOUND_TROLL_ROAR                    35
#define SOUND_TROLL_DIE                     36
#define SOUND_OGGRE                         37
#define SOUND_OGGRE_AGGRO                   38
#define SOUND_OGGRE_HEAVYATTACK             39
#define SOUND_OGGRE_LIGHTATTACK             40
#define SOUND_OGGRE_ROAR                    41
#define SOUND_OGGRE_DIE                     42
#define SOUND_PICK_OBJECT                   43
#define SOUND_DRINK                         44
#define SOUND_LAOSHANLUNG                   45
#define SOUND_LAOSHANLUNG_AGGRO             46
#define SOUND_LAOSHANLUNG_HEAVYATTACK       47
#define SOUND_LAOSHANLUNG_LIGHTATTACK       48
#define SOUND_LAOSHANLUNG_ROAR              49
#define SOUND_LAOSHANLUNG_DIE               50
#define SOUND_SPIDER                        51
#define SOUND_SPIDER_AGGRO                  52
#define SOUND_SPIDER_HEAVYATTACK            53
#define SOUND_SPIDER_LIGHTATTACK            54
#define SOUND_SPIDER_ROAR                   55
#define SOUND_SPIDER_DIE                    56
#define SOUND_TREE_FALL                     57
#define SOUND_ROCK_CRUSH                    58
#define SOUND_SPELL_0                       59
#define SOUND_SPELL_1                       60
#define SOUND_SPELL_2                       61
#define SOUND_SPELL_3                       62
#define SOUND_SPELL_4                       63
#define SOUND_SPELL_5                       64
#define SOUND_SPELL_6                       65
#define SOUND_SPELL_7                       66
#define SOUND_TELEPORT                      67

#define MUSICEVENT_START_DAY    0
#define MUSICEVENT_CLOSE_FIGHT  1
#define MUSICEVENT_ENTER_GOBLIN_VILLAGE 2

// Sound type emit by monsters
#define HEAVY_ATTACK    0
#define LIGHT_ATTACK    1
#define ROAR            2
#define DIE             3
#define SOUND           4
#define AGGRO           5

#define HERO_STAND      0
#define HERO_BOT        1
#define HERO_LEFT       3
#define HERO_RIGHT      3
#define HERO_TOP        4

#define BORDERS_COLOR   "#1c1c1c"

// Items identification

#define IDENT_ITEM_OFFSET_COINS                 0x00000000  // 20 max

#define IDENT_ITEM_OFFSET_TOOLS                 0x00000014  // 30 max

#define IDENT_ITEM_OFFSET_EQUIPMENTS            0x00000032  // 20.000 max
    #define IDENT_ITEM_OFFSET_ARMORS            IDENT_ITEM_OFFSET_EQUIPMENTS
    #define IDENT_ITEM_OFFSET_WEAPONS           (IDENT_ITEM_OFFSET_ARMORS+10000)

#define IDENT_ITEM_OFFSET_CONSUMABLES           0x00004E52  // 5.000 max
    #define IDENT_ITEM_OFFSET_LIFEPOTION        IDENT_ITEM_OFFSET_CONSUMABLES
    #define IDENT_ITEM_OFFSET_MANAPOTION        (IDENT_ITEM_OFFSET_LIFEPOTION+10)
    #define IDENT_ITEM_OFFSET_STAMINAPOTION     (IDENT_ITEM_OFFSET_MANAPOTION+10)
    #define IDENT_ITEM_OFFSET_STRENGTHPOTION    (IDENT_ITEM_OFFSET_STAMINAPOTION+10)
    #define IDENT_ITEM_OFFSET_KNOWLEDGEPOTION   (IDENT_ITEM_OFFSET_STRENGTHPOTION+10)
    #define IDENT_ITEM_OFFSET_FISHES            (IDENT_ITEM_OFFSET_KNOWLEDGEPOTION+10)

#define IDENT_ITEM_OFFSET_MATERIALS             0x000061DA
    #define IDENT_ITEM_OFFSET_ORE               IDENT_ITEM_OFFSET_MATERIALS // 500 max
    #define IDENT_ITEM_OFFSET_MONSTERMATERIALS  (IDENT_ITEM_OFFSET_ORE+500) // 2.000 max

#define IDENT_ITEM_OFFSET_SCROLL                0x00006B9E

#define IDENT_ITEM_NOT_ATTRIBUATE               0xFFFFFFFF

// Identifier list :
#define BAGCOIN                         IDENT_ITEM_OFFSET_COINS
#define TOOL_PICKAXE                    IDENT_ITEM_OFFSET_TOOLS
#define TOOL_SHOVEL                     (TOOL_PICKAXE+1)
#define TOOL_FISHINGROD                 (TOOL_SHOVEL+1)
#define TOOL_COMPASS                    (TOOL_FISHINGROD+1)
#define TOOL_KNIFE                      (TOOL_COMPASS+1)
#define TOOL_MAPSCROLL                  (TOOL_KNIFE+1)

#define ARMOR_HELMET                    IDENT_ITEM_OFFSET_ARMORS
#define ARMOR_BREASTPLATE               (ARMOR_HELMET+1)
#define ARMOR_GLOVES                    (ARMOR_BREASTPLATE+1)
#define ARMOR_PANT                      (ARMOR_GLOVES+1)
#define ARMOR_FOOTWEARS                 (ARMOR_PANT+1)
#define ARMOR_AMULET                    (ARMOR_FOOTWEARS+1)

#define WEAPON_SWORD                    IDENT_ITEM_OFFSET_WEAPONS
#define WEAPON_BOW                      (WEAPON_SWORD+1)
#define WEAPON_STAFF                    (WEAPON_BOW+1)

#define CONSUMABLE_LIFEPOTION           IDENT_ITEM_OFFSET_LIFEPOTION
#define CONSUMABLE_MANAPOTION           IDENT_ITEM_OFFSET_MANAPOTION
#define CONSUMABLE_STAMINAPOTION        IDENT_ITEM_OFFSET_STAMINAPOTION
#define CONSUMABLE_STRENGTHPOTION       IDENT_ITEM_OFFSET_STRENGTHPOTION
#define CONSUMABLE_KNOWLEDGEPOTION      IDENT_ITEM_OFFSET_KNOWLEDGEPOTION
#define CONSUMABLE_REDFISH              IDENT_ITEM_OFFSET_FISHES
#define CONSUMABLE_BLUEFISH             (CONSUMABLE_REDFISH+1)
#define CONSUMABLE_YELLOWFISH           (CONSUMABLE_BLUEFISH+1)
#define CONSUMABLE_COMMUNFISH           (CONSUMABLE_YELLOWFISH+1)

#define ORE_STONE                       IDENT_ITEM_OFFSET_ORE
#define ORE_IRON                        (ORE_STONE+1)
#define ORE_SAPHIR                      (ORE_IRON+1)
#define ORE_EMERALD                     (ORE_SAPHIR+1)
#define ORE_RUBIS                       (ORE_EMERALD+1)
#define EARTH_CRISTAL                   (ORE_RUBIS+1)

#define MONSTERMATERIAL_WOLF_PELT       IDENT_ITEM_OFFSET_MONSTERMATERIALS
#define MONSTERMATERIAL_WOLF_FANG       (MONSTERMATERIAL_WOLF_PELT+1)
#define MONSTERMATERIAL_WOLF_MEAT       (MONSTERMATERIAL_WOLF_FANG+1)
#define MONSTERMATERIAL_MANDIBLES       (MONSTERMATERIAL_WOLF_MEAT+1)
#define MONSTERMATERIAL_POISON_POUCH    (MONSTERMATERIAL_MANDIBLES+1)
#define MONSTERMATERIAL_GOBLIN_EAR      (MONSTERMATERIAL_POISON_POUCH+1)
#define MONSTERMATERIAL_GOBLIN_BONES    (MONSTERMATERIAL_GOBLIN_EAR+1)
#define MONSTERMATERIAL_BEAR_PELT       (MONSTERMATERIAL_GOBLIN_BONES+1)
#define MONSTERMATERIAL_BEAR_MEAT       (MONSTERMATERIAL_BEAR_PELT+1)
#define MONSTERMATERIAL_BEAR_TALON      (MONSTERMATERIAL_BEAR_MEAT+1)
#define MONSTERMATERIAL_TROLL_SKULL     (MONSTERMATERIAL_BEAR_TALON+1)
#define MONSTERMATERIAL_TROLL_MEAT      (MONSTERMATERIAL_TROLL_SKULL+1)
#define MONSTERMATERIAL_OGGRE_SKULL     (MONSTERMATERIAL_TROLL_MEAT+1)
#define MONSTERMATERIAL_WOLFALPHA_PELT  (MONSTERMATERIAL_OGGRE_SKULL+1)
#define MONSTERMATERIAL_LAOSHANLUNG_HEART (MONSTERMATERIAL_WOLFALPHA_PELT+1)

#define SCROLL_X                        (IDENT_ITEM_OFFSET_SCROLL)

#define ABLE(hero)                          (1<<hero)
#define IS_ABLE(heroClass, classesAble)     ((classesAble>>heroClass)&1)

enum HeroAbleToUSe{
    eSwordman,
    eArcher,
    eWizard,
    eNbHeroClasses
};

#endif // CONSTANTS_H
