#define UNICODE
#include <windows.h>

//-----------------------------------------------------------------------------
// Размеры окна
int width = 400;
int height = 150;

//-----------------------------------------------------------------------------
// Строка, где будет записано то, что отобразится на экране
wchar_t Answer[1024];

//-----------------------------------------------------------------------------
// Функция, в которой запускается необходимая функция и выводится информация в строку
DWORD WINAPI ThreadFunc(void *) {
	typedef int (*ImportFunction)(wchar_t *, int);
	wchar_t String[1024];
	ImportFunction DLLInfo;

	// Считываем dll'ку
	HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));

	// Получаем необходимую функцию
	DLLInfo = (ImportFunction) GetProcAddress(hinstLib, "Information");

	// Вызываем эту функцию
	DLLInfo(String, 1024);

	// Освобождаем ресурсы библиотеки
	FreeLibrary(hinstLib);

	// Вывод информации в строку
	wsprintf(Answer, L"Windows directory path: %s\n\nYou can move this window by holding left mouse button.\n\nPress Esc to close the window.", String);

	return 0;
}

//-----------------------------------------------------------------------------
// Функция обработки сообщений главного окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	// Создаем шрифт для отображения текста
	HFONT hFont = CreateFont(20, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));

    switch (msg) {
    	// Обрабатываем сообщение, приходящее после создания окна
    	case WM_CREATE: {
    		HANDLE hThread;
			DWORD IDThread;

			// Создаем поток, где вычислется необходимая информация
    		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			WaitForSingleObject(hThread, INFINITE); // Ожидаем закрытия потока
			CloseHandle(hThread);
    	} break;

    	// Закрытие окна
		case WM_DESTROY: {
			PostQuitMessage(0);
		} break;


    	// Обрабатываем сообщение об отрисовке окна
    	case WM_PAINT: {
    		hdc = BeginPaint(hwnd, &ps);
    		PatBlt(hdc, 0, 0, width, height, WHITENESS); // Закрашиваем всё окно белым цветом
			SelectObject(hdc, hFont); // Меняем шрифт
			RECT rc;
			SetRect(&rc,10,10,width-10, height-10); // Устанавливаем границы для отображения текста

			// Отображаем текст в заданном прямоугольнике. При помощи этой функции автоматически расставляются переносы слов и учитывается перенос на новую строку.
			DrawText(hdc, Answer, wcslen(Answer), &rc, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
			EndPaint(hwnd, &ps);
    	} break;

    	// Закрываем окно, если была нажата клавиша Esc
		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);
		} break;

		// Обрабатываем это сообщение, чтобы при клике в любое место окна, оно воспринималось как часть заголовка, из чего следует, что окно можно перемммещать, просто зажав левую кнопку мыши на любой части окна.
		case WM_NCHITTEST: {
			return HTCAPTION;
		} break;
    }

    // Для всех остальных сообщений вызываем стандартный обработчик с стандартным поведением
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Главная функция, которая запускается при запуске программы
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Создаем и регистрируем класс окна
	WNDCLASSEX wcl = {0};
	wcl.cbSize 			= sizeof(wcl);
	wcl.lpfnWndProc 	= WindowProc;
	wcl.hInstance 		= hInstance;
	wcl.hIcon 			= LoadIcon(0, IDI_APPLICATION);
	wcl.hCursor 		= LoadCursor(0, IDC_ARROW);
	wcl.lpszClassName 	= L"OwnMovedWindow";
	if (!RegisterClassEx(&wcl))
	    return 1;

	// Создаем Popup окно - т. е. окно, у которого нет границ, заголовка и кнопок закрытия, открытия. Делаем его положение на переднем плане.
	HWND mainHwnd = CreateWindowEx(WS_EX_TOPMOST, wcl.lpszClassName, L"Windows path information", WS_POPUP, 200, 200, width, height, 0, 0, wcl.hInstance, 0);

	// Отображаем окно
	ShowWindow(mainHwnd, nShowCmd);
	UpdateWindow(mainHwnd);

	//-------------------------------------------------------------------------
	MSG msg;
	bool bRet;

	// Цикл обработки сообщений созданного окна
	while((bRet = GetMessage(&msg, mainHwnd, 0, 0)) != 0) { 
		if (bRet != -1) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}

	return 0;
}