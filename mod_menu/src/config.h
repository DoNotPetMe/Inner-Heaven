#pragma once

struct Config {
    static Config& Get() {
        static Config inst;
        return inst;
    }

    // ── Player Combat ──────────────────────────────────────────────────
    bool godMode            = false;
    bool infiniteAmmo       = false;
    bool noReload           = false;
    bool infiniteSuppressor = false;
    bool noRecoil           = false;
    bool oneHitKill         = false;
    bool stealthMode        = false;
    bool infiniteReflex     = false;
    bool noFallDamage       = false;
    bool rapidFire          = false;

    // ── Player Stats ───────────────────────────────────────────────────
    int  playerHealthScale  = 100;   // 0-650%
    bool superSpeed         = false;
    float speedMultiplier   = 2.0f;
    bool customPlayerScale  = false;
    float playerScale       = 1.0f;
    bool dontSubtractHero   = false;
    bool dontAddOgre        = false;
    bool heroSubtractsOgre  = false;

    // ── Appearance ─────────────────────────────────────────────────────
    int  playerType         = 0;     // 0=SNAKE,1=DD_MALE,2=DD_FEMALE,3=AVATAR,4=LIQUID,5=OCELOT,6=QUIET
    int  playerParts        = 0;     // suit/outfit index
    int  playerCamo         = 0;     // camo pattern index
    int  headgear           = 0;     // face equipment
    int  handType           = 0;     // 0=NONE..7=SILVER

    // ── Camera ─────────────────────────────────────────────────────────
    bool freeCamera         = false;
    float camSpeed          = 5.0f;
    bool customFOV          = false;
    float fovValue          = 75.0f;
    float fovNormal         = 75.0f;
    float fovAiming         = 40.0f;
    float fovHiding         = 55.0f;
    float fovCQC            = 60.0f;
    float focalLength       = 21.0f;
    float aperture          = 1.2f;
    float focusDistance      = 10.0f;

    // ── Resources ──────────────────────────────────────────────────────
    int  gmp                = 0;
    int  heroism            = 0;
    int  demonPoints        = 0;
    bool lockGMP            = false;
    bool lockHeroism        = false;
    bool lockDemonPoints    = false;
    int  resourceScaleMat   = 100;   // 10-1000%
    int  resourceScalePlant = 100;
    int  resourceScaleDia   = 100;
    int  resourceScaleContainer = 100;

    // ── World ──────────────────────────────────────────────────────────
    bool slowMotion         = false;
    float timeScale         = 0.3f;
    int  clockTimeScale     = 1;     // 1-10000
    bool overrideTime       = false;
    float timeOfDay         = 12.0f;
    bool weatherOverride    = false;
    int  weather            = 0;     // 0=NONE,1=SUNNY,2=CLOUDY,3=RAINY,4=SANDSTORM,5=FOGGY,6=POURING
    float fogDensity        = 0.0f;
    int  fogType            = 0;     // 0=NORMAL,1=PARASITE,2=EERIE
    bool noEnemyAI          = false;
    bool infiniteFulton     = false;

    // ── Enemy ──────────────────────────────────────────────────────────
    int  soldierHealthScale = 100;   // 0-900%
    int  soldierSightScale  = 100;   // 0-400%
    int  soldierNightSight  = 100;   // 0-400%
    int  soldierHearingScale= 100;   // 0-400%
    int  itemDropChance     = 50;    // 0-100%
    int  enemyPhase         = 0;     // 0=DEFAULT,1=SNEAK,2=CAUTION,3=EVASION,4=ALERT
    bool keepPhase          = false;
    int  customSoldierType  = 0;
    int  revengeMode        = 0;     // 0=DEFAULT,1=FREE,2=MISSION,3=ALL
    bool forceSuperReinforce= false;
    bool enableHeliReinforce= false;
    int  reinforceCountMin  = 0;
    int  reinforceCountMax  = 8;
    int  reinforceLevelMin  = 0;
    int  reinforceLevelMax  = 5;
    bool soldierAlertOnVehicle = false;
    bool enableWildCard     = false;
    bool enableLrrp         = false;

    // ── Enemy Prep ─────────────────────────────────────────────────────
    int  prepSniper         = 0;     // 0-100%
    int  prepMissile        = 0;
    int  prepMG             = 0;
    int  prepShotgun        = 0;
    int  prepSMG            = 0;
    int  prepArmor          = 0;
    int  prepShield         = 0;
    int  prepHelmet         = 0;
    int  prepNVG            = 0;
    int  prepGasMask        = 0;
    int  prepDecoy          = 0;
    int  prepMine           = 0;
    int  prepCamera         = 0;

