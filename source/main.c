// 3dsfindskitten: a zen simulation for the 3DS
// This is actually largely based on the 95LX port that I did from LR's
//  DOS source, just to keep it nice and simple (be gone, ncurses).
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
// and of course, this being a 3DS program ...
#include <3ds.h>
// a few limits ...
#define MAX_LW       256 // max line width (buffer size)
#define NUM_NKI      20  // number of objects not kitten
#define TOP_WIDTH    50  // text columns
#define BOTTOM_WIDTH 40  // text columns
#define HEIGHT       30  // text rows

#define EMPTY -1
#define ROBOT 0
#define KITTEN 1

enum colour
{
    BLACK,    RED,      GREEN,    BROWN,    INDIGO,   PURPLE,   TEAL,     GREY,
    DARKGREY, SALMON,   NEONGREEN,YELLOW,   BLUE,     MAGENTA,  CYAN,     WHITE
};  // notice that 0-7 are normal intensity, 8-15 are bold intensity

enum direction
{
    dUP, dDOWN, dLEFT, dRIGHT
};

typedef struct
{
    int x;
    int y;
    enum colour color; // because oh why not
    char character;
} object;

object robot;
object kitten;
object empty;
object nki[NUM_NKI];
int nki_lines[NUM_NKI];
int nki_chosen = 0;

int no_lines = 0;

int screen[TOP_WIDTH][HEIGHT];

void gotoxy(int x, int y)
{
    // in other words, move to row y, column x
    // (both zero-indexed!)
    printf("\x1b[%d;%dH",y,x);
}

void textcolour(enum colour c)
{
    printf("\x1b[0m");
    if(c>>3) // i.e. if normal intensity
    {
        printf("\x1b[3%d;1m",c%8);
    }
    else
    {
        printf("\x1b[3%dm",c%8);
    }
}

void fetch_line(FILE *f, int line_no, char line[MAX_LW])
{
    // thanks http://stackoverflow.com/questions/232237/
    int i;
    rewind(f);
    for(i=0;i<=line_no;i++)
    {
        fgets(line,MAX_LW,f);
    }
}

void draw_robot() /* Draws robot at current position */
{
    textcolour(robot.color);
    putchar(robot.character);
}

void draw_kitten() /* Draws kitten at current position */
{
    textcolour(kitten.color);
    putchar(kitten.character);
}

int message_collision(int line_no)
{
    int i;
    for (i = 0; i < nki_chosen; i++)
    {
        if (nki_lines[i] == line_no) { return 1; }
    }
    return 0;
}

void game_initialise(PrintConsole *ptopScr, PrintConsole *pbottomBar, PrintConsole *pbottomScr)
{
    int counter, counter2;
    for (counter = 0; counter < TOP_WIDTH; counter++)
    {
        for (counter2 = 0; counter2 < HEIGHT; counter2++)
        {
            screen[counter][counter2] = EMPTY;
        }
    }
    
    robot.x = rand() % TOP_WIDTH;
    robot.y = rand() % HEIGHT;
    robot.character = '\x23';
    robot.color = DARKGREY;
    screen[robot.x][robot.y] = ROBOT;

    // Assign the kitten a unique position.
    do
    {
        kitten.x = rand() % TOP_WIDTH;
        kitten.y = rand() % HEIGHT;
    } while (screen[kitten.x][kitten.y] != EMPTY);

    // Assign the kitten a character and a color.
    do
    {
        kitten.character = rand() % 254 + 1;
    } while ((kitten.character == '\x23') || (kitten.character == ' ') || (kitten.character == (char)(7)));
    kitten.color = rand() % 15 + 1;
    screen[kitten.x][kitten.y] = KITTEN;

    // let's work through the non-kitten (nki) objects here
    nki_chosen = 0;
    for (counter = 0; counter < NUM_NKI; counter++)
    {
        do
        {
            nki[counter].x = rand() % TOP_WIDTH;
            nki[counter].y = rand() % HEIGHT;
        } while (screen[nki[counter].x][nki[counter].y] != EMPTY);
        screen[nki[counter].x][nki[counter].y] = counter + 2;
        do
        {
            nki[counter].character = rand() % 254 + 1;
        } while (nki[counter].character == '\x23' || nki[counter].character == ' ' || nki[counter].character == (char)(7));
        nki[counter].color = rand() % 15 + 1;
        do
        {
            nki_lines[counter] = rand() % no_lines;
        } while (message_collision(nki_lines[counter]));
        nki_chosen++;
    }
    
    consoleSelect(ptopScr);
    consoleClear();
    gotoxy(TOP_WIDTH/2-7,HEIGHT/2);
    textcolour(NEONGREEN);
    printf("3dsfindskitten");
    
    consoleSelect(pbottomBar);
    consoleClear();
    textcolour(MAGENTA);
    printf("3dsfindskitten\na zen simulation for the 3DS");
    
    consoleSelect(pbottomScr);
    consoleClear();
    textcolour(WHITE);
    printf("In this game you are a 3DS (");
    draw_robot();
    textcolour(WHITE);
    printf("). Your job is to find kitten. This task ");
    printf("is complicated by the existence of various things which are not kitten.\n");
    printf("3DS must touch items to determine if they are kitten or not. The game ");
    printf("ends when 3dsfindskitten. Alternatively, you may end the game by ");
    printf("pressing START. Press SELECT to reset.\n");
    printf("   Press any key to start.\n");

}

