#include "../fft_JniFft.h"
#include "ooura.h"

#define _USE_MATH_DEFINES 1

#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include <tchar.h>
#include <strsafe.h>
#include <windows.h>

using namespace std;

#define MAX_THREADS 8
const int DATA_COUNT = MAX_THREADS * 512;

void ErrorHandler(LPTSTR lpszFunction);

const int LENGTH = 0x20000;
const int CUT_OFF = 0x80;

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
struct ThreadData {
    double* input;
    double* output;
};

DWORD WINAPI fftThreadFunction(LPVOID lpParam)
{
    ThreadData* threadData = (ThreadData*)lpParam;

    for (int i = 0; i < DATA_COUNT / MAX_THREADS; i++)
    {
        ooura_fft(threadData->input, threadData->output, LENGTH);
    }

    return 0;
}

void ErrorHandler(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

int main()
{
    clock_t start = clock();

    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];
    ThreadData threadData[MAX_THREADS];

    // Create MAX_THREADS worker threads.
    for (int i = 0; i < MAX_THREADS; i++)
    {
        threadData[i].input = new double[LENGTH];
        threadData[i].output = new double[LENGTH + 1];

        for (int x = 0; x < LENGTH; x++)
        {
            threadData[i].input[x] = (x < LENGTH / 2 ? 1 : 0) + cos(x * (double)2 * M_PI / (double)LENGTH);
        }

        // Create the thread to begin execution on its own.
        hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            fftThreadFunction,       // thread function name
            &threadData[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);   // returns the thread identifier 


          // Check the return value for success.
          // If CreateThread fails, terminate execution. 
          // This will automatically clean up threads and memory. 

        if (hThreadArray[i] == NULL)
        {
            ErrorHandler(TEXT("CreateThread"));
            ExitProcess(3);
        }
    } // End of main thread creation loop.

    // Wait until all threads have terminated.
    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    clock_t end = clock();
    cout << "time for " << DATA_COUNT << " data sets of length " << LENGTH << ": " << end - start << endl;

    ofstream resultFile("result.csv");
    for (int x = 0; x < LENGTH; x++)
    {
        resultFile << threadData[0].input[x] << ";";

        if (x * 2 < CUT_OFF)
            resultFile << sqrt(threadData[0].output[x * 2] * threadData[0].output[x * 2] + threadData[0].output[x * 2 + 1] * threadData[0].output[x * 2 + 1]) * 2 / (LENGTH) << endl;
        else
            resultFile << endl;
    }
    resultFile.flush();
    resultFile.close();

    // Close all thread handles and free memory allocations.
    for (int i = 0; i < MAX_THREADS; i++)
    {
        delete[]threadData[i].input;
        delete[]threadData[i].output;

        CloseHandle(hThreadArray[i]);
    }

    return 0;
}