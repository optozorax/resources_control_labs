#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

void ignore(int) { }

int main() {
	signal(SIGUSR1, ignore);
	int fd[2];
	pipe(fd);
	int fread = fd[0], fwrite = fd[1];

	printf("Make pipe\n");

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
		printf("start 0\n");
		int status1, status2;
		waitpid(pid_son1, &status1, WUNTRACED);
		waitpid(pid_son2, &status2, WUNTRACED);
		char buf[500];

		printf("0 after wait 1 and 2\n");

		int readed = read(fread, buf, 500);
		printf("readed: %d\n", readed);
		for (int i = 0; i < readed/44; i++) {
			int a = *((int32_t*)(buf+i*44));
			printf("%d: %s\n", a, buf + i*44 + 4);
		}
		printf("end 0\n");
	}

	if (iAmFirstSon) {
		printf("start 1\n");
		sigpause(SIGUSR1);
		printf("start 1 after signal\n");

		printf("write 1 in pipe\n");
		char buf1[4] = {getpid()};
		char buf2[40] = "hello from 1";
		write(fwrite, buf1, 4);
		write(fwrite, buf2, 40);

		printf("end 1\n");
	}

	if (iAmSecondSon) {
		printf("start 2 \n");

		printf("write 2 in pipe\n");
		char buf1[4] = {getpid()};
		char buf2[40] = "hello from 2";
		write(fwrite, buf1, 4);
		write(fwrite, buf2, 40);

		printf("send signal from 2 to 1\n");
		kill(pid_son1, SIGUSR1);

		printf("end 2\n");
	}

	return 1;
}