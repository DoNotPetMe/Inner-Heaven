#include "menu_defs.h"
#include "../config.h"
#include "../features/resources.h"
#include "../features/world.h"
#include "../features/misc.h"
#include "../features/lua_console.h"

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

// ── Category builders ──────────────────────────────────────────────────────

static MenuNode BuildPlayerMenu(Config& c) {
    return MakeSub("Player", "Player combat, movement, health, and hero settings", {
        MakeSub("Combat", "Offensive and defensive combat cheats", {
            MakeToggle("godMode",             "Become invincible - take no damage from any source",                    &c.godMode),
            MakeToggle("infiniteAmmo",        "Never run out of ammunition in any weapon",                             &c.infiniteAmmo),
            MakeToggle("noReload",            "Skip magazine changes entirely - fire continuously",                    &c.noReload),
            MakeToggle("infiniteSuppressor",  "Suppressors never wear out or degrade",                                 &c.infiniteSuppressor),
            MakeToggle("noRecoil",            "Perfectly stable aim - no weapon recoil or camera kick",                &c.noRecoil),
            MakeToggle("oneHitKill",          "All enemies die in a single hit from any weapon",                       &c.oneHitKill),
            MakeToggle("infiniteReflex",      "Reflex mode lasts forever when triggered",                              &c.infiniteReflex),
            MakeToggle("rapidFire",           "Remove fire rate limiter on all weapons",                               &c.rapidFire),
        }),
        MakeSub("Movement", "Player movement and physics", {
            MakeToggle("superSpeed",          "Move faster than normal",                                               &c.superSpeed),
            MakeFloat ("speedMultiplier",     "Speed multiplier when super speed is enabled (1.5-10x)",                &c.speedMultiplier, 1.5f, 10.0f, 0.5f, "x"),
            MakeToggle("noFallDamage",        "Take no damage from falling, no matter the height",                     &c.noFallDamage),
            MakeToggle("customPlayerScale",   "Apply custom scale to the player model",                                &c.customPlayerScale),
            MakeFloat ("playerScale",         "Player model scale factor (0.1 = tiny, 5.0 = giant)",                   &c.playerScale, 0.1f, 5.0f, 0.1f),
        }),
        MakeSub("Stealth", "Detection and alert settings", {
            MakeToggle("stealthMode",         "Detection meter is frozen at zero - guards never see you",              &c.stealthMode),
        }),
        MakeSep(),
        MakeInt   ("playerHealthScale",       "Player health multiplier as a percentage (100 = normal, 650 = max)",    &c.playerHealthScale, 0, 650, 10, "%"),
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
        MakeEnum  ("playerType",    "Select which character model to use",                                             &c.playerType, s_PlayerTypes, 7),
        MakeEnum  ("playerParts",   "Select suit or outfit (Sneaking Suit, Battle Dress, etc.)",                       &c.playerParts, s_PlayerParts, 17),
        MakeInt   ("playerCamo",    "Camo pattern index (0-110+, game-dependent)",                                     &c.playerCamo, 0, 120, 1),
        MakeInt   ("headgear",      "Face equipment / headgear ID",                                                    &c.headgear, 0, 100, 1),
        MakeEnum  ("handType",      "Bionic arm type selection",                                                       &c.handType, s_HandTypes, 8),
    });
}