    // ── Mission ────────────────────────────────────────────────────────
    bool skipMissionPrep    = false;
    int  startOnFoot        = 0;     // 0=OFF,1=FREE,2=MISSION,3=ALL
    bool disableHeliAttack  = false;
    bool gameOverOnDiscovery= false;
    bool disableGameOver    = false;
    bool disableRetry       = false;
    bool disableOutOfBounds = false;
    bool disableHeadMarkers = false;
    bool disableWorldMarkers= false;
    bool disableXrayMarkers = false;
    bool setSubsistence     = false;
    bool disableSupportMenu = false;
    bool disableBuddyMenu   = false;
    bool disableAttackMenu  = false;
    bool disableFultonMenu  = false;
    int  primaryWeaponOSP   = 0;     // 0=DEFAULT
    int  secondaryWeaponOSP = 0;
    bool clearItems         = false;

    // ── Helicopter ─────────────────────────────────────────────────────
    bool invincibleHeli     = false;
    int  searchLight        = 0;     // 0=DEFAULT,1=OFF,2=ON
    bool disablePullOut     = false;
    int  lzWaitHeight       = 20;    // 5-50
    int  doorOpenTime       = 15;    // 0-120
    int  disableLZs         = 0;     // 0=OFF,1=ASSAULT,2=REGULAR
    bool disableDescentToLZ = false;

    // ── Buddy ──────────────────────────────────────────────────────────
    int  quietWeapon        = 0;
    int  ddogEquip          = 0;
    int  dhorseEquip        = 0;
    int  dwalkerEquip       = 0;

    // ── Mother Base ────────────────────────────────────────────────────
    bool mbEnableOcelot     = false;
    bool mbEnablePuppy      = false;
    bool mbShowCodeTalker   = false;
    bool mbEnableBirds      = false;
    int  mbSoldierEquip     = 0;     // 0=SHORT,1=MEDIUM,2=LONG,3=RANDOM
    int  supportHeliMB      = 0;     // 0-3
    int  attackHeliMB       = 0;     // 0-4
    bool mbEnableWalkerGears= false;
    bool mbCollectionRepop  = false;
    bool mbEnableLethal     = false;
    bool mbMoraleBoosts     = false;
    bool mbShowPosters      = false;
    bool mbEnableBuddies    = false;
    int  mbAdditionalSoldiers = 0;
    bool mbPrioritizeFemale = false;
    bool mbUnlockGoalDoors  = false;

    // ── Fulton ─────────────────────────────────────────────────────────
    int  autoFulton         = 0;     // 0=OFF,1=FREE,2=MISSION,3=ALL
    bool recoverCritical    = false;
    bool disableFultonAbility = false;
    int  fultonLevel        = 4;     // 1-4
    int  wormholeLevel      = 0;     // 0=DISABLE,1=ENABLE
    int  fultonVariation    = 0;     // 0-100%
    int  fultonDyingPenalty = 0;
    int  fultonSleepPenalty = 0;

    // ── Side Ops ───────────────────────────────────────────────────────
    int  forceQuestNumber   = 0;     // 0-157
    int  questSelectMode    = 0;     // 0=FIRST,1=RANDOM,2=ADDON
    int  questRepopMode     = 0;     // 0=NONE_LEFT,1=ALWAYS
    bool enableMissionQuest = false;
    bool questRetry         = false;

    // ── Cutscenes ──────────────────────────────────────────────────────
    bool useSoldierForDemos = false;
    bool forceDemoAllowAction = false;
    int  demoOverrideWeather = 0;    // 0=NONE,1-6=weather types
    int  demoOverrideHour   = -1;    // -1=default, 0-23

    // ── Time Scale ─────────────────────────────────────────────────────
    int  speedCamWorldScale = 100;   // 0-10000
    int  speedCamPlayerScale= 100;   // 0-10000
    int  speedCamContinueTime = 10;  // 0-1000 seconds

    // ── Visuals (Unique) ───────────────────────────────────────────────
    bool espEnabled         = false;
    bool espBoxes           = true;
    bool espDistance         = true;
    bool espHealthBar       = false;
    bool nightVision        = false;
    float nightVisionStr    = 2.0f;
    bool crosshair          = false;
    int  crosshairSize      = 8;

    // ── Position ───────────────────────────────────────────────────────
    bool hasSavedPos        = false;
    float savedX = 0, savedY = 0, savedZ = 0;

    // ── Events ─────────────────────────────────────────────────────────
    int  eventHunted        = 0;     // 0-100% chance
    int  eventCrashLand     = 0;
    int  eventLostComs      = 0;
    bool mbWarGames         = false;
    int  mbWarGameProfile   = 0;

    // ── Debug ──────────────────────────────────────────────────────────
    bool debugMode          = false;
    bool debugMessages      = false;
    bool printButtons       = false;
    bool showPosition       = false;
    bool skipLogos          = false;
    bool startOffline       = false;

    // ── System ─────────────────────────────────────────────────────────
    bool luaConsoleOpen     = false;
    bool showHelp           = true;

private:
    Config() = default;
};
