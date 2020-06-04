#include "pch.h"
#include "Header.h"
#include <iostream>

#define MAX_SIZE 256
static char buf[MAX_SIZE + 2];
static int size = 0;
static bool finished_reading{ false };
static bool finished_writing{ false };
static bool ended{ false };

void reader(std::vector<std::string> filenames)
{
	HANDLE ioEvent;
	ioEvent = CreateEventA(NULL, TRUE, FALSE, "lab5 windows event1");
	if (!ioEvent)
	{
		std::cout << "couldn't create event" << std::endl;
		ended = true;
		return;
	}

	OVERLAPPED overlapped;
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.hEvent = ioEvent;
	overlapped.Pointer = 0;
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;

	uint64_t offset;
	while (!finished_writing);
	if (ended)
	{
		CloseHandle(ioEvent);
		return;
	}
	for (auto filename : filenames)
	{
		HANDLE file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
		{
			std::cout << "couldn't open \"" << filename << "\"" << std::endl;
			continue;
		}
		offset = 0;

		std::cout << "reading from \"" << filename << "\"" << std::endl;
		bool file_ended{ false };
		while (!file_ended)
		{			
			while (!finished_writing);
			overlapped.Offset = offset;
			overlapped.OffsetHigh = offset >> 32;
			if (!ReadFile(file, buf, MAX_SIZE, NULL, &overlapped))
			{
				size = 0;
				//std::cout << "couldn't read from file" << std::endl;
				break;
			}
			//std::cout << "fine" << std::endl;
			WaitForSingleObject(ioEvent, INFINITE);
			ResetEvent(ioEvent);
			if (overlapped.InternalHigh != MAX_SIZE)
			{
				buf[overlapped.InternalHigh] = 0x0D;
				buf[overlapped.InternalHigh + 1] = 0x0A;
				overlapped.InternalHigh += 2;
				file_ended = true;
			}
			size = overlapped.InternalHigh;
			offset += overlapped.InternalHigh;
			finished_writing = false;
			finished_reading = true;
		}
		std::cout << "file \"" << filename << "\" has ended" << std::endl;
		CloseHandle(file);
	}
	while (!finished_writing);
	size = 0;
	ended = true;
	finished_reading = true;
	CloseHandle(ioEvent);
}

void writer(std::string filename)
{
	std::cout << filename << std::endl;
	HANDLE ioEvent;
	ioEvent = CreateEventA(NULL, TRUE, FALSE, "lab5 windows event2");
	if (!ioEvent)
	{
		std::cout << "couldn't create event" << std::endl;
		ended = true;
		finished_writing = true;
		return;
	}
	OVERLAPPED overlapped;
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.hEvent = ioEvent;
	overlapped.Pointer = 0;
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;
	uint64_t offset = 0;
	HANDLE file;
	file = CreateFileA(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		std::cout << "couldn't open file for writing" << std::endl;
		ended = true;
		finished_writing = true;
		return;
	}
	finished_reading = false;
	finished_writing = true;
	while (!ended)
	{
		while (!finished_reading);
		overlapped.Offset = offset;
		overlapped.OffsetHigh = offset >> 32;
		WriteFile(file, buf, size, NULL, &overlapped);
		WaitForSingleObject(ioEvent, INFINITE);
		ResetEvent(ioEvent);
		offset += size;
		finished_reading = false;
		finished_writing = true;
	}
	CloseHandle(file);
	CloseHandle(ioEvent);
}

