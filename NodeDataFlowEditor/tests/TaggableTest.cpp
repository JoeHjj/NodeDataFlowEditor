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

#include "taggable/Taggable.hpp"
#include <gtest/gtest.h>

// Test tag types
struct TagA
{};
struct TagB
{};
struct TagC
{};
struct TagD
{};

class Dummy : public Taggable
{};

// ----------------------------
// Basic Tag Add / Remove
// ----------------------------

TEST(TaggableTest, AddTag)
{
    Dummy d;
    d.addTag<TagA>();
    EXPECT_TRUE(d.hasTag<TagA>());
    EXPECT_FALSE(d.hasTag<TagB>());
}

TEST(TaggableTest, RemoveTag)
{
    Dummy d;
    d.addTag<TagA>();
    EXPECT_TRUE(d.hasTag<TagA>());
    d.removeTag<TagA>();
    EXPECT_FALSE(d.hasTag<TagA>());
}

TEST(TaggableTest, ToggleTag)
{
    Dummy d;
    d.toggleTag<TagA>();
    EXPECT_TRUE(d.hasTag<TagA>());
    d.toggleTag<TagA>();
    EXPECT_FALSE(d.hasTag<TagA>());
}

// ----------------------------
// Variadic Tag Operations
// ----------------------------

TEST(TaggableTest, AddTagsVariadic)
{
    Dummy d;
    d.addTags<TagA, TagB, TagC>();
    EXPECT_TRUE((d.hasTags<TagA, TagB, TagC>()));
}

TEST(TaggableTest, RemoveTagsVariadic)
{
    Dummy d;
    d.addTags<TagA, TagB, TagC>();
    d.removeTags<TagA, TagC>();
    EXPECT_FALSE((d.hasTag<TagA>()));
    EXPECT_TRUE((d.hasTag<TagB>()));
    EXPECT_FALSE((d.hasTag<TagC>()));
}

TEST(TaggableTest, ToggleTagsVariadic)
{
    Dummy d;
    d.toggleTags<TagA, TagB>();
    EXPECT_TRUE((d.hasTags<TagA, TagB>()));
    d.toggleTags<TagA, TagB>();
    EXPECT_FALSE((d.hasAnyOfTags<TagA, TagB>()));
}

// ----------------------------
// Tag Checking
// ----------------------------

TEST(TaggableTest, HasAllTags)
{
    Dummy d;
    d.addTags<TagA, TagB>();
    EXPECT_TRUE((d.hasTags<TagA, TagB>()));
}

TEST(TaggableTest, HasAnyTags)
{
    Dummy d;
    d.addTag<TagA>();
    EXPECT_TRUE((d.hasAnyOfTags<TagA, TagB>()));
    EXPECT_FALSE((d.hasAnyOfTags<TagB, TagC>()));
}

TEST(TaggableTest, HasNoneTags)
{
    Dummy d;
    d.addTag<TagA>();
    EXPECT_TRUE((d.hasNoneOfTags<TagB, TagC>()));
    EXPECT_FALSE((d.hasNoneOfTags<TagA, TagB>()));
}

// ----------------------------
// Bitmask Operations
// ----------------------------

TEST(TaggableTest, CopyTags)
{
    Dummy a, b;
    a.addTags<TagA, TagB>();
    b.copyTagsFrom(a);
    EXPECT_TRUE((b.hasTags<TagA, TagB>()));
}

TEST(TaggableTest, MergeTags)
{
    Dummy a, b;
    a.addTag<TagA>();
    b.addTag<TagB>();

    a.mergeTagsFrom(b);
    EXPECT_TRUE((a.hasTags<TagA, TagB>()));
}

TEST(TaggableTest, MoveTags)
{
    Dummy a, b;

    a.addTags<TagA, TagB>();
    b.MoveTagsFrom(a);

    EXPECT_TRUE((b.hasTags<TagA, TagB>()));
    EXPECT_FALSE((a.hasAnyOfTags<TagA, TagB>()));
}

// ----------------------------
// Swap
// ----------------------------

TEST(TaggableTest, SwapTags)
{
    Dummy a, b;
    a.addTag<TagA>();
    b.addTag<TagB>();

    a.swapTagsWith(b);

    EXPECT_TRUE((a.hasTag<TagB>()));
    EXPECT_TRUE((b.hasTag<TagA>()));
}

// ----------------------------
// Free Helper Functions
// ----------------------------

TEST(TaggableFreeFunctions, AddRemoveToggle)
{
    Dummy d;

    addTag<TagA>(d);
    EXPECT_TRUE((hasTag<TagA>(d)));

    toggleTag<TagA>(d);
    EXPECT_FALSE((hasTag<TagA>(d)));

    addTags<TagA, TagB>(d);
    EXPECT_TRUE((hasTags<TagA, TagB>(d)));

    removeTags<TagA, TagB>(d);
    EXPECT_TRUE((hasNoneOfTags<TagA, TagB>(d)));
}

TEST(TaggableFreeFunctions, CommonTags)
{
    Dummy a, b;

    a.addTag<TagA>();
    b.addTag<TagB>();
    EXPECT_FALSE(haveAnyCommonTag(a, b));

    b.addTag<TagA>();
    EXPECT_TRUE(haveAnyCommonTag(a, b));
}

TEST(TaggableFreeFunctions, EqualsTagSets)
{
    Dummy a, b;

    a.addTags<TagA, TagB>();
    b.addTags<TagA, TagB>();
    EXPECT_TRUE(haveSameTags(a, b));

    b.addTag<TagC>();
    EXPECT_FALSE(haveSameTags(a, b));
}

TEST(TaggableFreeFunctions, HasAllTagsOf)
{
    Dummy a, b;

    a.addTags<TagA, TagB, TagC>();
    b.addTags<TagA, TagC>();

    EXPECT_TRUE(hasAllTagsOf(a, b));
    EXPECT_FALSE(hasAllTagsOf(b, a));
}

// ----------------------------
// Clear Tags (Resets Registry)
// ----------------------------

TEST(TaggableTest, ClearTags)
{
    Dummy d;
    d.addTags<TagA, TagB>();
    d.clearTags();

    EXPECT_FALSE((d.hasAnyOfTags<TagA, TagB>()));
}
