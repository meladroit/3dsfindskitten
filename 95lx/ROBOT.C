/*robotfindskitten v1600000.150 source code.
 *
 *Copyright (C) 1997,1999 Leonard Richardson leonardr@segfault.org
 *(ported to Turbo C and 95LX screen by willidleaway 2015)
 *
 *Note that this is not the source for the compiled program that comes
 *with this package (v1600000.200). I don't know where the source for
 *that is. I don't even know to what extent this code will compile and
 *run, as I no longer use DOS. As far as I'm concerned, this code has
 *historical value only.
 *
 *If you want this to work like the compiled version, look at the binary
 *and add the other 50 messages to the array. Also take a look at the problem
 *near the end where it says "Here's where the problem is." I don't know
 *what the problem is, but if I said there was one, there probably was one.
 */

#include <conio.h>
#include <dos.h>
#include <process.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -1
#define ROBOT 0
#define KITTEN 1
#define MESSAGES 150
#define WIDTH 40
#define HEIGHT 16
/* #define NUM_BOGUS 10 */

void instructions();
void draw_robot();
void draw_kitten();
void process_input(char input);

typedef struct
{
	int x;
	int y;
	int color;
	char character;
} object;

object robot;
object kitten;
object empty;
object bogus[MESSAGES];
int bogus_messages[MESSAGES];

