#include "segy.h"

void binHeaderSwapEndian(binaryFileHeader& BH) {
    BH.JOB_ID = swapEndian(BH.JOB_ID);
    BH.LINE_NUM = swapEndian(BH.LINE_NUM);
    BH.REEL_NUM = swapEndian(BH.REEL_NUM);;
    BH.NUM_OF_TRACE = swapEndian(BH.NUM_OF_TRACE);
    BH.NUM_OF_AUX = swapEndian(BH.NUM_OF_AUX);
    BH.INTERVAL_MS = swapEndian(BH.INTERVAL_MS);
    BH.INTERVAL_MS_ORI = swapEndian(BH.INTERVAL_MS_ORI);
    BH.NUM_OF_SAMPLES = swapEndian(BH.NUM_OF_SAMPLES);
    BH.NUM_OF_SAMPLES_ORI = swapEndian(BH.NUM_OF_SAMPLES_ORI);
    BH.SAMPLE_FORMAT = swapEndian(BH.SAMPLE_FORMAT);
    BH.ENSEMBLE = swapEndian(BH.ENSEMBLE);
    BH.TRACE_SORT = swapEndian(BH.TRACE_SORT);
    BH.VERT_SUM = swapEndian(BH.VERT_SUM);
    BH.SWEEP_FREQ_START;
    BH.SWEEP_FREQ_END;
    BH.SWEEP_LENGTH = swapEndian(BH.SWEEP_LENGTH);
    BH.SWEEP_TYPE = swapEndian(BH.SWEEP_TYPE);
    BH.SWEEP_NUM_CHANNEL = swapEndian(BH.SWEEP_NUM_CHANNEL);
    BH.SWEEP_TAPER_LEN_START = swapEndian(BH.SWEEP_TAPER_LEN_START);
    BH.SWEEP_TAPER_LEN_END = swapEndian(BH.SWEEP_TAPER_LEN_END);
    BH.TAPER_TYPE = swapEndian(BH.TAPER_TYPE);
    BH.CORRELATED = swapEndian(BH.CORRELATED);
    BH.BINARY_GAIN = swapEndian(BH.BINARY_GAIN);
    BH.AMP_RECOR = swapEndian(BH.AMP_RECOR);
    BH.MEASURE_SYSTEM = swapEndian(BH.MEASURE_SYSTEM);
    BH.IMPULSE_POLAR = swapEndian(BH.IMPULSE_POLAR);
    BH.POLAR_CODE = swapEndian(BH.POLAR_CODE);   
    BH.SEGY_REV_NUM = swapEndian(BH.SEGY_REV_NUM);
    BH.FIXED_LEN = swapEndian(BH.FIXED_LEN);
    BH.NUM_EXT_HEAD = swapEndian(BH.NUM_EXT_HEAD); 
}

void traceHeaderSwapEndian(traceHeader& TH) {
    uint8_t buf[sizeof(TRCHDR_SIZES)];    
    for (int i = 0; i < 86; i++) {       
        const uint8_t* source_ptr = reinterpret_cast<const uint8_t*>(&TH) + (TRCHDR_BYTE_NUMS[i] - 1);     
    
        if (TRCHDR_SIZES[i] == 2) {
            int16_t temp;
            std::memcpy(&temp, source_ptr, 2);
            temp = swapEndian(temp);
            std::memcpy(&buf[TRCHDR_BYTE_NUMS[i]-1], &temp, 2);
        }
        else {
            int32_t temp;
            std::memcpy(&temp, source_ptr, 4);
            temp = swapEndian(temp);
            std::memcpy(&buf[TRCHDR_BYTE_NUMS[i] - 1], &temp, 4);
        }        
    }
    std::memcpy(&TH, &buf, sizeof(traceHeader));    
}

