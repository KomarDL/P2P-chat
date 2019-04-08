#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <vector>
#include <sysinfoapi.h>
#include "Define.h"
#include "List.h"

void ReciveData(BOOL &Work, PList &Client, PList &ClientList, std::mutex &mtx, char* &Nickname);