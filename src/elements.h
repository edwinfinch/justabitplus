#define SETTINGS_KEY 0

Window *window;
Layer *display_layer;
TextLayer *update_at_a_glance;
InverterLayer *theme;

AppTimer *glanceTimer;
int holdUpSentence;
bool holdUpVibrate;
int holdUpVibrateNum;
int holdUpAnimationLength, intro, glanceTimerCalled;
bool holdUpFullNotify;
bool currentlyGlancing = 0;
bool booted = 1;
int animationNumber;
bool bootedTime;
int versionChecked = 0;
int currentAppVer = 5;

enum {
  	THEME_KEY = 0x0,
	BTDISALERT_KEY = 0x1,
	BTREALERT_KEY = 0x2,
	SHOWSEC_KEY = 0x3,
	WATCHAPPVER_KEY = 0x4,
};

typedef struct persist{
	bool theme;
	uint8_t btReAlert;
	uint8_t btDisAlert;
	bool showSec;
	uint8_t rows;
}__attribute__((__packed__)) persist;

persist settings = {
	.theme = 0,
	.btReAlert = 1,
	.btDisAlert = 1,
	.showSec = 1,
	.rows = 6,
};

#define CIRCLE_PADDING 14 - CIRCLE_RADIUS
#define CELL_SIZE (2 * (CIRCLE_RADIUS + CIRCLE_PADDING))
#define SIDE_PADDING (144 - (4 * CELL_SIZE))/2

#define CELLS_PER_ROW 4
#define CELLS_PER_COLUMN 6

#define HOURS_FIRST_DIGIT_ROW 0
#define HOURS_SECOND_DIGIT_ROW 1
#define MINUTES_FIRST_DIGIT_ROW 2
#define MINUTES_SECOND_DIGIT_ROW 3
#define SECONDS_FIRST_DIGIT_ROW 4
#define SECONDS_SECOND_DIGIT_ROW 5

#define DEFAULT_MAX_COLS 4
#define HOURS_FIRST_DIGIT_MAX_COLS 2
#define MINUTES_FIRST_DIGIT_MAX_COLS 3
#define SECONDS_FIRST_DIGIT_MAX_COLS 3