static MenuNode BuildCameraMenu(Config& c) {
    return MakeSub("Camera", "Camera control, FOV, and free camera settings", {
        MakeSub("Free Camera", "Detach camera from player and fly freely", {
            MakeToggle("freeCamera",       "Enable free camera mode (WASD + Space/Ctrl to move)",                      &c.freeCamera),
            MakeFloat ("camSpeed",         "Free camera movement speed",                                               &c.camSpeed, 1.0f, 50.0f, 1.0f),
        }),
        MakeSep(),
        MakeSub("FOV Override", "Field of view control per camera mode", {
            MakeToggle("customFOV",        "Enable custom field of view override",                                     &c.customFOV),
            MakeFloat ("fovValue",         "Global FOV in degrees (default 75)",                                       &c.fovValue, 30.0f, 170.0f, 1.0f, " deg"),
            MakeSep(),
            MakeFloat ("fovNormal",        "FOV for normal third-person view",                                         &c.fovNormal, 30.0f, 170.0f, 1.0f, " deg"),
            MakeFloat ("fovAiming",        "FOV when aiming/shouldering",                                              &c.fovAiming, 20.0f, 120.0f, 1.0f, " deg"),
            MakeFloat ("fovHiding",        "FOV when hiding behind cover",                                             &c.fovHiding, 30.0f, 120.0f, 1.0f, " deg"),
            MakeFloat ("fovCQC",           "FOV during CQC animations",                                                &c.fovCQC, 30.0f, 120.0f, 1.0f, " deg"),
        }),
        MakeSep(),
        MakeFloat ("focalLength",          "Camera focal length (default 21.0, lower = wider angle)",                  &c.focalLength, 0.1f, 200.0f, 0.5f),
        MakeFloat ("aperture",             "Camera aperture for depth of field (default 1.2)",                         &c.aperture, 0.001f, 100.0f, 0.1f),
        MakeFloat ("focusDistance",         "Camera focus distance in meters",                                          &c.focusDistance, 0.01f, 1000.0f, 1.0f),
    });
}

static MenuNode BuildResourcesMenu(Config& c) {
    return MakeSub("Resources", "GMP, heroism, demon points, and resource scaling", {
        MakeInt   ("gmp",                  "Current GMP value (set and press Enter to apply)",                         &c.gmp, 0, 99999999, 100000),
        MakeToggle("lockGMP",              "Freeze GMP at current value every frame",                                  &c.lockGMP),
        MakeCmd   ("Set GMP to 5M",        "Instantly set GMP to 5,000,000",                                           []() { auto& c = Config::Get(); c.gmp = 5000000; Features::Resources::SetGMP(5000000); }),
        MakeSep(),
        MakeInt   ("heroism",              "Current heroism value",                                                     &c.heroism, 0, 9999999, 10000),
        MakeToggle("lockHeroism",          "Freeze heroism at current value every frame",                               &c.lockHeroism),
        MakeCmd   ("Set Heroism to 1M",    "Instantly set heroism to 1,000,000",                                        []() { auto& c = Config::Get(); c.heroism = 1000000; Features::Resources::SetHeroism(1000000); }),
        MakeSep(),
        MakeInt   ("demonPoints",          "Current demon points (affects horn growth)",                                &c.demonPoints, 0, 9999999, 10000),
        MakeToggle("lockDemonPoints",      "Freeze demon points at current value",                                      &c.lockDemonPoints),
        MakeCmd   ("Zero Demon Points",    "Reset demon points to zero",                                                []() { auto& c = Config::Get(); c.demonPoints = 0; Features::Resources::SetDemonPoints(0); }),
        MakeSep(),
        MakeSub("Resource Scale", "Multiply resource collection rates per type", {
            MakeInt("resourceScaleMat",        "Material collection multiplier (%)",                                    &c.resourceScaleMat, 10, 1000, 10, "%"),
            MakeInt("resourceScalePlant",      "Plant collection multiplier (%)",                                       &c.resourceScalePlant, 10, 1000, 10, "%"),
            MakeInt("resourceScaleDia",        "Diamond collection multiplier (%)",                                     &c.resourceScaleDia, 10, 1000, 10, "%"),
            MakeInt("resourceScaleContainer",  "Container collection multiplier (%)",                                   &c.resourceScaleContainer, 10, 1000, 10, "%"),
        }),
        MakeSep(),
        MakeCmd   ("Max All Plants",       "Fill all plant resource slots to 999",                                      Features::Resources::MaxAllPlants),
        MakeCmd   ("Max All Materials",    "Fill all material resource slots to 999999",                                 Features::Resources::MaxAllMaterials),
    });
}

