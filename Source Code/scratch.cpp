#include "scratchHeader.h"

Sphere InitialBall, Ball1;
Point previous_center_1;
Paddle CurrentPaddle, InitialPaddle;
Velocity InitialVelocity, Velocity1, Velocity2, Velocity3;
Powerup AllPowerups[totalpowerups];
Brick AllBricks[total_bricks];
int score = 0;
int life = 3;
int bricks = 0;
int isThruBall = 0;
int AreBricksFalling = 0;
char shownTime[25] ;
int seconds = 0;
int minutes = 0;
char shownScore[200];
int shouldRestart = 0;
int isSoundOn = 1;
double speed_change = 1.0025;
double InitialSpeedChange = 1.0025;
int isPaused = 0;
double reference_x, reference_y;//for setting the bricks
Page CurrentPage = Welcome;//used for toggling between different pages
int isTextBoxActivated = 0;//name entering box
char EnteredName[200];
char PlayerName[200];
OverallProgress ThisPlayer;//name, score, time everything in it
char shownStrings[10][300];//the to-be-shown High Scores
int len = 0;//for displaying the name that is being entered
int haveHighScoresBeenExtracted = 0;//for reading the high scores from the file
OverallProgress All[10];//used for showing the high scores

void InitializePaddle(void);
void drawPaddle(void);
void InitializeBall(void);
void drawBall(void);
void InitializeBricks(void);
void drawBricks(void);
void BallPositionChange(void);
void reStart(void);
void BallSpeedChange(void);
void InitializePowerups(void);
void drawPowerups(void);
void ChangePowerupPosition(void);
void ApplyPowerups(Powerup* this_powerup);
void HandlePowerups(void);
void FallBricks(void);
void storeGameStates(void);
void loadSavedItems(void);
void scoreToBMP(void);
void changeTime(void);
void updateHighScores(void);
void showHighScores(void);
void InitializeOtherThings(void);

