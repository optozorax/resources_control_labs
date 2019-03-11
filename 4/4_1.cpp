#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

using namespace std;

int main() {
	char* myfifo1 = "/tmp/myfifo1";
	char* myfifo2 = "/tmp/myfifo2";

	double lambda = 1;
	int k = 5;
	cout << "Enter k: ";
	cin >> k;
	cout << "Enter lambda: ";
	cin >> lambda;

	// Создаем потомков
	int pid_son1 = fork();
	int pid_son2 = 0;
	bool iAmFather = pid_son1 != 0;
	bool iAmFirstSon = false;
	bool iAmSecondSon = false;
	if (iAmFather) {
		pid_son2 = fork();
		iAmFather &= pid_son2 != 0;
		if (!iAmFather) {
			iAmSecondSon = true;
		}
	} else {
		iAmFirstSon = true;
	}

	if (iAmFather) {
		double factorial = 0;
		double exponent = 0;

		// Создаем именованный канал для экспоненты и считываем данные из него
		{
			mkfifo(myfifo1, 0666);
			int fd = open(myfifo1, O_RDONLY);
			char buf[500] = {};
			int readed = read(fd, (void*)(buf), 500);
			close(fd);
			unlink(myfifo1);

			stringstream sout;
			sout << buf;
			sout >> exponent;
		}

		// Создаем именованный канал для факториала и считываем данные из него
		{
			mkfifo(myfifo2, 0666);
			int fd = open(myfifo2, O_RDONLY);
			char buf[500] = {};
			int readed = read(fd, (void*)(buf), 500);
			close(fd);
			unlink(myfifo2);

			stringstream sout;
			sout << buf;
			sout >> factorial;
		}

		double result = pow(lambda, k) * exponent/factorial;

		cout << "result: " << result << endl;
	}

	if (iAmFirstSon) {
		execl("exp", "exp", to_string(lambda).c_str(), myfifo1, 0);
	}

	if (iAmSecondSon) {
		execl("fact", "fact", to_string(k).c_str(), myfifo2, 0);	
	}
}