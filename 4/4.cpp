#include <iostream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main() {
	double lambda = 1;
	int k = 5;
	//cin >> k;

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

	//cout << iAmFather << " " << iAmFirstSon << " " << iAmSecondSon << " | " << pid_son1 << " " << pid_son2 << " my pid: " << getpid() << endl;
	if (iAmFather) {
		int a;
		wait(&a);

		double factorial = 0;
		double exponent = 0;
		ifstream fin("a.txt");
		fin >> factorial;
		fin.close();

		fin.open("b.txt");
		fin >> exponent;
		fin.close();

		double result = pow(lambda, k) * exponent/factorial;

		cout << "result: " << result << endl;
	}

	if (iAmFirstSon) {
		// Вычисляем факториал
		int mul = 1;
		for (int i = 1; i <= k; ++i) {
			mul *= i;
		}

		ofstream fout("a.txt");
		fout << mul;
		fout.close();

		cout << "factorial: " << mul << endl;
	}

	if (iAmSecondSon) {
		// Вычисляем экспоненту
		// ...
		double exponent = exp(-lambda);

		ofstream fout("b.txt");
		fout << exponent;
		fout.close();

		cout << "exponent: " << exponent << endl;
	}
}