static MenuNode BuildWorldMenu(Config& c) {
    return MakeSub("World", "Time, weather, and environment settings", {
        MakeSub("Time Scale", "Control game simulation speed", {
            MakeToggle("slowMotion",            "Enable slow motion effect",                                            &c.slowMotion),
            MakeFloat ("timeScale",             "Time scale multiplier when slow motion is active (lower = slower)",    &c.timeScale, 0.01f, 1.0f, 0.05f),
            MakeInt   ("clockTimeScale",        "In-game clock speed multiplier (1 = normal, 10000 = max)",             &c.clockTimeScale, 1, 10000, 10),
            MakeInt   ("speedCamWorldScale",    "World time scale during speed camera (%)",                             &c.speedCamWorldScale, 0, 10000, 100, "%"),
            MakeInt   ("speedCamPlayerScale",   "Player time scale during speed camera (%)",                            &c.speedCamPlayerScale, 0, 10000, 100, "%"),
            MakeInt   ("speedCamContinueTime",  "Speed camera continue duration (seconds)",                             &c.speedCamContinueTime, 0, 1000, 5, "s"),
        }),
        MakeSep(),
        MakeSub("Time of Day", "Override the in-game clock", {
            MakeToggle("overrideTime",          "Lock the time of day to a specific hour",                              &c.overrideTime),
            MakeFloat ("timeOfDay",             "Hour of day (0.0 = midnight, 12.0 = noon, 18.0 = sunset)",            &c.timeOfDay, 0.0f, 24.0f, 0.5f, "h"),
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
        }),
        MakeSep(),
        MakeToggle("noEnemyAI",                 "Freeze all enemy behavior trees - enemies stand still",               &c.noEnemyAI),
        MakeToggle("infiniteFulton",            "Unlimited fulton extractions, never run out",                          &c.infiniteFulton),
        MakeCmd   ("Teleport to Waypoint",      "Instantly move player to the current map marker position",            Features::World::TeleportToWaypoint),
    });
}