float ibmFloatToIeee(unsigned int value) {

    int32_t* int_addr = reinterpret_cast<int32_t*>(&value);
    int32_t int_val = *int_addr;

    int32_t sign = int_val >> 31;
    int32_t fraction = int_val & FRACTION_BIT_MASK;

    if (fraction == 0) {
        return sign ? -0.0f : 0.0f;
    }

    // Convert exponent to be of base 2 and remove IBM exponent bias.
    int32_t exponent = ((int_val & EXPONENT_BIT_MASK) >> 22) - 256;

    // Drop the last bit since we can store only 23 bits in IEEE.
    fraction >>= 1;

    // Normalize such that the implicit leading bit of the fraction is 1.
    while (fraction && (fraction & FRACTION_IMPLICIT_BIT_MASK) == 0) {
        fraction <<= 1;
        --exponent;
    }

    // Drop the implicit leading bit.
    fraction &= 0x007fffff;

    // Add IEEE bias to the exponent.
    exponent += 127;

    // Handle overflow.
    if (exponent >= 255) {
        return (sign ? -std::numeric_limits<float>::max()
            : std::numeric_limits<float>::max());
    }

    int32_t ieee_value;

    // Handle underflow.
    if (exponent <= 0)
        ieee_value = (sign << 31) | fraction;
    else
        ieee_value = (sign << 31) | (exponent << 23) | fraction;

    float* float_addr = reinterpret_cast<float*>(&ieee_value);
    return *float_addr;
}


float getSpecifiedTraceHeaderValue(const traceHeader& TH, int byteNo, int numberType) {
    if (((byteNo - 1) + sizeof(int32_t) > sizeof(TH)) | ((byteNo <= 0))) {
        std::cerr << "Wrong byteNo!" << std::endl;
        return -1;
    }

    // Create a pointer to the start of the traceHeader
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&TH) + (byteNo - 1);

    // Interpret the 4 bytes at the given shift as an int32_t
    unsigned int word_int32 = 0;
    int16_t word_int16 = 0;
    float word_float = 0.0;

    if (numberType == HDRWORD_INT16) {
        std::memcpy(&word_int16, ptr, sizeof(int16_t));        
        return (float)word_int16;
    }          
    if ((numberType == HDRWORD_INT32) | (numberType == HDRWORD_FLOATIBM)) {
        std::memcpy(&word_int32, ptr, 4);      
        if (numberType == HDRWORD_FLOATIBM) {          
            word_float = ibmFloatToIeee(word_int32);       
            return word_float;
        }
        else return (float)word_int32;
    }
    if (numberType == HDRWORD_FLOATIEEE) {
        std::memcpy(&word_float, ptr, 4);       
        return word_float;
    }
    return -1;
}