static char* messages[] =
{
    "\"I pity the fool who mistakes me\nfor kitten!\", sez Mr. T.",
    "That's just an old tin can.",
    "It's an altar to the horse god.",
    "A box of dancing mechanical pencils.\nThey dance! They sing!",
    "It's an old Duke Ellington record.",
    "A box of fumigation pellets.",
    "A digital clock. It's stuck at 2:17 PM.",
    "That's just a charred human corpse.",
    "I don't know what that is,\nbut it's not kitten.",
    "An empty shopping bag.\nPaper or plastic?",
    "Could it be...\na big ugly bowling trophy?",
    "A coat hanger hovers in thin air. Odd.",
    "Not kitten,\njust a packet of Kool-Aid(tm).",
    "A freshly-baked pumpkin pie.",
    "A lone, forgotten comma, sits here,\nsobbing.",
    "ONE HUNDRED THOUSAND CARPET FIBERS!!!!!",
    "It's Richard Nixon's nose!",
    "It's Lucy Ricardo.\n\"Aaaah, Ricky!\", she says.",
    "You stumble upon Bill Gates'\nstand-up act.",
    "Just an autographed copy\nof the Kama Sutra.",
    "It's the Will Rogers Highway.\nWho was Will Rogers, anyway?",
    "It's another robot, more advanced in\ndesign than you but strangely immobile.",
    "Leonard Richardson is here,\nasking people to lick him.",
    "It's a stupid mask,\nfashioned after a beagle.",
    "Your State Farm Insurance(tm)\nrepresentative!",
    "It's the local draft board.",
    "Seven 1/4\" screws\nand a piece of plastic.",
    "An 80286 machine.",
    "One of those stupid\n\"Homes of the Stars\" maps.",
    "A signpost saying \"TO KITTEN\".\nIt points in no particular direction.",
    "A hammock stretched between\na tree and a volleyball pole.",
    "A Texas Instruments of Destruction\ncalculator.",
    "It's a dark, amphorous blob of matter.",
    "Just a pincushion.",
    "It's a mighty zombie\ntalking about some love and prosperity.",
    "\"Dear robot, you have been personally\nselected to recieve this Visa card...\"",
    "It's just an object.",
    "A mere collection of pixels.",
    "A badly dented high-hat cymbal\nlies on its side here.",
    "A marijuana brownie.",
    "A plush Chewbacca.",
    "Daily hunger conditioner\nfrom Australasia",
    "Just some stuff.",
    "Why are you touching this\nwhen you should be finding kitten?",
    "A glorious fan of peacock feathers.",
    "It's some compromising photos\nof Babar the Elephant.",
    "A copy of the Weekly World News.\nWatch out for the chambered nautilus!",
    "It's the proverbial wet blanket.",
    "A \"Get Out of Jail Free\" card.",
    "An incredibly expensive\n\"Mad About You\" collector plate.",
    "Paul Moyer's necktie.",
    "A haircut and a real job.\nNow you know where to get one!",
    "An automated robot-hater.\nIt frowns disapprovingly at you.",
    "An automated robot-liker.\nIt smiles at you.",
    "It's a black hole. Don't fall in!",
    "Just a big brick wall.",
    "You found kitten! No, just kidding.",
    "Heart of Darkness brand pistachio nuts.",
    "A smoking branding iron\nshaped like a 24-pin connector.",
    "It's a Java applet.",
    "An abandoned used-car lot.",
    "A shameless plug for Crummy - The Site\nwas here.",
    "A shameless plug for Nerth Pork\nwas here.",
    "A can of Spam Lite.",
    "This is another fine mess you've\ngotten us into, Stanley.",
    "It's scenery for \"Waiting for Godot\".",
    "A grain elevator towers high above you.",
    "A Mentos wrapper.",
    "It's the constellation Pisces.",
    "It's a fly on the wall. Hi, fly!",
    "This kind of looks like kitten,\nbut it's not.",
    "It's a banana! Oh, joy!",
    "A helicopter has crashed here.",
    "Carlos Tarango stands here, doing his\nbest impression of Pat Smear.",
    "A patch of mushrooms grows here.",
    "A patch of grape jelly grows here.",
    "A spindle, and a grindle,\nand a bucka-wacka-woom!",
    "A geyser sprays water high into the air.",
    "A toenail? What good is a toenail?",
    "You've found the fish! Not that it\ndoes you much good in this game.",
    "A Buttertonsils bar.",
    "One of the few remaining discoes.",
    "Ah, the uniform of a\nRevolutionary-era minuteman.",
    "A punch bowl, filled with punch\nand lemon slices.",
    "It's nothing but a G-thang, baby.",
    "IT'S ALIVE! AH HA HA HA HA!",
    "This was no boating accident!",
    "Wait! This isn't the poker chip!\nYou've been tricked! DAMN YOU, MENDEZ!",
    "A livery stable! Get your livery!",
    "It's a perpetual immobility machine.",
    "\"On this spot in 1962, Henry Winkler\n    was sick.\"",
    "There's nothing here; it's just\nan optical illusion.",
    "The World's Biggest Motzah Ball!",
    "A tribe of cannibals lives here. They\neat Malt-O-Meal for breakfast, you know.",
    "This appears to be a large stack\nof trashy romance novels.",
    "Look out! Exclamation points!",
    "A herd of wild coffee mugs\nslumbers here.",
    "It's a limbo bar! How low can you go?",
    "It's the horizon. Now THAT'S weird.",
    "A vase full of artificial flowers\nis stuck to the floor here.",
    "A large snake bars your way.",
    "A pair of saloon-style doors\nswing slowly back and forth here.",
    "It's an ordinary bust of Beethoven... \nbut why is it painted green?",
    "It's TV's lovable wisecracking Crow!\n\"Bite me!\", he says.",
    "Hey, look, it's war. What is it good\nfor? Absolutely nothing. Say it again.",
    "It's the amazing self-referential\nthing that's not kitten.",
    "A flamboyant feather boa. Now you can\ndress up like Carol Channing!",
    "\"Sure hope we get some rain soon,\"\nsays Farmer Joe.",
    "\"How can I wash my neck if it ain't\n gonna rain no more?\" asks Farmer Al.",
    "\"Topsoil's all gone, ma,\"\nweeps Lil' Greg.",
    "A large brown bear. Oddly enough,\nit's currently peeing in the woods.",
    "A team of arctic explorers\nis camped here.",
    "This object here appears to be\nLouis Farrakhan's bow tie.",
    "This is the world-famous\nChain of Jockstraps.",
    "A trash compactor, compacting away.",
    "This toaster strudel is riddled\nwith bullet holes!",
    "It's a hologram of a crashed helicopter.",
    "This is a television. On screen you see\na robot strangely similar to yourself.",
    "This balogna has a first name,\nit's R-A-N-C-I-D.",
    "A salmon hatchery? Look again.\nIt's merely a single salmon.",
    "It's a rim shot. Ba-da-boom!",
    "It's creepy and it's kooky, mysterious\nand spooky. It's also somewhat ooky.",
    "This is an anagram.",
    "This object is like an analogy.",
    "It's a symbol. You see in it a model\nfor all symbols everywhere.",
    "The object pushes back at you.",
    "A traffic signal. It appears to have\nbeen recently terrorized.",
    "\"There is no kitten!\" cackles the old\ncrone. You are shocked by her blasphemy.",
    "This is a Lagrange point.\nDon't come too close now.",
    "The dirty old tramp bemoans the loss\nof his harmonica.",
    "Look, it's Fanny the Irishman!",
    "What in blazes is this?",
    "It's the instruction manual for\na previous version of this game.",
    "A brain cell. Oddly enough, it seems\nto be functioning.",
    "Tea and/or crumpets.",
    "This jukebox has nothing but\nCliff Richards albums in it.",
    "It's a Quaker Oatmeal tube, converted\ninto a drum.",
    "This is a remote control. Being\na robot, you keep a wide berth.",
    "It's a roll of industrial-strength\ncopper wire.",
    "Oh boy! Grub! Er, grubs.",
    "A puddle of mud,\nwhere the mudskippers play.",
    "Plenty of nothing.",
    "Look at that, it's the Crudmobile.",
    "Just Walter Mattheau and Jack Lemmon.",
    "Two crepes, two crepes in a box.",
    "An autographed copy\nof \"Primary Colors\", by Anonymous.",
    "Another rabbit? That's three today!",
    "It's a segmentation fault.\nCore dumped, by the way.",
    "A historical marker showing\nthe actual location of /dev/null.",
    "Thar's Mobius Dick,\nthe convoluted whale. Arrr!",
    "It's a charcoal briquette, smoking away.",
    "A pizza, melting in the sun.",
    "It's a \"HOME ALONE 2: Lost in New York\"\nnovelty cup.",
    "A stack of 7 inch floppies wobbles\nprecariously.",
    "It's nothing but a corrupted floppy.\nCoaster anyone?",
    "A section of glowing phosphor cells\nsing a song about radiation to you.",
    "The unplugged TRS-80 III fails to be\na good conversationalist.",
    "A toilet bowl occupies this space.",
    "Tim the Enchanter sits,\nknowing much that is hidden.",
    "It's a solitary vaccuum tube.",
    "Particle man does the things\na particle can.",
    "\"Hi, I'm Anson Williams, TV's 'Potsy'.\"",
    "Looks like a power converter\nfrom Taashi Station.",
    "An R2 unit with a bad motivator.",
    "It's the missing chapter to\n\"A Clockwork Orange\".",
    "It's a burrito stand flyer.\n\"Taqueria El Ranchito\".",
    "Whoever they are, they're happy\nbecause they eat LARD.",
    "Roger Avery, persona un famoso\nde los Estados Unidos.",
    "Ne'er but a potted plant.",
    "A parrot, kipping on its back.",
    "A forgotten telephone switchboard.",
    "A forgotten telephone switchboard\noperator.",
    "It's an automated robot-disdainer.\nIt pretends you're not there.",
    "Shoot. Thought it was kitten, but it\nwas an exploded pen.",
    "Just a moldy loaf of bread.",
    "A little glass tub of Carmex. ($.89)\nToo bad you have no lips.",
    "A Swiss-Army knife. All of its\nappendages are out. (toothpick lost)",
    "It's a zen simulation, trapped within\nan ASCII character.",
    "It's a copy of\n\"The Rubaiyat of Spike Schudy\".",
    "It's \"War and Peace\" (unabridged,\nvery small print).",
    "A willing, ripe tomato bemoans\nyour inability to digest fruit.",
    "A robot comedian. Dang, if that\nservo bit wasn't hilarious!",
    "It's KITT, the automated car...\nYou were close, though.",
    "Here's Pete Peterson. His batteries\nseem to have long gone dead.",
    "\"Blup, blup, blup\", says the mud pot.",
    "More grist for the mill.",
    "Grind 'em up, spit 'em out,\nthey're twigs.",
    "The boom box belts out\nan old Ethel Merman tune.",
    "It's \"Finding kitten\", published by\nO'Reilly and Associates.",
    "Pumpkin pie spice.",
    "It's the Bass-Matic '76!\nMmm, that's good bass!",
    "\"Lend me a fiver 'til Thursday\",\npleas Andy Capp.",
    "It's a tape of '70s rock. All original\nhits! All original artists!",
    "You've found the fabled America Online\ndisk graveyard!",
    "Empty jewelboxes litter the landscape.",
    "It's the astounding meta-object.",
    "Ed McMahon stands here, lost in thought.\nSeeing you, he bellows, \"YES SIR!\"",
    "...thingy???",
    "It's 1000 secrets the government\ndoesn't want you to know!",
    "The letters O and R.",
    "A magical... magic thing."
};

