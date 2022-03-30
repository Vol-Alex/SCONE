
#include "src/TaggedPtr.h"

#include <gtest/gtest.h>

#include <memory>

namespace SCONE
{
namespace UT
{

TEST(TaggedPtrTestSuite, testHeapPtr)
{
    auto heapPtr = std::make_unique<int>(10);

    TaggedPtr ptr(heapPtr.get());
    EXPECT_FALSE(ptr.hasFlag());

    ptr.setFlag(true);
    EXPECT_TRUE(ptr.hasFlag());
    EXPECT_EQ(heapPtr.get(), ptr.getAs<int>());
}

TEST(TaggedPtrTestSuite, testStackPtr)
{
    int stackPtr = 1;

    TaggedPtr ptr(&stackPtr);
    EXPECT_FALSE(ptr.hasFlag());

    ptr.setFlag(true);
    EXPECT_TRUE(ptr.hasFlag());
    EXPECT_EQ(&stackPtr, ptr.getAs<int>());
}
} // namespace UT
} // namespace SCONE
