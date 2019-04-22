#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>

using namespace std;

int main() {
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
		printf("start main\n");
		int status1, status2;
		printf("wait compile pr1.c & pr2.c\n");
		waitpid(pid_son1, &status1, WUNTRACED);
		waitpid(pid_son2, &status2, WUNTRACED);
		printf("end wait compile pr1.c & pr2.c\n");

		if (status2) {
			printf("compile pr1.c or pr2.c had been ended with error\n");
			return 1;
		}

		printf("open prall.c\n");
		int fd = open("prall.c", O_CREAT|O_RDWR|O_TRUNC, 0644);
		printf("close stadart output\n");	
		printf("dublicate discriptor of prall.c to standart output discritor\n");
		printf("cat pr1.c and pr2.c in prall.c\n");
		close(1);
		fcntl(fd, F_DUPFD, 1);
		execlp("cat", "cat", "pr1.c", "pr2.c", 0);

		printf("end\n");
		return 0;
	}

	if (iAmFirstSon) {
		printf("start compile pr1.c\n");
		return execlp("cc", "cc", "pr1.c", 0);
	}

	if (iAmSecondSon) {
		printf("start compile pr2.c\n");
		return execlp("cc", "cc", "pr2.c", 0);
	}
}