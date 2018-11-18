#include "../Project2/lib.hpp"

#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdio>
#include <algorithm>

#define DLL_IMPORT __declspec(dllimport)

DLL_IMPORT extern const int maxPackSize; //max packet size
DLL_IMPORT extern const int mesNum; //total number of messages
DLL_IMPORT extern const int tPack; //period of packets (msec)
DLL_IMPORT extern const queHeader qHead; // initial parameters of queue

DLL_IMPORT int q(int[], int, FILE*);

int MesGen(FILE* fp, FILE* logFile, HANDLE hMutex)
{
	int count = 0;
	int mes[100] = { 0 };
	std::srand(time(0));
	int cnt = 0;
	while (count < mesNum) {
		int pack = std::rand() % maxPackSize + 1;
		for (int i = 0; i != pack; ++i) {
			mes[i] = ++cnt;
		}
		count += pack;
		WaitForSingleObject(hMutex, INFINITE);
		if (0 != q(mes, pack, fp)) {
			ReleaseMutex(hMutex);
			return 1;
		}
		std::cout << "Place in queue " << pack << std::endl;
		fprintf(logFile, "Place in queue %d\n", pack);
		fflush(logFile);
		ReleaseMutex(hMutex);
		Sleep(tPack);
	}
	WaitForSingleObject(hMutex, INFINITE);
	std::cout << "End of message generation" << std::endl;
	fprintf(logFile, "End of message generation\n");
	fflush(logFile);
	ReleaseMutex(hMutex);
	return 0;
}

int main()
{
	HANDLE hMutex = CreateMutexA(NULL, FALSE, "queMutex");
	if (0 == hMutex) {
		return GetLastError();
	}
	
	FILE* fp = _fsopen("../Files/que.dat", "wb+", _SH_DENYNO);
	fwrite(&qHead, sizeof(queHeader), 1, fp);
	fflush(fp);
	
	FILE* logFile = _fsopen("../Files/log.txt", "w", _SH_DENYNO); // truncate file
	logFile = freopen("../Files/log.txt", "a", logFile);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFO);
	if (!CreateProcessA(NULL, "Project3.exe", NULL, NULL, FALSE, 0, NULL, NULL, (LPSTARTUPINFOA)&si, &pi)) {
		fclose(fp);
		fclose(logFile);
		CloseHandle(hMutex);
		std::cout << "Failed to execute Project3.exe" << std::endl;
		return GetLastError();
	}

	int error = MesGen(fp, logFile, hMutex);
	if (0 != error) {
		TerminateProcess(pi.hProcess, error);
		CloseHandle(pi.hProcess);
		fclose(fp);
		fclose(logFile);
		CloseHandle(hMutex);
		return error;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	fclose(fp);
	fclose(logFile);
	CloseHandle(hMutex);
	std::cout << "End of Project1 execution" << std::endl;
	return 0;
}