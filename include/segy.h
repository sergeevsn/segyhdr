#pragma once

#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdint>
#include <limits>

#define SIGN_BIT_MASK 0x80000000
#define EXPONENT_BIT_MASK 0x7f000000
#define FRACTION_BIT_MASK 0x00ffffff
#define FRACTION_IMPLICIT_BIT_MASK 0x00800000

const int HDRWORD_INT16 = 0;
const int HDRWORD_INT32 = 1;
const int HDRWORD_FLOATIEEE = 2;
const int HDRWORD_FLOATIBM = 3;

struct binaryFileHeader {
    int JOB_ID;
    int LINE_NUM;
    int REEL_NUM;
    short int NUM_OF_TRACE;
    short int NUM_OF_AUX;
    short int INTERVAL_MS;
    short int INTERVAL_MS_ORI;
    unsigned short int NUM_OF_SAMPLES;
    unsigned short int NUM_OF_SAMPLES_ORI;
    short int SAMPLE_FORMAT;
    short int ENSEMBLE;
    short int TRACE_SORT;
    short int VERT_SUM;
    short int SWEEP_FREQ_START;
    short int SWEEP_FREQ_END;
    short int SWEEP_LENGTH;
    short int SWEEP_TYPE;
    short int SWEEP_NUM_CHANNEL;
    short int SWEEP_TAPER_LEN_START;
    short int SWEEP_TAPER_LEN_END;
    short int TAPER_TYPE;
    short int CORRELATED;
    short int BINARY_GAIN;
    short int AMP_RECOR;
    short int MEASURE_SYSTEM;
    short int IMPULSE_POLAR;
    short int POLAR_CODE;
    char UNNASSIGNED1[240];
    short int SEGY_REV_NUM;
    short int FIXED_LEN;
    short int NUM_EXT_HEAD;
    char UNNASSIGNED2[94];
};


struct traceHeader {
    int TRACE_SEQ_GLOBAL;
    int TRACE_SEQ_LOCAL;
    int ORI_RECORD_NUM;
    int TRACE_NUM_FIELD;
    int SOURCE_POINT;
    int ENSEMBLE_NUM;
    int ENS_TRACE_NUM;
    short int TRACE_CODE;
    short int NUM_VERT_SUM;
    short int NUM_HORZ_SUM;
    short int DATA_USE;
    int DIST_CENT_RECV;
    int RECV_GRP_ELEV;
    int SURF_ELEV_SRC;
    int SOURCE_DEPTH;
    int DATUM_ELEV_RECV;
    int DATUM_ELAV_SRC;
    int WATER_DEPTH_SRC;
    int WATER_DEPTH_GRP;
    short int SCALE_DEPTH;
    short int SCALE_COOR;
    int SRC_COOR_X;
    int SRC_COOR_Y;
    int GRP_COOR_X;
    int GRP_COOR_Y;
    short int COOR_UNIT;
    short int WEATHER_VEL;
    short int SWEATHER_VEL;
    short int UPHOLE_T_SRC;
    short int UPHOLE_T_GRP;
    short int SRC_STA_CORRC;
    short int GRP_STA_CORRC;
    short int TOTAL_STA;
    short int LAG_TIME_A;
    short int LAG_TIME_B;
    short int DELAY_T;
    short int MUTE_T_STRT;
    short int MUTE_T_END;
    unsigned short int NUM_OF_SAMPL;
    unsigned short int SAMPLE_INTRVL;
    short int GAIN_TYPE;
    short int GAIN_CONST;
    short int GAIN_INIT;
    short int CORRLTD;
    short int SWEEP_FREQ_START;
    short int SWEEP_FREQ_END;
    short int SWEEP_LENGTH;
    short int SWEEP_TYPE;
    short int SWEEP_TAPER_LEN_START;
    short int SWEEP_TAPER_LEN_END;
    short int TAPER_TYPE;
    short int ALIAS_FREQ;
    short int ALIAS_SLOPE;
    short int NOTCH_FREQ;
    short int NOTCH_SLOPE;
    short int LOWCUT_FREQ;
    short int HIGHCUT_FREQ;
    short int LOWCUT_SLOPE;
    short int HIGHCUT_SLOPE;
    short int YEAR;
    short int DAY;
    short int HOUR;
    short int MINUTE;
    short int SECOND;
    short int TIME_CODE;
    short int WEIGHT_FACT;
    short int GEOPHNE_ROLL;
    short int GEOPHNE_TRACE;
    short int GEOPHNE_LAST;
    short int GAP_SIZE;
    short int OVER_TRAVEL;
    int ENS_COOR_X;
    int ENS_COOR_Y;
    int INLINE;
    int CROSS;
    int SHOOTPOINT;
    short int SHOOTPOINT_SCALE;
    short int TRACE_UNIT;
    char TRANSD_CONST[6];
    short int TRANSD_UNIT;
    short int TRACE_IDENT;
    short int SCALE_TIME;
    short int SRC_ORIENT;
    char SRC_DIRECTION[6];
    char SRC_MEASUREMT[6];
    short int SRC_UNIT;
    char UNNASSIGNED1[6];
};

const int TRCHDR_BYTE_NUMS[86] = { 1, 5, 9, 13, 17, 21, 25, 29, 31, 33, 35, 37, 41, 45, 49, 53, 57, 61, 65, 69, 71, 73, 77, 81, 85, 89, 91, 93, 95, 97, 99,\
                                  101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 149, 151, 153,\
                                  155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 185, 189, 193, 197, 201, 203, 205, 211, 213, 215, 217,\
                                  217, 219, 225, 231, 233 };

const int TRCHDR_SIZES[86] = { 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,\
                                  2, 2, 2, 2, 2, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 2, 2, 6, 2, 2, 2, 0, 2, 6, 6, 2, 4 };


static const unsigned char e2a[256] = {
          0,  1,  2,  3,156,  9,134,127,151,141,142, 11, 12, 13, 14, 15,
         16, 17, 18, 19,157,133,  8,135, 24, 25,146,143, 28, 29, 30, 31,
        128,129,130,131,132, 10, 23, 27,136,137,138,139,140,  5,  6,  7,
        144,145, 22,147,148,149,150,  4,152,153,154,155, 20, 21,158, 26,
         32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
         38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94,
         45, 47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
        186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34,
        195, 97, 98, 99,100,101,102,103,104,105,196,197,198,199,200,201,
        202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,208,
        209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
        216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,
        123, 65, 66, 67, 68, 69, 70, 71, 72, 73,232,233,234,235,236,237,
        125, 74, 75, 76, 77, 78, 79, 80, 81, 82,238,239,240,241,242,243,
         92,159, 83, 84, 85, 86, 87, 88, 89, 90,244,245,246,247,248,249,
         48, 49, 50, 51, 52, 53, 54, 55, 56, 57,250,251,252,253,254,255
};


template <typename T>
T swapEndian(T value) {
    // Create a union to handle the byte swapping
    union {
        T value;
        char bytes[sizeof(T)];
    } src, dest;

    src.value = value;

    // Reverse the byte order
    for (size_t i = 0; i < sizeof(T); ++i) {
        dest.bytes[i] = src.bytes[sizeof(T) - i - 1];
    }

    return dest.value;
}

float getSpecifiedTraceHeaderValue(const traceHeader& TH, int byteNo, int numberType = HDRWORD_INT32);
void printBinaryHeader(binaryFileHeader BH);
void printTraceHeader(traceHeader TH);
void binHeaderSwapEndian(binaryFileHeader& BH);
void traceHeaderSwapEndian(traceHeader& TH); 