int used_messages[MESSAGES];
int NUM_BOGUS;
int screen[WIDTH+1][HEIGHT+1];

int main(int argc, char *argv[])
{
	int supplied_number; /* Number of bogus objects, supplied on command line. */
	int counter, counter2, index;
	int old_x, old_y;
	char input;

	/* Do general start-of-program stuff. */
	srand(time(NULL));
	empty.x = -1;
	empty.y = -1;
	empty.color = 0;
	empty.character = ' ';

	if (argc == 1)
	{
	 NUM_BOGUS = 20;
	} else {
	 supplied_number = atoi(argv[1]);
	 if (supplied_number < 0 || supplied_number > MESSAGES)
	 {
	  printf("Error: run-time parameter must be between 0 and %d.", MESSAGES);
	  exit(0);
	 } else {
	  NUM_BOGUS = supplied_number;
	 }
	}

	/*
	//Now we have the filling in of the various arrays.
	//

	//Create an array to represent the screen so that we can make sure all
	//the objects go on different squares.
	*/
	for (counter = 0; counter <= WIDTH; counter++)
	{
		for (counter2 = 0; counter2 <= HEIGHT; counter2++)
		{
			screen[counter][counter2] = EMPTY;
		}
	}
	/* Create an array to ensure we don't get duplicate messages. */
	for (counter = 0; counter < MESSAGES; counter++)
	{
		used_messages[counter] = 0;
		bogus_messages[counter] = 0;
		bogus[counter] = empty;
	}

	/*
	//Now we initialize the various game objects.
	//
	//Assign a position to the player.
	*/
	robot.x = rand() % WIDTH+1;
	robot.y = rand() % (HEIGHT-3)+4;
	robot.character = '\x23';
	/* robot.color = DARKGRAY; */
	screen[robot.x][robot.y] = ROBOT;

	/*Assign the kitten a unique position.*/
	do
	{
		kitten.x = rand() % WIDTH+1;
		kitten.y = rand() % (HEIGHT-3)+4;
	} while (screen[kitten.x][kitten.y] != EMPTY);

	/*Assign the kitten a character and a color.*/
	do
	{
		kitten.character = rand() % 254 + 1;
	} while ((kitten.character == '\x23') || (kitten.character == ' ') || (kitten.character == (char)(7)));
	kitten.color = rand() % 15 + 1;
	screen[kitten.x][kitten.y] = KITTEN;

	/* Now, initialize non-kitten objects. */
	for (counter = 0; counter < NUM_BOGUS; counter++)
	{
		/* Assign a unique position. */
		do
		{
			bogus[counter].x = rand() % WIDTH+1;
			bogus[counter].y = rand() % (HEIGHT-3)+4;
		} while (screen[bogus[counter].x][bogus[counter].y] != EMPTY);
		screen[bogus[counter].x][bogus[counter].y] = counter+2;

		/* Assign a character. */
		do
		{
			bogus[counter].character = rand() % 254 + 1;
		} while (bogus[counter].character == '\x23' || bogus[counter].character == ' ' || bogus[counter].character == (char)(7));
		bogus[counter].color = rand() % 15 + 1;

		/* Assign a unique message. */
		index = 0;
		do
		{
			index = rand() % MESSAGES;
		} while (used_messages[index] != 0);
		bogus_messages[counter] = index;
		used_messages[index] = 1;
	}

	/* Print instructions. */
	clrscr();
	instructions();
	clrscr();

	/* Put stuff on the screen. */
	/* textcolor(WHITE); */
	gotoxy(1,1);
	printf("robotfindskitten LX");
	gotoxy(1,3);
	for (counter = 0; counter < WIDTH; counter++){ putch('Ä'); }
	gotoxy(kitten.x,kitten.y);
	draw_kitten();
	for (counter = 0; counter < NUM_BOGUS; counter++)
	{
		gotoxy(bogus[counter].x,bogus[counter].y);
		/* textcolor(bogus[counter].color); */
		putch(bogus[counter].character);
	}

	gotoxy(robot.x,robot.y);
	draw_robot();
	old_x = robot.x;
	old_y = robot.y;

	/*
	//Now the fun begins.
	*/
	input = getch();
	while ((int)(input) != 27)
	{
		process_input(input);
		/* Redraw robot, where avaliable */
		if (!(old_x == robot.x && old_y == robot.y))
		{
			gotoxy(old_x,old_y);
			putch(' ');
			gotoxy(robot.x,robot.y);
			draw_robot();
			gotoxy(robot.x,robot.y);
			screen[old_x][old_y] = EMPTY;
			screen[robot.x][robot.y] = ROBOT;
            old_x = robot.x;
			old_y = robot.y;
		}
		input = getch();
	}
	gotoxy(1,3);
}

