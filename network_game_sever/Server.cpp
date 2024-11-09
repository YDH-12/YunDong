#include "Common.h"
#include <string>



#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock1;
	SOCKET client_sock2;
	struct sockaddr_in clientaddr1, clientaddr2;
	int addrlen;
	char buf[BUFSIZE + 1];
	bool game_start_count = false;// 이게 true가 되면 게임 시작된것을 알림
	bool client_soket1_connect = false;//클라 연결 확인
	bool client_soket2_connect = false;//클라 연결 확인
	bool start_btn = true;//게임 시작값
	int client_count = 0;//2인 플레이까지만 하도록 제한하기 위함
	std::string ip;//클라에서 주는 ip 한번만 필요
	unsigned int nMessageID;// 클라에서 주는 버튼값 입력 한번만 필요


	while (1) {
		// accept()
		// 
		// accept client 1
		if(!client_soket1_connect && client_count < MAX_CLIENT_COUNT){
			addrlen = sizeof(clientaddr1);
			client_sock1 = accept(listen_sock, (struct sockaddr*)&clientaddr1, &addrlen);
			if (client_sock1 == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			
			// 접속한 클라이언트 정보 출력
			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientaddr1.sin_addr, addr, sizeof(addr));
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				addr, ntohs(clientaddr1.sin_port));

			retval = recv(client_sock1, buf, BUFSIZE, 0);// 버튼 누른거 확인
			buf[retval] = '\0';

			printf("%s\n", buf);
			if (sscanf(buf, "IP=%s, nMessageID=%x", ip, nMessageID) == 2) {
				if (nMessageID == WM_LBUTTONDOWN) {
					client_soket1_connect = true;
					client_count++;
					printf("\n[TCP 서버] 첫 번째 클라이언트 접속: IP 주소=%s, nMessageID=0x%x\n", ip, nMessageID);
				}

		}
		
		//accept client 2
		if (client_soket1_connect && !client_soket2_connect && client_count < MAX_CLIENT_COUNT) {
			addrlen = sizeof(clientaddr2);
			client_sock2 = accept(listen_sock, (struct sockaddr*)&clientaddr2, &addrlen);
			if (client_sock2 == INVALID_SOCKET) {
				err_display("accept() failed for client 2");
				break;
			}

			
			// 두 번째 클라이언트 접속 정보 출력
			char addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &clientaddr2.sin_addr, addr, sizeof(addr));
			printf("\n[TCP 서버] 두 번째 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				addr, ntohs(clientaddr2.sin_port));

			retval = recv(client_sock2, buf, BUFSIZE, 0);// 버튼 누른거 확인
			
			buf[retval] = '\0';
			printf("%s\n", buf);
			if (sscanf(buf, "IP=%s, nMessageID=%x", ip, nMessageID) == 2) {//클라에서 어떤식으로 보낼지 보고 수정 필요 위에도 마찬가지
				if (nMessageID == WM_LBUTTONDOWN) {
					client_soket2_connect = true;
					client_count++;
					printf("\n[TCP 서버] 두 번째 클라이언트 접속: IP 주소=%s, nMessageID=0x%x\n", ip, nMessageID);
				}
		}
		
		
		//게임 시작 조건 양 클라로 보내기
		if (client_soket2_connect && !game_start_count) {
			snprintf(buf, sizeof(buf), "%d", start_btn);
			retval = send(client_sock1, buf, strlen(buf),0);
			retval = send(client_sock2, buf, strlen(buf),0);
			game_start_count = true;
		}
		//UDP부분 만들어야함

		// 클라이언트와 데이터 통신
		while (1) {
			recv_thread//이부분 만들어야함
			send_thread
		}

		// 소켓 닫기
		closesocket(client_sock1);
		closesocket(client_sock2);
	}

	// 소켓 닫기
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

void recv_thread;
void send_thread;