static MenuNode BuildEnemyMenu(Config& c) {
    return MakeSub("Enemy", "Enemy soldier parameters, phases, reinforcements, and prep", {
        MakeSub("Soldier Params", "Scale enemy capabilities globally", {
            MakeInt   ("soldierHealthScale",    "Enemy health multiplier (100 = normal, 900 = max)",                    &c.soldierHealthScale, 0, 900, 10, "%"),
            MakeInt   ("soldierSightScale",     "Enemy visual detection range (100 = normal)",                          &c.soldierSightScale, 0, 400, 10, "%"),
            MakeInt   ("soldierNightSight",     "Enemy night vision detection range",                                   &c.soldierNightSight, 0, 400, 10, "%"),
            MakeInt   ("soldierHearingScale",   "Enemy hearing detection range",                                        &c.soldierHearingScale, 0, 400, 10, "%"),
            MakeInt   ("itemDropChance",        "Chance that enemies drop items on death (%)",                          &c.itemDropChance, 0, 100, 5, "%"),
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
            MakeInt   ("reinforceCountMin",     "Minimum number of reinforcement soldiers",                             &c.reinforceCountMin, 0, 20, 1),
            MakeInt   ("reinforceCountMax",     "Maximum number of reinforcement soldiers",                             &c.reinforceCountMax, 0, 20, 1),
            MakeInt   ("reinforceLevelMin",     "Minimum reinforcement equipment level",                                &c.reinforceLevelMin, 0, 10, 1),
            MakeInt   ("reinforceLevelMax",     "Maximum reinforcement equipment level",                                &c.reinforceLevelMax, 0, 10, 1),
        }),
        MakeSep(),
        MakeSub("Custom Prep", "Customize enemy equipment loadouts", {
            MakeInt   ("prepSniper",    "Sniper rifle prevalence (%)",   &c.prepSniper, 0, 100, 5, "%"),
            MakeInt   ("prepMissile",   "Missile launcher prevalence",   &c.prepMissile, 0, 100, 5, "%"),
            MakeInt   ("prepMG",        "Machine gun prevalence",        &c.prepMG, 0, 100, 5, "%"),
            MakeInt   ("prepShotgun",   "Shotgun prevalence",            &c.prepShotgun, 0, 100, 5, "%"),
            MakeInt   ("prepSMG",       "SMG prevalence",                &c.prepSMG, 0, 100, 5, "%"),
            MakeSep(),
            MakeInt   ("prepArmor",     "Body armor prevalence",         &c.prepArmor, 0, 100, 5, "%"),
            MakeInt   ("prepShield",    "Riot shield prevalence",        &c.prepShield, 0, 100, 5, "%"),
            MakeInt   ("prepHelmet",    "Helmet prevalence",             &c.prepHelmet, 0, 100, 5, "%"),
            MakeInt   ("prepNVG",       "Night vision goggle prevalence",&c.prepNVG, 0, 100, 5, "%"),
            MakeInt   ("prepGasMask",   "Gas mask prevalence",           &c.prepGasMask, 0, 100, 5, "%"),
            MakeSep(),
            MakeInt   ("prepDecoy",     "Decoy deployment prevalence",   &c.prepDecoy, 0, 100, 5, "%"),
            MakeInt   ("prepMine",      "Mine placement prevalence",     &c.prepMine, 0, 100, 5, "%"),
            MakeInt   ("prepCamera",    "Security camera prevalence",    &c.prepCamera, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeEnum  ("revengeMode",               "Enemy revenge/adaptation mode",                                        &c.revengeMode, s_Revenge, 4),
        MakeInt   ("customSoldierType",         "Override soldier type ID (0 = default)",                               &c.customSoldierType, 0, 30, 1),
        MakeToggle("enableWildCard",            "Spawn wild card soldiers with unique models and high-end weapons",     &c.enableWildCard),
        MakeToggle("enableLrrp",                "Enable long-range foot patrols between checkpoints",                   &c.enableLrrp),
    });
}

static MenuNode BuildMissionMenu(Config& c) {
    return MakeSub("Mission", "Mission preparation, restrictions, and gameplay rules", {
        MakeSub("Preparation", "Mission start settings", {
            MakeToggle("skipMissionPrep",       "Skip the mission preparation screen",                                 &c.skipMissionPrep),
            MakeEnum  ("startOnFoot",           "Start missions on foot instead of in the helicopter",                  &c.startOnFoot, s_StartFoot, 4),
        }),
        MakeSep(),
        MakeSub("Restrictions", "Gameplay restrictions and difficulty modifiers", {
            MakeToggle("disableHeliAttack",     "Disable helicopter attack support",                                    &c.disableHeliAttack),
            MakeToggle("gameOverOnDiscovery",   "Instant game over when any enemy spots you",                           &c.gameOverOnDiscovery),
            MakeToggle("disableGameOver",       "Disable game over entirely - never fail a mission",                    &c.disableGameOver),
            MakeToggle("disableRetry",          "Remove the retry option from the pause menu",                          &c.disableRetry),
            MakeToggle("disableOutOfBounds",    "Disable out-of-bounds area restrictions",                              &c.disableOutOfBounds),
            MakeToggle("setSubsistence",        "Force subsistence loadout (no equipment)",                             &c.setSubsistence),
            MakeToggle("clearItems",            "Start with no items or support equipment",                             &c.clearItems),
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
    });
}

static MenuNode BuildHelicopterMenu(Config& c) {
    return MakeSub("Helicopter", "Support helicopter behavior and landing zone settings", {
        MakeToggle("invincibleHeli",        "Make the support helicopter invincible",                                   &c.invincibleHeli),
        MakeEnum  ("searchLight",           "Helicopter search light override",                                         &c.searchLight, s_SearchLight, 3),
        MakeToggle("disablePullOut",        "Prevent the helicopter from pulling out under fire",                       &c.disablePullOut),
        MakeInt   ("lzWaitHeight",          "Helicopter hover height at landing zone (meters)",                         &c.lzWaitHeight, 5, 50, 5, "m"),
        MakeInt   ("doorOpenTime",          "Time doors stay open after landing (seconds)",                             &c.doorOpenTime, 0, 120, 5, "s"),
        MakeEnum  ("disableLZs",            "Disable landing zone types",                                               &c.disableLZs, s_DisableLZ, 3),
        MakeToggle("disableDescentToLZ",    "Prevent helicopter from descending to landing zones",                      &c.disableDescentToLZ),
    });
}

static MenuNode BuildBuddyMenu(Config& c) {
    return MakeSub("Buddy", "Buddy equipment and weapon selection", {
        MakeEnum  ("quietWeapon",   "Quiet's weapon selection",   &c.quietWeapon, s_QuietWpn, 6),
        MakeEnum  ("ddogEquip",     "D-Dog equipment loadout",    &c.ddogEquip, s_DDogEquip, 5),
        MakeEnum  ("dhorseEquip",   "D-Horse equipment variant",  &c.dhorseEquip, s_DHorseEquip, 4),
        MakeEnum  ("dwalkerEquip",  "D-Walker weapon loadout",    &c.dwalkerEquip, s_DWalkerEquip, 5),
    });
}

static MenuNode BuildMotherBaseMenu(Config& c) {
    return MakeSub("Mother Base", "Mother Base NPCs, patrols, assets, and behavior", {
        MakeSub("Characters", "Toggle NPC characters on Mother Base", {
            MakeToggle("mbEnableOcelot",    "Enable Ocelot NPC on Mother Base",         &c.mbEnableOcelot),
            MakeToggle("mbEnablePuppy",     "Enable puppy D-Dog on Mother Base",        &c.mbEnablePuppy),
            MakeToggle("mbShowCodeTalker",  "Show Code Talker NPC",                     &c.mbShowCodeTalker),
            MakeToggle("mbEnableBirds",     "Enable bird wildlife on Mother Base",      &c.mbEnableBirds),
            MakeToggle("mbEnableBuddies",   "Enable buddy NPCs to roam Mother Base",   &c.mbEnableBuddies),
        }),
        MakeSep(),
        MakeSub("Soldiers", "Soldier behavior and appearance on Mother Base", {
            MakeEnum  ("mbSoldierEquip",        "Soldier weapon range on Mother Base",  &c.mbSoldierEquip, s_EquipRange, 4),
            MakeInt   ("mbAdditionalSoldiers",  "Extra soldiers to spawn on MB",        &c.mbAdditionalSoldiers, 0, 50, 5),
            MakeToggle("mbPrioritizeFemale",    "Prioritize female soldier models",     &c.mbPrioritizeFemale),
            MakeToggle("mbEnableLethal",        "Allow lethal actions on Mother Base",  &c.mbEnableLethal),
            MakeToggle("mbMoraleBoosts",        "Enable morale boost salute reactions", &c.mbMoraleBoosts),
        }),
        MakeSep(),
        MakeSub("Patrols", "Helicopter and vehicle patrols", {
            MakeInt   ("supportHeliMB",     "Support helicopter patrol count (0-3)",    &c.supportHeliMB, 0, 3, 1),
            MakeInt   ("attackHeliMB",      "Attack helicopter patrol count (0-4)",     &c.attackHeliMB, 0, 4, 1),
            MakeToggle("mbEnableWalkerGears","Deploy Walker Gears on Mother Base",     &c.mbEnableWalkerGears),
        }),
        MakeSep(),
        MakeSub("Assets", "Mother Base visual assets and facilities", {
            MakeToggle("mbShowPosters",     "Display Big Boss posters on Mother Base",  &c.mbShowPosters),
            MakeToggle("mbCollectionRepop", "Regenerate plants/diamonds on Mother Base",&c.mbCollectionRepop),
            MakeToggle("mbUnlockGoalDoors", "Unlock all goal/objective doors on MB",    &c.mbUnlockGoalDoors),
        }),
    });
}

static MenuNode BuildFultonMenu(Config& c) {
    return MakeSub("Fulton", "Fulton recovery system settings", {
        MakeEnum  ("autoFulton",        "Automatically fulton extracted targets",                &c.autoFulton, s_AutoFulton, 4),
        MakeToggle("recoverCritical",   "Allow recovery of critically wounded soldiers",        &c.recoverCritical),
        MakeToggle("disableFultonAbil", "Completely disable the fulton ability",                &c.disableFultonAbility),
        MakeInt   ("fultonLevel",       "Fulton device level (1-4, higher = more reliable)",    &c.fultonLevel, 1, 4, 1),
        MakeInt   ("wormholeLevel",     "Wormhole fulton (0=disabled, 1=enabled)",               &c.wormholeLevel, 0, 1, 1),
        MakeInt   ("fultonVariation",   "Fulton animation variation range (%)",                  &c.fultonVariation, 0, 100, 5, "%"),
        MakeInt   ("fultonDyingPenalty", "Penalty reduction for fultoning dying soldiers (%)",   &c.fultonDyingPenalty, 0, 100, 10, "%"),
        MakeInt   ("fultonSleepPenalty", "Penalty reduction for fultoning sleeping soldiers (%)", &c.fultonSleepPenalty, 0, 100, 10, "%"),
    });
}

static MenuNode BuildSideOpsMenu(Config& c) {
    return MakeSub("Side Ops", "Side ops / quest management and filtering", {
        MakeInt   ("forceQuestNumber",  "Force a specific side op number (0 = none)",           &c.forceQuestNumber, 0, 157, 1),
        MakeEnum  ("questSelectMode",   "How side ops are selected for each area",              &c.questSelectMode, s_QuestSelect, 3),
        MakeEnum  ("questRepopMode",    "When cleared side ops respawn",                         &c.questRepopMode, s_QuestRepop, 2),
        MakeToggle("enableMissionQuest","Allow side ops to appear during story missions",       &c.enableMissionQuest),
        MakeToggle("questRetry",        "Enable retry option for shooting practice quests",     &c.questRetry),
    });
}

static MenuNode BuildCutscenesMenu(Config& c) {
    return MakeSub("Cutscenes", "Cutscene (demo) behavior and overrides", {
        MakeToggle("useSoldierForDemos",    "Use selected soldier model in all cutscenes",          &c.useSoldierForDemos),
        MakeToggle("forceDemoAllowAction",  "Allow player actions during cutscenes (skip, etc.)",   &c.forceDemoAllowAction),
        MakeEnum  ("demoOverrideWeather",   "Force weather during cutscenes",                       &c.demoOverrideWeather, s_Weather, 7),
        MakeInt   ("demoOverrideHour",      "Force time of day during cutscenes (-1 = default)",    &c.demoOverrideHour, -1, 23, 1),
    });
}

static MenuNode BuildEventsMenu(Config& c) {
    return MakeSub("Events", "Random free-roam and Mother Base events", {
        MakeSub("Free Roam Events", "Chance-based random events during free roam", {
            MakeInt   ("eventHunted",       "HUNTED event chance - continuous combat alert (%)",         &c.eventHunted, 0, 100, 5, "%"),
            MakeInt   ("eventCrashLand",    "CRASHLAND event chance - random start, OSP (%)",           &c.eventCrashLand, 0, 100, 5, "%"),
            MakeInt   ("eventLostComs",     "LOST COMS event chance - disabled support (%)",            &c.eventLostComs, 0, 100, 5, "%"),
        }),
        MakeSep(),
        MakeSub("MB War Games", "Mother Base combat training events", {
            MakeToggle("mbWarGames",        "Enable war game events on Mother Base",                    &c.mbWarGames),
            MakeEnum  ("mbWarGameProfile",  "War game type / faction profile",                          &c.mbWarGameProfile, s_WarGameProf, 8),
        }),
    });
}

static MenuNode BuildVisualsMenu(Config& c) {
    return MakeSub("Visuals", "ESP overlays, night vision, crosshair, and rendering", {
        MakeSub("ESP", "Extra-sensory perception overlays for enemy tracking", {
            MakeToggle("espEnabled",    "Enable ESP overlay (shows enemies through walls)",              &c.espEnabled),
            MakeToggle("espBoxes",      "Draw bounding boxes around enemies",                            &c.espBoxes),
            MakeToggle("espDistance",   "Show distance to each enemy in meters",                         &c.espDistance),
            MakeToggle("espHealthBar",  "Show health bars above enemies",                                &c.espHealthBar),
        }),
        MakeSep(),
        MakeSub("Night Vision", "Brightness amplification for dark environments", {
            MakeToggle("nightVision",       "Enable night vision (brightness boost)",                    &c.nightVision),
            MakeFloat ("nightVisionStr",    "Night vision intensity multiplier",                         &c.nightVisionStr, 1.0f, 5.0f, 0.1f, "x"),
        }),
        MakeSep(),
        MakeSub("Crosshair", "On-screen crosshair overlay", {
            MakeToggle("crosshair",         "Show crosshair overlay at screen center",                   &c.crosshair),
            MakeInt   ("crosshairSize",     "Crosshair line length in pixels",                           &c.crosshairSize, 4, 24, 1, "px"),
        }),
    });
}

static MenuNode BuildMiscMenu(Config& c) {
    return MakeSub("Misc", "Position saving, teleportation, and utility features", {
        MakeCmd   ("Save Position",     "Save current player world position to memory",         Features::Misc::SavePosition),
        MakeCmd   ("Load Position",     "Teleport back to the last saved position",             Features::Misc::LoadPosition),
        MakeSep(),
        MakeCmd   ("Open Lua Console",  "Open the Fox Engine Lua 5.1 interactive console",     []() { Config::Get().luaConsoleOpen = !Config::Get().luaConsoleOpen; }),
        MakeSep(),
        MakeToggle("showHelp",          "Show help text at bottom of menu for selected item",   &c.showHelp),
    });
}

static MenuNode BuildDebugMenu(Config& c) {
    return MakeSub("Debug", "Debug output, logging, and diagnostic tools", {
        MakeToggle("debugMode",         "Enable debug mode (verbose game state logging)",       &c.debugMode),
        MakeToggle("debugMessages",     "Show debug messages in the announce log",              &c.debugMessages),
        MakeToggle("printButtons",      "Print pressed button codes to console",                &c.printButtons),
        MakeToggle("showPosition",      "Display player XYZ coordinates on screen",             &c.showPosition),
        MakeSep(),
        MakeToggle("skipLogos",         "Skip startup logos on next launch",                    &c.skipLogos),
        MakeToggle("startOffline",      "Start the game in offline mode",                       &c.startOffline),
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
        BuildMissionMenu(c),
        BuildHelicopterMenu(c),
        BuildBuddyMenu(c),
        BuildMotherBaseMenu(c),
        BuildFultonMenu(c),
        BuildSideOpsMenu(c),
        MakeSep(),
        BuildCutscenesMenu(c),
        BuildEventsMenu(c),
        BuildVisualsMenu(c),
        BuildMiscMenu(c),
        BuildDebugMenu(c),
    });
}
