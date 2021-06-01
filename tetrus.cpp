// Tetris.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "tetrus.h"
#include <ctime>
#include <string>



enum class TetraminoType { Column = 0, T = 1, G = 2, Four_R = 3, G_R = 4, Cube = 5, Four = 6 };

//HDC globalHDC = nullptr;
COLORREF** field;
int x = 8, y = 17;
int size_x = 25; // для отображения по x
int size_y = 25; // для отображения по y
int score = 0;
bool gameOver = false;

struct tetramino
{
	COORD pos = { 0,0 };
	TetraminoType type;
	TetraminoType next_type;


	int width = 1;
	int height = 1;

	void init()
	{
		pos.Y = 0;

		type = next_type;
		next_type = (TetraminoType)(rand() % 7);

		if (quad != nullptr)
		{
			for (int i = 0; i < 4; i++)
				delete[] quad[i];
			delete[] quad;
		}

		switch (type)
		{
		case TetraminoType::Column:
			quad = new bool* [4]{
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0}
			};
			color = RGB(255, 255, 0);
			break;
		case TetraminoType::T:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0}
			};
			color = RGB(255, 0, 255);
			break;
		case TetraminoType::G:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0}
			};
			color = RGB(255, 0, 0);
			break;
		case TetraminoType::Four_R:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{0,1,0,0}
			};
			color = RGB(0, 255, 0);
			break;
		case TetraminoType::G_R:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{0,1,0,0},
				new bool[4]{0,1,0,0}
			};
			color = RGB(0, 0, 255);
			break;
		case TetraminoType::Cube:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,1,0,0}
			};
			color = RGB(255, 127, 127);
			break;
		case TetraminoType::Four:
			quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{0,1,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0}
			};
			color = RGB(0, 127, 127);
			break;
		}


		initMeasurements();

		int randnum = rand() % 3;
		for (int i = 0; i < randnum; i++)
			rotate();

		pos.X = rand() % x;
		if (pos.X + width >= x)
			pos.X -= (width - 1);

	}

	bool isBlockLeft()
	{
		for (int i = 3; i >= 4 - height; i--)
		{
			for (int j = 0; j < 4 - (4 - width); j++)
			{
				if (pos.Y + i - 3 < y && pos.Y + i - 3 >= 0 && pos.X + j - 1 &&
					field[pos.Y + i - 3][pos.X + j - 1] != RGB(255, 255, 255) && quad[i][j])
				{
					return true;
				}
			}
		}
		return false;
	}

	bool isBlockRight()
	{
		for (int i = 3; i >= 4 - height; i--)
		{
			for (int j = 0; j < 4 - (4 - width); j++)
			{
				if (pos.Y + i - 3 < y && pos.Y + i - 3 >= 0 && pos.X + j + 1 &&
					field[pos.Y + i - 3][pos.X + j + 1] != RGB(255, 255, 255) && quad[i][j])
				{
					return true;
				}
			}
		}
		return false;
	}

	bool isOnBlock()
	{
		for (int i = 3; i >= 4 - height; i--)
		{
			for (int j = 0; j < 4 - (4 - width); j++)
			{
				if (pos.Y + i - 3 < y && pos.Y + i - 3 >= 0 &&
					field[pos.Y + i - 3][pos.X + j] != RGB(255, 255, 255) && quad[i][j])
				{
					return true;
				}
			}
		}
		return false;
	}

	void tofield()
	{
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < 4; j++)
			{
				if (quad[i][j])
				{
					if (pos.Y + i - 3 >= 0)
						field[pos.Y + i - 3][pos.X + j] = color;
					else
					{
						gameOver = true;
						return;
					}
				}
			}
		}
		score += 1;

		int fullrow;
		for (int i = 0; i < y; i++)
		{
			fullrow = true;
			for (int j = 0; j < x; j++)
			{
				if (field[i][j] == RGB(255, 255, 255))
				{
					fullrow = false;
					break;
				}
			}
			if (!fullrow)
				continue;
			else
			{
				score += 10;
				for (int j = i; j > 0; j--)
				{
					for (int k = 0; k < x; k++)
						field[j][k] = field[j - 1][k];
				}
			}
		}
	}

	void draw(HDC hdc, COORD coord)
	{
		HBRUSH brush = CreateSolidBrush(color);
		HPEN pen = CreatePen(0, 1, RGB(
			GetRValue(color) >= 50 ? GetRValue(color) - 50 : GetRValue(color),
			GetGValue(color) >= 50 ? GetGValue(color) - 50 : GetGValue(color),
			GetBValue(color) >= 50 ? GetBValue(color) - 50 : GetBValue(color)
		));
		SelectObject(hdc, pen);
		SelectObject(hdc, brush);
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < 4; j++)
			{
				if (quad[i][j] && coord.X + j >= 0 && coord.Y + i - 3 >= 0)
				{
					Rectangle(hdc,
						50 + (coord.X + j) * size_x,
						50 + (coord.Y + i - 3) * size_y,
						50 + size_x + (coord.X + j) * size_x,
						50 + size_y + (coord.Y + i - 3) * size_y
					);
				}
			}
		}
		DeleteObject(pen);
		DeleteObject(brush);
	}

	void drawNext(HDC hdc)
	{
		bool** next_quad = new bool* [4];
		COLORREF next_color;
		switch (next_type)
		{
		case TetraminoType::Column:
			next_quad = new bool* [4]{
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0}
			};
			next_color = RGB(255, 255, 0);
			break;
		case TetraminoType::T:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0}
			};
			next_color = RGB(255, 0, 255);
			break;
		case TetraminoType::G:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,0,0,0}
			};
			next_color = RGB(255, 0, 0);
			break;
		case TetraminoType::Four_R:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{0,1,0,0}
			};
			next_color = RGB(0, 255, 0);
			break;
		case TetraminoType::G_R:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{0,1,0,0},
				new bool[4]{0,1,0,0}
			};
			next_color = RGB(0, 0, 255);
			break;
		case TetraminoType::Cube:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{0,0,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,1,0,0}
			};
			next_color = RGB(255, 127, 127);
			break;
		case TetraminoType::Four:
			next_quad = new bool* [4]{
				new bool[4]{0,0,0,0},
				new bool[4]{0,1,0,0},
				new bool[4]{1,1,0,0},
				new bool[4]{1,0,0,0}
			};
			next_color = RGB(0, 127, 127);
			break;
		}

		HBRUSH brush = CreateSolidBrush(next_color);
		HPEN pen = CreatePen(0, 1, RGB(
			GetRValue(next_color) >= 50 ? GetRValue(next_color) - 50 : GetRValue(next_color),
			GetGValue(next_color) >= 50 ? GetGValue(next_color) - 50 : GetGValue(next_color),
			GetBValue(next_color) >= 50 ? GetBValue(next_color) - 50 : GetBValue(next_color)
		));
		SelectObject(hdc, pen);
		SelectObject(hdc, brush);
		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < 4; j++)
			{
				if (next_quad[i][j])
				{
					Rectangle(hdc,
						50 + (x + j + 4) * size_x,
						50 + (6 + i - 4) * size_y,
						50 + size_x + (x + j + 4) * size_x,
						50 + size_y + (6 + i - 4) * size_y
					);
				}
			}
		}
		DeleteObject(pen);
		DeleteObject(brush);

	}

	bool isRotationPossible()
	{
		int width = 0, height = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (quad[i][j] && j + 1 > width)
				{
					width = j + 1;
				}
				if (quad[i][j] && abs(i - 4) > height)
				{
					height = abs(i - 4);
				}
			}
		}


		bool** shr = new bool* [height];
		for (int i = 0; i < height; i++)
			shr[i] = new bool[width];

		for (int i = 3; i >= 4 - height; i--)
			for (int j = 0; j < width; j++)
				shr[i - (4 - height)][j] = quad[i][j];

		bool** shr_2 = new bool* [width];
		for (int i = 0; i < width; i++)
			shr_2[i] = new bool[height];

		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				shr_2[width - j - 1][i] = shr[i][j];

		int testquad[4][4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				testquad[i][j] = 0;

		for (int i = 3; i >= 4 - width; i--)
		{
			for (int j = 0; j < height; j++)
				testquad[i][j] = shr_2[i - (4 - width)][j];
		}

		for (int i = 0; i < height; i++)
			delete shr[i];
		delete shr;

		for (int i = 0; i < width; i++)
			delete shr_2[i];
		delete shr_2;

		for (int i = 3; i >= 0; i--)
		{
			for (int j = 0; j < height; j++)
			{
				if (testquad[i][j] && pos.Y + i - 3 >= 0 && field[pos.Y + i - 3][pos.X + j] != RGB(255, 255, 255))
					return false;
			}
		}

		return true;

	}

	void rotate()
	{

		int width = 0, height = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (quad[i][j] && j + 1 > width)
				{
					width = j + 1;
				}
				if (quad[i][j] && abs(i - 4) > height)
				{
					height = abs(i - 4);
				}
			}
		}


		bool** shr = new bool* [height];
		for (int i = 0; i < height; i++)
			shr[i] = new bool[width];

		for (int i = 3; i >= 4 - height; i--)
			for (int j = 0; j < width; j++)
				shr[i - (4 - height)][j] = quad[i][j];

		bool** shr_2 = new bool* [width];
		for (int i = 0; i < width; i++)
			shr_2[i] = new bool[height];

		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				shr_2[width - j - 1][i] = shr[i][j];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				quad[i][j] = 0;

		for (int i = 3; i >= 4 - width; i--)
		{
			for (int j = 0; j < height; j++)
				quad[i][j] = shr_2[i - (4 - width)][j];
		}

		for (int i = 0; i < height; i++)
			delete shr[i];
		delete shr;

		for (int i = 0; i < width; i++)
			delete shr_2[i];
		delete shr_2;

		initMeasurements();

	}



