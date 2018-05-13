#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread> 
#include <random>

using namespace std;

int nScreenWidth = 120, nScreenHeight = 40;

int nMapHeight = 17;
int nMapWidth = 12;

struct tetromino{

	tetromino() {};

	~tetromino() {};


	wstring sprite = L"$$$@"
					  "$$@$"
					  "$@$$"
					  "@$$$";

	int xPos = 54;
	int yPos = 8;

	int width = 4;
	int height = 4;

	//draws sprite onto passed buffer but does not draw spaces
	void draw(wchar_t buffer[])
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (sprite[y * width + x] != ' ')
					buffer[(y + yPos) * nScreenWidth + (x + xPos)] = sprite[y * width + x];
			}
		}
	}

	void rotate(bool cw)
	{
		wstring newT = L"    "
			"    "
			"    "
			"    ";

		if (!cw)
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					newT[12 + y - x * 4] = sprite[y * width + x];
				}
			}
		}
		else
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					newT[3 + 4 * x - y] = sprite[y * width + x];
				}
			}
		}
		sprite = newT;
	}

};

bool collide(tetromino *T, wchar_t *buffer, char direction)
{
	int Xdir = 0;
	int Ydir = 0;

	switch (direction) {
		bool block;
	case 'R':
		Xdir = 1;
		break;

	case 'L':
		Xdir = -1;
		break;

	case 'D':
		Ydir = 1;
		break;

	default:
		return false;
	}

	// iterate through tetromino sprite and check blocks adjasent to them
	for (int x = 0; x < T->width; x++)
	{
		for (int y = 0; y < T->height; y++)
		{
			if (T->sprite[y * T->width + x] == '@' &&
				buffer[(T->yPos + y + Ydir) * nScreenWidth + (T->xPos + x + Xdir)] == '#')
				return true;
		}
	}
	return false;
}



int main()
{

	// creating tetrominoes

	tetromino I;

	I.sprite = L"  @ "
			    "  @ "
				"  @ "
				"  @ ";

	tetromino L;

	L.sprite = L"    "
				"  @ "
				"  @ "
				"  @@";

	tetromino J;

	J.sprite = L"    "
				"  @ "
				"  @ "
				" @@ ";

	tetromino O;

	O.sprite = L"    "
				" @@ "
				" @@ "
				"    ";

	tetromino S;

	S.sprite = L"    "
				" @@ "
				"@@  "
				"    ";

	tetromino Z;

	Z.sprite = L"    "
				" @@ "
				"  @@"
				"    ";

	tetromino T;

	T.sprite = L"    "
				"  @ "
				" @@@"
				"    ";

	tetromino pieces[7] = { I, L, O, J, S, Z, T };

	tetromino *current = &T;

	wstring map;

	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"#          #";
	map += L"############";

	// create screen buffer
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	float fallSpeed = 1.0;

	int score = 0;

	bool running = true;

	while (running)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		float fElapsedTime = elapsedTime.count();

		if (fElapsedTime >= fallSpeed)
		{
			if (!collide(current, screen, 'D'))
				current->yPos++;
			else
			{
				// draw sprite to map
				for (int x = 0; x < current->width; x++)
				{
					for (int y = 0; y < current->height; y++)
					{
						if (current->sprite[y * current->width + x] != ' ')
							map[(y + current->yPos - 10) * nMapWidth + (x + current->xPos - 50)] = '#';
					}
				}
				//check if there are any blocks above the limit
				for (int i = 1; i < 10; i++)
				{
					if (map[i] == '#')
						running = false;
				}

				int lines = 0;

				// check if there are any lines
				for (int y = 1; y < 16; y++)
				{
					int count = 0;

					for (int x = 1; x < 11; x++)
					{
						if (map[y * nMapWidth + x] == '#')
							count++;
					}
					if (count == 10)
					{
						//move all squares above y down
						wstring insert = L"#          #";
						map.erase(y * nMapWidth, 12);
						map.insert(0, insert);
						lines++;
					}
				}

				score += lines * 100;

				current->xPos = 54;
				current->yPos = 8;
				current = &pieces[rand() % 7];
			}
			fallSpeed = 1.0;
			tp1 = chrono::system_clock::now();
		}

		if (GetAsyncKeyState((unsigned short)'A') && 0x8000)
		{
			if (!collide(current, screen, 'L'))
				current->xPos--;
		}

		if (GetAsyncKeyState((unsigned short)'D') && 0x8000)
		{
			if (!collide(current, screen, 'R'))
				current->xPos++;
		}

		if (GetAsyncKeyState((unsigned short)'E') && 0x8000)
		{
			current->rotate(true);
		}

		if (GetAsyncKeyState((unsigned short)'Q') && 0x8000)
		{
			current->rotate(false);
		}

		if (GetAsyncKeyState((unsigned short)'S') && 0x8000)
		{
			fallSpeed = 0.1;
		}

		for (int i = 0; i < nScreenHeight * nScreenWidth; i++)
			screen[i] = ' ';

		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapHeight; ny++)
			{
				screen[(ny + 10) * nScreenWidth + (nx + 50)] = map[ny * nMapWidth + nx];
			}

		current->draw(screen);


		// display stats
		swprintf_s(screen, 20, L"fElapsed time=%3.2f", fElapsedTime);

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);

		this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
}