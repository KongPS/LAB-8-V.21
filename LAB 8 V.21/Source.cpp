#include <stdio.h>
#include <windows.h>
#include <time.h>
#define scount 80
#define screen_x 80
#define screen_y 25
HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
COORD bufferSize = { screen_x,screen_y };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD characterPos = { 0,0 };
COORD star[scount];
// function setConsole() and setMode()

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}
void setcolor(int fg, int bg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, bg * 16 + fg);
}
void draw_ship(int posx2, int posy2)
{
	COORD c = { posx2, posy2 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("<-O->");
}
void erase_ship(int posx, int posy)
{
	COORD c = { posx, posy };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf("     ");
}
void erase_star(int px, int py)
{
	COORD c = { px, py };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	printf(" ");
}
void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 7;
		}
	}
}
void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}
void init_star()
{
	for (int z = 0; z < scount; z++)
	{
		int x = rand() % 80;
		int y = rand() % 25;
		star[z].X = x;
		star[z].Y = y;
	}

}
void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { (rand() % screen_x),1 };
		}
		else {
			star[i] = { star[i].X,star[i].Y + 1 };
		}
	}
}
void fill_star_to_buffer()
{
	for (int z = 0; z < scount; z++)
	{
		consoleBuffer[star[z].X + screen_x * star[z].Y].Char.AsciiChar = '*';
		consoleBuffer[star[z].X + screen_x * star[z].Y].Attributes = 7;
	}
}
char cursor(int x, int y)
{
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	char buf[2]; COORD c = { x,y }; DWORD num_read;
	if (!ReadConsoleOutputCharacter(hStd, (LPTSTR)buf, 1, c, (LPDWORD)&num_read))
	{
		return '\0';
	}
	else
	{
		return buf[0];
	}
}
int main()
{
	int HP = 10;
	int change = 0;
	int color = 0;
	int count = 0;
	int posx = 200, posy = 200;
	int posx2, posy2;
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	srand(NULL(time));
	int i;
	init_star();
	i = 0;
	while (play)
	{
		star_fall();
		clear_buffer();
		fill_star_to_buffer();
		fill_buffer_to_console();
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c')
					{
						color = 1;
						change = 0;
					}
					if (color == 1)
					{
						setcolor(0, 0);
						erase_ship(posx, posy);
						if (change == 0)
						{
							count = rand() % 255;
							change = 1;
						}
						setcolor(7, count);
						draw_ship(posx2, posy2);
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					posx2 = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					posy2 = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {

						for (int z = 0; z < 5; z++)
						{
							if (cursor(posx2 + z, posy2) == '*')
							{
								int px = posx2 + z;
								int py = posy2;
								erase_star(px, py);
								for (i = 0; i < scount; i++) {
									if (star[i].X == px && star[i].Y == py)
									{
										star[i].X = rand() % screen_x;
										star[i].Y = rand() % screen_y;
										if (star[i].Y >= screen_y - 1) {
											star[i] = { (rand() % screen_x),1 };
										}
										else {
											star[i] = { star[i].X,star[i].Y + 1 };
										}
									}
								}
								HP--;
							}
						}
						if (HP == 0)
						{
							play = false;
						}
						color = 1;
						change = 0;
						if (color == 1)
						{
							setcolor(0, 0);
							erase_ship(posx, posy);
							if (change == 0)
							{
								count = rand() % 255;
								change = 1;
							}
							setcolor(7, count);
							draw_ship(posx2, posy2);
						}
						posx = posx2;
						posy = posy2;
					}
					else if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						RIGHTMOST_BUTTON_PRESSED) {

					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						for (int z = 0; z < 5; z++)
						{
							if (cursor(posx2 + z, posy2) == '*')
							{
								int px = posx2 + z;
								int py = posy2;
								erase_star(px, py);
								for (i = 0; i < scount; i++) {
									if (star[i].X == px && star[i].Y == py)
									{
										star[i].X = rand() % screen_x;
										star[i].Y = rand() % screen_y;
										if (star[i].Y >= screen_y - 1) {
											star[i] = { (rand() % screen_x),1 };
										}
										else {
											star[i] = { star[i].X,star[i].Y + 1 };
										}
									}
								}
								HP--;
							}
						}
						if (HP == 0)
						{
							play = false;
						}
						if (color == 0)
						{
							erase_ship(posx, posy);
							draw_ship(posx2, posy2);
						}
						if (color == 1)
						{
							setcolor(0, 0);
							erase_ship(posx, posy);
							if (change == 0)
							{
								count = rand() % 255;
								change = 1;
							}
							setcolor(7, count);
							draw_ship(posx2, posy2);
						}
						posx = posx2;
						posy = posy2;
					}
				}
			}
			delete[] eventBuffer;
		}

		Sleep(100);
		i++;


	}
	return 0;
}