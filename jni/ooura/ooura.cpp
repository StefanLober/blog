/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include <map>
#include <string>

#include "../de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement.h"
#include "fftsg.c"

#define LIBNAME "ooura"

#ifdef _DEBUG
#if defined(WIN32) || defined (WIN64)
#define log(msg) printf(msg)
#else
#include <android/log.h>
#define log(msg) __android_log_print(ANDROID_LOG_VERBOSE, LIBNAME, msg)
#endif
#else
#define log(msg)
#endif

using namespace std;

map <string, bool> interruptedMap;

JNIEXPORT double* ooura_fft(const string& nativeName, double* input, int length, int iterations)
{
	log("ooura_fft\n");

	double* output = new double[length + 1];

	int lengthSqrt = (int)(sqrt((double)length) + 0.5) + 2;
	int* ip = new int[lengthSqrt];
	ip[0] = 0;

	double* w = new double[length * 5 / 4];

	for (int i = 0; i < iterations && !interruptedMap[nativeName]; i++)
	{
		copy(&input[0], &input[length - 1], output);
		rdft(length, 1, output, ip, w);
	}

	delete[]ip;
	delete[]w;

	log("ooura_fft end\n");

	return output;
}

JNIEXPORT void delete_output(double* output)
{
	delete[]output;
}

JNIEXPORT jdoubleArray JNICALL Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalCalculate
(JNIEnv* env, jobject obj, jstring name, jdoubleArray inputArray, jint iterations)
{
	log("Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalCalculate\n");

	const char* nativeName = env->GetStringUTFChars(name, 0);
	int length = env->GetArrayLength(inputArray);

	interruptedMap[nativeName] = false;

	double* input = env->GetDoubleArrayElements(inputArray, 0);
	double* output = ooura_fft(nativeName, input, length, iterations);

	log("ReleaseStringUTFChars\n");
	env->ReleaseStringUTFChars(name, nativeName);

	jdoubleArray outputArray = env->NewDoubleArray(length);

	if (outputArray != NULL)
	{
		env->SetDoubleArrayRegion(outputArray, 0, length, output);
	}

	env->ReleaseDoubleArrayElements(inputArray, input, 0);

	log("delete []output\n");
	delete[]output;

	log("Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalCalculate end\n");

	return outputArray;
}

JNIEXPORT void JNICALL Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalInterrupt
(JNIEnv* env, jobject obj, jstring name)
{
	log("Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalInterrupt\n");

	const char* nativeName = env->GetStringUTFChars(name, 0);
	interruptedMap[nativeName] = true;
	env->ReleaseStringUTFChars(name, nativeName);

	log("Java_de_stefanlober_mathsmark_calc_fft_NativeFFTCalcElement_internalInterrupt end\n");
}