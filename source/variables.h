#define TILE_SIZE 15
#define EASY 0
#define MEDIUM 1
#define HARD 2

touchPosition Stylus;

u32 kDown;
u32 kHeld;

SDL_Window * 	window;
SDL_Renderer * 	renderer;
SDL_Surface *	surface;

TTF_Font* font;
TTF_Font* font48;

Mix_Chunk *son;

#define SAVEPATH "sdmc:/switch/Pixel_Shuffle_NX/"
#define SAVEFILE "Pixel_Shuffle_NX.sav"

static inline SDL_Color SDL_MakeColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_Color colour = {r, g, b, a};
	return colour;
}

#define WHITE		SDL_MakeColour(255, 255, 255, 255)
#define BLACK		SDL_MakeColour(0, 0, 0, 255)
#define GREY		SDL_MakeColour(100, 100, 100, 255)
#define YELLOW		SDL_MakeColour(255, 255, 0, 255)
#define RED			SDL_MakeColour(255, 0, 0, 255)

char filename[52]; // strlen(SAVEPATH) + strlen(SAVEFILE) + 1 = 52
const char* savepath;

typedef struct 
{
	char * name;
	u8 mode;
	bool allowed;
	bool unlocked;
	bool selected;
	bool finished;
} 
images;
images Level[81];

SDL_Color colors[420];

u8 colonnes, lignes;
u8 choix_colonne, select_colonne, number_colonne;
u8 niveau_courant;

u16 temp_colonne[15];

bool menu_mode;
bool game_mode;

bool held_mode;
bool down_mode;

int i, j;

int widthtext, widthmenu[9];

int page_courante;
int Select, Selected, Old_selected;

bool touched, pressed;

bool victory;

u8 succes;

u16 timing;
u16 total_check;

bool sound_played;