void printTraceHeader(traceHeader TH) {
    printf("TRACE_SEQ_GLOBAL    : %i \n", TH.TRACE_SEQ_GLOBAL);
    printf("TRACE_SEQ_LOCAL     : %i \n", TH.TRACE_SEQ_LOCAL);
    printf("ORI_RECORD_NUM      : %i \n", TH.ORI_RECORD_NUM);
    printf("TRACE_NUM_FIELD     : %i \n", TH.TRACE_NUM_FIELD);
    printf("SOURCE_POINT        : %i \n", TH.SOURCE_POINT);
    printf("ENSEMBLE_NUM        : %i \n", TH.ENSEMBLE_NUM);
    printf("ENS_TRACE_NUM       : %i \n", TH.ENS_TRACE_NUM);
    printf("TRACE_CODE          : %hi \n", TH.TRACE_CODE);
    printf("NUM_VERT_SUM        : %hi \n", TH.NUM_VERT_SUM);
    printf("NUM_HORZ_SUM        : %hi \n", TH.NUM_HORZ_SUM);
    printf("DATA_USE            : %hi \n", TH.DATA_USE);
    printf("DIST_CENT_RECV      : %i \n", TH.DIST_CENT_RECV);
    printf("RECV_GRP_ELEV       : %i \n", TH.RECV_GRP_ELEV);
    printf("SURF_ELEV_SRC       : %i \n", TH.SURF_ELEV_SRC);
    printf("SOURCE_DEPTH        : %i \n", TH.SOURCE_DEPTH);
    printf("DATUM_ELEV_RECV     : %i \n", TH.DATUM_ELEV_RECV);
    printf("DATUM_ELAV_SRC      : %i \n", TH.DATUM_ELAV_SRC);
    printf("WATER_DEPTH_SRC     : %i \n", TH.WATER_DEPTH_SRC);
    printf("WATER_DEPTH_GRP     : %i \n", TH.WATER_DEPTH_GRP);
    printf("SCALE_DEPTH         : %hi \n", TH.SCALE_DEPTH);
    printf("SCALE_COOR          : %hi \n", TH.SCALE_COOR);
    printf("SRC_COOR_X          : %i \n", TH.SRC_COOR_X);
    printf("SRC_COOR_Y          : %i \n", TH.SRC_COOR_Y);
    printf("GRP_COOR_X          : %i \n", TH.GRP_COOR_X);
    printf("GRP_COOR_Y          : %i \n", TH.GRP_COOR_Y);
    printf("COOR_UNIT           : %hi \n", TH.COOR_UNIT);
    printf("WEATHER_VEL         : %hi \n", TH.WEATHER_VEL);
    printf("SWEATHER_VEL        : %hi \n", TH.SWEATHER_VEL);
    printf("UPHOLE_T_SRC        : %hi \n", TH.UPHOLE_T_SRC);
    printf("UPHOLE_T_GRP        : %hi \n", TH.UPHOLE_T_GRP);
    printf("SRC_STA_CORRC       : %hi \n", TH.SRC_STA_CORRC);
    printf("GRP_STA_CORRC       : %hi \n", TH.GRP_STA_CORRC);
    printf("TOTAL_STA           : %hi \n", TH.TOTAL_STA);
    printf("LAG_TIME_A          : %hi \n", TH.LAG_TIME_A);
    printf("LAG_TIME_B          : %hi \n", TH.LAG_TIME_B);
    printf("DELAY_T             : %hi \n", TH.DELAY_T);
    printf("MUTE_T_STRT         : %hi \n", TH.MUTE_T_STRT);
    printf("MUTE_T_END          : %hi \n", TH.MUTE_T_END);
    printf("NUM_OF_SAMPL        : %hi \n", TH.NUM_OF_SAMPL);
    printf("SAMPLE_INTRVL       : %hi \n", TH.SAMPLE_INTRVL);
    printf("GAIN_TYPE           : %hi \n", TH.GAIN_TYPE);
    printf("GAIN_CONST          : %hi \n", TH.GAIN_CONST);
    printf("GAIN_INIT           : %hi \n", TH.GAIN_INIT);
    printf("CORRLTD             : %hi \n", TH.CORRLTD);
    printf("SWEEP_FREQ_START    : %hi \n", TH.SWEEP_FREQ_START);
    printf("SWEEP_FREQ_END      : %hi \n", TH.SWEEP_FREQ_END);
    printf("SWEEP_LENGTH        : %hi \n", TH.SWEEP_LENGTH);
    printf("SWEEP_TYPE          : %hi \n", TH.SWEEP_TYPE);
    printf("SWEEP_TAPER_LEN_ST  : %hi \n", TH.SWEEP_TAPER_LEN_START);
    printf("SWEEP_TAPER_LEN_END : %hi \n", TH.SWEEP_TAPER_LEN_END);
    printf("TAPER_TYPE          : %hi \n", TH.TAPER_TYPE);
    printf("ALIAS_FREQ          : %hi \n", TH.ALIAS_FREQ);
    printf("ALIAS_SLOPE         : %hi \n", TH.ALIAS_SLOPE);
    printf("NOTCH_FREQ          : %hi \n", TH.NOTCH_FREQ);
    printf("NOTCH_SLOPE         : %hi \n", TH.NOTCH_SLOPE);
    printf("LOWCUT_FREQ         : %hi \n", TH.LOWCUT_FREQ);
    printf("HIGHCUT_FREQ        : %hi \n", TH.HIGHCUT_FREQ);
    printf("LOWCUT_SLOPE        : %hi \n", TH.LOWCUT_SLOPE);
    printf("HIGHCUT_SLOPE       : %hi \n", TH.HIGHCUT_SLOPE);
    printf("YEAR                : %hi \n", TH.YEAR);
    printf("DAY                 : %hi \n", TH.DAY);
    printf("HOUR                : %hi \n", TH.HOUR);
    printf("MINUTE              : %hi \n", TH.MINUTE);
    printf("SECOND              : %hi \n", TH.SECOND);
    printf("TIME_CODE           : %hi \n", TH.TIME_CODE);
    printf("WEIGHT_FACT         : %hi \n", TH.WEIGHT_FACT);
    printf("GEOPHNE_ROLL        : %hi \n", TH.GEOPHNE_ROLL);
    printf("GEOPHNE_TRACE       : %hi \n", TH.GEOPHNE_TRACE);
    printf("GEOPHNE_LAST        : %hi \n", TH.GEOPHNE_LAST);
    printf("GAP_SIZE            : %hi \n", TH.GAP_SIZE);
    printf("OVER_TRAVEL         : %hi \n", TH.OVER_TRAVEL);
    printf("ENS_COOR_X          : %i \n", TH.ENS_COOR_X);
    printf("ENS_COOR_Y          : %i \n", TH.ENS_COOR_Y);
    printf("INLINE              : %i \n", TH.INLINE);
    printf("CROSS               : %i \n", TH.CROSS);
    printf("SHOOTPOINT          : %i \n", TH.SHOOTPOINT);
    printf("SHOOTPOINT_SCALE    : %hi \n", TH.SHOOTPOINT_SCALE);
    printf("TRACE_UNIT          : %hi \n", TH.TRACE_UNIT);
    printf("TRANSD_UNIT         : %hi \n", TH.TRANSD_UNIT);
    printf("TRACE_IDENT         : %hi \n", TH.TRACE_IDENT);
    printf("SCALE_TIME          : %hi \n", TH.SCALE_TIME);
    printf("SRC_ORIENT          : %hi \n", TH.SRC_ORIENT);
    printf("SRC_UNIT            : %hi \n", TH.SRC_UNIT);
    // printf("%hi \n",sgy.swapEndian( sgy.TH.char UNNASSIGNED1 [6]) );
    // printf("%hi \n",sgy.swapEndian( sgy.TH.char TRANSD_CONST [6]) );
    // printf("%hi \n",sgy.swapEndian( sgy.TH.char SRC_DIRECTION [6]) );
    // printf("%hi \n",sgy.swapEndian( sgy.TH.char SRC_MEASUREMT [6]) );
}

