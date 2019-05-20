#include <windows.h>

// Функция динамической библиотеки
extern "C" _declspec(dllexport) void Information(wchar_t* Buffer, int BufferSize) {
	// Определяем директорию Windows
	GetWindowsDirectoryW(Buffer, BufferSize);
}