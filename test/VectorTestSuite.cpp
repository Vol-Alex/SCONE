#include "src/Vector.h"

#include <gmock/gmock.h>

#include <memory>
#include <vector>

namespace SCONE
{
namespace UT
{
using namespace testing;

template <typename T>
class VectorTestSuiteFundamentalType : public ::testing::Test
{
};

TYPED_TEST_SUITE(VectorTestSuiteFundamentalType, ::testing::Types<CompactVector<int>>);

TYPED_TEST(VectorTestSuiteFundamentalType, testConstruction)
{
    std::vector<int> intArray = {1, 2, 3, 5, 4};
    TypeParam testArray(intArray);
    EXPECT_EQ(intArray.size(), testArray.size());
    EXPECT_EQ(5U, testArray.capacity());
    EXPECT_THAT(testArray, ElementsAreArray(intArray));
    EXPECT_TRUE(std::equal(intArray.rbegin(), intArray.rend(), testArray.rbegin()));
    EXPECT_NE(testArray.begin(), testArray.end());
    EXPECT_EQ(5, testArray[3U]);
}

TYPED_TEST(VectorTestSuiteFundamentalType, testEmpty)
{
    TypeParam testArray;
    EXPECT_TRUE(testArray.empty());
    EXPECT_EQ(0U, testArray.size());
    EXPECT_EQ(testArray.begin(), testArray.end());
}

TYPED_TEST(VectorTestSuiteFundamentalType, testReserve)
{
    auto intArray = {1, 2, 3, 5, 4};
    TypeParam testArray(intArray);
    EXPECT_EQ(5U, testArray.capacity());

    testArray.reserve(1);
    EXPECT_EQ(5U, testArray.size());
    EXPECT_EQ(5U, testArray.capacity());

    testArray.reserve(10);
    EXPECT_EQ(5U, testArray.size());
    EXPECT_EQ(10U, testArray.capacity());
    EXPECT_THAT(testArray, ElementsAreArray(intArray));

    auto longCapacity = std::numeric_limits<uint16_t>::max() + 1U;
    testArray.reserve(longCapacity);
    EXPECT_EQ(5U, testArray.size());
    EXPECT_EQ(longCapacity, testArray.capacity());
    EXPECT_THAT(testArray, ElementsAreArray(intArray));
}

TYPED_TEST(VectorTestSuiteFundamentalType, testCopy)
{
    auto intArray = {1, 2, 3, 5, 4};
    TypeParam testArray(intArray);
    EXPECT_THAT(testArray, ElementsAreArray(intArray));

    TypeParam copyArray(testArray);
    EXPECT_THAT(copyArray, ElementsAreArray(intArray));
}

TYPED_TEST(VectorTestSuiteFundamentalType, testMove)
{
    auto intArray = {1, 2, 3, 5, 4};
    TypeParam testArray = {1, 2, 3, 5, 4};
    EXPECT_THAT(testArray, ElementsAreArray(intArray));

    TypeParam movedArray(std::move(testArray));
    EXPECT_THAT(movedArray, ElementsAreArray(intArray));
    EXPECT_EQ(0U, testArray.size()); // Container must be in empty state after move.
}

template <typename VectorType>
std::vector<int> getCapacities()
{
    return {0, 1, 3, 3};
}

TYPED_TEST(VectorTestSuiteFundamentalType, testPushBack)
{
    const auto& capacities = getCapacities<TypeParam>();
    ASSERT_EQ(4U, capacities.size());

    TypeParam testArray;
    EXPECT_EQ(0U, testArray.size());
    EXPECT_EQ(capacities[0], testArray.capacity());

    testArray.push_back(5);
    EXPECT_EQ(1U, testArray.size());
    EXPECT_EQ(capacities[1], testArray.capacity());
    EXPECT_EQ(5, testArray[0U]);

    testArray.push_back(2);
    EXPECT_EQ(2U, testArray.size());
    EXPECT_EQ(capacities[2], testArray.capacity());
    EXPECT_EQ(2, testArray[1U]);

    testArray.push_back(1);
    EXPECT_EQ(3U, testArray.size());
    EXPECT_EQ(capacities[3], testArray.capacity());
    EXPECT_EQ(1, testArray[2U]);
}

TYPED_TEST(VectorTestSuiteFundamentalType, testErase)
{
    TypeParam testArray({1, 2, 3, 4, 5});
    EXPECT_EQ(5U, testArray.size());

    {
        auto result = testArray.erase(testArray.begin(), testArray.begin());
        EXPECT_EQ(1, *result);
        EXPECT_EQ(5U, testArray.size());
    }
    {
        auto result = testArray.erase(testArray.begin() + 2U);
        EXPECT_EQ(4, *result);
        EXPECT_EQ(4U, testArray.size());
    }

    {
        auto result = testArray.erase(testArray.begin(), testArray.begin() + 3U);
        EXPECT_EQ(5, *result);
        EXPECT_EQ(1U, testArray.size());
    }
}

TYPED_TEST(VectorTestSuiteFundamentalType, testInsert)
{
    TypeParam testArray = {1, 2, 3, 4, 5};
    EXPECT_EQ(5U, testArray.size());

    auto result = testArray.insert(testArray.begin(), 6);
    EXPECT_EQ(6, *result);
    EXPECT_EQ(6U, testArray.size());
    EXPECT_THAT(testArray, ElementsAreArray({6, 1, 2, 3, 4, 5}));
}

TYPED_TEST(VectorTestSuiteFundamentalType, testInsertRange)
{
    {
        TypeParam testArray = {1, 2, 3, 4, 5};
        EXPECT_EQ(5U, testArray.size());

        int array[] = {6, 7, 8, 9};
        auto result = testArray.insert(testArray.begin() + 3U, array, array + 4U);
        EXPECT_EQ(6, *result);
        EXPECT_EQ(9U, testArray.size());
        EXPECT_THAT(testArray, ElementsAreArray({1, 2, 3, 6, 7, 8, 9, 4, 5}));
    }

    {
        TypeParam testArray = {1, 2, 3, 4, 5};
        EXPECT_EQ(5U, testArray.size());

        int array[] = {6, 7};
        auto result = testArray.insert(testArray.begin(), array, array + 2U);
        EXPECT_EQ(6, *result);
        EXPECT_EQ(7U, testArray.size());
        EXPECT_THAT(testArray, ElementsAreArray({6, 7, 1, 2, 3, 4, 5}));
    }
}

TYPED_TEST(VectorTestSuiteFundamentalType, testFrontBack)
{
    TypeParam testArray = {1, 2, 3, 4, 5};
    EXPECT_EQ(1, testArray.front());
    EXPECT_EQ(5, testArray.back());
}

TYPED_TEST(VectorTestSuiteFundamentalType, testPopBack)
{
    TypeParam testArray = {1, 2};
    EXPECT_EQ(2, testArray.back());

    testArray.pop_back();
    EXPECT_EQ(1, testArray.back());

    testArray.pop_back();
    EXPECT_EQ(0U, testArray.size());
}

TYPED_TEST(VectorTestSuiteFundamentalType, testRvalueOverloads)
{
    TypeParam testArray;

    testArray.push_back({});
    EXPECT_EQ(1, testArray.size());
    EXPECT_EQ(0, testArray.back());

    testArray.insert(testArray.end(), {});
    EXPECT_EQ(2, testArray.size());
    EXPECT_EQ(0, testArray.back());
}

TYPED_TEST(VectorTestSuiteFundamentalType, testSwap)
{
    TypeParam testArray = {1, 2, 3, 4, 5};
    TypeParam otherTestArray = {6};

    testArray.swap(otherTestArray);

    ASSERT_EQ(1, testArray.size());
    EXPECT_EQ(6, testArray.front());
    EXPECT_EQ(6, testArray.back());

    ASSERT_EQ(5, otherTestArray.size());
    EXPECT_EQ(1, otherTestArray.front());
    EXPECT_EQ(5, otherTestArray.back());
}

struct TestType
{
    TestType(int& liveObjCounter)
        : _liveObjCounter(liveObjCounter)
    {
        ++(_liveObjCounter.get());
    }

