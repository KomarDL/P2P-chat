#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <vector>
#include "Define.h"
#include "List.h"
#include "ReciveData.h"

void AcceptConnectionsThread(ULONG IP, BOOL &Work, PList &ClientList, std::mutex &mtx, char* &Nickname);