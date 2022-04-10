#include "Vector.h"

namespace SCONE
{
int8_t getHighestBit(size_t value)
{
    int8_t result = -1;
    while (value)
    {
        ++result;
        value >>= 1;
    }
    return result;
}
} // namespace SCONE
