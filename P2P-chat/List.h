#pragma once
#include <winsock2.h>

typedef struct _List
{
	SOCKET Socket;
	ULONG IP;
	char *Nickname;
	BOOL InThread;
	BOOL Delete;
	_List *Last;
	_List *Next;

} List, *PList, **PPList;

void ListAdd(PPList Head, PPList NewEl);

void ListDelete(PPList Head, PPList ForDelEl);