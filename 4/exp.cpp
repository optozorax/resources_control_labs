#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[]) {
	// Получаем входные параметры
	stringstream sout;
	sout << argv[1];
	double lambda;
	sout >> lambda;

	// Вычисляем экспоненту
	double exponent = exp(lambda);

	// Передаем данные главному процессу
	int pin = open(argv[2], O_WRONLY); 
	string str = to_string(exponent);
	write(pin, str.c_str(), str.size());
	close(pin);

	return 0;
}