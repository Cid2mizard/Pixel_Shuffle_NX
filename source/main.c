#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

#include <switch.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "variables.h"
#include "niveau.h"


//TOUCH
bool DowntouchInBox(touchPosition touch, int x1, int y1, int x2, int y2)
{
	int tx=touch.px;
	int ty=touch.py;

	if (kDown & KEY_TOUCH && tx > x1 && tx < x2 && ty > y1 && ty < y2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool HeldtouchInBox(touchPosition touch, int x1, int y1, int x2, int y2)
{
	int tx=touch.px;
	int ty=touch.py;

	if (kHeld & KEY_TOUCH && tx > x1 && tx < x2 && ty > y1 && ty < y2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SDL_DrawText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, SDL_Color colour, const char *text)
{
	SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, colour, 1280);
	SDL_SetSurfaceAlphaMod(surface, colour.a);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect position;
	position.x = x; position.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
	SDL_RenderCopy(renderer, texture, NULL, &position);
	SDL_DestroyTexture(texture);
}

void SDL_DrawTextf(SDL_Renderer *renderer, TTF_Font *font, int x, int y, SDL_Color colour, const char* text, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	SDL_DrawText(renderer, font, x, y, colour, buffer);
	va_end(args);
}

void SDL_DrawRect(SDL_Renderer *renderer, int x, int y, int w, int h, SDL_Color colour)
{
	SDL_Rect rect;
	rect.x = x; rect.y = y; rect.w = w; rect.h = h;
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderFillRect(renderer, &rect);
}

void Save()
{
	FILE *file = fopen(filename,"w+b");

	//Write file
	for (i = 0; i < 81; i++)
	{
		fwrite(&Level[i].unlocked, sizeof(Level[i].unlocked), 1, file);
		fwrite(&Level[i].finished, sizeof(Level[i].finished), 1, file);
	}

	//Close file
	fclose(file);
}


void Load()
{
	u8 erreur = 0;

	FILE* file = fopen(filename, "r+b");

	if (file != 0)
	{
		for (i = 0; i < 81; i++)
		{
			fread(&Level[i].unlocked, 1, sizeof(Level[i].unlocked), file);
			fread(&Level[i].finished, 1, sizeof(Level[i].finished), file);
		}

		//Close file
		fclose(file);
	}
	else
	{
		erreur = 1;
	}

	if (erreur == 1)
	{
		Save();
	}
}

void InitGame()
{
	//On Debloque les 19 premiers niveau
	for (i = 0; i < 18; i++)
	{
		Level[i].unlocked = true;
	}

	//On alloue tous les niveaux
	for (i = 0; i < 81; i++)
	{
		Level[i].allowed = true;
	}
	//On non-alloue les niveaux hard
	for (i = 6; i < 9; i++)
	{
		Level[i].allowed = false;
		Level[i+9].allowed = false;
		Level[i+18].allowed = false;
		Level[i+27].allowed = false;
		Level[i+36].allowed = false;
		Level[i+45].allowed = false;
		Level[i+54].allowed = false;
		Level[i+63].allowed = false;
		Level[i+72].allowed = false;
	}

	//On selectionne le premier niveau pour le curseur
	Level[0].selected = true;

	//On met les modes 0 = easy, 1 = normal, 2 = hard
	for (i = 0; i < 3; i++)
	{
		//Easy
		Level[i].mode = EASY;
		Level[i+9].mode = EASY;
		Level[i+18].mode = EASY;
		Level[i+27].mode = EASY;
		Level[i+36].mode = EASY;
		Level[i+45].mode = EASY;
		Level[i+54].mode = EASY;
		Level[i+63].mode = EASY;
		Level[i+72].mode = EASY;

		//Medium
		Level[i+3].mode = MEDIUM;
		Level[i+12].mode = MEDIUM;
		Level[i+21].mode = MEDIUM;
		Level[i+30].mode = MEDIUM;
		Level[i+39].mode = MEDIUM;
		Level[i+48].mode = MEDIUM;
		Level[i+57].mode = MEDIUM;
		Level[i+66].mode = MEDIUM;
		Level[i+75].mode = MEDIUM;

		//Hard
		Level[i+6].mode = HARD;
		Level[i+15].mode = HARD;
		Level[i+24].mode = HARD;
		Level[i+33].mode = HARD;
		Level[i+42].mode = HARD;
		Level[i+51].mode = HARD;
		Level[i+60].mode = HARD;
		Level[i+69].mode = HARD;
		Level[i+78].mode = HARD;
	}

	//Name of levels
	Level[0].name = "TV";
	Level[1].name = "ACORN";
	Level[2].name = "SAUCER GUY";
	Level[3].name = "FLOPPY";
	Level[4].name = "SKULL";
	Level[5].name = "FINGER";
	Level[6].name = "USA FLAG";
	Level[7].name = "CLOCK";
	Level[8].name = "STOP";

	Level[9].name = "BUSINESSMAN";
	Level[10].name = "FLOWER";
	Level[11].name = "DIAMOND";
	Level[12].name = "PANIC";
	Level[13].name = "PI";
	Level[14].name = "XMAS TREE";
	Level[15].name = "YING YANG";
	Level[16].name = "PEACE";
	Level[17].name = "SPEAKER";

	Level[18].name = "HECTOR";
	Level[19].name = "HEART";
	Level[20].name = ":D";
	Level[21].name = "EXPLORER";
	Level[22].name = "HOURGLASS";
	Level[23].name = "I LUV NY";
	Level[24].name = "CANADA FLAG";
	Level[25].name = "BOAT";
	Level[26].name = "CUBE";

	Level[27].name = "BLUEBERRIES";
	Level[28].name = "CHERRIES";
	Level[29].name = "SHOE";
	Level[30].name = "BASKET BALL";
	Level[31].name = "EARTH";
	Level[32].name = "PARROT";
	Level[33].name = "BIN";
	Level[34].name = "CURSOR";
	Level[35].name = "CALENDAR";

	Level[36].name = "SOCCER BALL";
	Level[37].name = "KID";
	Level[38].name = "BADHED";
	Level[39].name = "BALLOONBOY";
	Level[40].name = "COFFEE";
	Level[41].name = "CUPCAKE";
	Level[42].name = "MALE";
	Level[43].name = "TELEPHONE";
	Level[44].name = "PRINTER";

	Level[45].name = "BONFIRE";
	Level[46].name = "SUNGLASSES";
	Level[47].name = "BALLOON";
	Level[48].name = "KEY";
	Level[49].name = "ORANGE";
	Level[50].name = "TOBACCO";
	Level[51].name = "FEMALE";
	Level[52].name = "NOTES";
	Level[53].name = "PENCIL";

	Level[54].name = "ISLAND";
	Level[55].name = "BUBBLEGUM";
	Level[56].name = "B.H.BOY";
	Level[57].name = "PANDA";
	Level[58].name = "TREE";
	Level[59].name = "ONIGARI";
	Level[60].name = "MAGIC HAT";
	Level[61].name = "PADLOCK";
	Level[62].name = "WARNING";

	Level[63].name = "WEDDING";
	Level[64].name = "QUESTION";
	Level[65].name = "ZOMBIE BOD";
	Level[66].name = "MOUSE";
	Level[67].name = "LIGHTER";
	Level[68].name = ".";
	Level[69].name = "LIGHT BULE";
	Level[70].name = "NO PARKING";
	Level[71].name = "ZATTIKKA";

	Level[72].name = "IPOD";
	Level[73].name = "CALCULATOR";
	Level[74].name = "GIMME 5";
	Level[75].name = "WATERMELON";
	Level[76].name = "DINO EGG";
	Level[77].name = "BOW";
	Level[78].name = "GEM";
	Level[79].name = "EYE OF HORUS";
	Level[80].name = "BEE";
}

Uint32 obtenirPixel(SDL_Surface *surface, int x, int y)
{
    int nbOctetsParPixel = surface->format->BytesPerPixel;
    /* Ici p est l'adresse du pixel que l'on veut connaitre */
    /*surface->pixels contient l'adresse du premier pixel de l'image*/
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * nbOctetsParPixel;

    /*Gestion différente suivant le nombre d'octets par pixel de l'image*/
    switch(nbOctetsParPixel)
    {
        case 1:
            return *p;

        case 2:
            return *(Uint16 *)p;

        case 3:
            /*Suivant l'architecture de la machine*/
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;

        case 4:
            return *(Uint32 *)p;

        /*Ne devrait pas arriver, mais évite les erreurs*/
        default:
            return 0; 
    }
}

void InitColors()
{
	surface = IMG_Load("romfs:/resources/images/colour_palette_by_savingcleo.png");	

	for (colonnes = 0; colonnes < 21; colonnes++)
	{
		for (lignes = 0; lignes < 20; lignes++)
		{
			Uint32 pixel;
			pixel=obtenirPixel(surface, 68 + colonnes*20, 123 + lignes*25);

			Uint8 r,g,b,a;
			SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			colors[lignes * 21 + colonnes].r = r;
			colors[lignes * 21 + colonnes].g = g;
			colors[lignes * 21 + colonnes].b = b;
			colors[lignes * 21 + colonnes].a = a;
		}
	}

	SDL_FreeSurface(surface);
}

void checkunlock()
{
	succes = 0;

	for (i = 0; i < 81; i++)
	{
		if (Level[i].finished)
			succes++;
	}

	if (succes == 10)
	{
		for (i = 0; i < 9; i++)
			Level[18+i].unlocked = true;
	}
	else if (succes == 15)
	{
		for (i = 0; i < 9; i++)
			Level[27+i].unlocked = true;
	}
	else if (succes == 20)
	{
		for (i = 0; i < 9; i++)
			Level[36+i].unlocked = true;
	}
	else if (succes == 25)
	{
		for (i = 0; i < 9; i++)
			Level[45+i].unlocked = true;
	}
	else if (succes == 30)
	{
		for (i = 0; i < 9; i++)
			Level[54+i].unlocked = true;
	}
	else if (succes == 35)
	{
		for (i = 0; i < 9; i++)
			Level[63+i].unlocked = true;
	}
	else if (succes == 40)
	{
		for (i = 0; i < 9; i++)
			Level[72+i].unlocked = true;
	}
}

void InitMenu()
{
	for (i = 0; i < 9; i++)
	{
		int w,h;
		TTF_SizeText(font , Level[i + page_courante*9].name , &w , &h );
		widthmenu[i] = w;
	}

	game_mode = false;
	menu_mode = true;

	checkunlock();
}

void InitLevel()
{
	for (colonnes = 0; colonnes < 15; colonnes++)
	{
		for (lignes = 0; lignes < 15; lignes++)
		{
			level_courant_help[colonnes + lignes * 15] = level[niveau_courant][colonnes + lignes * 15];
			level_courant[colonnes + lignes * 15] = 999;
		}
	}

	int w,h;
	TTF_SizeText(font , Level[niveau_courant].name , &w , &h );
	widthtext = w;

	for (colonnes = 0; colonnes < 15; colonnes++)
	{
		//On choisit une colonne de destination au hasard
		choix_colonne = rand() % 15;

		//Tant que la ligne n'est pas vierge, on relance le random
		while (level_courant[choix_colonne] != 999)
		{
			choix_colonne = rand() % 15;
		}

		for (lignes = 0; lignes < 15; lignes++)
		{
			level_courant[choix_colonne + lignes * 15] = level_courant_help[colonnes + lignes * 15];
		}
	}

	timing = 0;

	menu_mode = false;
	game_mode = true;
	sound_played = false;

	victory = false;
}

void CheckVictory()
{
	total_check = 0;

	for (colonnes = 0; colonnes < 15; colonnes++)
	{
		for (lignes = 0; lignes < 15; lignes++)
		{
			if(level_courant[colonnes + lignes * 15] == level_courant_help[colonnes + lignes * 15])
				total_check++;
		}
	}

	//Victory ????
	if (total_check == 15*15)
	{
		Level[niveau_courant].finished = true;
		Level[niveau_courant].allowed = true;

		victory = true;

		checkunlock();

		Save();
	}
}

void ManageInput()
{
	hidScanInput();

	kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

	hidTouchRead(&Stylus, 0);

	if (menu_mode)
	{
		if (DowntouchInBox(Stylus, 400, 90, 520, 228))
		{
			Select = 0;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 610, 90, 748, 228))
		{			
			Select = 1;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 820, 90, 958, 228))
		{
			Select = 2;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 400, 300, 520, 438))
		{
			Select = 3;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 610, 300, 748, 438))
		{
			Select = 4;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 820, 300, 958, 438))
		{
			Select = 5;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 400, 510, 520, 648))
		{
			Select = 6;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 610, 510, 748, 648))
		{
			Select = 7;
			touched = true;
		}
		else if (DowntouchInBox(Stylus, 820, 510, 958, 648))
		{
			Select = 8;
			touched = true;
		}

		if ((kDown & KEY_RIGHT) && (Select != 2) && (Select != 5) && (Select != 8))
		{
			Level[Selected].selected = false;
			Select++;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;
		}
		else if ((kDown & KEY_LEFT) && (Select != 0) && (Select != 3) && (Select != 6))
		{
			Level[Selected].selected = false;
			Select--;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;
		}
		else if ((kDown & KEY_DOWN) && (Select < 6))
		{
			Level[Selected].selected = false;
			Select+=3;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;
		}
		else if ((kDown & KEY_UP) && (Select > 2))
		{
			Level[Selected].selected = false;
			Select-=3;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;
		}
		else if (kDown & KEY_A)
			touched = true;

		if (touched)
		{
			//La nouvelle devient l'ancienne
			Old_selected = Selected;

			//On efface la selection dans le tableau
			Level[Selected].selected = false;

			//On determine la nouvelle en fonction de la position actuel
			Selected = Select + page_courante*9;

			//On change la selection dans le tableau
			Level[Selected].selected = true;

			//Si l'ancienne correspond à la nouvelle et level débloqué
			if ((Old_selected == Selected) && (Level[Selected].unlocked))
			{
				//On lance le jeu
				niveau_courant = Selected;
				InitLevel();
			}

			touched = false;
		}

		if ((kDown & KEY_R) && (page_courante < 8))
		{
			Level[Selected].selected = false;
			page_courante++;
			Select = 0;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;

			InitMenu();
		}
		else if ((kDown & KEY_L) && (page_courante > 0))
		{
			Level[Selected].selected = false;
			page_courante--;
			Select = 0;
			Selected = Select + page_courante*9;
			Level[Selected].selected = true;

			InitMenu();
		}
	}
	else if (game_mode)
	{
		if (!victory)
		{
			if (DowntouchInBox(Stylus, 540, 0, 1260, 720))
			{
				select_colonne = (Stylus.px-540)/48;
				down_mode = true;
			}
			else if (HeldtouchInBox(Stylus, 540, 0, 1260, 720) && down_mode)
			{
				held_mode = true;

				//Si plus à sa place (avant)
				if ((Stylus.px-540)/48 < select_colonne)
				{
					number_colonne = select_colonne - (Stylus.px-540)/48;

					for (i = number_colonne; i > 0; i--)
					{
						for (lignes = 0; lignes < 15; lignes++)
						{
							//Save la ligne d'avant
							temp_colonne[lignes] = level_courant[select_colonne - i + lignes * 15];

							//ligne d'avant devient select
							level_courant[select_colonne - i + lignes * 15] = level_courant[select_colonne + lignes * 15];

							//select devient le save
							level_courant[select_colonne + lignes * 15] = temp_colonne[lignes];
						}
					}

					//maj select
					select_colonne = (Stylus.px-540)/48;
				}
				//Si plus à sa place (devant)
				else if ((Stylus.px-540)/48 > select_colonne)
				{
					number_colonne = (Stylus.px-540)/48 - select_colonne;

					for (i = number_colonne; i > 0; i--)
					{
						for (lignes = 0; lignes < 15; lignes++)
						{
							//Save la ligne précédente
							temp_colonne[lignes] = level_courant[select_colonne + i + lignes * 15];

							//ligne suivante devient select
							level_courant[select_colonne + i + lignes * 15] = level_courant[select_colonne + lignes * 15];

							//select devient le save
							level_courant[select_colonne + lignes * 15] = temp_colonne[lignes];
						}
					}

					//update select_colonne
					select_colonne = (Stylus.px-540)/48;
				}
			}
			else
			{
				held_mode = false;
				down_mode = false;
				pressed = false;

				CheckVictory();
			}

			//View Solution
			/*else if (HeldtouchInBox(Stylus, 150, 360, 390, 600) && (Level[Selected].mode!=HARD))
			{
				pressed = true;
				held_mode = false;
				down_mode = false;
			}*/

			//Return to Menu
			if (kDown & KEY_B)
				InitMenu();
			//Re-Shuffle
			else if (kDown & KEY_Y)
				InitLevel();
			else if (((kHeld & KEY_L) || (kHeld & KEY_R)) && (Level[Selected].mode!=HARD))
				pressed = true;
			else
				pressed = false;
		}
		else
		{
			if ((kDown & KEY_A) || (kDown & KEY_B) || DowntouchInBox(Stylus, 0, 0, 1280, 720) || (kDown & KEY_X) || (kDown & KEY_Y))
				InitMenu();
		}
	}
}


