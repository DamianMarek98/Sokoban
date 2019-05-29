#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<fstream>
#include<cstdlib>
#include<iostream>
#include<string>


using namespace std;

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
//punkt w kt�rymzaczynamy rysowa� plansz�
#define wysokosc  110
#define szerokosc  158
//punkt pisania menu planszy
#define start_x 75
#define start_y 8


// narysowanie napisu txt na powierzchni screen, zaczynaj�c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj�ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt �rodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d�ugo�ci l w pionie (gdy dx = 0, dy = 1) 
// b�d� poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok�ta o d�ugo�ci bok�w l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

//rysowanie pola gry
void maluj(int *plansza[], int x, int y, SDL_Surface *screen, SDL_Surface *cel, SDL_Surface *postac, SDL_Surface *puste, SDL_Surface *skrzynia, SDL_Surface *sciana)
{
	int szer = szerokosc, wys = wysokosc;
	int rys_postac = 1;
	//deklaracja zmiennych odpowiadaj�cych za miejsce rozpocz�cia rysowania pola gry
	for (int j = 0; j < x + 1; j++) {
		for (int i = 0; i < y + 1; i++) {
			switch (plansza[j][i])
			{
			case 0: DrawSurface(screen, puste, szer, wys); break;
			case 1: DrawSurface(screen, sciana, szer, wys); break;
			case 2: DrawSurface(screen, skrzynia, szer, wys); break;
			case 3: DrawSurface(screen, postac, szer, wys); break;
			case 4: DrawSurface(screen, cel, szer, wys); break;
			}
			wys += 36;
		}
		szer += 36;
		wys = wysokosc;
	}
}
//rysowanie wybranej w menu planszy
void inna_plansza(int *plansza[], int *baza[], int *nowa_gra[], int x, int y,string nazwa)
{
	int k, m; //mno�niki rozmiary planszy
	//pobranie bazowej planszy z pliku
	//x+1,y+1= wysoko�� i szeroko�� tablicy
	fstream plik;
	plik.open(nazwa);
	if (plik.good() == true)
	{
		printf("Poprawny plik zrodlowy planszy\n");
	}
	else
	{
		printf("Niepoprawny plik zrodlowy planszy\n");
	}
	plik >> x >> k;//pobranie rozmiar�w planszy
	plik >> y >> m;//pobranie rozmiar�w planszy
	//okre�lenie warto�ci wysoko�ci i szeroko�ci pola
	x = x*k;
	y = y*m;
	int zmienna;
	//przypisanie danych do tablicy z pliku
	for (int i = 0; i < y + 1; i++) {
		for (int j = 0; j < x + 1; j++) {
			plik >> zmienna;
			plansza[j][i] = zmienna;
			baza[j][i] = zmienna;
			nowa_gra[j][i] = zmienna;
		}
	}
	plik.close();
	plansza[1][1] = 3;
}


