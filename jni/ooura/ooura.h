#pragma once

#include <jni.h>

JNIEXPORT double *ooura_fft(const std::string& nativeName, double *input, int length, int iterations);
JNIEXPORT void delete_output(double* output);