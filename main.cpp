#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

typedef void (*Reader)(std::vector<std::string>);
typedef void (*Writer)(std::string);

int main(int argC, char** argV)
{
    if (argC < 2)
    {
        return -1;
    }
    HINSTANCE hDLL;
    Reader reader;
    Writer writer;

    hDLL = LoadLibraryA("LAB5-dll");
    if (!hDLL)
    {
        return -1;
    }
    reader = (Reader)GetProcAddress(hDLL, "reader");
    writer = (Writer)GetProcAddress(hDLL, "writer");
    if (!reader || !writer)
    {
        FreeLibrary(hDLL);
        return -1;
    }
    std::vector<std::string> filenames{};
    for (int i = 1; i < argC; i++)
    {
        filenames.emplace_back(argV[i]);
    }
    std::thread w_thread{ writer, "output.txt" };
    std::thread r_thread{ reader, filenames };
    r_thread.join();
    w_thread.join();
    system("pause");
	return 0;
}