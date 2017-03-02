#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct echo_msg {
	unsigned int seq;
	unsigned int reserve;
	char msg[32];
};

int main(int argc, char *argv[])
{
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	struct echo_msg s_msg;
	unsigned len;
	int sock = 0;
	int count;
	in_port_t port = atoi(argv[1]);
	socklen_t sktlen;
	int pid, status;


	/* ソケットの作成 */
	sock0 = socket(AF_INET, SOCK_STREAM, 0);

	/* ソケットの設定 */
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

	/* TCPクライアントからの接続要求を待てる状態にする */
	listen(sock0, 5);
	for (;;) {
		//generate child process
		pid = fork();
		//pidが0より小さいなら
		if (pid < 0) {
			perror("fork");
			exit(-1);
		} else if (pid == 0) {
			/* TCPクライアントからの接続要求を受け付ける */
			len = sizeof(client);
			sock = accept(sock0, (struct sockaddr *)&client, &len);
			for (;;) {
				//受信待ち
				sktlen = sizeof sock;
				if ((count = recv(sock, &s_msg, sizeof s_msg, 0)) < 0) {
					perror("recvfrom");
					exit(1);
				}
				/* 5文字送信 */
				//write(sock, "HELLO", 5);
				printf("seq : %d, msg : %s\n", s_msg.seq, s_msg.msg);
				s_msg.seq = s_msg.seq + 1;
				if ((count = send(sock, &s_msg, sizeof s_msg, 0)) < 0) {
					perror("sendto");
					exit(1);
				}
				printf("%s\n", s_msg.msg);
				if (strcmp(s_msg.msg, "FIN") == 0 || s_msg.seq >= 10) break;
			}
			exit(-1);
		} else {
			//親プロセス
			wait(&status);
		}
		/* TCPセッションの終了 */
		close(sock);
	}

	/* listen するsocketの終了 */
	close(sock0);

	return 0;
}
