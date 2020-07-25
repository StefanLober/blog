#include <string>
#include <time.h>

#include "../de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement.h"
#include "ooura.h"

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

using namespace std;

#define MAX_THREADS 8
//#define BUF_SIZE 255

void ErrorHandler(LPTSTR lpszFunction);

const int LENGTH = 0x40000;
const int DATA_COUNT = 256;
const int ITERATIONS = 100;

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
  int val1;
  int val2;
} MYDATA, *PMYDATA;

DWORD WINAPI fftThreadFunction(LPVOID lpParam)
{
  for (int i = 0; i < DATA_COUNT / MAX_THREADS; i++)
  {
    double *input = new double[LENGTH];
    double *output = ooura_fft("", input, LENGTH, ITERATIONS);
    delete []input;
    delete_output(output);
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
    (LPTSTR) &lpMsgBuf,
    0, NULL);

  // Display the error message.

  lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
    (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
  StringCchPrintf((LPTSTR) lpDisplayBuf,
    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
    TEXT("%s failed with error %d: %s"),
    lpszFunction, dw, lpMsgBuf);
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

  // Create MAX_THREADS worker threads.
  for (int i = 0; i < MAX_THREADS; i++)
  {
    // Create the thread to begin execution on its own.
    hThreadArray[i] = CreateThread(
      NULL,                   // default security attributes
      0,                      // use default stack size  
      fftThreadFunction,       // thread function name
      NULL,//pDataArray[i],          // argument to thread function 
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

  // Close all thread handles and free memory allocations.
  for (int i = 0; i < MAX_THREADS; i++)
  {
    CloseHandle(hThreadArray[i]);
  }

  clock_t end = clock();

  printf("time for %d data sets of length %d and %d iterations: %d\n", DATA_COUNT, LENGTH, ITERATIONS, end - start);

  return 0;
}