#include <Windows.h>
#include <stdio.h>
#include "processInjection.h"

int main(int argc, char* argv[])
{  
    PVOID rBuffer = NULL;
    DWORD dwPID = NULL, dwTID = NULL;
    HANDLE hProcess = NULL, hThread = NULL;

    dwPID = parsePidParam(argc, argv);
    if (dwPID == NULL) 
    {
        printHelp();
        return EXIT_FAILURE;
    }
    else
    {
        printf("PID: %i", dwPID);
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
    if (hProcess == NULL)
    {
        printf("Failed to open process, error: 0x%lx", GetLastError());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void printHelp()
{
    printf("Usage processInjection.exe -p PID\n");
    printf("\tPID\tProcess ID\n");
}

DWORD parsePidParam(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp("-p", argv[i]) && i < argc - 1)
        {
            return atoi(argv[i + 1]);
        }
    }

    return NULL;
}