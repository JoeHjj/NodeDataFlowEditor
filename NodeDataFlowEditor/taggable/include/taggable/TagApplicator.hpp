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

#pragma once
#include "taggable/TagRegistry.hpp"
#include "taggable/Taggable.hpp"
#include <functional>
#include <string>
#include <unordered_map>

class TagApplicator
{
public:
    // Register a tag type with a string name
    template <typename Tag>
    static void registerTag()
    {
        auto name = TagRegistry::getTagName<Tag>();
        size_t idx = TagRegistry::getTagIndex<Tag>();
        nameToIndex[name] = idx;

        indexToApply[idx] = [](Taggable& t) {
            t.addTag<Tag>();
        };
    }

    // Apply a tag from string
    static bool apply(const std::string& tagName, Taggable& t)
    {
        auto it = nameToIndex.find(tagName);
        if (it == nameToIndex.end())
            return false;

        size_t idx = it->second;
        indexToApply[idx](t);
        return true;
    }

    template <typename... Tags>
    struct MultiTagRegistrar
    {
        MultiTagRegistrar()
        {
            if constexpr (sizeof...(Tags) > 0) // handle empty case
            {
                (TagApplicator::registerTag<Tags>(), ...);
            }
        }
    };

private:
    static inline std::unordered_map<std::string, size_t> nameToIndex;
    static inline std::unordered_map<size_t, std::function<void(Taggable&)>> indexToApply;
};
