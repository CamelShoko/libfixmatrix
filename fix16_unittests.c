#include <stdio.h>
#include "unittests.h"
#include "fix16_base.h"

const fix16_t testcases[] = {
    // Small numbers
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    -1, -2, -3, -4, -5, -6, -7, -8, -9, -10,
    
    // Integer numbers
    0x10000, -0x10000, 0x20000, -0x20000, 0x30000, -0x30000,
    0x40000, -0x40000, 0x50000, -0x50000, 0x60000, -0x60000,
    
    // Fractions (1/2, 1/4, 1/8)
    0x8000, -0x8000, 0x4000, -0x4000, 0x2000, -0x2000,
    
    // Smallest and largest values
    0x7FFFFFFF, 0x80000000,
    
    // Large random numbers
    831858892, 574794913, 2147272293, -469161054, -961611615,
    -1032264749, -803649977, -605088485, 1866267750, -1062497641,
    -3095916, -39790344, 811905058, 220333412, -174602573,
    1189051665, 1715691766, 1165913681, -51096862, -693805499,
    
    // Small random numbers
    -14985, 30520, -83587, 41129, 42137, 58537, -2259, 84142,
    -28283, 90914, 19865, 33191, 81844, -66273, -63215, -44459,
    -11326, 84295, 47515, -39324
};

#define TESTCASES_COUNT (sizeof(testcases)/sizeof(testcases[0]))

int main()
{
    int status = 0;
    
    {
        COMMENT("Testing basic multiplication");
        TEST(fix16_mul(fix16_from_int(5), fix16_from_int(5)) == fix16_from_int(25));
        TEST(fix16_mul(fix16_from_int(-5), fix16_from_int(5)) == fix16_from_int(-25));
        TEST(fix16_mul(fix16_from_int(-5), fix16_from_int(-5)) == fix16_from_int(25));
        TEST(fix16_mul(fix16_from_int(5), fix16_from_int(-5)) == fix16_from_int(-25));
    }
    
#ifndef FIXMATH_NO_ROUNDING
    {
        COMMENT("Testing multiplication rounding corner cases");
        TEST(fix16_mul(0, 10) == 0);
        TEST(fix16_mul(2, 0x8000) == 1);
        TEST(fix16_mul(-2, 0x8000) == -1);
        TEST(fix16_mul(3, 0x8000) == 2);
        TEST(fix16_mul(-3, 0x8000) == -2);
        TEST(fix16_mul(2, 0x7FFF) == 1);
        TEST(fix16_mul(-2, 0x7FFF) == -1);
        TEST(fix16_mul(2, 0x8001) == 1);
        TEST(fix16_mul(-2, 0x8001) == -1);
    }
#endif
    
    {
        // Note that these tests make no assumptions on rounding.
        // They just require that fix16_from_double and fix16_mul round
        // identically.
        
        unsigned int i, j;
        int failures = 0;
        COMMENT("Running testcases for multiplication");
        
        for (i = 0; i < TESTCASES_COUNT; i++)
        {
            for (j = 0; j < TESTCASES_COUNT; j++)
            {
                fix16_t a = testcases[i];
                fix16_t b = testcases[j];
                fix16_t result = fix16_mul(a, b);
                
                double fa = fix16_to_double(a);
                double fb = fix16_to_double(b);
                fix16_t fresult = fix16_from_double(fa * fb);
                
                double max = fix16_to_double(fix16_max);
                double min = fix16_to_double(fix16_min);
                if (fa * fb > max || fa * fb < min)
                {
                    // Legitimate overflow
                    continue;
                }
                
                if (fresult != result)
                {
                    printf("\n%d * %d = %d\n", a, b, result);
                    printf("%f * %f = %d\n", fa, fb, fresult);
                    failures++;
                }
            }
        }
        
        TEST(failures == 0);
    }
    
    {
        COMMENT("Testing basic division");
        TEST(fix16_div(fix16_from_int(15), fix16_from_int(5)) == fix16_from_int(3));
        TEST(fix16_div(fix16_from_int(-15), fix16_from_int(5)) == fix16_from_int(-3));
        TEST(fix16_div(fix16_from_int(-15), fix16_from_int(-5)) == fix16_from_int(3));
        TEST(fix16_div(fix16_from_int(15), fix16_from_int(-5)) == fix16_from_int(-3));
    }
    
#ifndef FIXMATH_NO_ROUNDING
    {
        COMMENT("Testing division rounding corner cases");
        TEST(fix16_div(0, 10) == 0);
        TEST(fix16_div(1, fix16_from_int(2)) == 1);
        TEST(fix16_div(-1, fix16_from_int(2)) == -1);
        TEST(fix16_div(1, fix16_from_int(-2)) == -1);
        TEST(fix16_div(-1, fix16_from_int(-2)) == 1);
        TEST(fix16_div(3, fix16_from_int(2)) == 2);
        TEST(fix16_div(-3, fix16_from_int(2)) == -2);
        TEST(fix16_div(3, fix16_from_int(-2)) == -2);
        TEST(fix16_div(-3, fix16_from_int(-2)) == 2);
        TEST(fix16_div(2, 0x7FFF) == 4);
        TEST(fix16_div(-2, 0x7FFF) == -4);
        TEST(fix16_div(2, 0x8001) == 4);
        TEST(fix16_div(-2, 0x8001) == -4);
    }
#endif
    
    {
        unsigned int i, j;
        int failures = 0;
        COMMENT("Running testcases for division");
        
        for (i = 0; i < TESTCASES_COUNT; i++)
        {
            for (j = 0; j < TESTCASES_COUNT; j++)
            {
                fix16_t a = testcases[i];
                fix16_t b = testcases[j];
                
                // We don't require a solution for /0 :)
                if (b == 0) continue;
                
                fix16_t result = fix16_div(a, b);
                
                double fa = fix16_to_double(a);
                double fb = fix16_to_double(b);
                fix16_t fresult = fix16_from_double(fa / fb);
                
                double max = fix16_to_double(fix16_max);
                double min = fix16_to_double(fix16_min);
                if (fa / fb > max || fa / fb < min)
                {
                    // Legitimate overflow
                    continue;
                }
                
                if (fresult != result)
                {
                    printf("\n%d / %d = %d\n", a, b, result);
                    printf("%f / %f = %d\n", fa, fb, fresult);
                    failures++;
                }
            }
        }
        
        TEST(failures == 0);
    }
    
    if (status != 0)
        fprintf(stdout, "\n\nSome tests FAILED!\n");
    
    return status;
}