int process_input(enum direction d)
{
    // returns 0 if no other objects encountered
    //      (which includes both out-of-bounds and successful moves)
    // otherwise, returns whatever object was encountered
    int check_x = robot.x;
    int check_y = robot.y;
    switch (d)
    {
        case dUP:
            check_y--;
            break;
        case dDOWN:
            check_y++;
            break;
        case dLEFT:
            check_x--;
            break;
        case dRIGHT:
            check_x++;
            break;
    }
    
    if (check_y < 0 || check_y >= HEIGHT || check_x < 0 || check_x >= TOP_WIDTH)
    {
        return 0;
    }
    if (screen[check_x][check_y] != EMPTY)
    {
        return screen[check_x][check_y];
    }
    robot.x = check_x;
    robot.y = check_y;
    return 0;
}

int main()
{
    int old_x, old_y;
    int counter = 0;
    int input_result = 0;
    int game_in_progress = 0;
    char line[MAX_LW];
    FILE *f;
    
    gfxInitDefault();
    
    // let's have three PrintConsole instances
    //      one for the whole top screen
    //      one for a little bar along the top of the bottom screen
    //      one for the rest of the bottom screen
    PrintConsole topScr, bottomBar, bottomScr;
    consoleInit(GFX_TOP, &topScr);
    consoleInit(GFX_BOTTOM, &bottomBar);
    consoleInit(GFX_BOTTOM, &bottomScr);
    
    consoleSetWindow(&bottomBar,0,0,BOTTOM_WIDTH,2);
    consoleSetWindow(&bottomScr,0,2,BOTTOM_WIDTH,HEIGHT-2);

    srand(time(0));
    f = fopen("nki","r");
    // and now count how many lines are supplied in the nki file
    while(!(feof(f)))
    {
        fgets(line,MAX_LW,f);
        no_lines++;
    }
    // assume a trailing newline before EOF inflates our count by 1
    no_lines--;
    
    game_initialise(&topScr,&bottomBar,&bottomScr);

    // Main loop
    while (aptMainLoop())
    {
        gspWaitForVBlank();
        hidScanInput();

        // Your code goes here

        u32 kDown = hidKeysDown();
        if (kDown && !game_in_progress) {
            game_in_progress = 1;
            consoleSelect(&bottomScr);
            consoleClear();
            consoleSelect(&topScr);
            consoleClear();
            gotoxy(robot.x,robot.y);
            draw_robot();
            gotoxy(kitten.x,kitten.y);
            draw_kitten();
            for (counter = 0; counter < NUM_NKI; counter++)
            {
                gotoxy(nki[counter].x,nki[counter].y);
                textcolour(nki[counter].color);
                putchar(nki[counter].character);
            }
        }
        else {
            if (kDown & KEY_START)
                break; // break in order to return to hbmenu
            if (kDown & KEY_SELECT)
            {
                game_in_progress = 0;
                game_initialise(&topScr,&bottomBar,&bottomScr);
            }
            old_x = robot.x;
            old_y = robot.y;
            if (kDown & KEY_UP)
            {
                input_result = process_input(dUP);
            }
            if (kDown & KEY_DOWN)
            {
                input_result = process_input(dDOWN);
            }
            if (kDown & KEY_LEFT)
            {
                input_result = process_input(dLEFT);
            }
            if (kDown & KEY_RIGHT)
            {
                input_result = process_input(dRIGHT);
            }
            if (input_result)
            {
                consoleSelect(&bottomScr);
                consoleClear();
                switch (input_result)
                {
                    case KITTEN:
                        textcolour(kitten.color);
                        printf("oh look, you found kitten!");
                        break;
                    default:
                        textcolour(nki[input_result-2].color);
                        fetch_line(f,nki_lines[input_result-2],line);
                        printf(line);
                        break;
                }
            }
            else
            {
                /* Redraw robot, where avaliable */
                if (!(old_x == robot.x && old_y == robot.y))
                {
                    gotoxy(old_x,old_y);
                    putchar(' ');
                    gotoxy(robot.x,robot.y);
                    draw_robot();
                    gotoxy(robot.x,robot.y);
                    screen[old_x][old_y] = EMPTY;
                    screen[robot.x][robot.y] = ROBOT;
                    old_x = robot.x;
                    old_y = robot.y;
                }
            }
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    gfxExit();
    return 0;
}
