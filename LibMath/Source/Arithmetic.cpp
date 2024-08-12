#include <cmath>

#include "Arithmetic.h"

namespace LibMath
{
    float   squareRoot(float square)
    {
        //if (square <= 0.f)
        //    return 0.f;
        //
        //const float     closeness = 0.00001F;
        //float           x = square, root = 0.F, count = 0.F;
        //
        //while (true)
        //{
        //    count++;
        //
        //    root = 0.5F * (x + (square / x));
        //
        //    float   res = root - x;
        //    float   abs_val = (res < 0) ? -res : res;
        //
        //    if (abs_val < closeness)
        //        break;
        //
        //    x = root;
        //}
        
        return sqrtf(square); // TODO: fix
    }

    float power(float base, unsigned int power)
    {
        if (0 == power)
            return 1;
        else if (1 == power)
            return base;

        if (base == 0.0f)
            return base;

        float baseCpy = base;

        for (unsigned int it = 0; it < power; it++)
            base *= baseCpy;
        base;

        return base;
    }

    unsigned int	factorial(unsigned int num)
    {
        if (0 == num || 1 == num)
            return 1;
        else if (2 == num)
            return 2;

        unsigned int numCpy = num;

        for (unsigned int i = num; i > 0; num--)
            num *= --numCpy;

        return num;
    }

    float LibMath::absolute(float num)
    {
        return (num >= 0.F) ? num : -num;
    }

    bool LibMath::almostEqual(float left, float right)
    {
        const float precision = 0.001F;

        return absolute(left - right) < precision;
    }

    float LibMath::min(float a, float b)
    {
        return (a < b) ? a : b;
    }

    float LibMath::max(float a, float b)
    {
        return (a > b) ? a : b;
    }

    int LibMath::absolute(int num)
    {
        return (num >= 0) ? num : -num;
    }

    float LibMath::clamp(float val, float low, float high)
    {
        if (val < low)
            return low;
        else if (val > high)
            return high;

        return val;
    }

    float LibMath::floor(float val)
    {
        return static_cast<float>((int)val);
    }

    float LibMath::ceiling(float val)
    {
        float floored = floor(val);

        return (floored == val) ? floored : floored + 1.f;
    }
}