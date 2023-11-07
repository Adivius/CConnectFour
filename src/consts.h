#pragma once

#define WINDOW_TITLE "Connect Four - Waiting for player..."
#define WINDOW_TITLE_SERVER "Connect Four - Server"
#define WINDOW_TITLE_CLIENT "Connect Four - Client"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800

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
#define COLOR_GRID 117, 117, 117, 255

#define PACKET_REMATCH 8
#define PACKET_END 9

#define USAGE "\e[31m" "Usage: ./cconnectfour <port> <ip>" "\e[0m"

#define STATUS_ERROR 0
#define STATUS_STARTING 1
#define STATUS_RUNNING 2
#define STATUS_STOPPING 3