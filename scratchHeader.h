#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <mmsystem.h>
#include "iGraphics.h"
#define PI acos(-1.0)
#define window_width 1280
#define window_height 720
#define bottom_margin 50
#define total_bricks 105
#define BrickLength 65
#define BrickHeight 25
#define PowerUpWidth 50
#define PowerUpHeight 46
#define totalpowerups 16
#define scoreBMPwidth 35

typedef struct _Point
{
	double x, y;
} Point;
typedef struct _Line
{
	double a, b, c; // ax + by + c = 0
} Line;
typedef struct _Velocity
{
	double dx, dy;
} Velocity;
typedef struct _Color
{
	int red, green, blue;
} Color;
typedef struct _Paddle
{
	double bottom, left, dx, dy;
	double side_circle_radius;
} Paddle;
typedef struct _Brick
{
	double left, bottom, dx, dy;
	int type;		  // defines the design or also the type, brick maybe?
	Color temp_color; // this is probably temporary
	int isShown;
	int PowerUpAssociation;
} Brick;
typedef struct _Box
{
	double left, bottom, dx, dy;
} Box;
typedef struct _OverallProgress
{
	char name[200];
	int score;
	int seconds;
} OverallProgress;
typedef enum _PowerupType
{
	ExtraLife = 0,
	WidePaddle,
	FasterBall,
	ThruBall,
	SlowerBall,
	ShrinkBall,
	ShrinkPaddle,
	SuperShrinkPaddle,
	MegaBall,
	Death,
	FallingBricks
} PowerupType;
typedef enum _Page
{
	Gameplay,
	MainMenu,
	Credits,
	Play_New_Continue,
	Instructions,
	Welcome,
	EnterYourName,
	HighScores,
	Continue_Save_Menu_ExitGame
} Page;
typedef struct _Powerup
{
	int type; // which bmp, sound if caught and which activity
	// 0 extra life, 1 wide paddle, 2 faster ball speed, 3 thru ball, 4 slow ball, 5 shrinkball,  6 shrinkpaddle, 7 supershrinkpaddle, 8 megaball, 9 death, 10 fallingbricks
	int index; // with which brick it is, ranges from 0 to totalbricks-1
	int state; // 0 for not triggered yet, 1 for triggered and moving, 2 for finished(either caught or not caught)
	int isCaught;
	double left, bottom;
} Powerup;
typedef struct _Sphere
{
	Point center;
	double radius;
	short int isMoving, isActivated;
} Sphere;

double maximum(double, double);
double minimum(double, double);
double distanceOfAPointFromALine(Line *, Point *);
Line findStraightLinePassingTwoPoints(Point *, Point *);
double evaluateFunction(Line *, Point *);
int doesBallHitBrick(Brick *, Sphere *, Point *);
void iPassiveMouseMove(int, int);

void mousePassiveMoveHandlerFF(int mx, int my)
{
	iPassiveMouseMove(mx, window_height - my);
	glFlush();
}
double maximum(double a, double b)
{
	return ((a > b) ? a : b);
}
double minimum(double a, double b)
{
	return ((a < b) ? a : b);
}
Line findStraightLinePassingTwoPoints(Point *a, Point *b)
{
	Line ans;
	ans.a = a->y - b->y;
	ans.b = b->x - a->x;
	ans.c = a->x * b->y - b->x * a->y;
	return ans;
}
double evaluateFunction(Line *l, Point *p)
{
	double ans = l->a * p->x + l->b * p->y + l->c;
	return ans;
}
double distanceOfAPointFromALine(Line *l, Point *p)
{
	double distance;
	distance = evaluateFunction(l, p);
	double also = sqrt((l->a * l->a) + (l->b * l->b));
	distance = distance / also;
	if (distance < 0)
		distance *= -1;
	return distance;
}
int doesBallHitBrick(Brick *target_brick, Sphere *this_ball, Point *previous_center)
{
	Point bottom_left = {target_brick->left, target_brick->bottom};
	Point bottom_right = {target_brick->left + target_brick->dx, target_brick->bottom};
	Point top_left = {target_brick->left, target_brick->bottom + target_brick->dy};
	Point top_right = {target_brick->left + target_brick->dx, target_brick->bottom + target_brick->dy};

	Line now_line;
	double now_distance, prev_distance;
	if (target_brick->isShown == 0)
		return -1;
	// from left side
	now_line = findStraightLinePassingTwoPoints(&top_left, &bottom_left);
	now_distance = distanceOfAPointFromALine(&now_line, &(this_ball->center)); // distance from center
	prev_distance = distanceOfAPointFromALine(&now_line, previous_center);	   // previous distance from center
	if (this_ball->center.y >= bottom_left.y && this_ball->center.y <= top_left.y && now_distance <= prev_distance && now_distance <= this_ball->radius + 1)
	{
		return 1; // indicates left collision
	}

	// from right side
	now_line = findStraightLinePassingTwoPoints(&top_right, &bottom_right);
	now_distance = distanceOfAPointFromALine(&now_line, &(this_ball->center)); // distance from center
	prev_distance = distanceOfAPointFromALine(&now_line, previous_center);	   // previous distance from center
	if (this_ball->center.y >= bottom_right.y && this_ball->center.y <= top_right.y && now_distance <= prev_distance && now_distance <= this_ball->radius + 1)
	{
		return 2; // indicates right collision
	}

	// from up side
	now_line = findStraightLinePassingTwoPoints(&top_left, &top_right);
	now_distance = distanceOfAPointFromALine(&now_line, &(this_ball->center)); // distance from center
	prev_distance = distanceOfAPointFromALine(&now_line, previous_center);	   // previous distance from center
	if (this_ball->center.x >= top_left.x && this_ball->center.x <= top_right.x && now_distance <= prev_distance && now_distance <= this_ball->radius + 1)
	{
		return 3; // indicates up collision
	}

	// from bottom side
	now_line = findStraightLinePassingTwoPoints(&bottom_left, &bottom_right);
	now_distance = distanceOfAPointFromALine(&now_line, &(this_ball->center)); // distance from center
	prev_distance = distanceOfAPointFromALine(&now_line, previous_center);	   // previous distance from center
	if (this_ball->center.x >= bottom_left.x && this_ball->center.x <= bottom_right.x && now_distance <= prev_distance && now_distance <= this_ball->radius + 1)
	{
		return 4; // indicates bottom collision
	}

	return -1; // no collision
}
