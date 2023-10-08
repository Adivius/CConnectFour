

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 75
#define PADDLE_OFFSET 20

#define BALL_HEIGHT 15
#define BALL_WIDTH 15

#define START_SPEED 200
#define FACTOR_SPEED 20.0f
#define FACTOR_DIRECTION 120

#define randNum(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))
#define randBool randNum(0, 1)