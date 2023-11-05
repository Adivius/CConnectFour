// Constants
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "Connect Four - Waiting for player..."
#define ICON_PATH_WINDOW "textures/icon.png"
#define ICON_PATH_PLAYER_ONE "textures/pl1.png"
#define ICON_PATH_PLAYER_TWO "textures/pl2.png"
#define ICON_PATH_PREVIEW "textures/preview.png"
#define FONT_PATH "fonts/IndieFlower-Regular.ttf"

#define ROW_MAX 6
#define COL_MAX 7
#define CELL_SIZE 80
#define GRID_OFFSET_X ((WINDOW_WIDTH - COL_MAX * CELL_SIZE) / 2)
#define GRID_OFFSET_Y ((WINDOW_HEIGHT - ROW_MAX * CELL_SIZE) / 2)

#define COLOR_BACKGROUND 36, 36, 36, 255
#define COLOR_GRID 117, 117, 117, 117

#define USAGE "\033[0;31mUsgae: ./cconnectfour <port> <ip>"

void update(int column);
void reset();