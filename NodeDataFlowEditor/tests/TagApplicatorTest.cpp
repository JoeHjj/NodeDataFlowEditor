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

#include "taggable/TagApplicator.hpp"
#include "taggable/TagRegistry.hpp"
#include "taggable/Taggable.hpp"
#include <gtest/gtest.h>

namespace data
{
    struct ImageType
    {};
    struct DataFrame
    {};
    struct OnnxData
    {};
    template <typename T>
    struct ValueWrapper
    {};
} // namespace data

class TagApplicatorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Clear TagRegistry before each test to avoid collisions
        TagRegistry::unregisterAllTags();
    }
};

TEST_F(TagApplicatorTest, RegisterSingleTagAndApply)
{
    TagApplicator::registerTag<data::ImageType>();

    Taggable t;
    bool applied = TagApplicator::apply(TagRegistry::getTagName<data::ImageType>(), t);

    EXPECT_TRUE(applied);
    EXPECT_TRUE(t.hasTag<data::ImageType>());
}

TEST_F(TagApplicatorTest, MultiTagRegistrarRegistersAll)
{
    using Registrar = TagApplicator::MultiTagRegistrar<data::ImageType, data::DataFrame, data::OnnxData, data::ValueWrapper<int>>;
    Registrar registrar;

    Taggable t;
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::ImageType>(), t));
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::DataFrame>(), t));
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::OnnxData>(), t));
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::ValueWrapper<int>>(), t));

    EXPECT_TRUE(t.hasTag<data::ImageType>());
    EXPECT_TRUE(t.hasTag<data::DataFrame>());
    EXPECT_TRUE(t.hasTag<data::OnnxData>());
    EXPECT_TRUE(t.hasTag<data::ValueWrapper<int>>());
}

TEST_F(TagApplicatorTest, MultiTagRegistrarHandlesEmpty)
{
    using EmptyRegistrar = TagApplicator::MultiTagRegistrar<>;
    EmptyRegistrar registrar;

    Taggable t;
    // Nothing to apply, ensure no crash
    EXPECT_FALSE(TagApplicator::apply("NonExistentTag", t));
}

TEST_F(TagApplicatorTest, ApplyMultipleTimes)
{
    TagApplicator::registerTag<data::ImageType>();
    TagApplicator::registerTag<data::DataFrame>();

    Taggable t;

    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::ImageType>(), t));
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::DataFrame>(), t));

    // Apply again
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::ImageType>(), t));
    EXPECT_TRUE(TagApplicator::apply(TagRegistry::getTagName<data::DataFrame>(), t));

    EXPECT_TRUE(t.hasTag<data::ImageType>());
    EXPECT_TRUE(t.hasTag<data::DataFrame>());
}
