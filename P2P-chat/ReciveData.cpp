#include "ReciveData.h"

void ReciveData(BOOL &Work, PList &Client, PList &ClientList, std::mutex &mtx, char* &Nickname)
{
	int Res;
	char *ReciveBuff = (char*)calloc(RECIVE_BUFFER_SIZE, sizeof(char));

	Res = send(Client->Socket, Nickname, strlen(Nickname) + 1, 0);

	Res = recv(Client->Socket, ReciveBuff, RECIVE_BUFFER_SIZE, 0);
	if (Res == SOCKET_ERROR)
	{
		Res = WSAGetLastError();
		mtx.lock();

		printf_s("Unnamed user left chat\n");
		shutdown(Client->Socket, SD_BOTH);
		closesocket(Client->Socket);
		ListDelete(&ClientList, &Client);

		mtx.unlock();

		free(ReciveBuff);
		return;
	}
	else
	{
		mtx.lock();

		Client->Nickname = (char*)calloc(Res, sizeof(char));
		memcpy_s(Client->Nickname, Res, ReciveBuff, Res);
		printf_s("User \"%s\" join chat\n", Client->Nickname);

		mtx.unlock();
	}

	while (Work)
	{
		Res = recv(Client->Socket, ReciveBuff, RECIVE_BUFFER_SIZE, 0);
		if ((Res == SOCKET_ERROR) || (Res == 0))
		{
			mtx.lock();

			printf_s("User \"%s\" left chat\n", Client->Nickname);
			shutdown(Client->Socket, SD_BOTH);
			closesocket(Client->Socket);
			ListDelete(&ClientList, &Client);

			mtx.unlock();
			break;
		}
		else
		{
			SYSTEMTIME Time;
			GetLocalTime(&Time);

			mtx.lock();
			printf_s("%02d:%02d:%02d ", Time.wHour, Time.wMinute, Time.wSecond);
			if (Client->Nickname != NULL)
			{
				printf_s("%s: ", Client->Nickname);
			}
			printf_s("%s\n", ReciveBuff);


			mtx.unlock();
		}
			
	}
}