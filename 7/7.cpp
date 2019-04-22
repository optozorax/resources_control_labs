#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/sem.h> 

using namespace std;

struct message_buf {
	long mtype;
	int prog;
};


void enter_semaphore() {
	// get semaphore
	static const key_t semkey = 777;		
	const int semid = semget(semkey, 1, IPC_CREAT | 0666);
	
	// consider enter in critical zone
							// sem_number, operate, flags
	struct sembuf op_array = { sembuf({0, -1, 0}) };
	semop(semid, &op_array, 1);
}

void leave_semaphore() {
	// get semaphore
	static const key_t semkey = 777;		
	const int semid = semget(semkey, 1, IPC_CREAT | 0666);
	
	// consider enter in critical zone
							// sem_number, operate, flags
	struct sembuf op_array = { sembuf({0, 1, 0}) };
	semop(semid, &op_array, 1);
}

int main() {
    const int prog_count = 5;
    const int max_progs = 10;
    
	// create semaphore
	const key_t semkey = 777;		
	const int semid = semget(semkey, 1, IPC_CREAT | 0666);
	if(semid == -1) ;// error

	// semaphore init
	const int sem_restrict = 3; // amount process in critical zone
	semctl(semid, 0, SETVAL, sem_restrict);

	vector<int> first_progs;
	vector<int> second_progs;


	key_t key = 2234;
	int msqid;

	if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
		perror("msgget");
		exit(1);
	}



	    msqid_ds dsbuf;
    msgctl(msqid, IPC_STAT, &dsbuf);
   	cout << dsbuf.msg_qnum << endl;

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

		message_buf rbuf;
	    //
		msqid = msgget(key, 0666);


		int status1, status2;

		while (true) {
			if (msgrcv(msqid, &rbuf, sizeof(int), 1, IPC_NOWAIT) > 0)
				cout << "GET OLOLOL: " << rbuf.mtype << " " << rbuf.prog << endl;
			if (msgrcv(msqid, &rbuf, sizeof(int), 2, IPC_NOWAIT) > 0)
				cout << "GET OLOLOL: " << rbuf.mtype << " " << rbuf.prog << endl;
		}

		// delete message queue
		msgctl(msqid, IPC_RMID, &dsbuf);

		// delete semaphore
		semctl(semid, 0, IPC_RMID);

		return 0;
	}

	if (iAmFirstSon) {
		srand(5);

		//for(int i = 0; i < 5; i++) {
		while (true) {
		    sleep(rand()%3 + 1);

		    message_buf buf;
		    buf.mtype = 1;
		    buf.prog = rand()%prog_count; 
		    if((msgsnd(msqid, &buf, sizeof(int), IPC_NOWAIT)) < 0){
				perror("msgsnd");
				exit(1);
			} //else
				//printf("Message: \"%d\" Sent\n", buf.prog);

		}
	}

	if (iAmSecondSon) {
		srand(123);

		//for(int i = 0; i < 5; i++) {
		while (true) {
		    sleep(rand()%3 + 1);

		    message_buf buf;
		    buf.mtype = 2;
		    buf.prog = rand()%prog_count; 
		    if((msgsnd(msqid, &buf, sizeof(int), IPC_NOWAIT)) < 0){
				perror("msgsnd");
				exit(1);
			} 
				//printf("Message: \"%d\" Sent\n", buf.prog);
		}
	}
}