#include "ReciveBroadcastUDP.h"

void ListenBroadcastUDPThread(ULONG IP, BOOL &Work, PList &ClientList, std::mutex &mtx, char* &Nickname)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf_s("Socket failed with error %d\n", WSAGetLastError());
		getchar();
		return;
	}

	int Res;

	sockaddr_in Adr;
	Adr.sin_family = AF_INET;
	Adr.sin_port = htons((USHORT)DEFAULT_PORT);
	Adr.sin_addr.s_addr = IP;

	Res = bind(sock, (SOCKADDR *)&Adr, sizeof(Adr));
	if (Res == SOCKET_ERROR)
	{
		mtx.lock();

		Work = FALSE;
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(sock);

		mtx.unlock();

		getchar();
		return;
	}

	std::vector <std::thread> ThreadPull;
	UINT i = 0;

	PList NewClient = NULL;
	timeval Delay = { 5, 0 };
	fd_set UDPSocket = { 1, sock };

	while (Work)
	{

		Res = select(0, &UDPSocket, NULL, NULL, &Delay);
		if (Res == 0)
		{
			UDPSocket.fd_count = 1;
			UDPSocket.fd_array[0] = sock;
			continue;
		}

		if (Res > 0)
		{
			NewClient = (PList)calloc(1, sizeof(List));
			recv(sock, (char*)&NewClient->IP, sizeof(ULONG), 0);

			Adr.sin_family = AF_INET;
			Adr.sin_port = htons((USHORT)DEFAULT_PORT);
			Adr.sin_addr.s_addr = NewClient->IP;
			
			NewClient->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			Res = connect(NewClient->Socket, (SOCKADDR*)&Adr, sizeof(Adr));
			if (Res == SOCKET_ERROR)
			{
				closesocket(NewClient->Socket);
				free(NewClient);
			}
			else
			{
				mtx.lock();

				ListAdd(&ClientList, &NewClient);

				mtx.unlock();

				ThreadPull.push_back(std::thread(ReciveData, std::ref(Work), std::ref(NewClient), std::ref(ClientList), std::ref(mtx), std::ref(Nickname)));
				ThreadPull[i++].detach();
			}
		}
	}
}