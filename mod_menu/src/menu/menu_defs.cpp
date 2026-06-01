#include "menu_defs.h"
#include "../config.h"
#include "../features/resources.h"
#include "../features/world.h"
#include "../features/misc.h"
#include "../features/lua_console.h"
#include "../features/game_lua.h"
#include "../features/wavemode.h"

// ── Enum name tables ───────────────────────────────────────────────────────

static const char* s_OnOff[]       = { "OFF", "ON" };
static const char* s_PlayerTypes[] = { "SNAKE", "DD_MALE", "DD_FEMALE", "AVATAR", "LIQUID", "OCELOT", "QUIET" };
static const char* s_HandTypes[]   = { "NONE", "NORMAL", "STUN_ARM", "JEHUTY", "STUN_ROCKET", "KILL_ROCKET", "GOLD", "SILVER" };
static const char* s_Weather[]     = { "NONE", "SUNNY", "CLOUDY", "RAINY", "SANDSTORM", "FOGGY", "POURING" };
static const char* s_FogTypes[]    = { "NORMAL", "PARASITE", "EERIE" };
static const char* s_Phases[]      = { "DEFAULT", "SNEAK", "CAUTION", "EVASION", "ALERT" };
static const char* s_Revenge[]     = { "DEFAULT", "FREE", "MISSION", "ALL" };
static const char* s_SearchLight[] = { "DEFAULT", "OFF", "ON" };
static const char* s_EquipRange[]  = { "SHORT", "MEDIUM", "LONG", "RANDOM" };
static const char* s_StartFoot[]   = { "OFF", "FREE", "MISSION", "ALL" };
static const char* s_AutoFulton[]  = { "OFF", "FREE", "MISSION", "ALL" };
static const char* s_DisableLZ[]   = { "OFF", "ASSAULT", "REGULAR" };
static const char* s_QuestSelect[] = { "FIRST_FOUND", "RANDOM", "ADDON" };
static const char* s_QuestRepop[]  = { "NONE_LEFT", "ALWAYS" };

static const char* s_QuietWpn[]    = { "WICKED_BUTTERFLY", "GUILTY_BUTTERFLY", "SINFUL_BUTTERFLY",
                                       "WICKED_BUTTERFLY_S", "GUILTY_BUTTERFLY_S", "SINFUL_BUTTERFLY_S" };
static const char* s_DDogEquip[]   = { "NAKED", "SNEAKING_STUN", "SNEAKING_KNIFE", "BATTLE_DRESS", "FULTON" };
static const char* s_DHorseEquip[] = { "NORMAL", "BATTLE_DRESS_2", "BATTLE_DRESS_3", "BATTLE_DRESS_4" };
static const char* s_DWalkerEquip[]= { "GATLING_GUN", "ATGM", "H_DISCHARGER", "FLAMETHROWER", "F_BALISTA" };

static const char* s_PlayerParts[] = {
    "NORMAL", "SNEAKING_SUIT", "BATTLE_DRESS", "PARASITE_SUIT", "NAKED",
    "MGS1", "RAIDEN", "NINJA", "TUXEDO", "LEATHER_JACKET",
    "EVA_JUMPSUIT", "BOSS_JUMPSUIT", "SWIMWEAR_M", "SWIMWEAR_F",
    "GROUND_ZEROES", "HOSPITAL", "MSF"
};

static const char* s_WarGameProf[] = { "DEFAULT", "DD_TRAINING", "INVASION_SOVIET", "ZOMBIE_DD", "ZOMBIE_OBLIT",
                                       "ROGUE_COYOTE", "XOF", "FEMME_FATALES" };

static const char* s_ProstheticCol[] = { "RED", "GOLD", "SILVER", "BLACK" };
static const char* s_AvatarGender[]  = { "MALE", "FEMALE" };
static const char* s_ShrapnelType[]  = { "DEFAULT", "SHORT", "MEDIUM", "LONG" };
static const char* s_PhotoFilter[]   = { "NONE", "SEPIA", "BW", "NEGATIVE", "POSTERIZE", "VIVID" };
static const char* s_WaterLevel[]    = { "DEFAULT", "LOW", "HIGH", "FLOOD" };
static const char* s_Destruction[]   = { "DEFAULT", "LOW", "HIGH", "TOTAL" };
static const char* s_SkullsType[]    = { "MIST", "CAMO", "ARMOR", "METALLIC" };
static const char* s_SkullsPhase[]   = { "AUTO", "MELEE", "RANGED", "SNIPE" };
static const char* s_VehicleHandle[] = { "DEFAULT", "ARCADE", "REALISTIC", "DRIFT" };
static const char* s_VehicleSpawn[]  = { "NONE", "TRUCK", "JEEP", "APC", "TANK", "WALKER" };
static const char* s_VPatrolType[]   = { "TRUCK", "APC", "TANK", "RANDOM" };
static const char* s_HeliWeapon[]    = { "DEFAULT", "MINIGUN", "ROCKET", "GATLING", "NONE" };
static const char* s_QuietBehav[]    = { "DEFAULT", "SCOUT", "ATTACK", "COVER" };
static const char* s_DDogBehav[]     = { "DEFAULT", "GUARD", "SCOUT", "ATTACK" };
static const char* s_DDogTricks[]    = { "OFF", "SHAKE", "ROLL", "SIT", "ALL" };
static const char* s_DHorseBehav[]   = { "DEFAULT", "FOLLOW", "STAY", "WANDER" };
static const char* s_DWalkerBehav[]  = { "DEFAULT", "FOLLOW", "PATROL", "GUARD" };
static const char* s_MBSecurity[]    = { "DEFAULT", "LOW", "MEDIUM", "HIGH", "MAX" };
static const char* s_MBNpcRoute[]    = { "DEFAULT", "PATROL", "IDLE", "EXERCISE" };
static const char* s_MBLighting[]    = { "DEFAULT", "BRIGHT", "DIM", "OFF" };
static const char* s_MBAnimal[]      = { "GOAT", "SHEEP", "WOLF", "BEAR", "DONKEY" };
static const char* s_MissionRank[]   = { "DEFAULT", "S", "A", "B", "C", "D", "E" };
static const char* s_DemoSubtitles[] = { "DEFAULT", "ON", "OFF" };
static const char* s_NVColor[]       = { "GREEN", "WHITE", "AMBER", "BLUE" };
static const char* s_CrosshairStyle[]= { "CROSS", "DOT", "CIRCLE", "T_SHAPE", "CHEVRON" };
static const char* s_ThermalPal[]    = { "IRONBOW", "RAINBOW", "WHITE_HOT", "BLACK_HOT" };
static const char* s_ColorGrade[]    = { "NONE", "WARM", "COOL", "VINTAGE", "CINEMATIC", "BLEACH" };
static const char* s_EspSnap[]       = { "BOTTOM", "CENTER", "TOP" };
static const char* s_LogLevel[]      = { "ERROR", "WARN", "INFO", "DEBUG", "VERBOSE" };
static const char* s_AlertBGM[]      = { "DEFAULT", "CUSTOM1", "CUSTOM2", "SILENT" };
static const char* s_StaffRank[]     = { "DEFAULT", "S++", "S+", "S", "A++", "A+", "A" };
static const char* s_DDUniform[]     = { "STANDARD", "SNEAKING", "BATTLE", "PARASITE" };
static const char* s_WaveArena[]     = { "AFGHAN_OUTPOST", "AFRICA_VILLAGE", "MOTHER_BASE_DECK",
                                         "RUINS_COMPOUND", "AIRPORT_FACILITY", "CURRENT_LOCATION" };
static const char* s_WaveDiff[]      = { "RECRUIT", "NORMAL", "HARD", "INSANE" };

// ── Category builders ──────────────────────────────────────────────────────