//ruch w d�
bool sprawdzenie_dol(int *plansza[], int x, int y)
{
	if (plansza[x][y + 1] == 1)//sprawdzenie czy na polu nast�pnym jest sciana
	{
		return false;
	}
	if (plansza[x][y + 1] == 2)//sprawdzenie czy na nast�pnym polu jest skrzynia
	{
		if (plansza[x][y + 2] == 1)//je�li za skrzyni� jest sciana nie mo�esz poruszy�
		{
			return false;
		}
		if (plansza[x][y + 2] == 2)//je�li za skrzyni� jest druga skrzynia nie mo�na prouszy�
		{
			return false;
		}
	}
	return true;
}
void dol(int *plansza[], int *baza[], int x, int y)
{
	if (plansza[x][y + 1] == 2)//je�li pole na kt�re chcemy wej�� jest skrzyni�, przesuwamy j�
	{
		plansza[x][y + 2] = 2;
		if (baza[x][y + 1] == 2)//je�li bazowo na danym polu by�a skrzynia zamieniamy j� na puste pole
		{
			baza[x][y + 1] = 0;
		}
	}
		plansza[x][y + 1] = 3;
		plansza[x][y] = baza[x][y];
}
//ruch w g�r�
bool sprawdzenie_gora(int *plansza[], int x, int y)
{
	if (plansza[x][y - 1] == 1)//sprawdzenie czy na polu nast�pnym jest sciana
	{
		return false;
	}
	if (plansza[x][y - 1] == 2)//sprawdzenie czy na nast�pnym polu jest skrzynia
	{
		if (plansza[x][y - 2] == 1)//je�li za skrzyni� jest sciana nie mo�esz poruszy�
		{
			return false;
		}
		if (plansza[x][y - 2] == 2)//je�li za skrzyni� jest druga skrzynia nie mo�na prouszy�
		{
			return false;
		}
	}
	return true;
}
void gora(int *plansza[], int *baza[], int x, int y)
{
	if (plansza[x][y - 1] == 2)//je�li pole na kt�re chcemy wej�� jest skrzyni�, przesuwamy j�
	{
		plansza[x][y - 2] = 2;
		if (baza[x][y - 1] == 2)//je�li bazowo na danym polu by�a skrzynia zamieniamy j� na puste pole
		{
			baza[x][y - 1] = 0;
		}
	}
	plansza[x][y -1] = 3;
	plansza[x][y] = baza[x][y];
}
//ruch w prawo
bool sprawdzenie_prawo(int *plansza[], int x, int y)
{
	if (plansza[x+1][y] == 1)//sprawdzenie czy na polu nast�pnym jest sciana
	{
		return false;
	}
	if (plansza[x+1][y] == 2)//sprawdzenie czy na nast�pnym polu jest skrzynia
	{
		if (plansza[x+2][y] == 1)//je�li za skrzyni� jest sciana nie mo�esz poruszy�
		{
			return false;
		}
		if (plansza[x+2][y] == 2)//je�li za skrzyni� jest druga skrzynia nie mo�na prouszy�
		{
			return false;
		}
	}
	return true;
}
void prawo(int *plansza[], int *baza[], int x, int y)
{
	if (plansza[x+1][y] == 2)//je�li pole na kt�re chcemy wej�� jest skrzyni�, przesuwamy j�
	{
		plansza[x+2][y] = 2;
		if (baza[x + 1][y] == 2)//je�li bazowo na danym polu by�a skrzynia zamieniamy j� na puste pole
		{
			baza[x + 1][y] = 0;
		}
	}
	plansza[x+1][y] = 3;
	plansza[x][y] = baza[x][y];
}
//ruch w lewo
bool sprawdzenie_lewo(int *plansza[], int x, int y)//sprawdzenie czy na polu nast�pnym jest sciana
{
	if (plansza[x - 1][y] == 1)
	{
		return false;
	}
	if (plansza[x-1][y] == 2)//sprawdzenie czy na nast�pnym polu jest skrzynia
	{
		if (plansza[x-2][y] == 1)//je�li za skrzyni� jest sciana nie mo�esz poruszy�
		{
			return false;
		}
		if (plansza[x-2][y] == 2)//je�li za skrzyni� jest druga skrzynia nie mo�na prouszy�
		{
			return false;
		}
	}
	return true;
}
void lewo(int *plansza[], int *baza[], int x, int y)
{
	if (plansza[x-1][y] == 2)//je�li pole na kt�re chcemy wej�� jest skrzyni�, przesuwamy j�
	{
		plansza[x-2][y] = 2;
		if (baza[x - 1][y] == 2)//je�li bazowo na danym polu by�a skrzynia zamieniamy j� na puste pole
		{
			baza[x - 1][y] = 0;
		}
	}
	plansza[x - 1][y] = 3;
	plansza[x][y] = baza[x][y];
}

//nowa gra (reset planszy)
void reset(int *plansza[], int *nowa_gra[], int x, int y, int a, int b)
{
	for (int i = 0; i < y + 1; i++) {
		for (int j = 0; j < x + 1; j++) {
			plansza[j][i] = nowa_gra[j][i];
		}
	}
	plansza[a][b] = 3;//ustawienie gracza na pozycji startowej
}

