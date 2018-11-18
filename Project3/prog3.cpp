#include "../Project2/lib.hpp"

#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdio>

#define DLL_IMPORT __declspec(dllimport)

DLL_IMPORT extern const int mesNum; //total number of messages
DLL_IMPORT extern const float mesPr; //coefficient of message processing time

DLL_IMPORT int deq(int[], FILE*);

void MesProc(FILE* fp, FILE* logFile, HANDLE hMutex)
{
	int count = 0;
	int mes[200] = { 0 };
	int sum = 0;
	int cnt = 0;
	char flag[1200] = { 0 };
	while(count < mesNum) {
		WaitForSingleObject(hMutex, INFINITE);
		int group = deq(mes, fp);
		std::cout << "Take from queue " << group << std::endl;
		fprintf(logFile, "Take from queue %d\n", group);
		fflush(logFile);
		ReleaseMutex(hMutex);
		sum += group;
		cnt++;
		if(0 == group) {
			Sleep(200);
		} else {
			for (int i = 1; i <= group; ++i) {
				flag[mes[i - 1]] = '1';
			}
			count += group;
			float time = (mesPr + group / 100) * 1e8;
			for (int i = 0; i < time; ++i);
		}
	}
	float avr = 0 != cnt ? (sum / static_cast<float>(cnt)) : 0.0;
	int handledMesNum = std::count(flag, flag + 1200, '1');
	WaitForSingleObject(hMutex, INFINITE);
	std::cout << "Count of handled messages: " << handledMesNum << std::endl;
	fprintf(logFile, "Count of handled messages: %d\n", handledMesNum);
	fflush(logFile);
	std::cout << "Average of group size: " << avr << std::endl;
	fprintf(logFile, "Average of group size: %.4f\n", avr);
	fflush(logFile);
	std::cout << "End of message processing" << std::endl;
	fprintf(logFile, "End of message processing\n");
	fflush(logFile);
	ReleaseMutex(hMutex);
}

int main()
{
	HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "queMutex");
	if (0 == hMutex) {
		return GetLastError();
	}
	FILE* fp = _fsopen("../Files/que.dat", "rb+", _SH_DENYNO);
	FILE* logFile = _fsopen("../Files/log.txt", "a+", _SH_DENYNO);
	MesProc(fp, logFile, hMutex);
	WaitForSingleObject(hMutex, INFINITE);
	std::cout << "End of Project3 execution" << std::endl;
}