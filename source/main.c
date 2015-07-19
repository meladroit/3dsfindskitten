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
#define WIDTH        BOTTOM_WIDTH
#define HEIGHT       30  // text rows

#define EMPTY -1
#define ROBOT 0
#define KITTEN 1

#define HELD_THRESHOLD 8

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
int kitten_dir = 0;
int kitten_found = 0;
object empty;
object nki[NUM_NKI];
int nki_lines[NUM_NKI];
char nki_strings[NUM_NKI][MAX_LW];
int nki_chosen = 0;

int no_lines = 0;
FILE *f;
char line[MAX_LW];

int screen[TOP_WIDTH][HEIGHT];

void sleep(int milliseconds)
{
    // attempts to emulate sleep in ms by assuming 60 fps
    int i;
    for (i = 0; i < (int)(milliseconds/16.67); i++)
    {
        gspWaitForVBlank();
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
}

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

int lastSpace(char *tocut)
{
    char *lastSpace;
    lastSpace = strrchr(tocut,' ');
    if(lastSpace)
    {
        return (int)(lastSpace-tocut);
    } else { return -1; }
}

void wordwrap(char *towrap)
{
    int idx = 0;
    int i, lastSpc;
    char buffer[WIDTH+1] = {0};
    do
    {
        strncpy(buffer,towrap+idx,WIDTH);
        if(strlen(buffer)<WIDTH)
        {
            printf(buffer);
            putchar(' ');
            break;
        }
        if(towrap[idx+WIDTH]==' ')
        {
            printf(buffer);
            idx = idx + WIDTH;
        } else {
            lastSpc = lastSpace(buffer);
            if (lastSpc>0)
            {
                for(i=0;i<lastSpc;i++)
                {
                    putchar(buffer[i]);
                    idx++;
                }
                putchar('\n');
            } else {
                for(i=0;i<WIDTH-1;i++)
                {
                    putchar(buffer[i]);
                }
                printf("- ");
                idx = idx + WIDTH - 1;
            }
        }
    }while (strlen(buffer)==WIDTH);
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
        fetch_line(f,nki_lines[counter],nki_strings[counter]);
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
    printf("\x1b[47;1m");
    printf("3dsfindskitten v0.2         \n");
    printf("a zen simulation for the 3DS");
    
    consoleSelect(pbottomScr);
    consoleClear();
    textcolour(WHITE);
    printf("In this game you are a 3DS (");
    draw_robot();
    textcolour(WHITE);
    printf("). Your job");
    printf(" is to find kitten. This task is compli-");
    printf(" cated by the existence of various      ");
    printf(" things which are not kitten.\n");
    printf("Your 3DS must touch items to determine  ");
    printf(" if they are kitten or not. The game    ");
    printf(" ends when 3dsfindskitten. Alternatively");
    printf(" you may end the game by pressing START,");
    printf(" or press SELECT to reset, at any time.\n");
    textcolour(GREY);
    printf("Press any key to start.\n");

}

int process_input(enum direction d)
{
    int counter;
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
        consoleClear();
        switch (screen[check_x][check_y])
        {
            case KITTEN:
                for (counter = 0; counter <=3; counter++)
                {
                    gotoxy(BOTTOM_WIDTH/2-4+counter-1,HEIGHT/2-2);
                    putchar(' ');
                    gotoxy(BOTTOM_WIDTH/2+3-counter+1,HEIGHT/2-2);
                    putchar(' ');
                    gotoxy(BOTTOM_WIDTH/2-4+counter,HEIGHT/2-2);
                    if (kitten_dir)
                    {
                        draw_kitten();
                    }
                    else
                    {
                        draw_robot();
                    }
                    gotoxy(BOTTOM_WIDTH/2+3-counter,HEIGHT/2-2);
                    if (kitten_dir)
                    {
                        draw_robot();
                    }
                    else
                    {
                        draw_kitten();
                    }
                    sleep(600);
                }
                gotoxy(BOTTOM_WIDTH/2-1,HEIGHT/2-3);
                textcolour(SALMON);
                putchar('\3');
                putchar('\3');
                gotoxy(0,0);
                sleep(1200);
                textcolour(WHITE);
                printf("You found kitten! Way to go, 3DS!\n");
                printf("Press START to quit.\n");
                printf("Press SELECT to restart your ceaseless, Sisyphean quest.");
                kitten_dir = 0;
                kitten_found = 1;
                break;
            default:
                textcolour(nki[screen[check_x][check_y]-2].color);
                wordwrap(nki_strings[screen[check_x][check_y]-2]);
                break;
        }
        return screen[check_x][check_y];
    }
    screen[check_x][check_y] = ROBOT;
    screen[robot.x][robot.y] = EMPTY;
    robot.x = check_x;
    robot.y = check_y;
    return 0;
}

int main()
{
    int old_x, old_y;
    int counter = 0;
    int game_in_progress = 0;
    int heldU = 0;
    int heldD = 0;
    int heldL = 0;
    int heldR = 0;
    u32 kDown, kHeld;
    //u32 kUp;
    
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

    srand(time(NULL));
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

        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        //kUp = hidKeysHeld();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu
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
        else if (kitten_found) {
            if (kDown & KEY_SELECT)
            {
                game_in_progress = 0;
                kitten_found = 0;
                game_initialise(&topScr,&bottomBar,&bottomScr);
            }
        }
        else {
            consoleSelect(&bottomScr);
            old_x = robot.x;
            old_y = robot.y;
            if (kDown & KEY_UP)
            {
                kitten_dir = 1;
                process_input(dUP);
                heldU = 0;
            }
            else if (kHeld & KEY_UP)
            {
                heldU++;
                if (heldU > HELD_THRESHOLD)
                {
                    process_input(dUP);
                    heldU = 0;
                }
            }
            if ((kDown & KEY_DOWN) && !kitten_found)
            {
                kitten_dir = 0;
                process_input(dDOWN);
                heldD = 0;
            }
            else if ((kHeld & KEY_DOWN) && !kitten_found)
            {
                heldD++;
                if (heldD > HELD_THRESHOLD)
                {
                    process_input(dDOWN);
                    heldD = 0;
                }
            }
            if ((kDown & KEY_LEFT) && !kitten_found)
            {
                kitten_dir = 1;
                process_input(dLEFT);
                heldL = 0;
            }
            else if ((kHeld & KEY_LEFT) && !kitten_found)
            {
                heldL++;
                if (heldL > HELD_THRESHOLD)
                {
                    process_input(dLEFT);
                    heldL = 0;
                }
            }
            if ((kDown & KEY_RIGHT) && !kitten_found)
            {
                kitten_dir = 0;
                process_input(dRIGHT);
                heldR = 0;
            }
            else if ((kHeld & KEY_RIGHT) && !kitten_found)
            {
                heldR++;
                if (heldR > HELD_THRESHOLD)
                {
                    process_input(dRIGHT);
                    heldR = 0;
                }
            }
            consoleSelect(&topScr);
            if (!(old_x == robot.x && old_y == robot.y))
            {
                gotoxy(old_x,old_y);
                putchar(' ');
                gotoxy(robot.x,robot.y);
                draw_robot();
                gotoxy(robot.x,robot.y);
                old_x = robot.x;
                old_y = robot.y;
            }
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    gfxExit();
    return 0;
}