void printGame()
{
	SDL_SetRenderDrawColor(renderer, 155, 100, 50, 255);
	//CLEAR
	SDL_RenderClear(renderer);

	if (menu_mode)
	{
		SDL_DrawTextf(renderer, font, 250, 150, YELLOW, "EASY");
		SDL_DrawTextf(renderer, font, 232, 150+210, YELLOW, "MEDIUM");
		SDL_DrawTextf(renderer, font, 250, 150+420, YELLOW, "HARD");

		if (page_courante != 8) SDL_DrawTextf(renderer, font, 1100, 6, WHITE, "BUTTON R =>");
		if (page_courante != 0) SDL_DrawTextf(renderer, font, 14, 6, WHITE, "<= BUTTON L");

		SDL_DrawTextf(renderer, font, 1010, 690, WHITE, "BY CID2MIZARD (2018)");
		SDL_DrawTextf(renderer, font, 10, 690, WHITE, "PIXEL SHUFFLE NX");

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				SDL_DrawTextf(renderer, font, 469 - (widthmenu[i+j*3]/2) + i*210, 45 + j*210, WHITE, "%s", Level[i+j*3+page_courante*9].name);

				if (Level[i+j*3+page_courante*9].selected)
				{
					SDL_DrawRect(renderer, 397 + i*210, 87 + j*210, 144, 144, YELLOW);
				}

				//La grille
				SDL_DrawRect(renderer, 400 + i*210, 90 + j*210, 138, 138, BLACK);

				for (colonnes = 0; colonnes < 15; colonnes++)
				{
					for (lignes = 0; lignes < 15; lignes++)
					{
						//Si finished
						if ((Level[i+j*3+page_courante*9].finished))
						{
							SDL_DrawRect(renderer, 403 + i*210 + colonnes*9, 93 + j*210 + lignes*9, 6, 6, colors[level[i+j*3+page_courante*9][colonnes + lignes * 15]]);
						}
						//Si selected & allowed
						else if ((Level[i+j*3+page_courante*9].selected) && (Level[i+j*3+page_courante*9].allowed) && (Level[i+j*3+page_courante*9].unlocked))
						{
							SDL_DrawRect(renderer, 403 + i*210 + colonnes*9, 93 + j*210 + lignes*9, 6, 6, colors[level[i+j*3+page_courante*9][colonnes + lignes * 15]]);
						}
						//Si selected & !allowed
						else if ((Level[i+j*3+page_courante*9].selected) && (!Level[i+j*3+page_courante*9].allowed) && (Level[i+j*3+page_courante*9].unlocked))
						{
							SDL_DrawRect(renderer, 403 + i*210 + colonnes*9, 93 + j*210 + lignes*9, 6, 6, colors[level[82][colonnes + lignes * 15]]);
						}
						//Si !unlocked
						else if ((!Level[i+j*3+page_courante*9].unlocked))
						{
							SDL_DrawRect(renderer, 403 + i*210 + colonnes*9, 93 + j*210 + lignes*9, 6, 6, colors[level[83][colonnes + lignes * 15]]);
						}
						//Sinon unknown
						else
						{
							SDL_DrawRect(renderer, 403 + i*210 + colonnes*9, 93 + j*210 + lignes*9, 6, 6, colors[level[81][colonnes + lignes * 15]]);
						}
					}
				}
			}
		}

		if(!Level[page_courante*9].unlocked)
			SDL_DrawTextf(renderer, font, 439, 690, WHITE, "SOLVE %d MORE PICTURES TO UNLOCK!", page_courante*5 - succes);
	}
	else if (game_mode)
	{
		SDL_DrawTextf(renderer, font, 235.5, 20, YELLOW, "TITLE");
		SDL_DrawTextf(renderer, font, 270-widthtext/2, 50, WHITE, "%s", Level[niveau_courant].name);

		SDL_DrawTextf(renderer, font, 235.5, 100, YELLOW, "STAGE");
		SDL_DrawTextf(renderer, font, 255, 130, WHITE, "%02d", niveau_courant+1);

		if (Level[niveau_courant].mode == EASY)
			SDL_DrawTextf(renderer, font, 240, 160, YELLOW, "EASY");
		else if (Level[niveau_courant].mode == MEDIUM)
			SDL_DrawTextf(renderer, font, 228, 160, YELLOW, "MEDIUM");
		else if (Level[niveau_courant].mode == HARD)
			SDL_DrawTextf(renderer, font, 240, 160, YELLOW, "HARD");

		SDL_DrawTextf(renderer, font, 156, 230, WHITE, "SHUFFLE COLUMNS");
		SDL_DrawTextf(renderer, font, 192, 260, WHITE, "WITH FINGER");

		SDL_DrawTextf(renderer, font, 172.5, 300, WHITE, "VIEW SOLUTION");
		SDL_DrawTextf(renderer, font, 172.5, 330, WHITE, "L OR R BUTTON");

		if (held_mode)
		{
			for (colonnes = 0; colonnes < 15; colonnes++)
			{
				if (colonnes != select_colonne)
				{
					for (lignes = 0; lignes < 15; lignes++)
					{
						SDL_DrawRect(renderer, 540 + colonnes*48, lignes*48, 48, 48, BLACK);
						SDL_DrawRect(renderer, 543 + colonnes*48, 3 + lignes*48, 42, 42, colors[level_courant[colonnes + lignes * 15]]);
					}
				}
			}

			for (lignes = 0; lignes < 15; lignes++)
			{
				SDL_DrawRect(renderer, Stylus.px-30, lignes*60 - 90, 60, 60, BLACK);
				SDL_DrawRect(renderer, Stylus.px-27, lignes*60 - 87, 54, 54, colors[level_courant[select_colonne + lignes * 15]]);
			}
		}
		else
		{
			for (colonnes = 0; colonnes < 15; colonnes++)
			{
				for (lignes = 0; lignes < 15; lignes++)
				{
					SDL_DrawRect(renderer, 540 + colonnes*48, lignes*48, 48, 48, BLACK);
					SDL_DrawRect(renderer, 543 + colonnes*48, 3 + lignes*48, 42, 42, colors[level_courant[colonnes + lignes * 15]]);
				}
			}
		}

		if (victory)
		{
			if (!sound_played)
				Mix_PlayChannel(1, son, 0);

			sound_played = true;

			SDL_DrawTextf(renderer, font48, 699, 217, GREY, "WELL");
			SDL_DrawTextf(renderer, font48, 694, 212, YELLOW, "WELL");
			SDL_DrawTextf(renderer, font48, 699, 367, GREY, "DONE!");
			SDL_DrawTextf(renderer, font48, 694, 362, YELLOW, "DONE!");
		}

		//Le Help
		if ((pressed) && (timing < 241))
		{
			for (colonnes = 0; colonnes < 15; colonnes++)
			{
				for (lignes = 0; lignes < 15; lignes++)
				{
					SDL_DrawRect(renderer,150 + colonnes*16, 360 + lignes*16, 16, 16, BLACK);
					SDL_DrawRect(renderer,151 + colonnes*16, 361 + lignes*16, 14, 14, colors[level_courant_help[colonnes + lignes * 15]]);
				}
			}

			if (Level[niveau_courant].mode==MEDIUM)
				timing+=2;
		}
		else
		{
			for (colonnes = 0; colonnes < 15; colonnes++)
			{
				for (lignes = 0; lignes < 15; lignes++)
				{
					SDL_DrawRect(renderer,150 + colonnes*16, 360 + lignes*16, 16, 16, BLACK);
					if (Level[niveau_courant].mode==HARD)
						SDL_DrawRect(renderer,151 + colonnes*16, 361 + lignes*16, 14, 14, colors[level[82][colonnes + lignes * 15]]);
					else
						SDL_DrawRect(renderer,151 + colonnes*16, 361 + lignes*16, 14, 14, colors[level[81][colonnes + lignes * 15]]);
				}
			}
		}

		if (Level[niveau_courant].mode==MEDIUM)
		{
			SDL_DrawRect(renderer, 149, 609, 242, 62, BLACK);
			SDL_DrawRect(renderer, 150, 610, 240, 60, WHITE);
			if (timing < 241) SDL_DrawRect(renderer, 150, 610, 240-timing, 60, RED);
		}
	}

	//REFRESH
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
	// Initialize
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	romfsInit();

	srand(time(0));

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
	Mix_AllocateChannels(2);

	savepath = SAVEPATH;
	struct stat s;
	int err = stat(SAVEPATH, &s);

	if(err == -1)
	{
		savepath = "./";
	}
	else if(!S_ISDIR(s.st_mode))
	{
		savepath = "./";
	}

	sprintf(filename, "%s%s", savepath, SAVEFILE);

	// Create an SDL window & renderer
	window = SDL_CreateWindow("Main-Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);

	// Font loading
	font = TTF_OpenFont("romfs:/resources/fonts/MiniSet2.ttf", 24);
	font48 = TTF_OpenFont("romfs:/resources/fonts/MiniSet2.ttf", 144);

	//Palette
	InitColors();

	//Les status des niveaux
	InitGame();

	//On charge la sauvegarde
	Load();

	//Le Menu
	InitMenu();

	Mix_Music *musique;
	musique = Mix_LoadMUS("romfs:/resources/sounds/Rubiks-Dodecahedron.mp3");
	Mix_PlayMusic(musique, -1);

	son = Mix_LoadWAV("romfs:/resources/sounds/Bells11.wav");

	// Game loop:
	while (appletMainLoop())
	{
		ManageInput();

		printGame();

		if (kDown & KEY_PLUS)
			break;
	}

	Mix_FreeMusic(musique);
	Mix_FreeChunk(son);
	Mix_CloseAudio();

	SDL_Quit();
	return EXIT_SUCCESS;
}
