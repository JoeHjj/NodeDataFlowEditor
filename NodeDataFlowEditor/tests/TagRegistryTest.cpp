/*
    MIT License

    Copyright (c) 2025 Joseph Al Hajjar

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "taggable/TagRegistry.hpp"
#include <gtest/gtest.h>
#include <thread>

// Dummy tag types
struct Tag1
{};
struct Tag2
{};
struct Tag3
{};
struct Tag4
{};
struct Tag5
{};

// ----------------------------
// Basic registration and retrieval
// ----------------------------

TEST(TagRegistryTest, RegisterAndRetrieve)
{
    TagRegistry::unregisterAllTags(); // Ensure clean state

    size_t idx1 = TagRegistry::getTagIndex<Tag1>();
    size_t idx2 = TagRegistry::getTagIndex<Tag2>();

    EXPECT_NE(idx1, idx2);                             // Unique indices
    EXPECT_EQ(TagRegistry::getTagIndex<Tag1>(), idx1); // Consistent index
    EXPECT_EQ(TagRegistry::getTagIndex<Tag2>(), idx2);

    // Name retrieval
    EXPECT_STREQ(TagRegistry::getTagName<Tag1>(), typeid(Tag1).name());
    EXPECT_EQ(TagRegistry::getTagNameByIndex(idx1), typeid(Tag1).name());
    EXPECT_EQ(TagRegistry::getTagNameByIndex(idx2), typeid(Tag2).name());
}

// ----------------------------
// Register multiple tags at once
// ----------------------------

TEST(TagRegistryTest, RegisterMultiple)
{
    TagRegistry::unregisterAllTags();

    TagRegistry::registerTags<Tag1, Tag2, Tag3>();
    EXPECT_EQ(TagRegistry::tagCount(), 3);

    EXPECT_TRUE(TagRegistry::getTagIndex<Tag1>() < MaxTags);
    EXPECT_TRUE(TagRegistry::getTagIndex<Tag2>() < MaxTags);
    EXPECT_TRUE(TagRegistry::getTagIndex<Tag3>() < MaxTags);
}

// ----------------------------
// Unregister individual tag
// ----------------------------

TEST(TagRegistryTest, UnregisterTag)
{
    TagRegistry::unregisterAllTags();
    TagRegistry::registerTags<Tag1, Tag2>();

    size_t idx1 = TagRegistry::getTagIndex<Tag1>();
    TagRegistry::unregisterTag<Tag1>();

    // Tag1 removed
    EXPECT_EQ(TagRegistry::tagCount(), 1);
    EXPECT_EQ(TagRegistry::getTagNameByIndex(idx1), ""); // Should be empty

    // Tag2 still present
    size_t idx2 = TagRegistry::getTagIndex<Tag2>();
    EXPECT_EQ(TagRegistry::getTagNameByIndex(idx2), typeid(Tag2).name());
}

// ----------------------------
// Unregister all tags
// ----------------------------

TEST(TagRegistryTest, UnregisterAllTags)
{
    TagRegistry::registerTags<Tag1, Tag2, Tag3>();
    EXPECT_GT(TagRegistry::tagCount(), 0);

    TagRegistry::unregisterAllTags();
    EXPECT_EQ(TagRegistry::tagCount(), 0);

    // Index lookup should give new index now
    size_t idx1 = TagRegistry::getTagIndex<Tag1>();
    EXPECT_LT(idx1, MaxTags);
}

// ----------------------------
// MaxTags enforcement (MSVC-safe)
// ----------------------------

// Define 32 unique structs manually
struct T0
{};
struct T1
{};
struct T2
{};
struct T3
{};
struct T4
{};
struct T5
{};
struct T6
{};
struct T7
{};
struct T8
{};
struct T9
{};
struct T10
{};
struct T11
{};
struct T12
{};
struct T13
{};
struct T14
{};
struct T15
{};
struct T16
{};
struct T17
{};
struct T18
{};
struct T19
{};
struct T20
{};
struct T21
{};
struct T22
{};
struct T23
{};
struct T24
{};
struct T25
{};
struct T26
{};
struct T27
{};
struct T28
{};
struct T29
{};
struct T30
{};
struct T31
{};

TEST(TagRegistryTest, MaxTagsExceeded)
{
    TagRegistry::unregisterAllTags();

    // Fill registry with MaxTags unique types
    TagRegistry::getTagIndex<T0>();
    TagRegistry::getTagIndex<T1>();
    TagRegistry::getTagIndex<T2>();
    TagRegistry::getTagIndex<T3>();
    TagRegistry::getTagIndex<T4>();
    TagRegistry::getTagIndex<T5>();
    TagRegistry::getTagIndex<T6>();
    TagRegistry::getTagIndex<T7>();
    TagRegistry::getTagIndex<T8>();
    TagRegistry::getTagIndex<T9>();
    TagRegistry::getTagIndex<T10>();
    TagRegistry::getTagIndex<T11>();
    TagRegistry::getTagIndex<T12>();
    TagRegistry::getTagIndex<T13>();
    TagRegistry::getTagIndex<T14>();
    TagRegistry::getTagIndex<T15>();
    TagRegistry::getTagIndex<T16>();
    TagRegistry::getTagIndex<T17>();
    TagRegistry::getTagIndex<T18>();
    TagRegistry::getTagIndex<T19>();
    TagRegistry::getTagIndex<T20>();
    TagRegistry::getTagIndex<T21>();
    TagRegistry::getTagIndex<T22>();
    TagRegistry::getTagIndex<T23>();
    TagRegistry::getTagIndex<T24>();
    TagRegistry::getTagIndex<T25>();
    TagRegistry::getTagIndex<T26>();
    TagRegistry::getTagIndex<T27>();
    TagRegistry::getTagIndex<T28>();
    TagRegistry::getTagIndex<T29>();
    TagRegistry::getTagIndex<T30>();
    TagRegistry::getTagIndex<T31>();

    // Adding one more should throw
    struct ExtraTag
    {};
    EXPECT_THROW(TagRegistry::getTagIndex<ExtraTag>(), std::runtime_error);
}

// ----------------------------
// Thread safety basic test
// ----------------------------

TEST(TagRegistryTest, ThreadSafety)
{
    TagRegistry::unregisterAllTags();

    auto worker = []() {
        for (int i = 0; i < 10; ++i)
        {
            struct T
            {};
            TagRegistry::getTagIndex<T>();
        }
    };

    std::thread t1(worker);
    std::thread t2(worker);

    t1.join();
    t2.join();

    EXPECT_GE(TagRegistry::tagCount(), 1); // At least one tag registered
}