void printBinaryHeader(binaryFileHeader BH) {
    printf("JOB_ID               :%i \n", BH.JOB_ID);
    printf("LINE_NUM             :%i \n",BH.LINE_NUM);
    printf("REEL_NUM             :%i \n", BH.REEL_NUM);
    printf("NUM_OF_TRACE         :%hi \n", BH.NUM_OF_TRACE);
    printf("NUM_OF_AUX           :%hi \n", BH.NUM_OF_AUX);
    printf("INTERVAL_M           :%hi \n", BH.INTERVAL_MS);
    printf("INTERVAL_MS_ORI      :%hi \n", BH.INTERVAL_MS_ORI);
    printf("NUM_OF_SAMPLES_ORI   :%hi \n", BH.NUM_OF_SAMPLES_ORI);
    printf("SAMPLE_FORMAT        :%hi \n", BH.SAMPLE_FORMAT);
    printf("ENSEMBLE             :%hi \n", BH.ENSEMBLE);
    printf("TRACE_SORT           :%hi \n", BH.TRACE_SORT);
    printf("VERT_SUM             :%hi \n", BH.VERT_SUM);
    printf("SWEEP_FREQ_START     :%hi \n", BH.SWEEP_FREQ_START);
    printf("SWEEP_FREQ_END       :%hi \n", BH.SWEEP_FREQ_END);
    printf("SWEEP_LENGTH         :%hi \n", BH.SWEEP_LENGTH);
    printf("SWEEP_TYPE           :%hi \n", BH.SWEEP_TYPE);
    printf("SWEEP_NUM_CHANNEL    :%hi \n", BH.SWEEP_NUM_CHANNEL);
    printf("SWEEP_TAPER_LEN_START:%hi \n", BH.SWEEP_TAPER_LEN_START);
    printf("SWEEP_TAPER_LEN_END  :%hi \n", BH.SWEEP_TAPER_LEN_END);
    printf("TAPER_TYPE           :%hi \n", BH.TAPER_TYPE);
    printf("CORRELATED           :%hi \n", BH.CORRELATED);
    printf("BINARY_GAIN          :%hi \n", BH.BINARY_GAIN);
    printf("AMP_RECOR            :%hi \n", BH.AMP_RECOR);
    printf("MEASURE_SYSTEM       :%hi \n", BH.MEASURE_SYSTEM);
    printf("IMPULSE_POLAR        :%hi \n", BH.IMPULSE_POLAR);
    printf("POLAR_CODE           :%hi \n", BH.POLAR_CODE);
    printf("SEGY_REV_NUM         :%hi \n", BH.SEGY_REV_NUM);
    printf("FIXED_LEN            :%hi \n", BH.FIXED_LEN);
    printf("NUM_EXT_HEAD         :%hi \n", BH.NUM_EXT_HEAD);
}