void iDraw()
{
	iClear();
    static int f = 1;
    if (f){
        glutPassiveMotionFunc(mousePassiveMoveHandlerFF);
        f = 0;
    }
    if (CurrentPage == Gameplay){
        iShowBMP(0, 0, "Background Photos\\bg1.bmp");
        drawPaddle();
        drawBall();
        drawBricks();
        drawPowerups();
        sprintf(shownTime, "%d:%d", minutes, seconds);
        int whereiscolon, length = strlen(shownTime);
        for (int i=0; i<length; i++){
            if (shownTime[i] == ':'){
                whereiscolon = i;
                break;
            }
        }
        if (length - whereiscolon == 2){
            shownTime[length] = shownTime[length-1];
            shownTime[length+1] = '\0';
            shownTime[length - 1] = '0';
        }
        iSetColor(255, 0, 0);
        iText(630, 700, shownTime, GLUT_BITMAP_TIMES_ROMAN_24);
        scoreToBMP();
        for (int i=0; i<life; i++){
            iShowBMP2(100 + i*45, 650, "pictures\\life.bmp", 0xffffff);
        }
        if (isPaused) iShowBMP2(400, 360, "Background Photos\\pause thing.bmp", 0x000000);
    }
    else if (CurrentPage == Welcome){
        iShowBMP(0, 0, "Background Photos\\welcome.bmp");
    }
    else if (CurrentPage == MainMenu){
        iShowBMP(0, 0, "Background Photos\\main-menu.bmp");
    }
    else if (CurrentPage == EnterYourName){
        iShowBMP(0, 0, "Background Photos\\Enter Name.bmp");
        if (isTextBoxActivated){
            iSetColor(255, 255, 255);
            iRectangle(151, 401, 998, 98);
            iRectangle(152, 402, 996, 96);
            iRectangle(150, 400, 1000, 100);
            iRectangle(149, 399, 1002, 102);
            iRectangle(148, 398, 1004, 104);
        }
        else{
            iSetColor(255, 255, 255);
            iRectangle(150, 400, 1000, 100);
        }
        iText(150 + (1000 - strlen(EnteredName)*15)/2, 455, EnteredName, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else if (CurrentPage == HighScores){
        iShowBMP(0, 0, "Background Photos\\highscore.bmp");
        iSetColor(0, 255, 255);
        showHighScores();
    }
    else if (CurrentPage == Credits){
        iShowBMP(0, 0, "Background Photos\\game_credits.bmp");
    }
    else if (CurrentPage == Instructions){
        iShowBMP(0, 0, "Background Photos\\instruction.bmp");
    }
    else if (CurrentPage == Play_New_Continue){
        iShowBMP(0, 0, "Background Photos\\new_continue.bmp");
    }
    else if (CurrentPage == Continue_Save_Menu_ExitGame){
        iShowBMP(0, 0, "Background Photos\\gameplay_escape.bmp");
    }
	return;
}
void iMouseMove(int mx, int my)
{
	return;
}
void iMouse(int button, int state, int mx, int my)
{
    //if (state == GLUT_DOWN) printf("%d %d\n", mx, my);
	if (CurrentPage == Gameplay){
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	    {
            if (!Ball1.isMoving) Ball1.isMoving = 1;
	    }
    }
    else if (CurrentPage == MainMenu){
        if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	    {
            if (mx >= 586 && mx <= 758 && my >= 552 && my <= 597){
                CurrentPage = Play_New_Continue;
            }
            else if (mx >= 431 && mx <= 912 && my >= 477 && my <= 526){
                isSoundOn = !isSoundOn;
                if (isSoundOn){
                    PlaySound("music\\Seasons converted.wav", NULL, SND_ASYNC);
                }
                else PlaySound("music\\completesilence.wav", NULL, SND_ASYNC);
            }
            else if (mx >= 440 && mx <= 905 && my >= 404 && my <= 452){
                CurrentPage = Instructions;
            }
            else if (mx >= 456 && mx <= 885 && my >= 333 && my <= 380){
                CurrentPage = HighScores;
            }
            else if (mx >= 532 && mx <= 811 && my >= 260 && my <= 310){
                CurrentPage = Credits;
            }
            else if (mx >= 487 && mx <= 854 && my >= 187 && my <= 238){
                exit(0);
            }
	    }
    }
    else if (CurrentPage == EnterYourName){
        if (mx >= 150 && mx <= 150 + 1000 && my >= 400 && my <= 400 + 125) isTextBoxActivated = 1;
        else isTextBoxActivated = 0;
    }
    else if (CurrentPage == Play_New_Continue){
        if (mx >= 366 && mx <= 954 && my >= 441 && my <= 492){
            InitializeOtherThings();
            InitializePaddle();
            InitializeBall();
            InitializeBricks();
            InitializePowerups();
            CurrentPage = Gameplay;
        }
        else if (mx >= 327 && mx <= 993 && my >= 297 && my <= 350){
            loadSavedItems();
            CurrentPage = Gameplay;
        }
    }
    else if (CurrentPage == Continue_Save_Menu_ExitGame){
        if (mx >= 442 && mx <= 820 && my >= 507 && my <= 559){
                isPaused = 0;
                CurrentPage = Gameplay;
            }
            else if (mx >= 422 && mx <= 840 && my >= 396 && my <= 446){
                CurrentPage = MainMenu;
            }
            else if (mx >= 450 && mx <= 814 && my >= 284 && my <= 332){
                storeGameStates();
            }
            else if (mx >= 452 && mx <= 815 && my >= 172 && my <= 222){
                exit(0);
            }
    }
    return;
}
void iPassiveMouseMove(int mx, int my)
{
    if (CurrentPage == Gameplay && !isPaused){
        CurrentPaddle.left = mx - CurrentPaddle.dx/2 - CurrentPaddle.side_circle_radius;
        if (CurrentPaddle.left < CurrentPaddle.side_circle_radius) CurrentPaddle.left = CurrentPaddle.side_circle_radius;
        if (CurrentPaddle.left > window_width - CurrentPaddle.dx - CurrentPaddle.side_circle_radius) CurrentPaddle.left = window_width - CurrentPaddle.dx - CurrentPaddle.side_circle_radius;
        if (Ball1.isActivated && Ball1.isMoving == 0){
            Ball1.center.x = CurrentPaddle.left + CurrentPaddle.dx / 2;
        }
    }
    return;
}
void iKeyboard(unsigned char key)
{
    if (CurrentPage == Welcome){
        if (key == ' '){
            CurrentPage = MainMenu;
        }
    }
    if (CurrentPage != EnterYourName){
        if (key == 'o' || key == 'O'){
            isSoundOn = !isSoundOn;
            if (CurrentPage == MainMenu || CurrentPage == Instructions || CurrentPage == HighScores || CurrentPage == Credits || CurrentPage == Play_New_Continue){
                if (isSoundOn){
                    PlaySound("music\\Seasons converted.wav", NULL, SND_ASYNC);
                }
                else PlaySound("music\\completesilence.wav", NULL, SND_ASYNC);
            }
            return;
        }
    }
    if (CurrentPage == Continue_Save_Menu_ExitGame){
        if (key == 27){
            isPaused = 0;
            CurrentPage = Gameplay;
        }
        return;
    }
    if (CurrentPage == Instructions || CurrentPage == HighScores || CurrentPage == Credits || CurrentPage == Play_New_Continue){
        if (key == 27 || key == ' '){
            CurrentPage = MainMenu;
            return;
        }
    }
    if (CurrentPage == EnterYourName){
        if (isTextBoxActivated){
            if (key == '\r'){
                strcpy(PlayerName, EnteredName);
                strcpy(EnteredName, "");
                len = 0;
                strcpy(ThisPlayer.name, PlayerName);
                updateHighScores();
                haveHighScoresBeenExtracted = 0;
                isTextBoxActivated = 0;
                CurrentPage = HighScores;
            }
            else if (key == '\b'){
                if (len > 0){
                    len--;
                }
            }
            else{
                if (len >= 50) return;
                EnteredName[len] = key;
                len++;
            }
            EnteredName[len] = '\0';
        }
    }
    else if (CurrentPage == Gameplay){
        switch (key)
        {
        case 27://ESC key
            isPaused = 1;
            CurrentPage = Continue_Save_Menu_ExitGame;
            break;
        case ' ':
            if (!Ball1.isMoving) Ball1.isMoving = 1;
            break;
        case 's':
        case 'S':
            storeGameStates();
            break;
        case 'r':
        case 'R':
            loadSavedItems();
            break;
        case 'p':
        case 'P':
            isPaused = !isPaused;
            break;
        default:
            break;
        }
    }
    return;
}
void iSpecialKeyboard(unsigned char key)
{
    if (key == GLUT_KEY_END){
        exit(0);
    }
    if (CurrentPage == Gameplay && !isPaused){
        switch (key)
        {
        case GLUT_KEY_LEFT:
            CurrentPaddle.left -= 30;
            if (CurrentPaddle.left - CurrentPaddle.side_circle_radius < 0) CurrentPaddle.left = CurrentPaddle.side_circle_radius;
            break;
        case GLUT_KEY_RIGHT:
            CurrentPaddle.left += 30;
            if (CurrentPaddle.left + CurrentPaddle.dx + CurrentPaddle.side_circle_radius > window_width) CurrentPaddle.left = window_width - CurrentPaddle.side_circle_radius - CurrentPaddle.dx;
            break;
        default:
            break;
        }
    }
    return;
}

int main()
{
    InitializePaddle();
    InitializeBall();
    InitializeBricks();
    InitializePowerups();
    InitializeOtherThings();
    iSetTimer(5, BallPositionChange);
    iSetTimer(5000, BallSpeedChange);
    iSetTimer(20, ChangePowerupPosition);
    iSetTimer(1000, changeTime);
    if (isSoundOn) PlaySound("music\\Seasons converted.wav", NULL, SND_ASYNC);
    iInitialize(window_width, window_height, "DxBall");
	return 0;
}

void changeTime()
{
    if (isPaused) return;
    seconds++;
    if (seconds == 60){
        minutes++;
        seconds = 0;
    }
    return;
}
void InitializePaddle()
{
    InitialPaddle.left = window_width/2;
    InitialPaddle.bottom = bottom_margin;
    InitialPaddle.dx = 90;
    InitialPaddle.dy = 15;
    InitialPaddle.side_circle_radius = 5;
    CurrentPaddle = InitialPaddle;
    return;
}
void drawPaddle()
{
    iSetColor(255, 255, 255);
    iFilledRectangle(CurrentPaddle.left, CurrentPaddle.bottom, CurrentPaddle.dx, CurrentPaddle.dy);
    iFilledCircle(CurrentPaddle.left, CurrentPaddle.bottom+CurrentPaddle.dy / 2, CurrentPaddle.side_circle_radius);
    iFilledCircle(CurrentPaddle.left+CurrentPaddle.dx, CurrentPaddle.bottom+CurrentPaddle.dy / 2, CurrentPaddle.side_circle_radius, 1000);
    return;
}
void InitializeBall()
{
    InitialBall.radius = 10;
    InitialBall.center.x = CurrentPaddle.left + CurrentPaddle.dx/2;
    InitialBall.center.y = CurrentPaddle.bottom + CurrentPaddle.dy + InitialBall.radius;
    InitialBall.isActivated = 1;
    InitialBall.isMoving = 0;
    Ball1 = InitialBall;
    previous_center_1 = InitialBall.center;
    InitialVelocity.dx = 0;
    InitialVelocity.dy = 7;
    Velocity1 = InitialVelocity;
    Velocity2 = InitialVelocity;
    Velocity3 = InitialVelocity;
    return;
}
void drawBall()
{
    if (Ball1.isActivated){
        if (!isThruBall) iSetColor(120, 120, 120);
        else iSetColor(37, 112, 110);
        iFilledCircle(Ball1.center.x, Ball1.center.y, Ball1.radius);
    }
    return;
}
void BallPositionChange()
{
    if (CurrentPage != Gameplay || isPaused) return;
    if (Ball1.isActivated){
        if (!Ball1.isMoving) return;
        Ball1.center.x += Velocity1.dx;
        Ball1.center.y += Velocity1.dy;
        int doesHit;
        //brick collision checking
        if (Ball1.center.x + Ball1.radius >= reference_x && Ball1.center.y - Ball1.radius <= reference_y){
            for (int i=0; i<total_bricks; i++){
                Brick* now = &(AllBricks[i]);
                doesHit = doesBallHitBrick(now, &Ball1, &previous_center_1);
                if (doesHit == -1) continue;
                else if (doesHit == 1 || doesHit == 2){
                    Velocity1.dx *= -1;
                    if (isThruBall) Velocity1.dx *= -1;
                }
                else if (doesHit == 3 || doesHit == 4){
                    Velocity1.dy *= -1;
                    if (isThruBall) Velocity1.dy *= -1;
                }
                //if no powerup is caught
                if (now->PowerUpAssociation == -1 && isSoundOn) PlaySound("music\\hitnormalbrick.wav", NULL, SND_ASYNC);
                else{
                    if (isSoundOn) PlaySound("music\\withpowerup.wav", NULL, SND_ASYNC);
                    AllPowerups[now->PowerUpAssociation].state = 1;
                }
                if (1){
                    now->isShown = 0;
                    score += ((life + 1) * 10);
                    bricks--;
                }
                if (bricks == 0){
                    ThisPlayer.score = score;
                    ThisPlayer.seconds = minutes*60 + seconds;
                    CurrentPage = EnterYourName;
                    if (isSoundOn) PlaySound("music\\over.wav", NULL, SND_ASYNC);
                }
                break;
            }
        }
        //paddle collision
        if (!shouldRestart && Ball1.center.y - Ball1.radius <= CurrentPaddle.bottom + CurrentPaddle.dy){
            if (Ball1.center.x + Ball1.radius >= CurrentPaddle.left - CurrentPaddle.side_circle_radius && Ball1.center.x - Ball1.radius <= CurrentPaddle.left + CurrentPaddle.dx + CurrentPaddle.side_circle_radius){
                    Velocity1.dy = -Velocity1.dy;
                    Velocity1.dx = ((Ball1.center.x - (CurrentPaddle.left + CurrentPaddle.dx / 2 )) / (CurrentPaddle.dx/2 + CurrentPaddle.side_circle_radius)) * Velocity1.dy;
                    if (AreBricksFalling) FallBricks();
                    if (Ball1.radius < 15.9 && isSoundOn && !AreBricksFalling) PlaySound("music\\dropnormal.wav", NULL, SND_ASYNC);
                    else{
                        if (isSoundOn && !AreBricksFalling) PlaySound("music\\largeballdrop.wav", NULL, SND_ASYNC);
                    }
            }
            else shouldRestart = 1;
        }
        //left or right wall
        if (Ball1.center.x >= window_width - Ball1.radius || Ball1.center.x <= Ball1.radius){
                Velocity1.dx *= -1;
                if (isSoundOn) PlaySound("music\\wallhit.wav", NULL, SND_ASYNC);
        }
        //top wall
        if (Ball1.center.y >= window_height - Ball1.radius){
                Velocity1.dy *= -1;
                if (isSoundOn) PlaySound("music\\wallhit.wav", NULL, SND_ASYNC);
        }
        HandlePowerups();
        if (Ball1.center.x >= window_width - Ball1.radius) Ball1.center.x -= (Ball1.center.x - window_width + Ball1.radius);
        if (Ball1.center.x <= Ball1.radius) Ball1.center.x += (Ball1.radius - Ball1.center.x);
        if (Ball1.center.y >= window_height - Ball1.radius) Ball1.center.y -= (Ball1.center.y - window_height + Ball1.radius);
        if (Ball1.center.y <= bottom_margin + Ball1.radius + CurrentPaddle.dy) {
            if (!shouldRestart) Ball1.center.y += (bottom_margin + Ball1.radius + CurrentPaddle.dy - Ball1.center.y);
        }
        previous_center_1 = Ball1.center;
        if (Ball1.center.y + Ball1.radius <= 0){
            reStart();
            if (isSoundOn) PlaySound("music\\balldrop.wav", NULL, SND_ASYNC);
        }
    }
    return;
}
void InitializeBricks(void)
{
    reference_x = 50;
    reference_y = window_height - 100;
    bricks = 0;
    for (int i=0; i<total_bricks; i++){
		int current_index = i + 1;
		int low = 0, high = 50;
		int middle, ans;
		while (low <= high){
			middle = (low + high)/ 2;
			if (middle*(middle+1) >= 2*current_index){
				ans = middle;
				high = middle - 1;
			}
			else low = middle + 1;
 		}

		int row = ans;
		AllBricks[i].bottom = reference_y - row*BrickHeight;
		int index_in_this_row;
		index_in_this_row = current_index - row*(row-1)/2;
		AllBricks[i].left = window_width/2 - BrickLength/2 - (row-1)*(BrickLength/2) + index_in_this_row*BrickLength;
		AllBricks[i].temp_color.red = (rand()%(255/3))*3;
		AllBricks[i].temp_color.green = (rand()%(255/3))*3;
		AllBricks[i].temp_color.blue = (rand()%(255/3))*3;
		AllBricks[i].dx = BrickLength;
		AllBricks[i].dy = BrickHeight;
        AllBricks[i].isShown = 1;
        AllBricks[i].type = 1;
//        else AllBricks[i].type = 2;
        AllBricks[i].PowerUpAssociation = -1;
        if (AllBricks[i].type == 1) bricks++;
	}
	AreBricksFalling = 0;
    return;
}
void drawBricks()
{
    for (int i=0; i<total_bricks; i++){
        if (AllBricks[i].isShown){
            Brick* now = &(AllBricks[i]);
            iSetColor(now->temp_color.red, now->temp_color.green, now->temp_color.blue);
            iFilledRectangle(now->left, now->bottom, now->dx, now->dy);
            if (now->type == 2){
                iSetColor(255, 255, 255);
                iLine(now->left, now->bottom+now->dy, now->left+now->dx, now->bottom);
                iLine(now->left, now->bottom, now->left+now->dx, now->bottom+now->dy);
            }
        }
    }
    return;
}
void reStart()
{
	shouldRestart = 0;
	life--;
    if (life < 0){
        Ball1.isActivated = 0;
        ThisPlayer.score = score;
        ThisPlayer.seconds = minutes*60 + seconds;
        CurrentPage = EnterYourName;
        if (isSoundOn) PlaySound("music\\over.wav", NULL, SND_ASYNC);
        return;
    }
    isThruBall = 0;
    AreBricksFalling = 0;
	CurrentPaddle = InitialPaddle;
	Ball1 = InitialBall;
	Velocity1 = InitialVelocity;
    speed_change = InitialSpeedChange;
    for (int i=0; i<totalpowerups; i++){
        if (AllPowerups[i].state == 1){
            AllPowerups[i].state = 2;
            AllPowerups[i].isCaught = 0;
        }
    }
	return;
}
void BallSpeedChange()
{
    if (isPaused) return;
    if (Ball1.isActivated && Ball1.isMoving){
        Velocity1.dx *= speed_change;
        Velocity1.dy *= speed_change;
    }
    return;
}
int PowerupTypes[] = {3, 3, 1, 1, 9, 6, 7, 5, 10, 10, 8, 8, 4, 0, 0, 2};
void InitializePowerups()
{
    for (int i=0; i<totalpowerups; i++){
        Powerup* this_powerup = &(AllPowerups[i]);
        int random = rand()%total_bricks;
        while (AllBricks[random].PowerUpAssociation != -1){
            random++;
            if (random == total_bricks) random = 0;
        }
        this_powerup->index = random;
        AllBricks[random].PowerUpAssociation = i;
        this_powerup->left = AllBricks[this_powerup->index].left;
        this_powerup->bottom = AllBricks[this_powerup->index].bottom - PowerUpHeight;
        this_powerup->isCaught = 0;
        this_powerup->state = 0;
        this_powerup->type = PowerupTypes[i];
    }
    return;
}
void ApplyPowerups(Powerup* this_powerup)
{
    int type = this_powerup->type;
    if (type == 0){ //extra life
        if (isSoundOn) PlaySound("music\\positivecaught.wav", NULL, SND_ASYNC);
        if (life < 5) life++;
    }
    if (type == 1){ //wider paddle
        if (isSoundOn) PlaySound("music\\largethings.wav", NULL, SND_ASYNC);
        if (10 + CurrentPaddle.dx + 2*CurrentPaddle.side_circle_radius <= window_width/2) CurrentPaddle.dx += 20;
    }
    if (type == 2){ //faster ball
        if (isSoundOn) PlaySound("music\\fallbrick.wav", NULL, SND_ASYNC);
        if (maximum(Velocity1.dx, Velocity2.dy) <= 20){
            Velocity1.dx *= 1.75;
            Velocity1.dy *= 1.75;
        }
    }
    if (type == 3){ //thru ball
        if (isSoundOn) PlaySound("music\\positivecaught.wav", NULL, SND_ASYNC);
        isThruBall = 1;
    }
    if (type == 4){ //slower ball
        if (isSoundOn) PlaySound("music\\positivecaught.wav", NULL, SND_ASYNC);
        double prev = Velocity1.dy;
        if (Velocity1.dy >= 0) Velocity1.dy = 3;
        else Velocity1.dy = -3;
        Velocity1.dx = (Velocity1.dy / prev) * Velocity1.dx;
    }
    if (type == 5){ //shrink ball
        if (isSoundOn) PlaySound("music\\fallbrick.wav", NULL, SND_ASYNC);
        Ball1.radius = 3;
    }
    if (type == 6){ //shrink paddle
        if (isSoundOn) PlaySound("music\\shrinkthings.wav", NULL, SND_ASYNC);
        if (CurrentPaddle.dx >= 30) CurrentPaddle.dx -= 20;
    }
    if (type == 7){ //supershrink paddle
    if (isSoundOn) PlaySound("music\\shrinkthings.wav", NULL, SND_ASYNC);
        CurrentPaddle.dx = InitialPaddle.dx/2;
    }
    if (type == 8){ //mega ball
        if (isSoundOn) PlaySound("music\\largethings.wav", NULL, SND_ASYNC);
        Ball1.radius = 16;
    }
    if (type == 9){ //death
        if (isSoundOn) PlaySound("music\\balldrop.wav", NULL, SND_ASYNC);
        reStart();
    }
    if (type == 10){ //falling bricks
        if (isSoundOn) PlaySound("music\\fallbrick.wav", NULL, SND_ASYNC);
        AreBricksFalling = 1;
    }
    return;
}

const char PowerupImageName[][35] = {"powerups\\extralife.bmp", "powerups\\widepaddle.bmp", "powerups\\fastball.bmp",
                                    "powerups\\thruball.bmp", "powerups\\slowball.bmp", "powerups\\shrinkball.bmp",
                                    "powerups\\shrinkpaddle.bmp", "powerups\\supershrinkpaddle.bmp", "powerups\\megaball.bmp",
                                    "powerups\\death.bmp", "powerups\\fallingbricks.bmp"};
void drawPowerups()
{
    for (int i=0; i<totalpowerups; i++){
        Powerup* this_powerup = &(AllPowerups[i]);
        if (this_powerup->state == 0 || this_powerup->state == 2) continue;
        iShowBMP2(this_powerup->left, this_powerup->bottom, (char*)PowerupImageName[this_powerup->type], 0x00ffffff);
    }
    return;
}
void ChangePowerupPosition()
{
    if (isPaused) return;
    for (int i=0; i<totalpowerups; i++){
        Powerup* this_powerup = &(AllPowerups[i]);
        if (this_powerup->state == 1){
            this_powerup->bottom -= 5;
            if (this_powerup->bottom <= 0){
                this_powerup->state = 2;
            }
        }
    }
    return;
}
void HandlePowerups()
{
    for (int i=0; i<totalpowerups; i++){
        Powerup* this_powerup = &(AllPowerups[i]);
        if (this_powerup->state == 1){
            //either left or right boundary has to stay inside
            if (this_powerup->bottom > CurrentPaddle.bottom + CurrentPaddle.dy) continue;
            if ((this_powerup->left >= CurrentPaddle.left - CurrentPaddle.side_circle_radius && this_powerup->left <= CurrentPaddle.left + CurrentPaddle.dx + CurrentPaddle.side_circle_radius) ||
            this_powerup->left+ PowerUpWidth >= CurrentPaddle.left - CurrentPaddle.side_circle_radius && this_powerup->left + PowerUpWidth <= CurrentPaddle.left + CurrentPaddle.dx + CurrentPaddle.side_circle_radius){
                this_powerup->isCaught = 1;
                this_powerup->state = 2;
                ApplyPowerups(this_powerup);
            }
        }
    }
    return;
}
void FallBricks()
{
    if (isSoundOn) PlaySound("music\\fallbrick.wav", NULL, SND_ASYNC);
    if (AllBricks[total_bricks-1].bottom - (CurrentPaddle.bottom + CurrentPaddle.dy + bottom_margin) <= 100) return;
    for (int i=0; i<total_bricks; i++){
        AllBricks[i].bottom -= 15;
        if (AllBricks[i].PowerUpAssociation != -1){
            AllPowerups[AllBricks[i].PowerUpAssociation].bottom -= 15;
        }
    }
    return;
}
void storeGameStates()
{
    FILE *fpOut = NULL;
    fpOut = fopen("game_state.txt", "w");
    fprintf(fpOut, "All_Bricks_left_bottom_dx_dy_isShown_PowerupAssociation_colors_type\n");
    for (int i=0; i<total_bricks; i++){
        Brick* now_brick = &(AllBricks[i]);
        fprintf(fpOut, "%lf %lf %lf %lf %d %d %d %d %d %d\n", now_brick->left, now_brick->bottom, now_brick->dx, now_brick->dy, now_brick->isShown, now_brick->PowerUpAssociation, now_brick->temp_color.red, now_brick->temp_color.green, now_brick->temp_color.blue, now_brick->type);
    }
    fprintf(fpOut, "All_Powerups_type_index_state_isCaught_left_bottom\n");
    for (int i=0; i<totalpowerups; i++){
        Powerup* now_powerup = &(AllPowerups[i]);
        fprintf(fpOut, "%d %d %d %d %lf %lf\n", now_powerup->type, now_powerup->index, now_powerup->state, now_powerup->isCaught, now_powerup->left, now_powerup->bottom);
    }
    fprintf(fpOut, "Score %d\n", score);
    fprintf(fpOut, "Bricks %d\n", bricks);
    fprintf(fpOut, "isThruBall %d\n", isThruBall);
    fprintf(fpOut, "AreBricksFalling %d\n", AreBricksFalling);
    fprintf(fpOut, "shownScore %s\n", shownScore);
    fprintf(fpOut, "shouldRestart %d\n", shouldRestart);
    fprintf(fpOut, "isSoundOn %d\n", isSoundOn);
    fprintf(fpOut, "SpeedChange %lf\n", speed_change);
    fprintf(fpOut, "Velocity_dx_dy %lf %lf\n", Velocity1.dx, Velocity1.dy);
    fprintf(fpOut, "Previous_Center_dx_dy %lf %lf\n", previous_center_1.x, previous_center_1.y);//
    fprintf(fpOut, "CurrentPaddle_Left_Bottom_dx_dy_sideCircleRadius %lf %lf %lf %lf %lf\n", CurrentPaddle.left, CurrentPaddle.bottom, CurrentPaddle.dx, CurrentPaddle.dy, CurrentPaddle.side_circle_radius);
    fprintf(fpOut, "Center_x_y_isActivated_isMoving_radius %lf %lf %d %d %lf\n", Ball1.center.x, Ball1.center.y, Ball1.isActivated, Ball1.isMoving, Ball1.radius);
    fprintf(fpOut, "Time %d %d %s\n", seconds, minutes, shownTime);
    fprintf(fpOut, "Life %d\n", life);
    fclose(fpOut);
    return;
}
void loadSavedItems()
{
    FILE *fp = NULL;
    char temp[200];
    fp = fopen("game_state.txt", "r");
    fscanf(fp, "%s", temp);
    for (int i=0; i<total_bricks; i++){
        Brick* now_brick = &(AllBricks[i]);
        fscanf(fp, "%lf %lf %lf %lf %d %d %d %d %d %d", &now_brick->left, &now_brick->bottom, &now_brick->dx, &now_brick->dy, &now_brick->isShown, &now_brick->PowerUpAssociation, &now_brick->temp_color.red, &now_brick->temp_color.green, &now_brick->temp_color.blue, &now_brick->type);
    }
    fscanf(fp, "%s", temp);
    for (int i=0; i<totalpowerups; i++){
        Powerup* now_powerup = &(AllPowerups[i]);
        fscanf(fp, "%d %d %d %d %lf %lf", &now_powerup->type, &now_powerup->index, &now_powerup->state, &now_powerup->isCaught, &now_powerup->left, &now_powerup->bottom);
    }
    fscanf(fp, "%s %d", temp, &score);
    fscanf(fp, "%s %d", temp, &bricks);
    fscanf(fp, "%s %d", temp, &isThruBall);
    fscanf(fp, "%s %d", temp, &AreBricksFalling);
    fscanf(fp, "%s %s", temp, shownScore);
    fscanf(fp, "%s %d", temp, &shouldRestart);
    fscanf(fp, "%s %d", temp, &isSoundOn);
    fscanf(fp, "%s %lf", temp, &speed_change);
    fscanf(fp, "%s %lf %lf", temp, &Velocity1.dx, &Velocity1.dy);
    fscanf(fp, "%s %lf %lf", temp, &previous_center_1.x, &previous_center_1.y);
    fscanf(fp, "%s %lf %lf %lf %lf %lf", temp, &CurrentPaddle.left, &CurrentPaddle.bottom, &CurrentPaddle.dx, &CurrentPaddle.dy, &CurrentPaddle.side_circle_radius);
    fscanf(fp, "%s %lf %lf %d %d %lf", temp, &Ball1.center.x, &Ball1.center.y, &Ball1.isActivated, &Ball1.isMoving, &Ball1.radius);
    fscanf(fp, "%s %d %d %s", temp, &seconds, &minutes, shownTime);
    fscanf(fp, "%s %d", temp, &life);
    Ball1.isActivated = 1;
    fclose(fp);
    return;
}

char digitsInBMP[][15] = {"digits\\0.bmp", "digits\\1.bmp", "digits\\2.bmp", "digits\\3.bmp", "digits\\4.bmp", "digits\\5.bmp", "digits\\6.bmp", "digits\\7.bmp", "digits\\8.bmp", "digits\\9.bmp"};

void scoreToBMP()
{
    double last_ending_x = window_width - 25;
    int digits = 0;
    int temp = score;
    int AllDigits[10] = {0};
    int index = 0;
    while(temp){
        AllDigits[index++] = temp%10;
        temp /= 10;
        digits++;
    }
    index--;
    double starts_from = last_ending_x - (scoreBMPwidth+2)*digits;
    for (int i=index, j=0; i>=0; i--, j++){
        iShowBMP2(starts_from+j*(scoreBMPwidth+2), window_height - 80, digitsInBMP[AllDigits[i]], 0x000000);
    }
    return;
}
void updateHighScores()
{
    FILE *fp = fopen("highscores.txt", "r");
    OverallProgress All[11];
    char everything[300] = "";
    for (int i=0; i<10; i++){
        All[i].score = 0;
        All[i].seconds = 0;
        strcpy(All[i].name, "Player");
    }
    int index = 0;
    int overall_serial = 0;
    while (1)
    {
        if (feof(fp)) break;
        fscanf(fp, "%[^\n]%*c", everything);
        if (index%2 == 0) strcpy(All[overall_serial].name, everything);
        else{
            sscanf(everything, "%d %d", &All[overall_serial].score, &All[overall_serial].seconds);
            overall_serial++;
            if (overall_serial == 10) break;
        }
        index++;
    }
    fclose(fp);
    All[10] = ThisPlayer;
    for (int i=0; i<11; i++){
        for (int j=i+1; j<11; j++){
            if (All[i].score < All[j].score || (All[i].score == All[j].score && All[i].seconds > All[j].seconds)){
                OverallProgress temp = All[i];
                All[i] = All[j];
                All[j] = temp;
            }
        }
    }
    FILE *fpOut = fopen("highscores.txt", "w");
    for (int i=0; i<10; i++){
        fprintf(fpOut, "%s\n%d %d\n", All[i].name, All[i].score, All[i].seconds);
        // printf("%s %d %d\n", All[i].name, All[i].score, All[i].seconds);
    }
    fclose(fpOut);
    return;
}
void showHighScores()
{
    FILE *fp = fopen("highscores.txt", "r");
    char everything[300] = "";
    int index = 0;
    int overall_serial = 0;
    if (!haveHighScoresBeenExtracted){
        for (int i=0; i<10; i++){
            strcpy(shownStrings[i], "");
        }
        while (1){
            if (feof(fp)) break;
            fscanf(fp, "%[^\n]%*c", everything);
            if (index%2 == 0){
                strcpy(shownStrings[overall_serial], everything);
                strcpy(All[overall_serial].name, everything);//
            }
            else{
                strcat(shownStrings[overall_serial], " ");
                strcat(shownStrings[overall_serial], everything);
                sscanf(everything, "%d %d", &All[overall_serial].score, &All[overall_serial].seconds);//
//                printf("%s %d %d\n", All[overall_serial].name, All[overall_serial].score, All[overall_serial].seconds);
                overall_serial++;
                if (overall_serial == 10) break;
            }
            index++;
        }
        haveHighScoresBeenExtracted = 1;
    }
    for (int i=0; i<10; i++){
//        if (strcmpi(shownStrings[i], "") == 0) continue;
//        iText(100, reference_y - 100 - 50*i, shownStrings[i], GLUT_BITMAP_TIMES_ROMAN_24);
        iText(100, reference_y - 75 - 50*i, All[i].name, GLUT_BITMAP_TIMES_ROMAN_24);
        sprintf(everything, "%d", All[i].score);
        iText(622, reference_y - 75 - 50*i, everything, GLUT_BITMAP_TIMES_ROMAN_24);
        sprintf(everything, "%d", All[i].seconds);
        iText(960, reference_y - 75 - 50*i, everything, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    fclose(fp);
    return;
}
void InitializeOtherThings()
{
    seconds = 0;
    minutes = 0;
    score = 0;
    isPaused = 0;
    life = 3;
    shouldRestart = 0;
    isThruBall = 0;
    strcpy(ThisPlayer.name, "");
    ThisPlayer.score = 0;
    ThisPlayer.seconds = 0;
    for (int i=0; i<10; i++){
        strcpy(All[i].name, "Player");
        All[i].score = 0;
        All[i].seconds = 0;
    }
    return;
}
