#include <iostream>
#include <mmintrin.h>
#include <iomanip>


int main() {
//    int8_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
//    int8_t b[8] = {-17, -17, -17, -17, -17, -17, -17, -17};
//    int8_t c[8] = {17, -17, 17, -17, 17, -17, 17, -17};
//    int16_t d[8] = {-300, 300, -300, 300, -300, 300, -300, 300};

    int8_t a[8];
    for (int8_t & i : a) {
        i = (int8_t) (rand() % 128 - 64);
    }

    int8_t b[8];
    for (int8_t & i : b) {
        i = (int8_t) (rand() % 128 - 64);
    }

    int8_t c[8];
    for (int8_t & i : c) {
        i = (int8_t) (rand() % 128 - 64);
    }

    int16_t d[8];
    for (int16_t & i : d) {
        i = (int16_t) (rand() % 32767 - 16384);
    }

    int32_t cpp_res[8];
    for (int i = 0; i < 8; ++i) {
        cpp_res[i] = (a[i] - b[i]) * (c[i] - d[i]);
    }


    __m64 *MM0 = (__m64 *) a;
    __m64 *MM1 = (__m64 *) b;
    __m64 *MM2 = (__m64 *) c;
    __m64 *MM3 = (__m64 *) (&d[0]);
    __m64 *MM4 = (__m64 *) (&d[4]);
    __m64 MM5 = _m_from_int64(0);

    *MM0 = _m_psubsb(*MM0, *MM1);

    *MM1 = _m_pcmpgtb(MM5, *MM2);
    __m64 MM6 = _m_punpcklbw(*MM2, *MM1);
    __m64 MM7 = _m_punpckhbw(*MM2, *MM1);

    *MM3 = _m_psubsw(MM6, *MM3);
    *MM4 = _m_psubsw(MM7, *MM4);

    *MM1 = _m_pcmpgtb(MM5, *MM0);
    *MM2 = _m_punpcklbw(*MM0, *MM1); // a-b 1-4
    *MM1 = _m_punpckhbw(*MM0, *MM1); // a-b 5-8

    MM6 = _m_pmullw(*MM2, *MM3); // mul 1-4 low
    MM5 = _m_pmulhw(*MM2, *MM3); // mul 1-4 high
    int64_t one_two = _m_to_int64(_m_punpcklwd(MM6, MM5));
    int64_t three_four = _m_to_int64(_m_punpckhwd(MM6, MM5));

    MM6 = _m_pmullw(*MM1, *MM4); // mul 5-8 low
    MM5 = _m_pmulhw(*MM1, *MM4); // mul 5-8 high
    int64_t five_six = _m_to_int64(_m_punpcklwd(MM6, MM5));
    int64_t seven_eight = _m_to_int64(_m_punpckhwd(MM6, MM5));

    int32_t result[8];
    result[0] = (int32_t) (one_two);
    result[1] = (int32_t) (one_two >> 32);
    result[2] = (int32_t) (three_four);
    result[3] = (int32_t) (three_four >> 32);
    result[4] = (int32_t) (five_six);
    result[5] = (int32_t) (five_six >> 32);
    result[6] = (int32_t) (seven_eight);
    result[7] = (int32_t) (seven_eight >> 32);

    std::cout << "C++" << std::setw(10) << "MMX" << std::endl;
    for (int i = 0; i < 8; ++i) {
        std::cout << cpp_res[i] << std::setw(10) << result[i] << std::endl;
    }
    return 0;
}