//sprawdzenie uko�czenia poziomu
bool sprawdzenie_ukonczenia(int *plansza[], int *nowa_gra[], int x, int y)
{
	int liczba_celow=0, liczba_skrzyn_na_celach=0;
	for (int j = 0; j < x + 1; j++) {
		for (int i = 0; i < y + 1; i++) {
			if (nowa_gra[j][i] == 4)
			{
				liczba_celow++;
				if (plansza[j][i] == 2) liczba_skrzyn_na_celach++;
			}
		}
	}
	if (liczba_celow == liczba_skrzyn_na_celach)
	{
		return true;
	}
	return false;
}
//wypis informacji
void wygrana(SDL_Surface *screen, SDL_Surface *charset)
{
	char text[128];
	sprintf(text, "WYGRANA");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 80, text, charset);
	
}
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *eti;
	SDL_Surface *cel;
	SDL_Surface *skrzynia;
	SDL_Surface *postac;
	SDL_Surface *puste;
	SDL_Surface *sciana;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	//Pozycja startowa gracza
	int a = 1, b = 1;
	//ilo�� ruch�w
	double ruchy = 0;

	// okno konsoli nie jest widoczne, je�eli chcemy zobaczy�
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieni� na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	
		//nazwa planszy dodanej przez gracza, pole startowe 2,2 wielko�� planszy 10x10
		
	
	//output dla okna konsoli windowsowej
	printf("Nacisnij enter, aby kontynuowac, albo podaj nazwe pliku z wlasna plansza!\n");
	string nazwa;
	getline(cin, nazwa);
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}
	

	// tryb pe�noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy��czenie widoczno�ci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	// wczytanie grafiki gry
	//z pliku warto�� pola do jego obrazka: 0-puste, 1-�ciana, 2-skrzynia, 3-posta�, 4-cel
	cel = SDL_LoadBMP("./cel.bmp");
	if (cel == NULL) {
		printf("SDL_LoadBMP(cel.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	puste = SDL_LoadBMP("./puste.bmp");
	if (puste == NULL) {
		printf("SDL_LoadBMP(puste.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	sciana = SDL_LoadBMP("./sciana.bmp");
	if (sciana == NULL) {
		printf("SDL_LoadBMP(sciana.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	skrzynia = SDL_LoadBMP("./skrzynia.bmp");
	if (skrzynia == NULL) {
		printf("SDL_LoadBMP(skrzynia.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	postac = SDL_LoadBMP("./postac.bmp");
	if (postac == NULL) {
		printf("SDL_LoadBMP(postac.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	//pobranie bazowej planszy z pliku
	//x+1,y+1= wysoko�� i szeroko�� tablicy
	int x, y;
	int k, m; //mno�niki wsoko�ci dla wi�kszych plansz
	fstream plik;
	plik.open("pole.txt");
	if (plik.good() == true)
	{
		printf("Poprawny plik zrodlowy planszy\n");
	}
	else
	{
		printf("Niepoprawny plik zrodlowy planszy\n");
	}
		plik >> x >> k;//pobranie rozmiar�w planszy
		plik >> y >> m;//pobranie rozmiar�w planszy
		x = x*k;
		y = y*m;
		//deklaracja dynamicznej tablicy zawieraj�cej pole
		int **plansza = new int *[x];
		//deklaracja dynamicznej tablicy kt�ra b�dzie zawiera�a bazowe pole gry i b�dzie pod��a�a za ruchem gracza
		int **baza = new int *[x];
		//deklaracja dynamicznej tablicy kt�ra b�dzie zawiera�a podstawowo wczytane pole gry i nie b�dzie zmieniane
		int **nowa_gra = new int *[x];
		int zmienna;

		for (int i = 0; i < x + 1; i++) {
			plansza[i] = new int[y];
			baza[i] = new int[y];
			nowa_gra[i] = new int[y];
		}
		//przypisanie danych do tablicy z pliku
		for (int i = 0; i < y + 1; i++) {
			for (int j = 0; j < x + 1; j++) {
				plik >> zmienna;
				plansza[j][i] = zmienna;
				baza[j][i] = zmienna;
				nowa_gra[j][i] = zmienna;
			}
		}
		plik.close();
		plansza[a][b] = 3;
	while(!quit) {
			t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna� od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			worldTime += delta;

			distance += etiSpeed * delta;

			SDL_FillRect(screen, NULL, czarny);




			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};
			// tekst informacyjny / info text
			DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 60, czerwony, niebieski);
			//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
			sprintf(text, "SOKOBAN");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			sprintf(text, "Czas trwania rozgrywki = %.1lf s  %.0lf klatek  / s", worldTime, fps);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
			//	      "Esc - exit, \030 - faster, \031 - slower"
			sprintf(text, "Esc - wyjscie, n - restart planszy, o - wczytaj wlasna plansze");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 42, text, charset);
			//menu plansz
			int linia = 20; //nowa linia co 20 pixeli
			DrawRectangle(screen, 4, 66, SCREEN_WIDTH - 550, 200, czerwony, niebieski);
			sprintf(text, "Wcisnij:");
			DrawString(screen, start_y, start_x, text, charset);
			sprintf(text, "0-startowa");
			DrawString(screen, start_y, start_x + linia, text, charset); linia += 20;
			sprintf(text, "1-latwy");
			DrawString(screen, start_y, start_x+linia, text, charset); linia += 20;
			sprintf(text, "2-sredni");
			DrawString(screen, start_y, start_x+linia, text, charset); linia += 20;
			sprintf(text, "3-trudny");
			DrawString(screen, start_y, start_x + linia , text, charset);
			//rysowanie planszy
			maluj(plansza, x, y, screen, cel, postac, puste, skrzynia, sciana);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);
			// obs�uga zdarze� (o ile jakie� zasz�y) / handling of events (if there were any)

			if (sprawdzenie_ukonczenia(plansza, nowa_gra, x, y) == true)
			{
				//informacja o wygraniej i spauzowanie rozgrywki(uniemo�liwienie ruchu postaci)
				wygrana(screen, charset);
				sprintf(text, "Ilosc ruchow %.0lf ", ruchy);
				DrawString(screen, 500, 135, text, charset);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
				//przyciski
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
						else if (event.key.keysym.sym == SDLK_n)
						{
							a = 1; b = 1;
							reset(plansza, nowa_gra, x, y, a, b);
							ruchy = 0;
						}
						else if (event.key.keysym.sym == SDLK_0)
						{
							//reset pozycja gracza
							a = 1; b = 1;
							worldTime = 0;
							inna_plansza(plansza, baza, nowa_gra, x, y, "pole.txt");
						}
						else if (event.key.keysym.sym == SDLK_1)
						{
							//reset pozycja gracza
							a = 1; b = 1;
							worldTime = 0;
							inna_plansza(plansza, baza, nowa_gra, x, y, "pole1.txt");
							ruchy = 0;
						}
						else if (event.key.keysym.sym == SDLK_3)
						{
							//reset pozycja gracza
							a = 1; b = 1;
							worldTime = 0;
							inna_plansza(plansza, baza, nowa_gra, x, y, "pole3.txt");
							ruchy = 0;
						}
						else if (event.key.keysym.sym == SDLK_2)
						{
							//reset pozycja gracza
							a = 1; b = 1;
							worldTime = 0;
							inna_plansza(plansza, baza, nowa_gra, x, y, "pole2.txt");
							ruchy = 0;
						}
						else if (event.key.keysym.sym == SDLK_o)
						{
							if (nazwa != "")
							{
								inna_plansza(plansza, baza, nowa_gra, 1, 1, nazwa);
								worldTime = 0;
							}
						}
						break;
					case SDL_QUIT:
						quit = 1;
						break;
					};
				};
				frames++;
				SDL_FillRect(screen, NULL, czarny);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			}
			else
			{
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP)
					{
						if (sprawdzenie_gora(plansza, a, b) == true)
						{
							gora(plansza, baza, a, b);
							b--;
							ruchy++;
						}
					}
					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						if (sprawdzenie_dol(plansza, a, b) == true)
						{
							dol(plansza, baza, a, b);
							b++;
							ruchy++;
						}
					}
					else if (event.key.keysym.sym == SDLK_RIGHT)
					{
						if (sprawdzenie_prawo(plansza, a, b) == true)
						{
							prawo(plansza, baza, a, b);
							a++;
							ruchy++;
						}
					}
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						if (sprawdzenie_lewo(plansza, a, b) == true)
						{
							lewo(plansza, baza, a, b);
							a--;
							ruchy++;
						}
					}
					else if (event.key.keysym.sym == SDLK_n)
					{
						a = 1; b = 1;
						reset(plansza, nowa_gra, x, y, a, b);
					}
					else if (event.key.keysym.sym == SDLK_0)
					{
						//reset pozycja gracza
						a = 1; b = 1;
						worldTime = 0;
						inna_plansza(plansza, baza, nowa_gra, x, y, "pole.txt");
					}
					else if (event.key.keysym.sym == SDLK_1)
					{
						//reset pozycja gracza
						a = 1; b = 1;
						worldTime = 0;
						inna_plansza(plansza, baza, nowa_gra, x, y, "pole1.txt");
					}
					else if (event.key.keysym.sym == SDLK_3)
					{
						//reset pozycja gracza
						a = 1; b = 1;
						worldTime = 0;
						inna_plansza(plansza, baza, nowa_gra, x, y, "pole3.txt");
					}
					else if (event.key.keysym.sym == SDLK_2)
					{
						//reset pozycja gracza
						a = 1; b = 1;
						worldTime = 0;
						inna_plansza(plansza, baza, nowa_gra, x, y, "pole2.txt");
					}
					else if (event.key.keysym.sym == SDLK_o)
					{
						if (nazwa != "")
						{
							inna_plansza(plansza, baza, nowa_gra, 1, 1, nazwa);
							worldTime = 0;
						}
					}
					break;
				case SDL_KEYUP:
					etiSpeed = 1.0;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
			frames++;
			SDL_FillRect(screen, NULL, czarny);
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			}
		};


	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
