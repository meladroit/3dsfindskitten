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

enum colour
{
    BLACK,    RED,      GREEN,    BROWN,    INDIGO,   PURPLE,   TEAL,     GREY,
    DARKGREY, SALMON,   NEONGREEN,YELLOW,   BLUE,     MAGENTA,  CYAN,     WHITE
};  // notice that 0-7 are normal intensity, 8-15 are bold intensity

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

int screen[TOP_WIDTH+1][HEIGHT+1];

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

void fetch_line(FILE *f, int no_lines, char line[MAX_LW])
{
    // thanks http://stackoverflow.com/questions/232237/
    int i;
    int line_no = rand() % no_lines;
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

int main()
{
    int no_lines = 0;
    int counter = 0;
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
    while(!(feof(f)))
    {
        fgets(line,MAX_LW,f);
        no_lines++;
    }
    no_lines--;
    
    consoleSelect(&topScr);
    gotoxy(TOP_WIDTH/2-8,HEIGHT/2);
    textcolour(NEONGREEN);
    printf("3dsfindskitten");
    
    consoleSelect(&bottomBar);
    textcolour(MAGENTA);
    printf("3dsfindskitten\na zen simulation for the 3DS");
    
    consoleSelect(&bottomScr);

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		// Your code goes here

		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) {
            fetch_line(f,no_lines,line);
            if(counter%16){
                textcolour(counter%16);
            } else { textcolour(rand()%15+1); }
			printf(line);
            counter++;
        }
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}