/* Given the keyboard input, interprets it. */
void process_input(char input)
{
	int check_x = robot.x;
	int check_y = robot.y;
	int counter;
	switch ((int)(input))
	{
		case 72: /* up */
			check_y--;
			break;
		case 80: /* down */
			check_y++;
			break;
		case 75: /* left */
			check_x--;
			break;
		case 77: /* right */
			check_x++;
			break;
		case 0:
			break;
		default: /* invalid command */
			gotoxy(1,2);
			/*textcolor(WHITE);*/
			clreol();
			printf("Invalid command: Use direction keys or Esc.");
			return;
	}

	/* Check for going off the edge of the screen. */
	if (check_y < 4 || check_y > HEIGHT || check_x < 1 || check_x > WIDTH)
	{
		return;
	}

	/* Check for collision */
	if (screen[check_x][check_y] != EMPTY)
	{
		switch (screen[check_x][check_y])
		{
			case ROBOT:
				/* We didn't move. */
				break;
			case KITTEN: /* Found it! */
				gotoxy(1,3);
				for (counter = 0; counter < WIDTH; counter++)
				{
					putch('\xc4');
				}
				gotoxy(1,2);
				clreol();
				printf("");

				/* The grand cinema scene. */
				for (counter = 0; counter <=3; counter++)
				{
					gotoxy(WIDTH-25+counter-1,2);
					putch(' ');
					gotoxy(WIDTH-18-counter+1,2);
					putch(' ');
					gotoxy(WIDTH-25+counter,2);
					if ((int)(input) == 72 || (int)(input) == 75)
						draw_kitten();
					else
						draw_robot();
					gotoxy(WIDTH-18-counter,2);
					if ((int)(input) == 72 || (int)(input) == 75)
						draw_robot();
					else
						draw_kitten();
					delay (600);
				}
				/* They're in love! */
				gotoxy(WIDTH-22,1);
				/* textcolor(LIGHTRED); */
				putch((char)(3));
				putch((char)(3));
				gotoxy(1,2);
				delay (1200);
				/* textcolor(WHITE); */
				printf("You found kitten! Way to go, robot!\n");
				exit(0);
				break;
			default:
				gotoxy(1,3);
				for (counter = 0; counter < WIDTH; counter++)
				{
					putch('\xc4');
				}
				gotoxy(1,2);
				/*Here's where the problem is.*/
				clreol();
				printf(messages[bogus_messages[screen[check_x][check_y]-2]]);
				break;
		}
		gotoxy(1,2);
		return;
	}
	/* Otherwise, move the robot. */
	robot.x = check_x;
	robot.y = check_y;
}

void instructions()
{
	char dummy;
	textcolor(WHITE);
	gotoxy(1,1);
	printf("robotfindskitten\n");
	printf("By the illustrious L. R. (C) 1997\n");
	printf("Ported especially to the 95LX!\n");
	printf("   In this game, you are robot (");
	draw_robot();
	textcolor(WHITE);
	printf(").\nYour job is to find kitten. This task\n");
	printf("is complicated by the existence of\nvarious things which are not kitten.\n");
	printf("Robot must touch items to determine\nif they are kitten or not. The game\n");
	printf("ends when robotfindskitten.\nAlternatively, you may end the game by\n");
	printf("hitting the Esc key. See the\ndocumentation for more information.\n\n");
	printf("   Press any key to start.\n");
	dummy = getch();

}

void draw_robot() /* Draws robot at current position */
{
	/* textcolor(robot.color); */
	putch(robot.character);
}

void draw_kitten() /* Draws kitten at current position */
{
	/* textcolor(kitten.color); */
	putch(kitten.character);
}
