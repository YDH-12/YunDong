#include "Common.h"
#include <string>

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2

int main(int argc, char* argv[]) {
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
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

    // ������ ��ſ� ����� ����
    SOCKET client_sock1, client_sock2;
    struct sockaddr_in clientaddr1, clientaddr2;
    int addrlen;
    char buf[BUFSIZE + 1];
    bool game_start_count = false; // �̰� true�� �Ǹ� ���� ���۵� ���� �˸�
    bool client_sock1_connect = false; // Ŭ���̾�Ʈ 1 ���� Ȯ��
    bool client_sock2_connect = false; // Ŭ���̾�Ʈ 2 ���� Ȯ��
    bool start_btn = true; // ���� ���۰�
    int client_count = 0; // 2�� �÷��̱����� �ϵ��� �����ϱ� ����
    std::string ip; // Ŭ���̾�Ʈ���� �ִ� ip �ѹ��� �ʿ�
    unsigned int nMessageID = 0; // Ŭ���̾�Ʈ���� �ִ� ��ư�� �Է� �ѹ��� �ʿ�

    while (1) {
        // accept()
        // Ŭ���̾�Ʈ 1 ����
        if (!client_sock1_connect && client_count < MAX_CLIENT_COUNT) {
            addrlen = sizeof(clientaddr1);
            client_sock1 = accept(listen_sock, (struct sockaddr*)&clientaddr1, &addrlen);
            if (client_sock1 == INVALID_SOCKET) {
                err_display("accept()");
                break;
            }

            // ������ Ŭ���̾�Ʈ ���� ���
            char addr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientaddr1.sin_addr, addr, sizeof(addr));
            printf("\n[TCP ����] Ŭ���̾�Ʈ 1 ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr1.sin_port));

            retval = recv(client_sock1, buf, BUFSIZE, 0); // ��ư ���� �� Ȯ��
            buf[retval] = '\0';

            printf("%s\n", buf);
            
       

            client_sock1_connect = true; // Ŭ���̾�Ʈ 1 ���� �Ϸ�
            client_count++; // Ŭ���̾�Ʈ 1 ���� �� 1 ����
            printf(" % d", client_count);
            // �� ��° Ŭ���̾�Ʈ ���� ���
            if (client_count == 1) {
                printf("\nŬ���̾�Ʈ 1�� �����Ͽ����ϴ�. Ŭ���̾�Ʈ 2�� ��ٸ��ϴ�...\n");
            }
        }

        // Ŭ���̾�Ʈ 2 ����
        if (!client_sock2_connect && client_count == 1) {
            addrlen = sizeof(clientaddr2);
            client_sock2 = accept(listen_sock, (struct sockaddr*)&clientaddr2, &addrlen);
            if (client_sock2 == INVALID_SOCKET) {
                err_display("accept() failed for client 2");
                break;
            }

            // �� ��° Ŭ���̾�Ʈ ���� ���� ���
            char addr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientaddr2.sin_addr, addr, sizeof(addr));
            printf("\n[TCP ����] Ŭ���̾�Ʈ 2 ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr2.sin_port));

            retval = recv(client_sock2, buf, BUFSIZE, 0); // ��ư ���� �� Ȯ��
            buf[retval] = '\0';
            printf("%s\n", buf);

            client_sock2_connect = true; // Ŭ���̾�Ʈ 2 ���� �Ϸ�
            client_count++; // Ŭ���̾�Ʈ 2 ���� �� 1 ����
            printf(" % d", client_count);
            // ���� ���� ���� �� Ŭ���̾�Ʈ�� ������
            if (client_sock2_connect = true&&client_count == 2) {
                snprintf(buf, sizeof(buf), "%d", start_btn);
                retval = send(client_sock1, buf, strlen(buf), 0);
                printf("Ŭ��1 ���ۿϷ�\n");
                retval = send(client_sock2, buf, strlen(buf), 0);
                printf("Ŭ��2 ���ۿϷ�\n");
                game_start_count = true;
            }

            // �� Ŭ���̾�Ʈ ��� ���������Ƿ� ���� ���� ���
            printf("\nŬ���̾�Ʈ 2�� �����Ͽ����ϴ�. ������ �����մϴ�.\n");
        }

        // ���� ���� ���� (���⼭ ���� ���� �߰� ����)
        if (client_count == MAX_CLIENT_COUNT) {
            // ���� ���� �� Ŭ���̾�Ʈ�� ������ ����� ���� �����峪 ������ �߰��� �� �ֽ��ϴ�.
            // recv_thread(); // ���⿡ ������ ���� ó��
            // send_thread(); // ���⿡ ������ �۽� ó��
            break; // ���÷� ���� ���� �� ������ �����ϴ� �ڵ�
        }
    }

    // ���� �ݱ�
    closesocket(client_sock1);
    closesocket(client_sock2);
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}