static MenuNode BuildPlayerMenu(Config& c) {
    return MakeSub("Player", "Player combat, movement, health, and hero settings", {
        MakeSub("Combat", "Offensive and defensive combat cheats", {
            MakeSub("Weapon Cheats", "Core weapon and ammo exploits", {
                MakeToggle("godMode",             "Become invincible - take no damage from any source",                    &c.godMode),
                MakeToggle("infiniteAmmo",        "Never run out of ammunition in any weapon",                             &c.infiniteAmmo),
                MakeToggle("noReload",            "Skip magazine changes entirely - fire continuously",                    &c.noReload),
                MakeToggle("infiniteSuppressor",  "Suppressors never wear out or degrade",                                 &c.infiniteSuppressor),
                MakeToggle("infiniteMags",        "Magazines never deplete - all weapons have infinite mags",              &c.infiniteMags),
                MakeToggle("infiniteThrowables",  "Grenades, C4, decoys, and other throwables never run out",              &c.infiniteThrowables),
                MakeToggle("infiniteBattery",     "NVG, sonar, and other battery devices never drain",                     &c.infiniteBattery),
            }),
            MakeSep(),
            MakeSub("Accuracy & Recoil", "Weapon handling and precision", {
                MakeToggle("noRecoil",            "Perfectly stable aim - no weapon recoil or camera kick",                &c.noRecoil),
                MakeToggle("noWeaponSway",        "Eliminate idle weapon sway when aiming",                                &c.noWeaponSway),
                MakeToggle("perfectAccuracy",     "All shots land exactly on crosshair - zero spread",                     &c.perfectAccuracy),
                MakeToggle("noBulletDrop",        "Bullets travel in a straight line ignoring gravity",                    &c.noBulletDrop),
            }),
            MakeSep(),
            MakeSub("Damage & Effects", "Damage output and special bullet effects", {
                MakeToggle("oneHitKill",          "All enemies die in a single hit from any weapon",                       &c.oneHitKill),
                MakeInt   ("damageMultiplier",    "Player damage output multiplier (100 = normal)",                         &c.damageMultiplier, 10, 1000, 10, "%"),
                MakeToggle("explosiveBullets",    "All bullets cause small explosions on impact",                           &c.explosiveBullets),
                MakeToggle("penetratingShots",    "Bullets penetrate through walls and cover",                              &c.penetratingShots),
                MakeToggle("silentWeapons",       "All weapons are silent - no detection from gunfire",                     &c.silentWeapons),
            }),
            MakeSep(),
            MakeToggle("rapidFire",           "Remove fire rate limiter on all weapons",                               &c.rapidFire),
            MakeToggle("infiniteReflex",      "Reflex mode lasts forever when triggered",                              &c.infiniteReflex),
            MakeInt   ("reflexDuration",      "Reflex mode duration multiplier (100 = normal)",                         &c.reflexDuration, 10, 1000, 10, "%"),
        }),
        MakeSub("Movement", "Player movement and physics", {
            MakeToggle("superSpeed",          "Move faster than normal",                                               &c.superSpeed),
            MakeFloat ("speedMultiplier",     "Speed multiplier when super speed is enabled (1.5-10x)",                &c.speedMultiplier, 1.5f, 10.0f, 0.5f, "x"),
            MakeToggle("noFallDamage",        "Take no damage from falling, no matter the height",                     &c.noFallDamage),
            MakeSep(),
            MakeSub("Fine Tuning", "Detailed movement speed controls", {
                MakeFloat ("sprintSpeed",     "Sprint speed multiplier (1.0 = normal)",                                &c.sprintSpeed, 0.5f, 5.0f, 0.1f, "x"),
                MakeFloat ("crawlSpeed",      "Prone crawl speed multiplier",                                          &c.crawlSpeed, 0.5f, 5.0f, 0.1f, "x"),
                MakeFloat ("diveDistance",    "Dive/roll distance multiplier",                                          &c.diveDistance, 0.5f, 5.0f, 0.1f, "x"),
                MakeFloat ("swimSpeed",       "Swimming speed multiplier",                                              &c.swimSpeed, 0.5f, 5.0f, 0.1f, "x"),
            }),
            MakeSep(),
            MakeSub("Physics", "Gravity and physical properties", {
                MakeFloat ("gravityScale",    "Gravity multiplier (0 = zero-G, 3 = heavy)",                            &c.gravityScale, 0.0f, 3.0f, 0.1f, "x"),
                MakeToggle("customPlayerScale","Apply custom scale to the player model",                               &c.customPlayerScale),
                MakeFloat ("playerScale",     "Player model scale factor (0.1 = tiny, 5.0 = giant)",                   &c.playerScale, 0.1f, 5.0f, 0.1f),
            }),
        }),
        MakeSub("Stealth", "Detection and alert settings", {
            MakeToggle("stealthMode",         "Detection meter is frozen at zero - guards never see you",              &c.stealthMode),
            MakeInt   ("noiseScale",          "Player noise output scale (0 = silent, 300 = very loud)",               &c.noiseScale, 0, 300, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Stats & Health", "Health, stamina, and endurance settings", {
            MakeInt   ("playerHealthScale",   "Player health multiplier as a percentage (100 = normal, 650 = max)",    &c.playerHealthScale, 0, 650, 10, "%"),
            MakeInt   ("staminaScale",        "Player stamina scale (100 = normal, 500 = max)",                        &c.staminaScale, 0, 500, 10, "%"),
            MakeToggle("unlimitedStamina",    "Stamina never depletes - unlimited sprinting and actions",              &c.unlimitedStamina),
            MakeToggle("noBreathLimit",       "Unlimited breath underwater and in gas",                                &c.noBreathLimit),
            MakeInt   ("gripStrength",        "Grip/hang strength scale (100 = normal)",                               &c.gripStrength, 10, 500, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Hero & Demon", "Hero point and demon point system controls", {
            MakeToggle("dontSubtractHero",    "Prevent hero points from being deducted for any reason",                &c.dontSubtractHero),
            MakeToggle("dontAddOgre",         "Prevent demon (ogre) points from being added",                          &c.dontAddOgre),
            MakeToggle("heroSubtractsOgre",   "Gaining hero points also subtracts ogre points",                        &c.heroSubtractsOgre),
        }),
    });
}

static MenuNode BuildAppearanceMenu(Config& c) {
    return MakeSub("Appearance", "Player model, suit, camo, and equipment selection", {
        MakeSub("Character Model", "Select the active character model", {
            MakeEnum  ("playerType",    "Select which character model to use",                                         &c.playerType, s_PlayerTypes, 7),
            MakeEnum  ("avatarGender",  "Avatar gender selection (when using Avatar model)",                           &c.avatarGender, s_AvatarGender, 2),
        }),
        MakeSep(),
        MakeSub("Outfit & Suit", "Suit, uniform, and outfit selection", {
            MakeEnum  ("playerParts",   "Select suit or outfit (Sneaking Suit, Battle Dress, etc.)",                   &c.playerParts, s_PlayerParts, 17),
            MakeEnum  ("ddFemaleUniform","DD Female soldier uniform variant",                                          &c.ddFemaleUniform, s_DDUniform, 4),
            MakeEnum  ("ddMaleUniform", "DD Male soldier uniform variant",                                             &c.ddMaleUniform, s_DDUniform, 4),
        }),
        MakeSep(),
        MakeSub("Camo & Paint", "Camouflage pattern and face paint", {
            MakeInt   ("playerCamo",    "Camo pattern index (0-120, game-dependent)",                                  &c.playerCamo, 0, 120, 1),
            MakeInt   ("facePaintIndex","Face paint pattern (0 = none, 1-20 = patterns)",                              &c.facePaintIndex, 0, 20, 1),
        }),
        MakeSep(),
        MakeSub("Head & Face", "Headgear, face equipment, and body variation", {
            MakeInt   ("headgear",      "Face equipment / headgear ID",                                                &c.headgear, 0, 100, 1),
            MakeInt   ("fovaFace",      "Face variation index (FOVA system)",                                          &c.fovaFace, 0, 50, 1),
            MakeInt   ("fovaBody",      "Body variation index (FOVA system)",                                          &c.fovaBody, 0, 50, 1),
            MakeInt   ("fovaHair",      "Hair style variation index",                                                  &c.fovaHair, 0, 30, 1),
            MakeToggle("hornHidden",    "Hide the shrapnel horn on Snake",                                             &c.hornHidden),
            MakeEnum  ("shrapnelType",  "Horn/shrapnel size override",                                                 &c.shrapnelType, s_ShrapnelType, 4),
        }),
        MakeSep(),
        MakeSub("Prosthetic Arm", "Bionic arm type and appearance", {
            MakeEnum  ("handType",      "Bionic arm type selection",                                                   &c.handType, s_HandTypes, 8),
            MakeEnum  ("prostheticColor","Prosthetic arm color variant",                                               &c.prostheticColor, s_ProstheticCol, 4),
        }),
        MakeSep(),
        MakeSub("Accessories", "Scarf, accessories, cigar, and blood", {
            MakeInt   ("scarf",         "Scarf index (0 = none, 1-6 = variants)",                                      &c.scarf, 0, 6, 1),
            MakeInt   ("accessory",     "Accessory index (0 = none, 1-10 = accessories)",                              &c.accessory, 0, 10, 1),
            MakeToggle("cigarEquipped", "Toggle the phantom cigar as equipped accessory",                              &c.cigarEquipped),
            MakeToggle("bloodStains",   "Toggle permanent blood stains on character model",                            &c.bloodStains),
        }),
    });
}

static MenuNode BuildCameraMenu(Config& c) {
    return MakeSub("Camera", "Camera control, FOV, and free camera settings", {
        MakeSub("Free Camera", "Detach camera from player and fly freely", {
            MakeToggle("freeCamera",       "Enable free camera mode (WASD + Space/Ctrl to move)",                      &c.freeCamera),
            MakeFloat ("camSpeed",         "Free camera movement speed",                                               &c.camSpeed, 1.0f, 50.0f, 1.0f),
            MakeToggle("firstPersonMode",  "Lock camera to first-person perspective",                                  &c.firstPersonMode),
        }),
        MakeSep(),
        MakeSub("FOV Override", "Field of view control per camera mode", {
            MakeToggle("customFOV",        "Enable custom field of view override",                                     &c.customFOV),
            MakeFloat ("fovValue",         "Global FOV in degrees (default 75)",                                       &c.fovValue, 30.0f, 170.0f, 1.0f, " deg"),
            MakeSep(),
            MakeSub("Per-Mode FOV", "FOV for specific camera states", {
                MakeFloat ("fovNormal",    "FOV for normal third-person view",                                         &c.fovNormal, 30.0f, 170.0f, 1.0f, " deg"),
                MakeFloat ("fovAiming",    "FOV when aiming/shouldering",                                              &c.fovAiming, 20.0f, 120.0f, 1.0f, " deg"),
                MakeFloat ("fovHiding",    "FOV when hiding behind cover",                                             &c.fovHiding, 30.0f, 120.0f, 1.0f, " deg"),
                MakeFloat ("fovCQC",       "FOV during CQC animations",                                                &c.fovCQC, 30.0f, 120.0f, 1.0f, " deg"),
            }),
        }),
        MakeSep(),
        MakeSub("Lens & Focus", "Focal length, aperture, depth of field", {
            MakeFloat ("focalLength",          "Camera focal length (default 21.0, lower = wider angle)",              &c.focalLength, 0.1f, 200.0f, 0.5f),
            MakeFloat ("aperture",             "Camera aperture for depth of field (default 1.2)",                     &c.aperture, 0.001f, 100.0f, 0.1f),
            MakeFloat ("focusDistance",         "Camera focus distance in meters",                                      &c.focusDistance, 0.01f, 1000.0f, 1.0f),
        }),
        MakeSep(),
        MakeSub("Position Offsets", "Shoulder and camera distance adjustments", {
            MakeFloat ("shoulderOffsetX",  "Horizontal shoulder offset (-2 = left, 2 = right)",                        &c.shoulderOffsetX, -2.0f, 2.0f, 0.1f),
            MakeFloat ("shoulderOffsetY",  "Vertical shoulder offset (-2 = down, 2 = up)",                             &c.shoulderOffsetY, -2.0f, 2.0f, 0.1f),
            MakeFloat ("cameraDistance",   "Camera distance offset (-5 = close, 5 = far)",                             &c.cameraDistance, -5.0f, 5.0f, 0.1f),
            MakeFloat ("cameraHeight",     "Camera height offset (-3 = low, 3 = high)",                                &c.cameraHeight, -3.0f, 3.0f, 0.1f),
            MakeFloat ("tiltAngle",        "Camera tilt / dutch angle in degrees",                                     &c.tiltAngle, -45.0f, 45.0f, 1.0f, " deg"),
        }),
        MakeSep(),
        MakeSub("Zoom & Scope", "Scope and binocular zoom overrides", {
            MakeFloat ("scopeZoomMult",    "Scope zoom multiplier (1.0 = normal)",                                     &c.scopeZoomMult, 0.5f, 4.0f, 0.1f, "x"),
            MakeFloat ("binocZoomMult",    "Binocular zoom multiplier",                                                &c.binocZoomMult, 0.5f, 4.0f, 0.1f, "x"),
        }),
        MakeSep(),
        MakeSub("Effects", "Camera shake, letterbox, and effects", {
            MakeInt   ("cameraShake",      "Camera shake intensity (0 = none, 200 = extreme)",                         &c.cameraShake, 0, 200, 10, "%"),
            MakeToggle("disableAutoCenter","Disable automatic camera re-centering",                                    &c.disableAutoCenter),
            MakeFloat ("letterboxAmount",  "Letterbox bar size (0.0 = none, 0.3 = cinematic)",                         &c.letterboxAmount, 0.0f, 0.3f, 0.01f),
        }),
        MakeSep(),
        MakeSub("Photo Mode", "Photo mode and post-processing filters", {
            MakeToggle("photoMode",        "Enable photo mode (freezes game, free camera)",                            &c.photoMode),
            MakeEnum  ("photoFilter",      "Photo mode color filter",                                                  &c.photoFilter, s_PhotoFilter, 6),
            MakeFloat ("photoFilterStr",   "Filter intensity (0.0 = off, 1.0 = full)",                                 &c.photoFilterStr, 0.0f, 1.0f, 0.05f),
        }),
    });
}

static MenuNode BuildResourcesMenu(Config& c) {
    return MakeSub("Resources", "GMP, heroism, demon points, and resource scaling", {
        MakeSub("GMP", "Gross Military Product (currency)", {
            MakeInt   ("gmp",                  "Current GMP value (set and press Enter to apply)",                     &c.gmp, 0, 99999999, 100000),
            MakeToggle("lockGMP",              "Freeze GMP at current value every frame",                              &c.lockGMP),
            MakeCmd   ("Set GMP to 5M",        "Instantly set GMP to 5,000,000",                                       []() { auto& c = Config::Get(); c.gmp = 5000000; Features::Resources::SetGMP(5000000); }),
            MakeCmd   ("Set GMP to 50M",       "Instantly set GMP to 50,000,000",                                      []() { auto& c = Config::Get(); c.gmp = 50000000; Features::Resources::SetGMP(50000000); }),
        }),
        MakeSep(),
        MakeSub("Heroism", "Heroism point management", {
            MakeInt   ("heroism",              "Current heroism value",                                                 &c.heroism, 0, 9999999, 10000),
            MakeToggle("lockHeroism",          "Freeze heroism at current value every frame",                           &c.lockHeroism),
            MakeCmd   ("Set Heroism to 1M",    "Instantly set heroism to 1,000,000",                                    []() { auto& c = Config::Get(); c.heroism = 1000000; Features::Resources::SetHeroism(1000000); }),
        }),
        MakeSep(),
        MakeSub("Demon Points", "Demon / Ogre point management", {
            MakeInt   ("demonPoints",          "Current demon points (affects horn growth)",                            &c.demonPoints, 0, 9999999, 10000),
            MakeToggle("lockDemonPoints",      "Freeze demon points at current value",                                  &c.lockDemonPoints),
            MakeCmd   ("Zero Demon Points",    "Reset demon points to zero",                                            []() { auto& c = Config::Get(); c.demonPoints = 0; Features::Resources::SetDemonPoints(0); }),
        }),
        MakeSep(),
        MakeSub("Resource Scale", "Multiply resource collection rates per type", {
            MakeInt("resourceScaleMat",        "Material collection multiplier (%)",                                    &c.resourceScaleMat, 10, 1000, 10, "%"),
            MakeInt("resourceScalePlant",      "Plant collection multiplier (%)",                                       &c.resourceScalePlant, 10, 1000, 10, "%"),
            MakeInt("resourceScaleDia",        "Diamond collection multiplier (%)",                                     &c.resourceScaleDia, 10, 1000, 10, "%"),
            MakeInt("resourceScaleContainer",  "Container collection multiplier (%)",                                   &c.resourceScaleContainer, 10, 1000, 10, "%"),
            MakeSep(),
            MakeInt("resourceScaleFuel",       "Fuel resource collection multiplier",                                   &c.resourceScaleFuel, 10, 1000, 10, "%"),
            MakeInt("resourceScaleBio",        "Biological material collection multiplier",                             &c.resourceScaleBio, 10, 1000, 10, "%"),
            MakeInt("resourceScaleMetal",      "Precious metal collection multiplier",                                  &c.resourceScaleMetal, 10, 1000, 10, "%"),
            MakeInt("resourceScaleMinor",      "Minor material collection multiplier",                                  &c.resourceScaleMinor, 10, 1000, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Staff & Development", "Staff recruitment and base development", {
            MakeInt   ("mbCoinMultiplier",     "MB Coin earning multiplier (%)",                                        &c.mbCoinMultiplier, 10, 1000, 10, "%"),
            MakeInt   ("staffRecruitLevel",    "Force minimum recruit quality level (0 = default)",                     &c.staffRecruitLevel, 0, 100, 5),
            MakeToggle("autoDismissLow",       "Auto-dismiss soldiers below a quality threshold",                       &c.autoDismissLow),
            MakeInt   ("autoDismissThresh",    "Auto-dismiss threshold (soldiers below this level)",                    &c.autoDismissThresh, 1, 80, 5),
            MakeToggle("preventVolunteers",    "Prevent volunteer soldier arrivals",                                    &c.preventVolunteers),
            MakeInt   ("processingScale",      "Staff processing speed multiplier (%)",                                 &c.processingScale, 10, 500, 10, "%"),
            MakeToggle("infiniteBaseDev",      "Instant base development - no wait times",                              &c.infiniteBaseDev),
        }),
        MakeSep(),
        MakeCmd   ("Max All Plants",       "Fill all plant resource slots to 999",                                      Features::Resources::MaxAllPlants),
        MakeCmd   ("Max All Materials",    "Fill all material resource slots to 999999",                                 Features::Resources::MaxAllMaterials),
    });
}

static MenuNode BuildWorldMenu(Config& c) {
    return MakeSub("World", "Time, weather, environment, and world settings", {
        MakeSub("Time Scale", "Control game simulation speed", {
            MakeToggle("slowMotion",            "Enable slow motion effect",                                            &c.slowMotion),
            MakeFloat ("timeScale",             "Time scale multiplier when slow motion is active (lower = slower)",    &c.timeScale, 0.01f, 1.0f, 0.05f),
            MakeInt   ("clockTimeScale",        "In-game clock speed multiplier (1 = normal, 10000 = max)",             &c.clockTimeScale, 1, 10000, 10),
            MakeSep(),
            MakeSub("Speed Camera", "Speed camera time controls", {
                MakeInt   ("speedCamWorldScale",    "World time scale during speed camera (%)",                         &c.speedCamWorldScale, 0, 10000, 100, "%"),
                MakeInt   ("speedCamPlayerScale",   "Player time scale during speed camera (%)",                        &c.speedCamPlayerScale, 0, 10000, 100, "%"),
                MakeInt   ("speedCamContinueTime",  "Speed camera continue duration (seconds)",                         &c.speedCamContinueTime, 0, 1000, 5, "s"),
            }),
        }),
        MakeSep(),
        MakeSub("Time of Day", "Override the in-game clock", {
            MakeToggle("overrideTime",          "Lock the time of day to a specific hour",                              &c.overrideTime),
            MakeFloat ("timeOfDay",             "Hour of day (0.0 = midnight, 12.0 = noon, 18.0 = sunset)",            &c.timeOfDay, 0.0f, 24.0f, 0.5f, "h"),
            MakeSep(),
            MakeCmd   ("Dawn",                  "Set time to 6:00 AM",                                                  []() { Config::Get().timeOfDay = 6.0f; }),
            MakeCmd   ("Noon",                  "Set time to 12:00 PM",                                                 []() { Config::Get().timeOfDay = 12.0f; }),
            MakeCmd   ("Dusk",                  "Set time to 18:00 PM",                                                 []() { Config::Get().timeOfDay = 18.0f; }),
            MakeCmd   ("Midnight",              "Set time to 0:00 AM",                                                  []() { Config::Get().timeOfDay = 0.0f; }),
        }),
        MakeSep(),
        MakeSub("Weather", "Control weather and atmospheric effects", {
            MakeToggle("weatherOverride",       "Enable weather override (force a specific weather type)",              &c.weatherOverride),
            MakeEnum  ("weather",               "Weather type to force (NONE disables override)",                       &c.weather, s_Weather, 7),
            MakeFloat ("fogDensity",            "Fog density (0.0 = clear, 1.0 = maximum fog)",                        &c.fogDensity, 0.0f, 1.0f, 0.05f),
            MakeEnum  ("fogType",               "Fog style variant",                                                    &c.fogType, s_FogTypes, 3),
            MakeFloat ("dustDensity",           "Dust particle density (0 = clear, 1 = thick)",                         &c.dustDensity, 0.0f, 1.0f, 0.05f),
            MakeFloat ("rainWetness",           "Ground wetness after rain (0 = dry, 1 = soaked)",                      &c.rainWetness, 0.0f, 1.0f, 0.05f),
        }),
        MakeSep(),
        MakeSub("Lighting", "Ambient light, sun, and shadow controls", {
            MakeFloat ("ambientLight",          "Ambient light intensity (1.0 = default)",                              &c.ambientLight, 0.0f, 3.0f, 0.1f, "x"),
            MakeFloat ("sunIntensity",          "Sun/directional light intensity",                                      &c.sunIntensity, 0.0f, 5.0f, 0.1f, "x"),
            MakeFloat ("shadowIntensity",       "Shadow darkness (0 = no shadows, 2 = very dark)",                     &c.shadowIntensity, 0.0f, 2.0f, 0.1f),
            MakeFloat ("temperatureShift",      "Color temperature shift (-20 = cold, 20 = warm)",                     &c.temperatureShift, -20.0f, 20.0f, 1.0f),
            MakeFloat ("skyboxRotation",        "Skybox rotation angle in degrees",                                     &c.skyboxRotation, 0.0f, 360.0f, 5.0f, " deg"),
        }),
        MakeSep(),
        MakeSub("Wind", "Wind speed and direction", {
            MakeFloat ("windSpeed",             "Wind speed in m/s (0 = calm, 30 = storm)",                            &c.windSpeed, 0.0f, 30.0f, 1.0f, " m/s"),
            MakeInt   ("windDirection",         "Wind direction in degrees (0 = North, 90 = East)",                    &c.windDirection, 0, 360, 15, " deg"),
        }),
        MakeSep(),
        MakeSub("Environment", "World object and hazard controls", {
            MakeToggle("noEnemyAI",             "Freeze all enemy behavior trees - enemies stand still",               &c.noEnemyAI),
            MakeToggle("noAnimalAI",            "Freeze all animal behavior - animals stand still",                    &c.noAnimalAI),
            MakeToggle("infiniteFulton",        "Unlimited fulton extractions, never run out",                          &c.infiniteFulton),
            MakeSep(),
            MakeSub("Disable Hazards", "Remove specific world hazards", {
                MakeToggle("disableMinefield",  "Disable all active minefields",                                       &c.disableMinefield),
                MakeToggle("disableTraps",      "Disable all traps (claymores, trip wires)",                           &c.disableTraps),
                MakeToggle("disableMines",      "Remove mine placements from guard posts",                             &c.disableMines),
                MakeToggle("disableCameras",    "Disable all security cameras",                                        &c.disableCameras),
                MakeToggle("disableDecoys",     "Disable enemy decoy placements",                                      &c.disableDecoys),
                MakeToggle("disableRadio",      "Disable enemy radio communications",                                  &c.disableRadio),
            }),
            MakeSep(),
            MakeEnum  ("waterLevel",            "Override water level in applicable areas",                            &c.waterLevel, s_WaterLevel, 4),
            MakeEnum  ("destructionLevel",      "Environment destruction level",                                       &c.destructionLevel, s_Destruction, 4),
        }),
        MakeSep(),
        MakeCmd   ("Teleport to Waypoint",      "Instantly move player to the current map marker position",            Features::World::TeleportToWaypoint),
    });
}

static MenuNode BuildEnemyMenu(Config& c) {
    return MakeSub("Enemy", "Enemy soldier parameters, phases, reinforcements, and prep", {
        MakeSub("Soldier Params", "Scale enemy capabilities globally", {
            MakeSub("Detection", "How well enemies detect the player", {
                MakeInt   ("soldierSightScale",     "Enemy visual detection range (100 = normal)",                      &c.soldierSightScale, 0, 400, 10, "%"),
                MakeInt   ("soldierNightSight",     "Enemy night vision detection range",                               &c.soldierNightSight, 0, 400, 10, "%"),
                MakeInt   ("soldierHearingScale",   "Enemy hearing detection range",                                    &c.soldierHearingScale, 0, 400, 10, "%"),
                MakeInt   ("cpCommunicationRange",  "CP communication broadcast range",                                &c.cpCommunicationRange, 0, 500, 10, "%"),
            }),
            MakeSep(),
            MakeSub("Combat", "Enemy combat effectiveness", {
                MakeInt   ("soldierHealthScale",    "Enemy health multiplier (100 = normal, 900 = max)",                &c.soldierHealthScale, 0, 900, 10, "%"),
                MakeInt   ("soldierAccuracy",       "Enemy shooting accuracy scale",                                    &c.soldierAccuracy, 0, 400, 10, "%"),
                MakeInt   ("soldierReaction",       "Enemy reaction time speed",                                        &c.soldierReaction, 0, 400, 10, "%"),
                MakeInt   ("soldierAggression",     "Enemy aggression / pursuit intensity",                            &c.soldierAggression, 0, 400, 10, "%"),
                MakeInt   ("soldierSpeed",          "Enemy movement speed",                                             &c.soldierSpeed, 0, 300, 10, "%"),
            }),
            MakeSep(),
            MakeSub("Status Effects", "Stun, sleep, and holdup durations", {
                MakeInt   ("stunDuration",          "Stun effect duration scale (100 = normal)",                        &c.stunDuration, 10, 500, 10, "%"),
                MakeInt   ("sleepDuration",         "Sleep effect duration scale",                                      &c.sleepDuration, 10, 500, 10, "%"),
                MakeInt   ("holdupDuration",        "Holdup compliance duration scale",                                 &c.holdupDuration, 10, 500, 10, "%"),
            }),
            MakeSep(),
            MakeInt   ("itemDropChance",        "Chance that enemies drop items on death (%)",                          &c.itemDropChance, 0, 100, 5, "%"),
            MakeInt   ("guardRouteLength",      "Guard patrol route length scale (%)",                                  &c.guardRouteLength, 50, 300, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Behavior", "Enemy behavioral overrides", {
            MakeToggle("disableRadioCall",      "Prevent guards from calling CP via radio",                            &c.disableRadioCall),
            MakeToggle("disableBodySearch",     "Guards won't search for unconscious/dead bodies",                     &c.disableBodySearch),
            MakeToggle("disableInvestigate",    "Guards won't investigate suspicious noises",                          &c.disableInvestigate),
            MakeToggle("noAlertPropagation",    "Alerts don't spread to nearby guard posts",                           &c.noAlertPropagation),
            MakeToggle("interrogationAlways",   "Interrogation always succeeds regardless of soldier",                 &c.interrogationAlways),
            MakeInt   ("commsJamDuration",      "Force communications jamming duration (seconds, 0=off)",              &c.commsJamDuration, 0, 120, 5, "s"),
        }),
        MakeSep(),
        MakeSub("Phases", "Control enemy alert phase behavior", {
            MakeEnum  ("enemyPhase",            "Force all enemies to a specific alert phase",                          &c.enemyPhase, s_Phases, 5),
            MakeToggle("keepPhase",             "Lock the current phase - prevent phase transitions",                   &c.keepPhase),
            MakeToggle("soldierAlertOnVehicle", "Soldiers alert when heavy vehicles take damage",                       &c.soldierAlertOnVehicle),
        }),
        MakeSep(),
        MakeSub("Reinforcements", "Control enemy reinforcement behavior", {
            MakeToggle("forceSuperReinforce",   "Force maximum reinforcement response to all alerts",                   &c.forceSuperReinforce),
            MakeToggle("enableHeliReinforce",   "Allow attack helicopters in reinforcement waves",                      &c.enableHeliReinforce),
            MakeSep(),
            MakeSub("Count & Level", "Reinforcement numbers and quality", {
                MakeInt   ("reinforceCountMin",     "Minimum number of reinforcement soldiers",                         &c.reinforceCountMin, 0, 20, 1),
                MakeInt   ("reinforceCountMax",     "Maximum number of reinforcement soldiers",                         &c.reinforceCountMax, 0, 20, 1),
                MakeInt   ("reinforceLevelMin",     "Minimum reinforcement equipment level",                            &c.reinforceLevelMin, 0, 10, 1),
                MakeInt   ("reinforceLevelMax",     "Maximum reinforcement equipment level",                            &c.reinforceLevelMax, 0, 10, 1),
            }),
        }),
        MakeSep(),
        MakeSub("Custom Prep", "Customize enemy equipment loadouts", {
            MakeSub("Weapons", "Enemy weapon prevalence", {
                MakeInt   ("prepSniper",    "Sniper rifle prevalence (%)",   &c.prepSniper, 0, 100, 5, "%"),
                MakeInt   ("prepMissile",   "Missile launcher prevalence",   &c.prepMissile, 0, 100, 5, "%"),
                MakeInt   ("prepMG",        "Machine gun prevalence",        &c.prepMG, 0, 100, 5, "%"),
                MakeInt   ("prepShotgun",   "Shotgun prevalence",            &c.prepShotgun, 0, 100, 5, "%"),
                MakeInt   ("prepSMG",       "SMG prevalence",                &c.prepSMG, 0, 100, 5, "%"),
                MakeInt   ("prepSidearm",   "Sidearm / pistol prevalence",   &c.prepSidearm, 0, 100, 5, "%"),
            }),
            MakeSep(),
            MakeSub("Armor & Protection", "Enemy defensive gear prevalence", {
                MakeInt   ("prepArmor",     "Body armor prevalence",         &c.prepArmor, 0, 100, 5, "%"),
                MakeInt   ("prepShield",    "Riot shield prevalence",        &c.prepShield, 0, 100, 5, "%"),
                MakeInt   ("prepHelmet",    "Helmet prevalence",             &c.prepHelmet, 0, 100, 5, "%"),
                MakeInt   ("prepNVG",       "Night vision goggle prevalence",&c.prepNVG, 0, 100, 5, "%"),
                MakeInt   ("prepGasMask",   "Gas mask prevalence",           &c.prepGasMask, 0, 100, 5, "%"),
            }),
            MakeSep(),
            MakeSub("Equipment & Traps", "Enemy utility equipment", {
                MakeInt   ("prepDecoy",     "Decoy deployment prevalence",   &c.prepDecoy, 0, 100, 5, "%"),
                MakeInt   ("prepMine",      "Mine placement prevalence",     &c.prepMine, 0, 100, 5, "%"),
                MakeInt   ("prepCamera",    "Security camera prevalence",    &c.prepCamera, 0, 100, 5, "%"),
                MakeInt   ("prepFlashlight","Flashlight prevalence",         &c.prepFlashlight, 0, 100, 5, "%"),
                MakeInt   ("prepRadio",     "Radio equipment prevalence",    &c.prepRadio, 0, 100, 5, "%"),
            }),
            MakeSep(),
            MakeSub("Explosives", "Enemy explosive equipment", {
                MakeInt   ("prepGrenade",     "Hand grenade prevalence",     &c.prepGrenade, 0, 100, 5, "%"),
                MakeInt   ("prepSmokeBomb",   "Smoke bomb prevalence",       &c.prepSmokeBomb, 0, 100, 5, "%"),
                MakeInt   ("prepStunGrenade", "Stun grenade prevalence",     &c.prepStunGrenade, 0, 100, 5, "%"),
                MakeInt   ("prepC4",          "C4 explosive prevalence",     &c.prepC4, 0, 100, 5, "%"),
            }),
            MakeSep(),
            MakeSub("Quality", "Overall equipment quality levels", {
                MakeInt   ("prepWeaponLevel", "Enemy weapon development level (0=default, 1-10)",                       &c.prepWeaponLevel, 0, 10, 1),
                MakeInt   ("prepArmorLevel",  "Enemy armor development level (0=default, 1-5)",                         &c.prepArmorLevel, 0, 5, 1),
            }),
        }),
        MakeSep(),
        MakeSub("Appearance", "Soldier visual variation", {
            MakeInt   ("soldierFaceVar",        "Soldier face variation (0 = default, 1-20 variants)",                  &c.soldierFaceVar, 0, 20, 1),
            MakeInt   ("soldierBodyVar",        "Soldier body variation (0 = default, 1-10 variants)",                  &c.soldierBodyVar, 0, 10, 1),
            MakeInt   ("customSoldierType",     "Override soldier type ID (0 = default)",                               &c.customSoldierType, 0, 30, 1),
        }),
        MakeSep(),
        MakeSub("Vehicle Patrols", "Enemy vehicle patrol spawns", {
            MakeInt   ("vehiclePatrolCount",    "Number of vehicle patrols active (0-10)",                              &c.vehiclePatrolCount, 0, 10, 1),
            MakeEnum  ("vehiclePatrolType",     "Vehicle type for patrols",                                             &c.vehiclePatrolType, s_VPatrolType, 4),
        }),
        MakeSep(),
        MakeSub("Special Forces", "Wild card, LRRP, and special enemy types", {
            MakeToggle("enableWildCard",        "Spawn wild card soldiers with unique models and high-end weapons",     &c.enableWildCard),
            MakeToggle("enableLrrp",            "Enable long-range foot patrols between checkpoints",                   &c.enableLrrp),
            MakeToggle("forceSniperGuards",     "Force all guards to use sniper rifles",                                &c.forceSniperGuards),
            MakeToggle("forceHeavyArmor",       "Force all soldiers to wear heavy body armor",                          &c.forceHeavyArmor),
        }),
        MakeSep(),
        MakeEnum  ("revengeMode",               "Enemy revenge/adaptation mode",                                        &c.revengeMode, s_Revenge, 4),
    });
}

static MenuNode BuildSkullsMenu(Config& c) {
    return MakeSub("Skulls / Parasite", "Skull unit and parasite encounter settings", {
        MakeSub("Spawning", "Skull unit spawn controls", {
            MakeToggle("enableSkulls",          "Enable Skull unit encounters in the field",                            &c.enableSkulls),
            MakeToggle("skullsInFreeRoam",      "Allow Skulls to appear during free roam",                             &c.skullsInFreeRoam),
            MakeEnum  ("skullsType",            "Skull unit type variant",                                              &c.skullsType, s_SkullsType, 4),
            MakeInt   ("skullsCount",           "Number of Skulls per encounter (1-12)",                               &c.skullsCount, 1, 12, 1),
            MakeInt   ("skullsRespawnTime",     "Respawn timer in seconds (0 = never respawn)",                        &c.skullsRespawnTime, 0, 300, 10, "s"),
        }),
        MakeSep(),
        MakeSub("Stats", "Skull combat parameters", {
            MakeInt   ("skullsHealth",          "Skull unit health scale (100 = normal)",                               &c.skullsHealth, 10, 1000, 10, "%"),
            MakeInt   ("skullsSpeed",           "Skull unit movement speed scale",                                      &c.skullsSpeed, 50, 500, 10, "%"),
            MakeInt   ("skullsDamage",          "Skull unit damage output scale",                                       &c.skullsDamage, 10, 1000, 10, "%"),
            MakeInt   ("skullsAggression",      "Skull unit aggression level",                                          &c.skullsAggression, 0, 500, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Behavior", "Skull unit behavioral toggles", {
            MakeToggle("skullsTeleport",        "Allow Skulls to teleport/dash",                                        &c.skullsTeleport),
            MakeToggle("skullsDisappear",       "Allow Skulls to vanish and reappear",                                  &c.skullsDisappear),
            MakeEnum  ("skullsPhase",           "Force Skulls into specific combat phase",                              &c.skullsPhase, s_SkullsPhase, 4),
            MakeToggle("skullsArmorBreak",      "Skulls' armor is breakable by sustained fire",                         &c.skullsArmorBreak),
        }),
        MakeSep(),
        MakeInt   ("skullsMistDensity",         "Parasite mist density during Skull encounters (%)",                    &c.skullsMistDensity, 0, 500, 10, "%"),
    });
}

static MenuNode BuildMissionMenu(Config& c) {
    return MakeSub("Mission", "Mission preparation, restrictions, and gameplay rules", {
        MakeSub("Preparation", "Mission start settings", {
            MakeToggle("skipMissionPrep",       "Skip the mission preparation screen",                                 &c.skipMissionPrep),
            MakeEnum  ("startOnFoot",           "Start missions on foot instead of in the helicopter",                  &c.startOnFoot, s_StartFoot, 4),
            MakeToggle("forceNight",            "Force all missions to start at night",                                 &c.forceNight),
            MakeToggle("forceDay",              "Force all missions to start during day",                               &c.forceDay),
        }),
        MakeSep(),
        MakeSub("Restrictions", "Gameplay restrictions and difficulty modifiers", {
            MakeSub("Core Rules", "Fundamental gameplay rule overrides", {
                MakeToggle("gameOverOnDiscovery",   "Instant game over when any enemy spots you",                       &c.gameOverOnDiscovery),
                MakeToggle("disableGameOver",       "Disable game over entirely - never fail a mission",                &c.disableGameOver),
                MakeToggle("disableRetry",          "Remove the retry option from the pause menu",                      &c.disableRetry),
                MakeToggle("disableOutOfBounds",    "Disable out-of-bounds area restrictions",                          &c.disableOutOfBounds),
                MakeToggle("noCheckpoints",         "Disable automatic checkpoint saving",                              &c.noCheckpoints),
            }),
            MakeSep(),
            MakeSub("Loadout", "Starting equipment restrictions", {
                MakeToggle("setSubsistence",        "Force subsistence loadout (no equipment)",                         &c.setSubsistence),
                MakeToggle("clearItems",            "Start with no items or support equipment",                         &c.clearItems),
                MakeToggle("disableHeliAttack",     "Disable helicopter attack support",                                &c.disableHeliAttack),
                MakeInt   ("handAbilityLevel",      "Prosthetic hand ability level (0=default, 1-4)",                   &c.handAbilityLevel, 0, 4, 1),
                MakeInt   ("itemLevel",             "Item development level (0=default, 1-6)",                          &c.itemLevel, 0, 6, 1),
            }),
            MakeSep(),
            MakeSub("Difficulty Mods", "Extra difficulty modifiers", {
                MakeToggle("noReflex",              "Disable reflex mode entirely",                                     &c.noReflex),
                MakeToggle("noMarking",             "Disable enemy marking with binoculars",                            &c.noMarking),
                MakeToggle("noSupplyDrop",          "Disable supply drop support",                                      &c.noSupplyDrop),
                MakeToggle("disableBuddySupport",   "Disable buddy support commands",                                  &c.disableBuddySupport),
                MakeToggle("disableAirStrikes",     "Disable air strike support",                                       &c.disableAirStrikes),
                MakeToggle("disableSpotter",        "Disable the buddy spotter call-outs",                              &c.disableSpotter),
                MakeToggle("disableIndicators",     "Disable directional damage indicators",                            &c.disableIndicators),
                MakeToggle("disableSloMo",          "Disable all slow motion effects",                                  &c.disableSloMo),
                MakeToggle("disableCompanionApp",   "Disable companion app integration",                                &c.disableCompanionApp),
                MakeToggle("disableMBSupport",      "Disable Mother Base support features",                             &c.disableMBSupport),
            }),
            MakeSep(),
            MakeSub("Timed Mission", "Time limit controls", {
                MakeToggle("timedMission",          "Enable mission time limit",                                        &c.timedMission),
                MakeInt   ("timedMissionMin",       "Time limit in minutes",                                            &c.timedMissionMin, 1, 120, 5, " min"),
            }),
        }),
        MakeSep(),
        MakeSub("HUD & Markers", "Control on-screen markers and indicators", {
            MakeToggle("disableHeadMarkers",    "Hide enemy head markers (red triangles)",                              &c.disableHeadMarkers),
            MakeToggle("disableWorldMarkers",   "Hide all world-space markers and icons",                               &c.disableWorldMarkers),
            MakeToggle("disableXrayMarkers",    "Hide X-ray/binoculars marking outlines",                               &c.disableXrayMarkers),
        }),
        MakeSep(),
        MakeSub("Support Menus", "Disable specific iDroid support options", {
            MakeToggle("disableSupportMenu",    "Disable the support menu in the iDroid",                               &c.disableSupportMenu),
            MakeToggle("disableBuddyMenu",      "Disable the buddy selection menu",                                    &c.disableBuddyMenu),
            MakeToggle("disableAttackMenu",     "Disable the attack support menu",                                      &c.disableAttackMenu),
            MakeToggle("disableFultonMenu",     "Disable the fulton menu",                                              &c.disableFultonMenu),
        }),
        MakeSep(),
        MakeSub("Score & Ranking", "Mission scoring and rank overrides", {
            MakeInt   ("scoreMultiplier",       "Mission score multiplier (100 = normal)",                              &c.scoreMultiplier, 10, 1000, 10, "%"),
            MakeEnum  ("missionRankOverride",   "Force a specific mission rank result",                                 &c.missionRankOverride, s_MissionRank, 7),
        }),
    });
}

static MenuNode BuildHelicopterMenu(Config& c) {
    return MakeSub("Helicopter", "Support helicopter behavior and landing zone settings", {
        MakeSub("Defense", "Helicopter durability and evasion", {
            MakeToggle("invincibleHeli",        "Make the support helicopter invincible",                               &c.invincibleHeli),
            MakeToggle("disablePullOut",        "Prevent the helicopter from pulling out under fire",                   &c.disablePullOut),
            MakeToggle("heliStealth",           "Make helicopter less detectable by enemies",                           &c.heliStealth),
            MakeToggle("heliAutoEvade",         "Helicopter auto-evades incoming missiles",                             &c.heliAutoEvade),
            MakeInt   ("heliArmor",             "Helicopter armor scale (100 = normal)",                                &c.heliArmor, 10, 500, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Landing & Approach", "Landing zone and approach settings", {
            MakeEnum  ("searchLight",           "Helicopter search light override",                                     &c.searchLight, s_SearchLight, 3),
            MakeInt   ("lzWaitHeight",          "Helicopter hover height at landing zone (meters)",                     &c.lzWaitHeight, 5, 50, 5, "m"),
            MakeInt   ("doorOpenTime",          "Time doors stay open after landing (seconds)",                         &c.doorOpenTime, 0, 120, 5, "s"),
            MakeEnum  ("disableLZs",            "Disable landing zone types",                                           &c.disableLZs, s_DisableLZ, 3),
            MakeToggle("disableDescentToLZ",    "Prevent helicopter from descending to landing zones",                  &c.disableDescentToLZ),
            MakeFloat ("heliApproachAngle",     "Helicopter approach angle override (degrees)",                         &c.heliApproachAngle, -90.0f, 90.0f, 5.0f, " deg"),
        }),
        MakeSep(),
        MakeSub("Armament", "Helicopter weapon loadout", {
            MakeEnum  ("heliWeapon",            "Helicopter weapon selection",                                           &c.heliWeapon, s_HeliWeapon, 5),
            MakeInt   ("heliSpeed",             "Helicopter flight speed scale (%)",                                     &c.heliSpeed, 50, 300, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Customization", "Helicopter appearance and audio", {
            MakeInt   ("heliColor",             "Helicopter color/skin index (0 = default, 1-12)",                       &c.heliColor, 0, 12, 1),
            MakeToggle("customHeliMusic",       "Enable custom helicopter arrival music",                               &c.customHeliMusic),
            MakeInt   ("heliMusicTrack",        "Helicopter arrival music track (0-45)",                                &c.heliMusicTrack, 0, 45, 1),
            MakeInt   ("heliRadioStation",      "In-flight radio station (0 = default, 1-15)",                          &c.heliRadioStation, 0, 15, 1),
        }),
    });
}

static MenuNode BuildBuddyMenu(Config& c) {
    return MakeSub("Buddy", "Buddy equipment, behavior, and commands", {
        MakeSub("Quiet", "Quiet weapon, behavior, and radio settings", {
            MakeSub("Weapon", "Quiet's weapon loadout", {
                MakeEnum  ("quietWeapon",   "Quiet's weapon selection",                                                 &c.quietWeapon, s_QuietWpn, 6),
            }),
            MakeSep(),
            MakeSub("Behavior", "Quiet's AI behavior and bond", {
                MakeEnum  ("quietBehavior", "Quiet's behavior mode",                                                    &c.quietBehavior, s_QuietBehav, 4),
                MakeInt   ("quietBondLevel","Bond level with Quiet (0=default, 1-5)",                                   &c.quietBondLevel, 0, 5, 1),
                MakeToggle("quietInvisible","Make Quiet invisible to enemies",                                          &c.quietInvisible),
            }),
            MakeSep(),
            MakeSub("Audio", "Quiet humming and radio", {
                MakeToggle("quietHumming",  "Quiet hums while idle",                                                    &c.quietHumming),
                MakeInt   ("quietRadioMode","Quiet radio frequency / track (0=off, 1-32=tracks)",                       &c.quietRadioMode, 0, 32, 1),
            }),
        }),
        MakeSep(),
        MakeSub("D-Dog", "D-Dog equipment, behavior, and tricks", {
            MakeSub("Equipment", "D-Dog gear loadout", {
                MakeEnum  ("ddogEquip",     "D-Dog equipment loadout",                                                  &c.ddogEquip, s_DDogEquip, 5),
            }),
            MakeSep(),
            MakeSub("Behavior", "D-Dog AI and interaction", {
                MakeEnum  ("ddogBehavior",  "D-Dog behavior mode",                                                      &c.ddogBehavior, s_DDogBehav, 4),
                MakeInt   ("ddogBondLevel", "Bond level with D-Dog (0=default, 1-5)",                                   &c.ddogBondLevel, 0, 5, 1),
                MakeToggle("ddogBarking",   "D-Dog barks at detected enemies",                                          &c.ddogBarking),
                MakeToggle("ddogAutoMark",  "D-Dog auto-marks nearby enemies",                                          &c.ddogAutoMark),
                MakeEnum  ("ddogTricks",    "D-Dog tricks commands",                                                    &c.ddogTricks, s_DDogTricks, 5),
            }),
        }),
        MakeSep(),
        MakeSub("D-Horse", "D-Horse equipment and behavior", {
            MakeSub("Equipment", "D-Horse gear selection", {
                MakeEnum  ("dhorseEquip",   "D-Horse equipment variant",                                                &c.dhorseEquip, s_DHorseEquip, 4),
            }),
            MakeSep(),
            MakeSub("Behavior", "D-Horse AI and speed", {
                MakeEnum  ("dhorseBehavior","D-Horse behavior mode when dismounted",                                    &c.dhorseBehavior, s_DHorseBehav, 4),
                MakeInt   ("dhorseBondLevel","Bond level with D-Horse (0=default, 1-5)",                                &c.dhorseBondLevel, 0, 5, 1),
                MakeToggle("dhorseDefecation","D-Horse deposits droppings on command",                                  &c.dhorseDefecation),
                MakeInt   ("dhorseSpeed",   "D-Horse speed scale (100 = normal)",                                       &c.dhorseSpeed, 50, 300, 10, "%"),
            }),
        }),
        MakeSep(),
        MakeSub("D-Walker", "D-Walker weapon and patrol settings", {
            MakeSub("Weapon", "D-Walker armament selection", {
                MakeEnum  ("dwalkerEquip",  "D-Walker weapon loadout",                                                  &c.dwalkerEquip, s_DWalkerEquip, 5),
                MakeInt   ("dwalkerAmmo",   "D-Walker ammo count (0 = infinite)",                                       &c.dwalkerAmmo, 0, 999, 50),
            }),
            MakeSep(),
            MakeSub("Behavior", "D-Walker AI and stealth", {
                MakeEnum  ("dwalkerBehavior","D-Walker behavior mode",                                                  &c.dwalkerBehavior, s_DWalkerBehav, 4),
                MakeToggle("dwalkerAutoAttack","D-Walker automatically fires at detected enemies",                      &c.dwalkerAutoAttack),
                MakeToggle("dwalkerInvisible","Make D-Walker invisible to enemies",                                     &c.dwalkerInvisible),
            }),
        }),
        MakeSep(),
        MakeSub("Multi-Buddy & Command Wheel", "Field several buddies at once and direct them from a radial wheel", {
            MakeToggle("multiBuddies",      "Keep several buddies deployed at the same time",                          &c.multiBuddies),
            MakeSep(),
            MakeSub("Active Buddies", "Which buddies multi-buddy and the wheel manage", {
                MakeToggle("buddyDDogActive",   "Include D-Dog",                                                       &c.buddyDDogActive),
                MakeToggle("buddyQuietActive",  "Include Quiet",                                                       &c.buddyQuietActive),
                MakeToggle("buddyDHorseActive", "Include D-Horse",                                                     &c.buddyDHorseActive),
                MakeToggle("buddyDWalkerActive","Include D-Walker",                                                    &c.buddyDWalkerActive),
            }),
            MakeSep(),
            MakeSub("Command Wheel", "Radial command-wheel overlay settings", {
                MakeToggle("buddyWheel",        "Enable the command wheel overlay",                                    &c.buddyWheel),
                MakeToggle("buddyWheelGamepad", "Open by holding Y on a controller",                                   &c.buddyWheelGamepad),
                MakeToggle("buddyWheelKeyboard","Open by holding X on the keyboard",                                   &c.buddyWheelKeyboard),
                MakeFloat ("buddyWheelScale",   "Wheel size multiplier",                          &c.buddyWheelScale, 0.5f, 2.0f, 0.1f, "x"),
            }),
        }),
    });
}

static MenuNode BuildMotherBaseMenu(Config& c) {
    return MakeSub("Mother Base", "Mother Base NPCs, patrols, assets, and behavior", {
        MakeSub("Characters", "Toggle NPC characters on Mother Base", {
            MakeToggle("mbEnableOcelot",    "Enable Ocelot NPC on Mother Base",                                         &c.mbEnableOcelot),
            MakeToggle("mbEnablePuppy",     "Enable puppy D-Dog on Mother Base",                                        &c.mbEnablePuppy),
            MakeToggle("mbShowCodeTalker",  "Show Code Talker NPC",                                                     &c.mbShowCodeTalker),
            MakeToggle("mbEnableBirds",     "Enable bird wildlife on Mother Base",                                      &c.mbEnableBirds),
            MakeToggle("mbEnableBuddies",   "Enable buddy NPCs to roam Mother Base",                                   &c.mbEnableBuddies),
            MakeSep(),
            MakeSub("Animals", "Animal NPC settings", {
                MakeToggle("mbShowAnimals", "Enable animal NPCs on Mother Base",                                        &c.mbShowAnimals),
                MakeEnum  ("mbAnimalType",  "Animal species to spawn",                                                  &c.mbAnimalType, s_MBAnimal, 5),
            }),
        }),
        MakeSep(),
        MakeSub("Soldiers", "Soldier behavior and appearance on Mother Base", {
            MakeEnum  ("mbSoldierEquip",        "Soldier weapon range on Mother Base",                                  &c.mbSoldierEquip, s_EquipRange, 4),
            MakeInt   ("mbAdditionalSoldiers",  "Extra soldiers to spawn on MB (0-50)",                                 &c.mbAdditionalSoldiers, 0, 50, 5),
            MakeToggle("mbPrioritizeFemale",    "Prioritize female soldier models",                                     &c.mbPrioritizeFemale),
            MakeToggle("mbEnableLethal",        "Allow lethal actions on Mother Base",                                  &c.mbEnableLethal),
            MakeToggle("mbMoraleBoosts",        "Enable morale boost salute reactions",                                 &c.mbMoraleBoosts),
            MakeEnum  ("mbNpcRouteMode",        "NPC route behavior mode",                                              &c.mbNpcRouteMode, s_MBNpcRoute, 4),
        }),
        MakeSep(),
        MakeSub("Patrols", "Helicopter and vehicle patrols", {
            MakeInt   ("supportHeliMB",     "Support helicopter patrol count (0-3)",                                    &c.supportHeliMB, 0, 3, 1),
            MakeInt   ("attackHeliMB",      "Attack helicopter patrol count (0-4)",                                     &c.attackHeliMB, 0, 4, 1),
            MakeToggle("mbEnableWalkerGears","Deploy Walker Gears on Mother Base",                                     &c.mbEnableWalkerGears),
        }),
        MakeSep(),
        MakeSub("Assets", "Mother Base visual assets and facilities", {
            MakeToggle("mbShowPosters",     "Display Big Boss posters on Mother Base",                                  &c.mbShowPosters),
            MakeToggle("mbCollectionRepop", "Regenerate plants/diamonds on Mother Base",                                &c.mbCollectionRepop),
            MakeToggle("mbUnlockGoalDoors", "Unlock all goal/objective doors on MB",                                    &c.mbUnlockGoalDoors),
        }),
        MakeSep(),
        MakeSub("Ocean", "Mother Base ocean environment settings", {
            MakeFloat ("mbOceanWaveHeight", "Ocean wave height multiplier",                                             &c.mbOceanWaveHeight, 0.0f, 5.0f, 0.1f, "x"),
            MakeFloat ("mbOceanWaveSpeed",  "Ocean wave animation speed",                                               &c.mbOceanWaveSpeed, 0.0f, 5.0f, 0.1f, "x"),
            MakeSep(),
            MakeSub("Color", "Ocean water color", {
                MakeInt ("mbOceanColorR",   "Ocean red channel (0-255)",                                                &c.mbOceanColorR, 0, 255, 5),
                MakeInt ("mbOceanColorG",   "Ocean green channel (0-255)",                                              &c.mbOceanColorG, 0, 255, 5),
                MakeInt ("mbOceanColorB",   "Ocean blue channel (0-255)",                                               &c.mbOceanColorB, 0, 255, 5),
            }),
            MakeSep(),
            MakeFloat ("mbOceanFoam",       "Ocean foam intensity (0 = none, 1 = heavy)",                               &c.mbOceanFoam, 0.0f, 1.0f, 0.05f),
            MakeFloat ("mbOceanTransparency","Ocean water transparency",                                                &c.mbOceanTransparency, 0.0f, 1.0f, 0.05f),
            MakeFloat ("mbOceanReflection", "Ocean surface reflection intensity",                                       &c.mbOceanReflection, 0.0f, 3.0f, 0.1f, "x"),
        }),
        MakeSep(),
        MakeSub("Security", "Mother Base security and alarm settings", {
            MakeEnum  ("mbSecurityLevel",   "Overall security alert level",                                             &c.mbSecurityLevel, s_MBSecurity, 5),
            MakeToggle("mbEnableAlarms",    "Enable security alarm systems",                                            &c.mbEnableAlarms),
            MakeToggle("mbEnableUAV",       "Deploy UAV surveillance drones",                                           &c.mbEnableUAV),
        }),
        MakeSep(),
        MakeSub("Facilities", "Platform and facility settings", {
            MakeEnum  ("mbLightingMode",    "Mother Base lighting mode",                                                &c.mbLightingMode, s_MBLighting, 4),
            MakeToggle("mbShowerTimer",     "Show timer until next required shower",                                    &c.mbShowerTimer),
            MakeInt   ("mbShowerInterval",  "Minutes between required showers",                                         &c.mbShowerInterval, 1, 999, 10, " min"),
            MakeToggle("mbPlatformSelect",  "Enable platform fast-travel selection",                                    &c.mbPlatformSelect),
            MakeInt   ("mbActivePlatform",  "Active platform index (0-6)",                                              &c.mbActivePlatform, 0, 6, 1),
        }),
    });
}

static MenuNode BuildFultonMenu(Config& c) {
    return MakeSub("Fulton", "Fulton recovery system settings", {
        MakeSub("Mode", "Fulton operation mode", {
            MakeEnum  ("autoFulton",        "Automatically fulton extracted targets",                                    &c.autoFulton, s_AutoFulton, 4),
            MakeToggle("recoverCritical",   "Allow recovery of critically wounded soldiers",                            &c.recoverCritical),
            MakeToggle("disableFultonAbil", "Completely disable the fulton ability",                                    &c.disableFultonAbility),
            MakeToggle("fultonSilent",      "Silent fulton - no balloon pop sound or visual",                           &c.fultonSilent),
        }),
        MakeSep(),
        MakeSub("Level & Quality", "Fulton device tier and reliability", {
            MakeInt   ("fultonLevel",       "Fulton device level (1-4, higher = more reliable)",                        &c.fultonLevel, 1, 4, 1),
            MakeInt   ("wormholeLevel",     "Wormhole fulton (0=disabled, 1=enabled)",                                   &c.wormholeLevel, 0, 1, 1),
            MakeInt   ("fultonSuccessRate", "Fulton success rate override (%)",                                          &c.fultonSuccessRate, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeSub("Targets", "What can be fultoned", {
            MakeToggle("fultonEverything",  "Fulton any object regardless of normal restrictions",                       &c.fultonEverything),
            MakeToggle("fultonVehicles",    "Enable fultoning vehicles without upgrade",                                &c.fultonVehicles),
            MakeToggle("fultonContainers",  "Enable fultoning containers without upgrade",                              &c.fultonContainers),
            MakeToggle("fultonGunEmplace",  "Enable fultoning gun emplacements",                                        &c.fultonGunEmplace),
        }),
        MakeSep(),
        MakeSub("Penalties & Variation", "Fulton penalties and animation", {
            MakeInt   ("fultonVariation",   "Fulton animation variation range (%)",                                      &c.fultonVariation, 0, 100, 5, "%"),
            MakeInt   ("fultonDyingPenalty", "Penalty reduction for fultoning dying soldiers (%)",                       &c.fultonDyingPenalty, 0, 100, 10, "%"),
            MakeInt   ("fultonSleepPenalty", "Penalty reduction for fultoning sleeping soldiers (%)",                    &c.fultonSleepPenalty, 0, 100, 10, "%"),
            MakeInt   ("fultonSpeed",       "Fulton extraction speed scale (%)",                                         &c.fultonSpeed, 50, 500, 10, "%"),
        }),
    });
}

static MenuNode BuildSideOpsMenu(Config& c) {
    return MakeSub("Side Ops", "Side ops / quest management and filtering", {
        MakeSub("Selection", "How side ops are chosen and displayed", {
            MakeInt   ("forceQuestNumber",  "Force a specific side op number (0 = none)",                               &c.forceQuestNumber, 0, 157, 1),
            MakeEnum  ("questSelectMode",   "How side ops are selected for each area",                                  &c.questSelectMode, s_QuestSelect, 3),
            MakeEnum  ("questRepopMode",    "When cleared side ops respawn",                                             &c.questRepopMode, s_QuestRepop, 2),
            MakeInt   ("maxActiveQuests",   "Maximum active side ops at once (1-10)",                                    &c.maxActiveQuests, 1, 10, 1),
        }),
        MakeSep(),
        MakeSub("Rules", "Side ops gameplay rules", {
            MakeToggle("enableMissionQuest","Allow side ops to appear during story missions",                           &c.enableMissionQuest),
            MakeToggle("questRetry",        "Enable retry option for shooting practice quests",                         &c.questRetry),
            MakeToggle("questEnemyScale",   "Scale enemy difficulty in side ops",                                       &c.questEnemyScale),
            MakeInt   ("questEnemyLevel",   "Side ops enemy level scale (50-300%)",                                     &c.questEnemyLevel, 50, 300, 10, "%"),
        }),
        MakeSep(),
        MakeSub("Time Limits", "Optional time limits for side ops", {
            MakeToggle("questTimeLimits",   "Enable time limits on side ops",                                           &c.questTimeLimits),
            MakeInt   ("questTimeMinutes",  "Time limit in minutes",                                                    &c.questTimeMinutes, 5, 120, 5, " min"),
        }),
    });
}

static MenuNode BuildCutscenesMenu(Config& c) {
    return MakeSub("Cutscenes", "Cutscene (demo) behavior and overrides", {
        MakeSub("Character", "Character model in cutscenes", {
            MakeToggle("useSoldierForDemos",    "Use selected soldier model in all cutscenes",                          &c.useSoldierForDemos),
        }),
        MakeSep(),
        MakeSub("Playback", "Cutscene playback controls", {
            MakeToggle("forceDemoAllowAction",  "Allow player actions during cutscenes (skip, etc.)",                   &c.forceDemoAllowAction),
            MakeToggle("skipAllCutscenes",      "Automatically skip all cutscenes",                                     &c.skipAllCutscenes),
            MakeFloat ("demoPlaybackSpeed",     "Cutscene playback speed (0.1 = slow, 4.0 = fast)",                    &c.demoPlaybackSpeed, 0.1f, 4.0f, 0.1f, "x"),
            MakeToggle("demoFreeCamera",        "Enable free camera during cutscenes",                                  &c.demoFreeCamera),
        }),
        MakeSep(),
        MakeSub("Overrides", "Override cutscene environment", {
            MakeEnum  ("demoOverrideWeather",   "Force weather during cutscenes",                                       &c.demoOverrideWeather, s_Weather, 7),
            MakeInt   ("demoOverrideHour",      "Force time of day during cutscenes (-1 = default)",                    &c.demoOverrideHour, -1, 23, 1),
        }),
        MakeSep(),
        MakeSub("Display", "Cutscene display settings", {
            MakeToggle("demoHideHUD",           "Hide all HUD elements during cutscenes",                               &c.demoHideHUD),
            MakeEnum  ("demoSubtitles",         "Subtitle display mode",                                                &c.demoSubtitles, s_DemoSubtitles, 3),
            MakeToggle("demoLetterbox",         "Show letterbox bars during cutscenes",                                  &c.demoLetterbox),
        }),
    });
}

static MenuNode BuildEventsMenu(Config& c) {
    return MakeSub("Events", "Random free-roam, Mother Base, and special events", {
        MakeSub("Free Roam Events", "Chance-based random events during free roam", {
            MakeInt   ("eventHunted",       "HUNTED event chance - continuous combat alert (%)",                         &c.eventHunted, 0, 100, 5, "%"),
            MakeInt   ("eventCrashLand",    "CRASHLAND event chance - random start, OSP (%)",                           &c.eventCrashLand, 0, 100, 5, "%"),
            MakeInt   ("eventLostComs",     "LOST COMS event chance - disabled support (%)",                            &c.eventLostComs, 0, 100, 5, "%"),
            MakeSep(),
            MakeInt   ("eventAmbush",       "AMBUSH event chance - surprise enemy attack (%)",                          &c.eventAmbush, 0, 100, 5, "%"),
            MakeInt   ("eventSandstorm",    "SANDSTORM event chance - sudden sandstorm (%)",                            &c.eventSandstorm, 0, 100, 5, "%"),
            MakeInt   ("eventBlackout",     "BLACKOUT event chance - power outage (%)",                                 &c.eventBlackout, 0, 100, 5, "%"),
            MakeInt   ("eventInvasion",     "INVASION event chance - large enemy force (%)",                            &c.eventInvasion, 0, 100, 5, "%"),
            MakeInt   ("eventSupplyLoss",   "SUPPLY LOSS event chance - lose random resources (%)",                     &c.eventSupplyLoss, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeSub("Skull Encounters", "Chance for Skull unit to appear in free roam", {
            MakeToggle("eventSkulls",       "Enable random Skull encounters in free roam",                              &c.eventSkulls),
            MakeInt   ("eventSkullsChance", "Skull encounter chance (%)",                                               &c.eventSkullsChance, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeSub("MB War Games", "Mother Base combat training events", {
            MakeToggle("mbWarGames",        "Enable war game events on Mother Base",                                    &c.mbWarGames),
            MakeEnum  ("mbWarGameProfile",  "War game type / faction profile",                                          &c.mbWarGameProfile, s_WarGameProf, 8),
        }),
        MakeSep(),
        MakeSub("Wildlife", "Animal encounter settings", {
            MakeInt   ("animalEncounterRate","Animal encounter frequency scale (%)",                                     &c.animalEncounterRate, 0, 500, 10, "%"),
            MakeToggle("eventMeteorShower",  "Enable meteor shower random events",                                      &c.eventMeteorShower),
            MakeInt   ("eventCustomFreq",    "Custom event check frequency scale (%)",                                  &c.eventCustomFreq, 0, 500, 10, "%"),
        }),
    });
}

static MenuNode BuildVehicleMenu(Config& c) {
    return MakeSub("Vehicle", "Vehicle combat, handling, and customization", {
        MakeSub("Durability", "Vehicle health and damage settings", {
            MakeToggle("vehicleGodMode",    "Vehicles are indestructible",                                              &c.vehicleGodMode),
            MakeToggle("noVehicleDamage",   "Vehicles take no cosmetic damage",                                         &c.noVehicleDamage),
            MakeInt   ("vehicleHealthScale","Vehicle health multiplier (%)",                                             &c.vehicleHealthScale, 10, 500, 10, "%"),
            MakeToggle("vehicleAutoRepair", "Vehicles slowly auto-repair over time",                                    &c.vehicleAutoRepair),
        }),
        MakeSep(),
        MakeSub("Performance", "Speed and handling", {
            MakeFloat ("vehicleSpeedMult",  "Vehicle speed multiplier",                                                 &c.vehicleSpeedMult, 0.5f, 5.0f, 0.1f, "x"),
            MakeEnum  ("vehicleHandling",   "Vehicle handling preset",                                                  &c.vehicleHandling, s_VehicleHandle, 4),
            MakeToggle("vehicleNitro",      "Enable nitro boost on vehicles (shift to activate)",                       &c.vehicleNitro),
            MakeFloat ("vehicleNitroMult",  "Nitro boost speed multiplier",                                             &c.vehicleNitroMult, 1.5f, 5.0f, 0.1f, "x"),
        }),
        MakeSep(),
        MakeSub("Weapons", "Vehicle armament", {
            MakeToggle("infiniteVehicleAmmo","Infinite vehicle ammunition",                                             &c.infiniteVehicleAmmo),
        }),
        MakeSep(),
        MakeSub("Spawning", "Spawn vehicles on demand", {
            MakeEnum  ("vehicleSpawnType",  "Vehicle type to spawn near player",                                        &c.vehicleSpawnType, s_VehicleSpawn, 6),
            MakeInt   ("vehicleColorIndex", "Vehicle color / livery (0 = default, 1-15)",                               &c.vehicleColorIndex, 0, 15, 1),
        }),
        MakeSep(),
        MakeSub("Misc", "Other vehicle settings", {
            MakeToggle("vehicleInvisible",  "Vehicle is invisible to enemies",                                          &c.vehicleInvisible),
            MakeToggle("vehicleHorn",       "Enable vehicle horn",                                                      &c.vehicleHorn),
            MakeInt   ("vehicleRadio",      "Vehicle radio station (0 = off, 1-15)",                                    &c.vehicleRadio, 0, 15, 1),
        }),
    });
}

static MenuNode BuildSoundMenu(Config& c) {
    return MakeSub("Sound / Music", "Audio volume, BGM, SFX, and music controls", {
        MakeSub("Volume", "Master volume controls per audio channel", {
            MakeInt   ("bgmVolume",         "Background music volume (0-100)",                                          &c.bgmVolume, 0, 100, 5, "%"),
            MakeInt   ("sfxVolume",         "Sound effects volume",                                                     &c.sfxVolume, 0, 100, 5, "%"),
            MakeInt   ("voiceVolume",       "Voice / dialogue volume",                                                  &c.voiceVolume, 0, 100, 5, "%"),
            MakeInt   ("ambientVolume",     "Ambient / environmental volume",                                           &c.ambientVolume, 0, 100, 5, "%"),
            MakeInt   ("radioVolume",       "Radio / codec volume",                                                     &c.radioVolume, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeSub("Mute", "Quickly mute specific channels", {
            MakeToggle("muteAllAudio",      "Mute all game audio",                                                      &c.muteAllAudio),
            MakeToggle("muteBGM",           "Mute background music only",                                               &c.muteBGM),
            MakeToggle("muteSFX",           "Mute sound effects only",                                                  &c.muteSFX),
            MakeToggle("muteVoice",         "Mute voice / dialogue only",                                               &c.muteVoice),
        }),
        MakeSep(),
        MakeSub("Cassette Player", "Cassette tape playback controls", {
            MakeInt   ("cassetteTrack",     "Cassette track number (0 = none, 1-60 = tracks)",                          &c.cassetteTrack, 0, 60, 1),
            MakeToggle("cassetteLoop",      "Loop the current cassette track",                                          &c.cassetteLoop),
            MakeToggle("cassetteAutoplay",  "Auto-play cassettes when entering helicopter",                             &c.cassetteAutoplay),
        }),
        MakeSep(),
        MakeSub("Combat Audio", "Alert and combat audio overrides", {
            MakeEnum  ("alertBGM",          "Alert phase BGM override",                                                 &c.alertBGM, s_AlertBGM, 4),
            MakeToggle("disableAlertSound", "Disable the alert sound effect (!)",                                       &c.disableAlertSound),
            MakeToggle("disableReflex",     "Disable the reflex mode sound cue",                                        &c.disableReflex),
            MakeInt   ("heliMusicOverride", "Helicopter music override track (0=default, 1-45)",                        &c.heliMusicOverride, 0, 45, 1),
        }),
    });
}

static MenuNode BuildProgressionMenu(Config& c) {
    return MakeSub("Progression", "Unlock weapons, items, outfits, and team levels", {
        MakeSub("Unlock All", "Instantly unlock categories of content", {
            MakeToggle("unlockAllWeapons",   "Unlock all weapon blueprints and developments",                           &c.unlockAllWeapons),
            MakeToggle("unlockAllItems",     "Unlock all usable items and support items",                               &c.unlockAllItems),
            MakeToggle("unlockAllOutfits",   "Unlock all suits, uniforms, and outfits",                                 &c.unlockAllOutfits),
            MakeToggle("unlockAllBuddyGear", "Unlock all buddy equipment and upgrades",                                &c.unlockAllBuddyGear),
            MakeToggle("unlockAllBlueprints","Unlock all development blueprints",                                       &c.unlockAllBlueprints),
            MakeToggle("unlockAllTapes",     "Unlock all cassette tapes",                                               &c.unlockAllTapes),
            MakeToggle("unlockAllMissions",  "Unlock all story missions",                                               &c.unlockAllMissions),
            MakeToggle("unlockAllSideOps",   "Unlock all side ops",                                                     &c.unlockAllSideOps),
        }),
        MakeSep(),
        MakeSub("Team Levels", "Override Mother Base team levels", {
            MakeInt   ("combatLevel",       "Combat unit level (0=default, 1-99)",                                      &c.combatLevel, 0, 99, 1),
            MakeInt   ("rdLevel",           "R&D team level",                                                           &c.rdLevel, 0, 99, 1),
            MakeInt   ("supportLevel",      "Support team level",                                                       &c.supportLevel, 0, 99, 1),
            MakeInt   ("intelLevel",        "Intel team level",                                                         &c.intelLevel, 0, 99, 1),
            MakeInt   ("medicalLevel",      "Medical team level",                                                       &c.medicalLevel, 0, 99, 1),
            MakeInt   ("baseDevLevel",      "Base development level",                                                   &c.baseDevLevel, 0, 99, 1),
            MakeInt   ("securityLevel",     "Security team level",                                                      &c.securityLevel, 0, 99, 1),
        }),
        MakeSep(),
        MakeSub("Max Out", "Max out platforms and teams instantly", {
            MakeToggle("maxAllPlatforms",   "Max out all Mother Base platform levels",                                  &c.maxAllPlatforms),
            MakeToggle("maxAllTeams",       "Max out all team levels",                                                  &c.maxAllTeams),
            MakeEnum  ("rankOverride",      "Force staff recruit rank",                                                 &c.rankOverride, s_StaffRank, 7),
        }),
    });
}

static MenuNode BuildWaveSurvivalMenu(Config& c) {
    return MakeSub("Wave Survival", "Inner Heaven's stealth wave-survival gamemode - pick an arena and hold out", {
        MakeSub("Start Mission", "Choose an arena and deploy (always replayable)", {
            MakeCmd("Afghan Outpost",   "Sunny desert outpost - start a wave session here",
                    []() { Config::Get().waveArena = 0; Features::WaveMode::Start(); }),
            MakeCmd("Africa Village",   "Overcast African village - start a wave session here",
                    []() { Config::Get().waveArena = 1; Features::WaveMode::Start(); }),
            MakeCmd("Mother Base Deck", "Mother Base platform - start a wave session here",
                    []() { Config::Get().waveArena = 2; Features::WaveMode::Start(); }),
            MakeCmd("Ruins Compound",   "Foggy ruins - start a wave session here",
                    []() { Config::Get().waveArena = 3; Features::WaveMode::Start(); }),
            MakeCmd("Airport Facility", "Rainy airfield - start a wave session here",
                    []() { Config::Get().waveArena = 4; Features::WaveMode::Start(); }),
            MakeCmd("Current Location", "Build the arena right where you are standing",
                    []() { Config::Get().waveArena = 5; Features::WaveMode::Start(); }),
        }),
        MakeCmd("Stop Mission",     "End the active wave session and clear all spawned enemies",
                Features::WaveMode::Stop),
        MakeSep(),
        MakeSub("Rules", "Tune wave counts, difficulty, and pacing", {
            MakeInt ("waveStartEnemies",   "Enemies in the first wave",                                  &c.waveStartEnemies, 1, 24, 1),
            MakeInt ("waveEnemyIncrement", "Extra enemies added each subsequent wave",                   &c.waveEnemyIncrement, 0, 12, 1),
            MakeInt ("waveMaxEnemies",     "Hard cap on simultaneous enemies per wave",                  &c.waveMaxEnemies, 4, 48, 2),
            MakeInt ("waveMaxWaves",       "Waves needed to win (0 = endless survival)",                 &c.waveMaxWaves, 0, 50, 1),
            MakeEnum("waveDifficulty",     "Enemy difficulty preset (scales health & accuracy)",         &c.waveDifficulty, s_WaveDiff, 4),
            MakeInt ("waveSpawnRadius",    "Radius of the spawn ring around you (metres)",               &c.waveSpawnRadius, 15, 120, 5, "m"),
            MakeInt ("waveReinforceDelay", "Pause between clearing a wave and the next (seconds)",       &c.waveReinforceDelay, 0, 20, 1, "s"),
        }),
        MakeSep(),
        MakeSub("Balancing", "Stealth-focused balancing options", {
            MakeToggle("waveNeutralUntilSpotted", "New waves spawn passive and only turn hostile once you're spotted", &c.waveNeutralUntilSpotted),
            MakeToggle("waveAutoStealthBonus",    "Award a ghost bonus for clearing a wave undetected",  &c.waveAutoStealthBonus),
            MakeToggle("waveLethalPenalty",       "Reduce score for waves where you were detected",      &c.waveLethalPenalty),
        }),
        MakeSep(),
        MakeSub("Display", "Gamemode HUD and presentation", {
            MakeToggle("waveShowHud",      "Show the top-left enemy counter and status HUD",             &c.waveShowHud),
            MakeToggle("waveForceWeather", "Apply each arena's themed weather when starting",            &c.waveForceWeather),
            MakeEnum  ("waveArena",        "Currently selected arena theme",                             &c.waveArena, s_WaveArena, 6),
        }),
    });
}

static MenuNode BuildVisualsMenu(Config& c) {
    return MakeSub("Visuals", "ESP overlays, night vision, crosshair, thermal, and rendering", {
        MakeSub("ESP", "Extra-sensory perception overlays for enemy tracking", {
            MakeToggle("espEnabled",    "Enable ESP overlay (shows enemies through walls)",                              &c.espEnabled),
            MakeSep(),
            MakeSub("Display", "What information to show", {
                MakeToggle("espBoxes",      "Draw bounding boxes around enemies",                                        &c.espBoxes),
                MakeToggle("espDistance",   "Show distance to each enemy in meters",                                     &c.espDistance),
                MakeToggle("espHealthBar",  "Show health bars above enemies",                                            &c.espHealthBar),
                MakeToggle("espNames",      "Show enemy name/type labels",                                               &c.espNames),
                MakeToggle("espSkeleton",   "Draw skeleton/bone overlay on enemies",                                     &c.espSkeleton),
                MakeToggle("espWeapons",    "Show what weapon each enemy is carrying",                                   &c.espWeapons),
                MakeToggle("espAlertState", "Show enemy alert state (sneak/caution/alert)",                              &c.espAlertState),
            }),
            MakeSep(),
            MakeSub("Range & Lines", "Detection range and snap lines", {
                MakeInt   ("espMaxRange",   "Maximum ESP render distance (meters)",                                      &c.espMaxRange, 10, 2000, 50, "m"),
                MakeToggle("espSnapLines",  "Draw lines from screen edge to each enemy",                                 &c.espSnapLines),
                MakeEnum  ("espSnapLineOrigin","Snap line starting point",                                               &c.espSnapLineOrigin, s_EspSnap, 3),
            }),
            MakeSep(),
            MakeSub("Colors", "ESP color customization", {
                MakeInt   ("espBoxColorR",  "Enemy box red channel (0-255)",                                             &c.espBoxColorR, 0, 255, 5),
                MakeInt   ("espBoxColorG",  "Enemy box green channel",                                                   &c.espBoxColorG, 0, 255, 5),
                MakeInt   ("espBoxColorB",  "Enemy box blue channel",                                                    &c.espBoxColorB, 0, 255, 5),
                MakeSep(),
                MakeInt   ("espAllyColorR", "Ally/neutral red channel",                                                  &c.espAllyColorR, 0, 255, 5),
                MakeInt   ("espAllyColorG", "Ally/neutral green channel",                                                &c.espAllyColorG, 0, 255, 5),
                MakeInt   ("espAllyColorB", "Ally/neutral blue channel",                                                 &c.espAllyColorB, 0, 255, 5),
            }),
            MakeSep(),
            MakeSub("Extra Targets", "ESP for non-enemy objects", {
                MakeToggle("espVehicles",   "Show ESP for vehicles",                                                     &c.espVehicles),
                MakeToggle("espItems",      "Show ESP for collectible items",                                            &c.espItems),
                MakeToggle("espObjectives", "Show ESP for mission objectives",                                           &c.espObjectives),
            }),
        }),
        MakeSep(),
        MakeSub("Night Vision", "Brightness amplification for dark environments", {
            MakeToggle("nightVision",       "Enable night vision (brightness boost)",                                    &c.nightVision),
            MakeFloat ("nightVisionStr",    "Night vision intensity multiplier",                                         &c.nightVisionStr, 1.0f, 5.0f, 0.1f, "x"),
            MakeEnum  ("nvColorMode",       "Night vision color tint",                                                   &c.nvColorMode, s_NVColor, 4),
            MakeFloat ("nvContrast",        "Night vision contrast level",                                               &c.nvContrast, 0.5f, 3.0f, 0.1f),
            MakeToggle("nvNoise",           "Add film grain noise to night vision",                                      &c.nvNoise),
        }),
        MakeSep(),
        MakeSub("Thermal Vision", "Heat-based imaging overlay", {
            MakeToggle("thermalVision",     "Enable thermal vision overlay",                                             &c.thermalVision),
            MakeFloat ("thermalStr",        "Thermal vision intensity",                                                  &c.thermalStr, 0.5f, 5.0f, 0.1f, "x"),
            MakeEnum  ("thermalPalette",    "Thermal color palette",                                                     &c.thermalPalette, s_ThermalPal, 4),
        }),
        MakeSep(),
        MakeSub("Crosshair", "On-screen crosshair overlay", {
            MakeToggle("crosshair",         "Show crosshair overlay at screen center",                                   &c.crosshair),
            MakeEnum  ("crosshairStyle",    "Crosshair shape preset",                                                    &c.crosshairStyle, s_CrosshairStyle, 5),
            MakeInt   ("crosshairSize",     "Crosshair line length in pixels",                                           &c.crosshairSize, 4, 24, 1, "px"),
            MakeInt   ("crosshairGap",      "Center gap between crosshair lines (px)",                                   &c.crosshairGap, 0, 20, 1, "px"),
            MakeInt   ("crosshairThickness","Crosshair line thickness (px)",                                             &c.crosshairThickness, 1, 5, 1, "px"),
            MakeFloat ("crosshairOpacity",  "Crosshair opacity (0.1 = faint, 1.0 = solid)",                             &c.crosshairOpacity, 0.1f, 1.0f, 0.05f),
            MakeToggle("crosshairDynamic",  "Crosshair expands when moving / firing",                                    &c.crosshairDynamic),
            MakeSep(),
            MakeSub("Color", "Crosshair color", {
                MakeInt ("crosshairColorR", "Red channel (0-255)",                                                       &c.crosshairColorR, 0, 255, 5),
                MakeInt ("crosshairColorG", "Green channel (0-255)",                                                     &c.crosshairColorG, 0, 255, 5),
                MakeInt ("crosshairColorB", "Blue channel (0-255)",                                                      &c.crosshairColorB, 0, 255, 5),
            }),
        }),
        MakeSep(),
        MakeEnum  ("colorGrading",         "Global color grading filter",                                                &c.colorGrading, s_ColorGrade, 6),
    });
}

static MenuNode BuildMiscMenu(Config& c) {
    return MakeSub("Misc", "Position saving, teleportation, and utility features", {
        MakeSub("Position Slots", "Save and load up to 3 world positions", {
            MakeCmd   ("Save Slot 1",       "Save current position to slot 1",                                          Features::Misc::SavePosition),
            MakeCmd   ("Load Slot 1",       "Teleport to slot 1 saved position",                                        Features::Misc::LoadPosition),
            MakeSep(),
            MakeCmd   ("Save Slot 2",       "Save current position to slot 2",                                          []() {
                auto& c = Config::Get();
                auto& m = Features::Misc::GetPlayerPos();
                if (m) { c.savedPos2X = m[0]; c.savedPos2Y = m[1]; c.savedPos2Z = m[2]; c.hasSavedPos2 = true; }
            }),
            MakeCmd   ("Load Slot 2",       "Teleport to slot 2 saved position",                                        []() {
                auto& c = Config::Get();
                if (c.hasSavedPos2) { auto& m = Features::Misc::GetPlayerPos(); if(m) { m[0]=c.savedPos2X; m[1]=c.savedPos2Y; m[2]=c.savedPos2Z; } }
            }),
            MakeSep(),
            MakeCmd   ("Save Slot 3",       "Save current position to slot 3",                                          []() {
                auto& c = Config::Get();
                auto& m = Features::Misc::GetPlayerPos();
                if (m) { c.savedPos3X = m[0]; c.savedPos3Y = m[1]; c.savedPos3Z = m[2]; c.hasSavedPos3 = true; }
            }),
            MakeCmd   ("Load Slot 3",       "Teleport to slot 3 saved position",                                        []() {
                auto& c = Config::Get();
                if (c.hasSavedPos3) { auto& m = Features::Misc::GetPlayerPos(); if(m) { m[0]=c.savedPos3X; m[1]=c.savedPos3Y; m[2]=c.savedPos3Z; } }
            }),
        }),
        MakeSep(),
        MakeCmd   ("Open Lua Console",  "Open the Fox Engine Lua 5.1 interactive console",                             []() { Config::Get().luaConsoleOpen = !Config::Get().luaConsoleOpen; }),
        MakeSep(),
        MakeToggle("showHelp",          "Show help text at bottom of menu for selected item",                           &c.showHelp),
    });
}

static MenuNode BuildPresetsMenu(Config& c) {
    return MakeSub("Quick Presets", "One-click configuration presets", {
        MakeCmd("Stealth Preset", "Silent assassin: stealth mode, silent weapons, no detection", []() {
            auto& c = Config::Get();
            c.stealthMode = true; c.silentWeapons = true; c.noiseScale = 0;
            c.infiniteSuppressor = true; c.disableAlertSound = true;
        }),
        MakeCmd("Rambo Preset", "Full combat: god mode, infinite ammo, one hit kill, explosive rounds", []() {
            auto& c = Config::Get();
            c.godMode = true; c.infiniteAmmo = true; c.noReload = true;
            c.oneHitKill = true; c.explosiveBullets = true; c.rapidFire = true;
            c.noRecoil = true; c.noFallDamage = true;
        }),
        MakeCmd("Hardcore Preset", "Maximum difficulty: no HUD, no reflex, no markers, tough enemies", []() {
            auto& c = Config::Get();
            c.noReflex = true; c.noMarking = true; c.noSupplyDrop = true;
            c.disableHeadMarkers = true; c.disableWorldMarkers = true;
            c.disableXrayMarkers = true; c.disableIndicators = true;
            c.soldierAccuracy = 300; c.soldierReaction = 200; c.soldierAggression = 300;
            c.soldierSightScale = 200; c.soldierHearingScale = 200;
        }),
        MakeCmd("Exploration Preset", "Free roam: super speed, no enemies, free camera, all fulton", []() {
            auto& c = Config::Get();
            c.superSpeed = true; c.speedMultiplier = 3.0f;
            c.noEnemyAI = true; c.noFallDamage = true;
            c.infiniteFulton = true; c.fultonEverything = true;
        }),
        MakeCmd("Photo Preset", "Photography: free camera, photo mode, time freeze, no HUD", []() {
            auto& c = Config::Get();
            c.freeCamera = true; c.photoMode = true;
            c.slowMotion = true; c.timeScale = 0.01f;
            c.disableHeadMarkers = true; c.disableWorldMarkers = true;
        }),
        MakeCmd("Max Resources", "Set GMP to max, heroism to max, zero demon points", []() {
            auto& c = Config::Get();
            c.gmp = 99999999; Features::Resources::SetGMP(99999999);
            c.heroism = 9999999; Features::Resources::SetHeroism(9999999);
            c.demonPoints = 0; Features::Resources::SetDemonPoints(0);
            c.lockGMP = true; c.lockHeroism = true; c.lockDemonPoints = true;
        }),
        MakeSep(),
        MakeCmd("Reset All", "Reset all settings to default values", []() {
            Config::Get().Reset();
        }),
    });
}

static MenuNode BuildDebugMenu(Config& c) {
    return MakeSub("Debug", "Debug output, logging, and diagnostic tools", {
        MakeSub("Logging", "Debug message and logging controls", {
            MakeToggle("debugMode",         "Enable debug mode (verbose game state logging)",                            &c.debugMode),
            MakeToggle("debugMessages",     "Show debug messages in the announce log",                                   &c.debugMessages),
            MakeToggle("printButtons",      "Print pressed button codes to console",                                     &c.printButtons),
            MakeToggle("logToFile",         "Write debug output to log file",                                            &c.logToFile),
            MakeEnum  ("logLevel",          "Minimum log verbosity level",                                               &c.logLevel, s_LogLevel, 5),
        }),
        MakeSep(),
        MakeSub("Display", "On-screen debug overlays", {
            MakeToggle("showPosition",      "Display player XYZ coordinates on screen",                                  &c.showPosition),
            MakeToggle("showFPS",           "Display frames per second counter",                                         &c.showFPS),
            MakeToggle("showFrameTime",     "Display frame time in milliseconds",                                        &c.showFrameTime),
        }),
        MakeSep(),
        MakeSub("Rendering", "Debug rendering modes", {
            MakeToggle("wireframeMode",     "Render world in wireframe mode",                                            &c.wireframeMode),
            MakeToggle("showColliders",     "Show collision geometry overlays",                                          &c.showColliders),
            MakeToggle("showNavMesh",       "Show navigation mesh visualization",                                        &c.showNavMesh),
            MakeToggle("showRoutes",        "Show enemy patrol route lines",                                             &c.showRoutes),
            MakeToggle("showCPZones",       "Show command post zone boundaries",                                         &c.showCPZones),
        }),
        MakeSep(),
        MakeSub("Startup", "Startup behavior", {
            MakeToggle("skipLogos",         "Skip startup logos on next launch",                                         &c.skipLogos),
            MakeToggle("startOffline",      "Start the game in offline mode",                                            &c.startOffline),
        }),
        MakeSep(),
        MakeToggle("dumpMemory",        "Dump memory scan results to file",                                              &c.dumpMemory),
        MakeSep(),
        MakeCmd("Pattern Scan Report", "Open a window showing which memory patches and the Lua bridge resolved on your game build", []() {
            Config::Get().showScanReport = !Config::Get().showScanReport;
        }),
        MakeCmd("Run Lua Diagnostics", "Probe which Lua namespaces (vars/gvars/Player/etc) exist (results appear in-game announce log)", []() {
            Features::GameLua::RunDiagnostics();
        }),
    });
}

// ── Root ───────────────────────────────────────────────────────────────────

MenuNode BuildMenuTree() {
    auto& c = Config::Get();
    return MakeSub("Inner Heaven", "MGSV:TPP Singleplayer Mod Menu", {
        BuildPlayerMenu(c),
        BuildAppearanceMenu(c),
        BuildCameraMenu(c),
        BuildResourcesMenu(c),
        BuildWorldMenu(c),
        MakeSep(),
        BuildEnemyMenu(c),
        BuildSkullsMenu(c),
        BuildMissionMenu(c),
        BuildHelicopterMenu(c),
        BuildBuddyMenu(c),
        BuildMotherBaseMenu(c),
        BuildFultonMenu(c),
        BuildSideOpsMenu(c),
        MakeSep(),
        BuildCutscenesMenu(c),
        BuildEventsMenu(c),
        BuildVehicleMenu(c),
        BuildSoundMenu(c),
        BuildProgressionMenu(c),
        MakeSep(),
        BuildWaveSurvivalMenu(c),
        BuildVisualsMenu(c),
        BuildPresetsMenu(c),
        BuildMiscMenu(c),
        BuildDebugMenu(c),
    });
}
