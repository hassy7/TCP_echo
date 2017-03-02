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
	struct sockaddr_in server;
	int sock;
	in_port_t port = atoi(argv[2]);
	in_addr_t addr = inet_addr(argv[1]);
	struct echo_msg c_msg;
	int count, i;
	socklen_t sktlen;
	c_msg.seq = 0;

	/* ソケットの作成 */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	/* 接続先指定用構造体の準備 */
	server.sin_family = AF_INET;
	server.sin_port = port;
	server.sin_addr.s_addr= addr;

	/* サーバに接続 */

	connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf("connected\n");
	/* サーバからデータを受信 */
	//memset(buf, 0, sizeof(buf));
	//n = read(sock, buf, sizeof(buf));

	//printf("%d, %s\n", n, buf);
	for (i = 0; i < 10; i++) {
		if (i < 9) {
			printf("Input message : ");
			fgets(c_msg.msg, 32, stdin);
			c_msg.msg[strlen(c_msg.msg) - 1] = '\0';
		} else {
			strcpy(c_msg.msg, "FIN");
		}
		printf("inputed\n");


		sktlen = sizeof sock;
		if ((count = send(sock, &c_msg, sizeof c_msg, 0)) < 0) {

			perror("sendto");
			exit(1);
		}
		if ((count = recv(sock, &c_msg, sizeof c_msg, 0)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		printf("%s\n", c_msg.msg);
		if (strcmp(c_msg.msg, "FIN") == 0) break;
	}
	/* socketの終了 */
	close(sock);

	return 0;
}
