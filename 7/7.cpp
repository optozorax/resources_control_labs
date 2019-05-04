#include <iostream>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/sem.h> 
#include <semaphore.h>

using namespace std;

struct message_buf {
	long mtype;
	int prog;
};

int semid;

//-----------------------------------------------------------------------------
void enter_semaphore() {	
	// consider enter in critical zone
	struct sembuf op_array = { sembuf({0, -1, 0}) }; // sem_number, operate, flags
	semop(semid, &op_array, 1);
}

//-----------------------------------------------------------------------------
void leave_semaphore() {
	// consider enter in critical zone
	struct sembuf op_array = { sembuf({0, 1, 0}) }; // sem_number, operate, flags
	semop(semid, &op_array, 1);
}

//-----------------------------------------------------------------------------
void generate_program_messages(int type, int msqid, int prog_count, int sleep_sum) {
	while (true) {
		sleep(rand() % 2 + sleep_sum);

		message_buf buf;
		buf.mtype = type;
		buf.prog = rand() % prog_count; 
		if((msgsnd(msqid, &buf, sizeof(int), IPC_NOWAIT)) < 0){
			perror("msgsnd");
			exit(1);
		}
	}
}

//-----------------------------------------------------------------------------
void run_prog(int type, int prog) {
	if (fork() == 0) {
		enter_semaphore();
		cout << type << " run program: " << prog << endl;
		sleep(rand() % 3 + 3);
		cout << "End program: " << prog << endl;
		leave_semaphore();
		exit(0);
	} else {
		rand();
		usleep(1000);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main() {
	const int prog_count = 5;
	const int max_progs = 10;
	
	//-------------------------------------------------------------------------
	// create semaphore
	const key_t semkey = 777;
	semid = semget(semkey, 1, IPC_CREAT | 0666);
	if(semid == -1) ;// error

	// semaphore init
	const int sem_restrict = 3; // amount process in critical zone
	semctl(semid, 0, SETVAL, sem_restrict);

	//-------------------------------------------------------------------------
	key_t key = 2234;
	int msqid;
	if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
		perror("msgget");
		exit(1);
	}

	msqid_ds dsbuf;
	msgctl(msqid, IPC_STAT, &dsbuf);

   	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	// Главный поток
	if (iAmFather) {
		message_buf rbuf;
		msqid = msgget(key, 0666);

		vector<int> first_progs;
		vector<int> second_progs;

		int status1, status2;

		while (true) {

			if (msgrcv(msqid, &rbuf, sizeof(int), 1, IPC_NOWAIT) > 0) {
				cout << "Receive message from: " << 1 << ", to run program: " << rbuf.prog << endl;
				first_progs.push_back(rbuf.prog);
			}
			if (msgrcv(msqid, &rbuf, sizeof(int), 2, IPC_NOWAIT) > 0) {
				cout << "Receive message from: " << 2 << ", to run program: " << rbuf.prog << endl;
				second_progs.push_back(rbuf.prog);
			}

			while (!first_progs.empty() && semctl(semid, 0, GETVAL, 0) > 0) {
				//cout << 1 << " add " << first_progs.front() << " to run, when semaphore: " << semctl(semid, 0, GETVAL, 0) << endl;
				run_prog(1, first_progs.front());
				//cout << "After adding semaphore value is: " << semctl(semid, 0, GETVAL, 0) << endl;
				first_progs.erase(first_progs.begin());
			}

			while (first_progs.empty() && !second_progs.empty() && semctl(semid, 0, GETVAL, 0) > 0) {
				//cout << 2 << " add " << second_progs.front() << " to run, when semaphore: " << semctl(semid, 0, GETVAL, 0) << endl;
				run_prog(2, second_progs.front());
				//cout << "After adding semaphore value is: " << semctl(semid, 0, GETVAL, 0) << endl;
				second_progs.erase(second_progs.begin());
			}
		}

		// delete message queue
		msgctl(msqid, IPC_RMID, &dsbuf);

		// delete semaphore
		semctl(semid, 0, IPC_RMID);

		return 0;
	}

	//-------------------------------------------------------------------------
	// Поток первого потомка
	if (iAmFirstSon) {
		srand(5); /// Для того, чтобы у потомков различались случайные числа
		generate_program_messages(1, msqid, prog_count, 2);
	}

	//-------------------------------------------------------------------------
	// Поток второго потомка
	if (iAmSecondSon) {
		srand(123); /// Для того, чтобы у потомков различались случайные числа
		generate_program_messages(2, msqid, prog_count, 1);
	}
}