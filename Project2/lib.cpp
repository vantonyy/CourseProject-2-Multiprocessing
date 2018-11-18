#include <cstdio>
#include "lib.hpp"

#define DLL_EXPORT __declspec(dllexport)

DLL_EXPORT extern const int maxPackSize = 20; //max packet size
DLL_EXPORT extern const int mesNum = 1000; //total number of messages
DLL_EXPORT extern const int tPack = 100; //period of packets (msec)
DLL_EXPORT extern const float mesPr = 1.0; //coefficient of message processing time
DLL_EXPORT extern const queHeader qHead = { 100, 12, 12 }; // initial parameters of queue

const int totSize = qHead.queSize * 4 + 12; //total size of queue file

DLL_EXPORT int q(int mes[], int n, FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	queHeader h;
	fread(&h, sizeof(queHeader), 1, fp);
	fseek(fp, h.Tail, SEEK_SET);
	for (int i = 0; i != n; ++i) {
		fwrite(mes + i, sizeof(int), 1, fp);
		fflush(fp);
		h.Tail += 4;
		if (h.Tail == totSize) {
			h.Tail = 12;
		}
		if (h.Head == h.Tail) {
			return 1;
		}
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(&h, sizeof(queHeader), 1, fp);
	fflush(fp);
	return 0;
}

DLL_EXPORT int deq(int mes[], FILE* fp)
{
	fseek(fp, 0, SEEK_SET);
	queHeader h;
	fread(&h, sizeof(queHeader), 1, fp);
	fseek(fp, h.Head, SEEK_SET);
	int k = 0;
	while (h.Head != h.Tail) {
		fread(mes + k, sizeof(4), 1, fp);
		h.Head += 4;
		if (h.Head == totSize) {
			h.Head = 12;
		}
		++k;
	}
	fseek(fp, 0, SEEK_SET);
	fwrite(&h, sizeof(queHeader), 1, fp);
	fflush(fp);
	return k;
}