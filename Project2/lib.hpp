#pragma once

struct queHeader {
	int queSize; //number of entries in queue
	int Head;   //offset in file
	int Tail;  //offset in file
};