private:
	COLORREF color;
	bool** quad = nullptr;
	void initMeasurements()
	{
		width = 1;
		height = 1;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (quad[i][j] && j + 1 > width)
				{
					width = j + 1;
				}
				if (quad[i][j] && abs(i - 4) > height)
				{
					height = abs(i - 4);
				}
			}
		}
	}
};
#define IDT_TIMER1S 1
#define IDT_TIMER2S 2
#define IDT_TIMER3S 3
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HWND hWndProc;
tetramino t;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	GameOver(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand(time(NULL));
	field = new COLORREF * [y];
	for (int i = 0; i < y; i++)
		field[i] = new COLORREF[x];

	for (int i = 0; i < y; i++)
		for (int j = 0; j < x; j++)
			field[i][j] = RGB(255, 255, 255);

	t.next_type = (TetraminoType)(rand() % 7); //TetraminoType::Column;
	t.init();

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TETRUS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRUS));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (gameOver)
		{
			KillTimer(hWndProc, IDT_TIMER1S);
			KillTimer(hWndProc, IDT_TIMER2S);
			KillTimer(hWndProc, IDT_TIMER3S);
			if (DialogBox(hInst, MAKEINTRESOURCE(IDD_GAME_OVER), NULL, GameOver) == IDOK)
			{
				for (int i = 0; i < y; i++)
					for (int j = 0; j < x; j++)
						field[i][j] = RGB(255, 255, 255);
				gameOver = false;
				score = 0;
				SetTimer(
					hWndProc,
					IDT_TIMER1S,
					400,
					(TIMERPROC)WndProc
				);
				t.init();
				t.init();
			}
			else exit(0);
		}

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRUS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TETRUS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	hWndProc = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,				// Старые значения для размеров окна
		(x + 12) * size_x,	// 750
		(y + 6) * size_y,	// 900
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);


	if (!hWndProc)
		return FALSE;

	// Инициализация таймера
	SetTimer(
		hWndProc,
		IDT_TIMER1S,
		400,
		(TIMERPROC)WndProc
	);

	ShowWindow(hWndProc, nCmdShow);
	UpdateWindow(hWndProc);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
	{

		if (t.pos.Y >= y || t.isOnBlock())
		{
			t.pos.Y--;
			t.tofield();
			t.init();
		}
		else
		{
			t.pos.Y++;
			if (t.pos.Y >= y || t.isOnBlock())
			{
				t.pos.Y--;
				t.tofield();
				t.init();
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		if (score > 100)
		{
			KillTimer(hWnd, IDT_TIMER1S);
			SetTimer(hWnd, IDT_TIMER2S, 300, (TIMERPROC)WndProc);
		}
		if (score > 700)
		{
			KillTimer(hWnd, IDT_TIMER2S);
			SetTimer(hWnd, IDT_TIMER3S, 160, (TIMERPROC)WndProc);
		}
		break;
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_DOWN:
			t.pos.Y < y || t.isOnBlock() ? t.pos.Y++ : t.pos.Y;
			if (t.pos.Y == y || t.isOnBlock())
			{
				t.pos.Y--;
				t.tofield();
				t.init();
			}

			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_LEFT:
			if (t.isBlockLeft())
			{
				if (t.isOnBlock())
				{
					t.tofield();
					t.init();
				}
			}
			else t.pos.X > 0 ? t.pos.X-- : t.pos.X;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RIGHT:
			if (t.isBlockRight())
			{
				if (t.isOnBlock())
				{
					t.tofield();
					t.init();
				}
			}
			else t.pos.X < x - t.width ? t.pos.X++ : t.pos.X;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_UP:
			if (t.isRotationPossible())
				t.rotate();
			else
			{
				t.pos.X -= abs(t.height - t.width);
				if (t.isRotationPossible())
					t.rotate();
				else t.pos.X += abs(t.height - t.width);
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case VK_RETURN:
			KillTimer(hWnd, IDT_TIMER1S);
			for (int i = 0; i < y; i++)
				for (int j = 0; j < x; j++)
					field[i][j] = RGB(255, 255, 255);
			gameOver = false;
			score = 0;
			SetTimer(
				hWndProc,
				IDT_TIMER1S,
				400,
				(TIMERPROC)WndProc
			);
			t.init();
			t.init();
			break;
		}
	}
	break;
	case WM_PAINT:
	{

		//mainWnd = hWnd;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HPEN pen = CreatePen(0, 1, RGB(0, 0, 0));
		SelectObject(hdc, pen);
		RECT r = { 50, 50, 75, 75 };
		//RECT full = { r.left, r.top, r.right + 25 * x, r.bottom + 25 * y };

		Rectangle(hdc, r.left, r.top, r.right + 25 * (x - 1), r.bottom + 25 * (y - 1));

		for (int i = 0; i < x; i++)
			for (int j = 0; j < y; j++)
			{
				if (field[j][i] != RGB(255, 255, 255))
				{
					HBRUSH brush = CreateSolidBrush(field[j][i]);
					SelectObject(hdc, brush);
					Rectangle(hdc, r.left + 25 * i, r.top + 25 * j, r.right + 25 * i, r.bottom + 25 * j);
					DeleteObject(brush);
				}
			}

		r = { 25 * (x + 6), 75, 25 * (x + 9), 100 };
		DrawText(hdc, std::to_wstring(score).c_str(), std::to_wstring(score).length(), &r, DT_SINGLELINE | DT_VCENTER);
		DeleteObject(pen);

		t.drawNext(hdc);
		t.draw(hdc, t.pos);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER1S);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Продолжать ли игру?
INT_PTR CALLBACK GameOver(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:

		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDOK:
			EndDialog(hDlg, wmId);
			return (INT_PTR)TRUE;
		case IDNO:
			EndDialog(hDlg, wmId);
			return (INT_PTR)FALSE;
		default:
			return (INT_PTR)FALSE;
		}
	}
	return (INT_PTR)FALSE;
}