    TestType(const TestType& other)
        : _liveObjCounter(other._liveObjCounter)
    {
        if (other.shouldCopyCtorThrow)
        {
            throw std::runtime_error("");
        }

        ++(_liveObjCounter.get());
    }

    TestType& operator=(const TestType& other) = default;

    ~TestType()
    {
        --(_liveObjCounter.get());
    }

public:
    bool shouldCopyCtorThrow = false;

private:
    std::reference_wrapper<int> _liveObjCounter;
};

template <typename T>
class VectorTestSuiteClassType : public ::testing::Test
{
};

TYPED_TEST_SUITE(VectorTestSuiteClassType, ::testing::Types<CompactVector<TestType>>);

TYPED_TEST(VectorTestSuiteClassType, testConstruction)
{
    int objectsCounter = 0;
    {
        std::vector<TestType> origArray = {TestType(objectsCounter), TestType(objectsCounter), TestType(objectsCounter)};
        TypeParam testArray(origArray);
        EXPECT_EQ(3U, testArray.size());
        EXPECT_EQ(3U, testArray.capacity());
        EXPECT_EQ(origArray.size(), testArray.size());
        EXPECT_EQ(6, objectsCounter);
    }
    EXPECT_EQ(0, objectsCounter);
}

TYPED_TEST(VectorTestSuiteClassType, testExceptionSafety)
{
    int objectsCounter = 0;
    {
        std::vector<TestType> origArray(3, TestType(objectsCounter));
        origArray.back().shouldCopyCtorThrow = true;

        std::unique_ptr<TypeParam> testArray;
        EXPECT_THROW(testArray = std::make_unique<TypeParam>(origArray), std::runtime_error);
        EXPECT_EQ(nullptr, testArray);
        EXPECT_EQ(3, objectsCounter);
    }
    EXPECT_EQ(0, objectsCounter);
}

TYPED_TEST(VectorTestSuiteClassType, testMove)
{
    int objectsCounter = 0;
    {
        TypeParam testArray = {TestType(objectsCounter)};
        TypeParam movedArray(std::move(testArray));
        EXPECT_EQ(1U, movedArray.size());
        EXPECT_EQ(0U, testArray.size()); // Container must be in empty state after move.
        EXPECT_EQ(1U, objectsCounter);
    }
    EXPECT_EQ(0U, objectsCounter);

    objectsCounter = 0;
    {
        TypeParam testArray = {
            TestType(objectsCounter), TestType(objectsCounter), TestType(objectsCounter)};
        TypeParam movedArray(std::move(testArray));
        EXPECT_EQ(3U, movedArray.size());
        EXPECT_EQ(0U, testArray.size()); // Container must be in empty state after move.
        EXPECT_EQ(3U, objectsCounter);
    }
    EXPECT_EQ(0U, objectsCounter);
}

TYPED_TEST(VectorTestSuiteClassType, testInsertRangeWithException)
{
    int objectsCounter = 0;
    {
        TypeParam testArray = {
            TestType(objectsCounter), TestType(objectsCounter), TestType(objectsCounter)};
        EXPECT_EQ(3U, testArray.size());
        EXPECT_EQ(3U, objectsCounter);

        TestType array[] = {TestType(objectsCounter), TestType(objectsCounter), TestType(objectsCounter)};
        array[2].shouldCopyCtorThrow = true;
        EXPECT_EQ(6U, objectsCounter);

        EXPECT_THROW(testArray.insert(testArray.begin() + 2U, array, array + 3U), std::runtime_error);
    }
    EXPECT_EQ(0U, objectsCounter);
}

} // namespace UT
} // namespace SCONE
