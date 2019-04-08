#include "AcceptConnections.h"

void AcceptConnectionsThread(ULONG IP, BOOL &Work, PList &ClientList, std::mutex &mtx, char* &Nickname)
{
	int Res = 0;
	SOCKET ListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSock == INVALID_SOCKET)
	{
		mtx.lock();

		Work = FALSE;
		printf_s("Unable to accept incoming connections");

		mtx.unlock();

		getchar();
		return;
	}

	sockaddr_in Adr;
	const int AdrLen = sizeof(Adr);
	Adr.sin_family = AF_INET;
	Adr.sin_port = htons((USHORT)DEFAULT_PORT);
	Adr.sin_addr.s_addr = IP;

	Res = bind(ListenSock, (SOCKADDR*)&Adr, AdrLen);
	if (Res == SOCKET_ERROR)
	{
		mtx.lock();

		Work = FALSE;
		printf_s("bind failed with error: %d\n", WSAGetLastError());

		mtx.unlock();

		getchar();
		return;
	}

	Res = listen(ListenSock, SOMAXCONN);
	if (Res == SOCKET_ERROR)
	{
		mtx.lock();

		Work = FALSE;
		printf_s("listen failed with error: %d\n", WSAGetLastError());

		mtx.unlock();

		getchar();
		return;
	}

	std::vector <std::thread> ThreadPull;
	UINT i = 0;

	fd_set TCPConnection = { 1, ListenSock };
	timeval Delay = { 5, 0 };
	PList NewClient = NULL;
 
	while (Work)
	{
		Res = select(0, &TCPConnection, NULL, NULL, &Delay);
		if (Res == 0)
		{
			TCPConnection.fd_count = 1;
			TCPConnection.fd_array[0] = ListenSock;
			continue;
		}

		if (Res > 0)
		{
			NewClient = (PList)calloc(1, sizeof(List));
			NewClient->Socket = accept(ListenSock, (SOCKADDR*)&Adr, (int*)&AdrLen);
			if (NewClient->Socket == INVALID_SOCKET)
			{
				closesocket(NewClient->Socket);
				free(NewClient);
			}
			else
			{
				NewClient->IP = Adr.sin_addr.s_addr;

				mtx.lock();

				ListAdd(&ClientList, &NewClient);

				mtx.unlock();

				ThreadPull.push_back(std::thread(ReciveData, std::ref(Work), std::ref(ClientList->Last), std::ref(ClientList), std::ref(mtx), std::ref(Nickname)));
				ThreadPull[i++].detach();
			}
		}
	}
}