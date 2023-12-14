/**
 * lcutil.h: This file is part of the mixbench GPU micro-benchmark suite.
 *
 * Contact: Elias Konstantinidis <ekondis@gmail.com>
 **/

#ifndef _CUTIL_H_
#define _CUTIL_H_

#include <stdio.h>
#include <musa.h>
#include <musa_runtime.h>
#include <musa_runtime_api.h>

#define MUSA_SAFE_CALL( call) {                                    \
    musaError err = call;                                                    \
    if( musaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",        \
                __FILE__, __LINE__, musaGetErrorString( err) );              \
        exit(EXIT_FAILURE);                                                  \
    } }

#define FRACTION_CEILING(numerator, denominator) ((numerator+denominator-1)/(denominator))

static inline int _ConvertSMVer2Cores(int major, int minor){
	switch(major){
		case 1:  return 8;
		case 2:  switch(minor){
			case 1:  return 48;
			default: return 32;
		}
		case 3:  return 192;
		case 6: switch(minor){
			case 0:  return 64;
			default: return 128;
		}
		case 7:  return 64;
		default: return 128;
	}
}

static inline bool IsFP16Supported(void){
	musaDeviceProp deviceProp;
	int current_device;
	MUSA_SAFE_CALL( musaGetDevice(&current_device) );
	MUSA_SAFE_CALL( musaGetDeviceProperties(&deviceProp, current_device) );
	return deviceProp.major>5 || (deviceProp.major == 5 && deviceProp.minor == 3);
}

static inline void GetDevicePeakInfo(double *aGIPS, double *aGBPS, musaDeviceProp *aDeviceProp = NULL){
	musaDeviceProp deviceProp;
	int current_device;
	if( aDeviceProp )
		deviceProp = *aDeviceProp;
	else{
		MUSA_SAFE_CALL( musaGetDevice(&current_device) );
		MUSA_SAFE_CALL( musaGetDeviceProperties(&deviceProp, current_device) );
	}
	const int TotalSPs = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor)*deviceProp.multiProcessorCount;
	*aGIPS = 1000.0 * deviceProp.clockRate * TotalSPs / (1000.0 * 1000.0 * 1000.0);  // Giga instructions/sec
	*aGBPS = 2.0 * (double)deviceProp.memoryClockRate * 1000.0 * (double)deviceProp.memoryBusWidth / 8.0;
}

static inline musaDeviceProp GetDeviceProperties(void){
	musaDeviceProp deviceProp;
	int current_device;
	MUSA_SAFE_CALL( musaGetDevice(&current_device) );
	MUSA_SAFE_CALL( musaGetDeviceProperties(&deviceProp, current_device) );
	return deviceProp;
}

// Print basic device information
static void StoreDeviceInfo(FILE *fout){
	musaDeviceProp deviceProp;
	int current_device, driver_version;
	MUSA_SAFE_CALL( musaGetDevice(&current_device) );
	MUSA_SAFE_CALL( musaGetDeviceProperties(&deviceProp, current_device) );
	MUSA_SAFE_CALL( musaDriverGetVersion(&driver_version) );
	fprintf(fout, "------------------------ Device specifications ------------------------\n");
	fprintf(fout, "Device:              %s\n", deviceProp.name);
	fprintf(fout, "CUDA driver version: %d.%d\n", driver_version/1000, driver_version%1000);
	fprintf(fout, "GPU clock rate:      %d MHz\n", deviceProp.clockRate/1000);
	fprintf(fout, "Memory clock rate:   %d MHz\n", deviceProp.memoryClockRate/1000/2);
	fprintf(fout, "Memory bus width:    %d bits\n", deviceProp.memoryBusWidth);
	fprintf(fout, "WarpSize:            %d\n", deviceProp.warpSize);
	fprintf(fout, "L2 cache size:       %d KB\n", deviceProp.l2CacheSize/1024);
	fprintf(fout, "Total global mem:    %d MB\n", (int)(deviceProp.totalGlobalMem/1024/1024));
	fprintf(fout, "ECC enabled:         %s\n", deviceProp.ECCEnabled?"Yes":"No");
	fprintf(fout, "Compute Capability:  %d.%d\n", deviceProp.major, deviceProp.minor);
	const int TotalSPs = _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor)*deviceProp.multiProcessorCount;
	fprintf(fout, "Total SPs:           %d (%d MPs x %d SPs/MP)\n", TotalSPs, deviceProp.multiProcessorCount, _ConvertSMVer2Cores(deviceProp.major, deviceProp.minor));
	double InstrThroughput, MemBandwidth;
	GetDevicePeakInfo(&InstrThroughput, &MemBandwidth, &deviceProp);
	fprintf(fout, "Compute throughput:  %.2f GFlops (theoretical single precision FMAs)\n", 2.0*InstrThroughput);
	fprintf(fout, "Memory bandwidth:    %.2f GB/sec\n", MemBandwidth/(1000.0*1000.0*1000.0));
	fprintf(fout, "-----------------------------------------------------------------------\n");
}

#endif
