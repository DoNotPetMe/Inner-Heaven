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
    int  damageMultiplier   = 100;   // 10-1000%
    bool infiniteMags       = false;
    bool noBulletDrop       = false;
    bool infiniteThrowables = false;
    int  reflexDuration     = 100;   // 10-1000%
    bool noWeaponSway       = false;
    bool perfectAccuracy    = false;
    bool explosiveBullets   = false;
    bool penetratingShots   = false;
    bool silentWeapons      = false;
    bool infiniteBattery    = false;

    // ── Player Stats ───────────────────────────────────────────────────
    int  playerHealthScale  = 100;   // 0-650%
    bool superSpeed         = false;
    float speedMultiplier   = 2.0f;
    bool customPlayerScale  = false;
    float playerScale       = 1.0f;
    bool dontSubtractHero   = false;
    bool dontAddOgre        = false;
    bool heroSubtractsOgre  = false;
    int  staminaScale       = 100;   // 0-500%
    float sprintSpeed       = 1.0f;  // 0.5-5.0x
    float crawlSpeed        = 1.0f;
    float diveDistance       = 1.0f;
    int  noiseScale         = 100;   // 0-300%
    float gravityScale      = 1.0f;  // 0.0-3.0
    bool unlimitedStamina   = false;
    float swimSpeed         = 1.0f;
    bool noBreathLimit      = false;
    int  gripStrength       = 100;   // 10-500%

    // ── Appearance ─────────────────────────────────────────────────────
    int  playerType         = 0;
    int  playerParts        = 0;
    int  playerCamo         = 0;
    int  headgear           = 0;
    int  handType           = 0;
    int  fovaFace           = 0;     // face variation index
    int  fovaBody           = 0;     // body variation
    int  fovaHair           = 0;     // hair style
    int  scarf              = 0;     // 0=NONE,1-6
    int  accessory          = 0;     // 0=NONE,1-10
    bool cigarEquipped      = false;
    int  prostheticColor    = 0;     // 0=RED,1=GOLD,2=SILVER,3=BLACK
    bool bloodStains        = false;
    int  facePaintIndex     = 0;     // 0-20
    int  avatarGender       = 0;     // 0=MALE,1=FEMALE
    int  ddFemaleUniform    = 0;     // 0=STANDARD,1=SNEAKING,2=BATTLE,3=PARASITE
    int  ddMaleUniform      = 0;
    bool hornHidden         = false;
    int  shrapnelType       = 0;     // 0=DEFAULT,1=SHORT,2=MEDIUM,3=LONG

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
    float shoulderOffsetX   = 0.0f;  // -2.0 to 2.0
    float shoulderOffsetY   = 0.0f;
    float cameraDistance     = 0.0f;  // -5.0 to 5.0
    float cameraHeight      = 0.0f;
    int  cameraShake        = 100;   // 0-200%
    bool disableAutoCenter  = false;
    bool photoMode          = false;
    float photoFilterStr    = 0.0f;
    int  photoFilter        = 0;     // 0=NONE,1=SEPIA,2=BW,3=NEGATIVE,4=POSTERIZE,5=VIVID
    float letterboxAmount   = 0.0f;  // 0.0-0.3
    bool firstPersonMode    = false;
    float scopeZoomMult     = 1.0f;  // 0.5-4.0
    float binocZoomMult     = 1.0f;
    float tiltAngle         = 0.0f;  // -45 to 45 deg

    // ── Resources ──────────────────────────────────────────────────────
    int  gmp                = 0;
    int  heroism            = 0;
    int  demonPoints        = 0;
    bool lockGMP            = false;
    bool lockHeroism        = false;
    bool lockDemonPoints    = false;
    int  resourceScaleMat   = 100;
    int  resourceScalePlant = 100;
    int  resourceScaleDia   = 100;
    int  resourceScaleContainer = 100;
    int  resourceScaleFuel  = 100;   // 10-1000%
    int  resourceScaleBio   = 100;
    int  resourceScaleMetal = 100;
    int  resourceScaleMinor = 100;
    int  mbCoinMultiplier   = 100;   // 10-1000%
    int  staffRecruitLevel  = 0;     // 0=DEFAULT,1-100
    bool autoDismissLow     = false;
    int  autoDismissThresh  = 20;    // 1-80
    bool preventVolunteers  = false;
    int  processingScale    = 100;   // 10-500%
    bool infiniteBaseDev    = false;

    // ── World ──────────────────────────────────────────────────────────
    bool slowMotion         = false;
    float timeScale         = 0.3f;
    int  clockTimeScale     = 1;
    bool overrideTime       = false;
    float timeOfDay         = 12.0f;
    bool weatherOverride    = false;
    int  weather            = 0;
    float fogDensity        = 0.0f;
    int  fogType            = 0;
    bool noEnemyAI          = false;
    bool infiniteFulton     = false;
    float windSpeed         = 0.0f;    // 0.0-30.0
    int  windDirection      = 0;       // 0-360
    float ambientLight      = 1.0f;    // 0.0-3.0
    float sunIntensity      = 1.0f;    // 0.0-5.0
    float shadowIntensity   = 1.0f;    // 0.0-2.0
    float rainWetness       = 0.0f;    // 0.0-1.0
    float dustDensity       = 0.0f;    // 0.0-1.0
    float temperatureShift  = 0.0f;    // -20 to 20
    bool  disableMinefield  = false;
    bool  disableTraps      = false;
    int   waterLevel        = 0;       // 0=DEFAULT,1=LOW,2=HIGH,3=FLOOD
    float skyboxRotation    = 0.0f;    // 0-360
    bool  noAnimalAI        = false;
    bool  disableMines      = false;
    bool  disableCameras    = false;
    bool  disableDecoys     = false;
    bool  disableRadio      = false;
    int   destructionLevel  = 0;       // 0=DEFAULT,1=LOW,2=HIGH,3=TOTAL

    // ── Enemy ──────────────────────────────────────────────────────────
    int  soldierHealthScale = 100;
    int  soldierSightScale  = 100;
    int  soldierNightSight  = 100;
    int  soldierHearingScale= 100;
    int  itemDropChance     = 50;
    int  enemyPhase         = 0;
    bool keepPhase          = false;
    int  customSoldierType  = 0;
    int  revengeMode        = 0;
    bool forceSuperReinforce= false;
    bool enableHeliReinforce= false;
    int  reinforceCountMin  = 0;
    int  reinforceCountMax  = 8;
    int  reinforceLevelMin  = 0;
    int  reinforceLevelMax  = 5;
    bool soldierAlertOnVehicle = false;
    bool enableWildCard     = false;
    bool enableLrrp         = false;
    int  soldierAccuracy    = 100;    // 0-400%
    int  soldierReaction    = 100;    // 0-400%
    int  soldierAggression  = 100;    // 0-400%
    int  soldierSpeed       = 100;    // 0-300%
    int  stunDuration       = 100;    // 10-500%
    int  sleepDuration      = 100;    // 10-500%
    int  holdupDuration     = 100;    // 10-500%
    bool disableRadioCall   = false;
    bool disableBodySearch  = false;
    bool disableInvestigate = false;
    int  commsJamDuration   = 0;     // 0=DEFAULT,1-120 seconds
    bool noAlertPropagation = false;
    int  cpCommunicationRange = 100; // 0-500%
    int  soldierFaceVar     = 0;     // 0=DEFAULT,1-20
    int  soldierBodyVar     = 0;     // 0=DEFAULT,1-10
    bool interrogationAlways = false;
    int  vehiclePatrolCount = 0;     // 0-10
    int  vehiclePatrolType  = 0;     // 0=TRUCK,1=APC,2=TANK,3=RANDOM
    bool forceSniperGuards  = false;
    bool forceHeavyArmor    = false;
    int  guardRouteLength   = 100;   // 50-300%

    // ── Skulls / Parasite ──────────────────────────────────────────────
    bool  enableSkulls      = false;
    int   skullsType        = 0;     // 0=MIST,1=CAMO,2=ARMOR,3=METALLIC
    int   skullsCount       = 4;     // 1-12
    int   skullsHealth      = 100;   // 10-1000%
    int   skullsSpeed       = 100;   // 50-500%
    int   skullsDamage      = 100;   // 10-1000%
    int   skullsAggression  = 100;   // 0-500%
    bool  skullsTeleport    = true;
    bool  skullsDisappear   = true;
    int   skullsPhase       = 0;     // 0=AUTO,1=MELEE,2=RANGED,3=SNIPE
    bool  skullsInFreeRoam  = false;
    int   skullsMistDensity = 100;   // 0-500%
    bool  skullsArmorBreak  = false;
    int   skullsRespawnTime = 0;     // 0=NEVER,1-300 seconds

    // ── Enemy Prep ─────────────────────────────────────────────────────
    int  prepSniper         = 0;
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
    int  prepFlashlight     = 0;     // 0-100%
    int  prepRadio          = 0;
    int  prepSidearm        = 0;
    int  prepGrenade        = 0;
    int  prepSmokeBomb      = 0;
    int  prepStunGrenade    = 0;
    int  prepC4             = 0;
    int  prepWeaponLevel    = 0;     // 0=DEFAULT,1-10
    int  prepArmorLevel     = 0;     // 0=DEFAULT,1-5

    // ── Mission ────────────────────────────────────────────────────────
    bool skipMissionPrep    = false;
    int  startOnFoot        = 0;
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
    int  primaryWeaponOSP   = 0;
    int  secondaryWeaponOSP = 0;
    bool clearItems         = false;
    int  missionRankOverride= 0;     // 0=DEFAULT,1=S,2=A,3=B,4=C,5=D,6=E
    bool noCheckpoints      = false;
    bool noReflex           = false;
    bool noMarking          = false;
    bool noSupplyDrop       = false;
    int  scoreMultiplier    = 100;   // 10-1000%
    bool disableBuddySupport = false;
    bool disableAirStrikes  = false;
    bool timedMission       = false;
    int  timedMissionMin    = 30;    // 1-120 minutes
    int  handAbilityLevel   = 0;     // 0=DEFAULT,1-4
    int  itemLevel          = 0;     // 0=DEFAULT,1-6
    bool disableCompanionApp = false;
    bool disableMBSupport   = false;
    bool forceNight         = false;
    bool forceDay           = false;
    bool disableSpotter     = false;
    bool disableIndicators  = false;
    bool disableSloMo       = false;

    // ── Helicopter ─────────────────────────────────────────────────────
    bool invincibleHeli     = false;
    int  searchLight        = 0;
    bool disablePullOut     = false;
    int  lzWaitHeight       = 20;
    int  doorOpenTime       = 15;
    int  disableLZs         = 0;
    bool disableDescentToLZ = false;
    int  heliSpeed          = 100;   // 50-300%
    int  heliArmor          = 100;   // 10-500%
    int  heliWeapon         = 0;     // 0=DEFAULT,1=MINIGUN,2=ROCKET,3=GATLING,4=NONE
    int  heliColor          = 0;     // 0=DEFAULT,1-12
    bool heliStealth        = false;
    bool heliAutoEvade      = false;
    float heliApproachAngle = 0.0f;  // -90 to 90
    int  heliRadioStation   = 0;     // 0=DEFAULT,1-15
    bool customHeliMusic    = false;
    int  heliMusicTrack     = 0;     // 0-45

    // ── Buddy ──────────────────────────────────────────────────────────
    int  quietWeapon        = 0;
    int  ddogEquip          = 0;
    int  dhorseEquip        = 0;
    int  dwalkerEquip       = 0;
    int  quietBehavior      = 0;     // 0=DEFAULT,1=SCOUT,2=ATTACK,3=COVER
    int  quietBondLevel     = 0;     // 0=DEFAULT,1-5
    bool quietHumming       = true;
    int  quietRadioMode     = 0;     // 0=OFF,1-32 tracks
    bool quietInvisible     = false;
    int  ddogBehavior       = 0;     // 0=DEFAULT,1=GUARD,2=SCOUT,3=ATTACK
    int  ddogBondLevel      = 0;     // 0=DEFAULT,1-5
    bool ddogBarking        = true;
    bool ddogAutoMark       = true;
    int  ddogTricks         = 0;     // 0=OFF,1=SHAKE,2=ROLL,3=SIT,4=ALL
    int  dhorseBehavior     = 0;     // 0=DEFAULT,1=FOLLOW,2=STAY,3=WANDER
    int  dhorseBondLevel    = 0;     // 0=DEFAULT,1-5
    bool dhorseDefecation   = true;
    int  dhorseSpeed        = 100;   // 50-300%
    int  dwalkerBehavior    = 0;     // 0=DEFAULT,1=FOLLOW,2=PATROL,3=GUARD
    bool dwalkerAutoAttack  = false;
    int  dwalkerAmmo        = 100;   // 0=INFINITE,1-999
    bool dwalkerInvisible   = false;

    // ── Buddy: Multi-Buddy & Command Wheel ─────────────────────────────
    bool multiBuddies       = false; // allow several buddies active at once
    bool buddyDDogActive    = true;  // which buddies the multi-buddy/wheel manages
    bool buddyQuietActive   = true;
    bool buddyDHorseActive  = true;
    bool buddyDWalkerActive = false;
    bool buddyWheel         = true;  // enable the radial command-wheel overlay
    bool buddyWheelGamepad  = true;  // open the wheel by holding Y on a controller
    bool buddyWheelKeyboard = true;  // open the wheel by holding X on the keyboard
    float buddyWheelScale   = 1.0f;  // wheel size multiplier

    // ── Mother Base ────────────────────────────────────────────────────
    bool mbEnableOcelot     = false;
    bool mbEnablePuppy      = false;
    bool mbShowCodeTalker   = false;
    bool mbEnableBirds      = false;
    int  mbSoldierEquip     = 0;
    int  supportHeliMB      = 0;
    int  attackHeliMB       = 0;
    bool mbEnableWalkerGears= false;
    bool mbCollectionRepop  = false;
    bool mbEnableLethal     = false;
    bool mbMoraleBoosts     = false;
    bool mbShowPosters      = false;
    bool mbEnableBuddies    = false;
    int  mbAdditionalSoldiers = 0;
    bool mbPrioritizeFemale = false;
    bool mbUnlockGoalDoors  = false;
    float mbOceanWaveHeight = 1.0f;  // 0.0-5.0
    float mbOceanWaveSpeed  = 1.0f;
    int  mbOceanColorR      = 30;    // 0-255
    int  mbOceanColorG      = 80;
    int  mbOceanColorB      = 120;
    float mbOceanFoam       = 0.5f;  // 0.0-1.0
    float mbOceanTransparency = 0.3f;
    float mbOceanReflection = 1.0f;
    int  mbSecurityLevel    = 0;     // 0=DEFAULT,1=LOW,2=MEDIUM,3=HIGH,4=MAX
    bool mbEnableAlarms     = true;
    bool mbEnableUAV        = false;
    int  mbNpcRouteMode     = 0;     // 0=DEFAULT,1=PATROL,2=IDLE,3=EXERCISE
    int  mbLightingMode     = 0;     // 0=DEFAULT,1=BRIGHT,2=DIM,3=OFF
    bool mbShowAnimals      = false;
    int  mbAnimalType       = 0;     // 0=GOAT,1=SHEEP,2=WOLF,3=BEAR,4=DONKEY
    bool mbShowerTimer      = false;
    int  mbShowerInterval   = 30;    // 1-999 minutes
    bool mbPlatformSelect   = false;
    int  mbActivePlatform   = 0;     // 0-6

    // ── Fulton ─────────────────────────────────────────────────────────
    int  autoFulton         = 0;
    bool recoverCritical    = false;
    bool disableFultonAbility = false;
    int  fultonLevel        = 4;
    int  wormholeLevel      = 0;
    int  fultonVariation    = 0;
    int  fultonDyingPenalty = 0;
    int  fultonSleepPenalty = 0;
    int  fultonSpeed        = 100;   // 50-500%
    bool fultonEverything   = false;
    bool fultonVehicles     = false;
    bool fultonContainers   = false;
    bool fultonGunEmplace   = false;
    int  fultonSuccessRate  = 100;   // 0-100%
    bool fultonSilent       = false;

    // ── Side Ops ───────────────────────────────────────────────────────
    int  forceQuestNumber   = 0;
    int  questSelectMode    = 0;
    int  questRepopMode     = 0;
    bool enableMissionQuest = false;
    bool questRetry         = false;
    int  maxActiveQuests    = 3;     // 1-10
    bool questEnemyScale    = false;
    int  questEnemyLevel    = 100;   // 50-300%
    bool questTimeLimits    = false;
    int  questTimeMinutes   = 30;    // 5-120

    // ── Cutscenes ──────────────────────────────────────────────────────
    bool useSoldierForDemos = false;
    bool forceDemoAllowAction = false;
    int  demoOverrideWeather = 0;
    int  demoOverrideHour   = -1;
    bool skipAllCutscenes   = false;
    float demoPlaybackSpeed = 1.0f;  // 0.1-4.0
    bool demoFreeCamera     = false;
    bool demoHideHUD        = false;
    int  demoSubtitles      = 0;     // 0=DEFAULT,1=ON,2=OFF
    bool demoLetterbox      = true;

    // ── Time Scale ─────────────────────────────────────────────────────
    int  speedCamWorldScale = 100;
    int  speedCamPlayerScale= 100;
    int  speedCamContinueTime = 10;

    // ── Visuals (Unique) ───────────────────────────────────────────────
    bool espEnabled         = false;
    bool espBoxes           = true;
    bool espDistance         = true;
    bool espHealthBar       = false;
    bool espNames           = false;
    bool espSkeleton        = false;
    bool espWeapons         = false;
    bool espAlertState      = false;
    int  espMaxRange        = 500;   // 10-2000
    int  espBoxColorR       = 255;   // 0-255
    int  espBoxColorG       = 0;
    int  espBoxColorB       = 0;
    int  espAllyColorR      = 0;
    int  espAllyColorG      = 255;
    int  espAllyColorB      = 0;
    bool espSnapLines       = false;
    int  espSnapLineOrigin  = 0;     // 0=BOTTOM,1=CENTER,2=TOP
    bool espVehicles        = false;
    bool espItems           = false;
    bool espObjectives      = false;
    bool nightVision        = false;
    float nightVisionStr    = 2.0f;
    int  nvColorMode        = 0;     // 0=GREEN,1=WHITE,2=AMBER,3=BLUE
    float nvContrast        = 1.0f;  // 0.5-3.0
    bool nvNoise            = false;
    bool crosshair          = false;
    int  crosshairSize      = 8;
    int  crosshairStyle     = 0;     // 0=CROSS,1=DOT,2=CIRCLE,3=T_SHAPE,4=CHEVRON
    int  crosshairColorR    = 0;     // 0-255
    int  crosshairColorG    = 255;
    int  crosshairColorB    = 0;
    int  crosshairGap       = 4;     // 0-20
    int  crosshairThickness = 1;     // 1-5
    float crosshairOpacity  = 1.0f;  // 0.1-1.0
    bool crosshairDynamic   = false;
    bool thermalVision      = false;
    float thermalStr        = 1.5f;
    int  thermalPalette     = 0;     // 0=IRONBOW,1=RAINBOW,2=WHITE_HOT,3=BLACK_HOT
    int  colorGrading       = 0;     // 0=NONE,1=WARM,2=COOL,3=VINTAGE,4=CINEMATIC,5=BLEACH

    // ── Position ───────────────────────────────────────────────────────
    bool hasSavedPos        = false;
    float savedX = 0, savedY = 0, savedZ = 0;
    float savedPos2X = 0, savedPos2Y = 0, savedPos2Z = 0;
    float savedPos3X = 0, savedPos3Y = 0, savedPos3Z = 0;
    bool hasSavedPos2       = false;
    bool hasSavedPos3       = false;

    // ── Events ─────────────────────────────────────────────────────────
    int  eventHunted        = 0;
    int  eventCrashLand     = 0;
    int  eventLostComs      = 0;
    bool mbWarGames         = false;
    int  mbWarGameProfile   = 0;
    int  eventAmbush        = 0;     // 0-100%
    int  eventSandstorm     = 0;     // 0-100%
    int  eventBlackout      = 0;     // 0-100%
    int  eventInvasion      = 0;     // 0-100%
    int  eventSupplyLoss    = 0;     // 0-100%
    bool eventSkulls        = false;
    int  eventSkullsChance  = 0;     // 0-100%
    int  animalEncounterRate = 100;  // 0-500%
    bool eventMeteorShower  = false;
    int  eventCustomFreq    = 100;   // 0-500%

    // ── Vehicle ────────────────────────────────────────────────────────
    bool vehicleGodMode     = false;
    int  vehicleHealthScale = 100;   // 10-500%
    float vehicleSpeedMult  = 1.0f;  // 0.5-5.0
    bool infiniteVehicleAmmo = false;
    bool noVehicleDamage    = false;
    int  vehicleHandling    = 0;     // 0=DEFAULT,1=ARCADE,2=REALISTIC,3=DRIFT
    bool vehicleNitro       = false;
    float vehicleNitroMult  = 2.0f;  // 1.5-5.0
    bool vehicleAutoRepair  = false;
    int  vehicleSpawnType   = 0;     // 0=NONE,1=TRUCK,2=JEEP,3=APC,4=TANK,5=WALKER
    int  vehicleColorIndex  = 0;     // 0=DEFAULT,1-15
    bool vehicleInvisible   = false;
    bool vehicleHorn        = true;
    int  vehicleRadio       = 0;     // 0=OFF,1-15

    // ── Sound / Music ──────────────────────────────────────────────────
    int  bgmVolume          = 100;   // 0-100
    int  sfxVolume          = 100;   // 0-100
    int  voiceVolume        = 100;   // 0-100
    int  ambientVolume      = 100;   // 0-100
    int  radioVolume        = 100;   // 0-100
    bool muteAllAudio       = false;
    bool muteBGM            = false;
    bool muteSFX            = false;
    bool muteVoice          = false;
    int  cassetteTrack      = 0;     // 0=NONE,1-60
    bool cassetteLoop       = false;
    bool cassetteAutoplay   = false;
    int  heliMusicOverride  = 0;     // 0=DEFAULT,1-45
    int  alertBGM           = 0;     // 0=DEFAULT,1=CUSTOM1,2=CUSTOM2,3=SILENT
    bool disableAlertSound  = false;
    bool disableReflex      = false;

    // ── Progression / Unlocks ──────────────────────────────────────────
    bool unlockAllWeapons   = false;
    bool unlockAllItems     = false;
    bool unlockAllOutfits   = false;
    bool unlockAllBuddyGear = false;
    bool unlockAllBlueprints= false;
    bool unlockAllTapes     = false;
    bool unlockAllMissions  = false;
    bool unlockAllSideOps   = false;
    int  rdLevel            = 0;     // 0=DEFAULT,1-99
    int  supportLevel       = 0;     // 0=DEFAULT,1-99
    int  intelLevel         = 0;     // 0=DEFAULT,1-99
    int  medicalLevel       = 0;     // 0=DEFAULT,1-99
    int  baseDevLevel       = 0;     // 0=DEFAULT,1-99
    int  combatLevel        = 0;     // 0=DEFAULT,1-99
    int  securityLevel      = 0;     // 0=DEFAULT,1-99
    bool maxAllPlatforms    = false;
    bool maxAllTeams        = false;
    int  rankOverride       = 0;     // 0=DEFAULT,1=S++,2=S+,3=S,4=A++,5=A+,6=A

    // ── Quick Presets ──────────────────────────────────────────────────
    // (no persistent state - these are commands that set multiple values)

    // ── Wave Survival (Inner Heaven Gamemode) ──────────────────────────
    int  waveArena          = 0;     // 0=AFGHAN_OUTPOST..5=CURRENT_LOCATION (theme)
    int  waveStartEnemies   = 6;     // enemies in wave 1
    int  waveEnemyIncrement = 3;     // additional enemies each subsequent wave
    int  waveMaxEnemies     = 24;    // hard cap on simultaneous enemies
    int  waveMaxWaves       = 10;    // 0 = endless
    int  waveDifficulty     = 1;     // 0=RECRUIT,1=NORMAL,2=HARD,3=INSANE
    int  waveSpawnRadius    = 45;    // metres - ring the enemies spawn on
    int  waveReinforceDelay = 5;     // seconds between a wave clearing and the next
    bool waveNeutralUntilSpotted = true; // Blacklist-style: spawns are passive until detected
    bool waveAutoStealthBonus = true;    // award bonus score for clearing a wave undetected
    bool waveLethalPenalty  = false;     // lethal takedowns reduce score (encourage non-lethal)
    bool waveShowHud        = true;      // draw the top-left enemy counter / status HUD
    bool waveForceWeather   = false;     // apply the arena's themed weather on start
    bool waveModeActive     = false;     // runtime: a session is currently running
    bool waveModeActiveFailed = false;   // runtime: last Start() couldn't anchor an arena

    // ── Debug ──────────────────────────────────────────────────────────
    bool debugMode          = false;
    bool debugMessages      = false;
    bool printButtons       = false;
    bool showPosition       = false;
    bool skipLogos          = false;
    bool startOffline       = false;
    bool showFPS            = false;
    bool showFrameTime      = false;
    bool logToFile          = false;
    int  logLevel           = 0;     // 0=ERROR,1=WARN,2=INFO,3=DEBUG,4=VERBOSE
    bool wireframeMode      = false;
    bool showColliders      = false;
    bool showNavMesh        = false;
    bool showRoutes         = false;
    bool showCPZones        = false;
    bool dumpMemory         = false;

    // ── System ─────────────────────────────────────────────────────────
    bool luaConsoleOpen     = false;
    bool showHelp           = true;

    // Restore every setting to its default value.
    void Reset() { *this = Config{}; }

private:
    Config() = default;
};
