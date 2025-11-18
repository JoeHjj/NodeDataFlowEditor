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

#include <bitset>
#include <mutex>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

constexpr size_t MaxTags = 32;           ///< Maximum number of tag types that can be registered.
using TagBitMask = std::bitset<MaxTags>; ///< Type alias for a tag bitmask used by Taggable objects.

/**
 * @brief Global registry for managing tag types and their unique indices.
 *
 * TagRegistry provides a thread-safe mechanism to assign and look up numeric indices
 * for arbitrary tag types (classes or structs). These indices are used to represent
 * tags as bits inside a TagBitMask, enabling fast tag comparison and matching.
 *
 * Each unique tag type (e.g., `struct FloatDataTag {}`) receives a unique integer index.
 * Tags can later be queried, unregistered, or listed by name and index.
 *
 * Example usage:
 * @code
 * struct FloatDataTag {};
 * struct ImageTag {};
 *
 * // Register tags
 * TagRegistry::registerTags<FloatDataTag, ImageTag>();
 *
 * // Retrieve indices
 * size_t idx = TagRegistry::getTagIndex<FloatDataTag>();
 * std::string_view name = TagRegistry::getTagNameByIndex(idx);
 * @endcode
 */
class TagRegistry
{
public:
    /**
     * @brief Get (or assign) a unique index for a given tag type.
     * @tparam Tag A class or struct type used as a tag.
     * @return Unique index corresponding to the Tag type.
     * @throws std::runtime_error if MaxTags is exceeded.
     *
     * Thread-safe. Registers the tag if it has not been seen before.
     */
    template <typename Tag>
    static size_t getTagIndex()
    {
        static_assert(std::is_class_v<Tag>, "Tags must be class or struct types");

        const std::type_index typeIdx(typeid(Tag));
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = tagIndices.find(typeIdx);
        if (it != tagIndices.end())
            return it->second;

        if (tagIndices.size() >= MaxTags)
            throw std::runtime_error("Maximum number of tags exceeded");

        size_t newIndex = tagIndices.size();
        tagIndices[typeIdx] = newIndex;
        indexToName[newIndex] = typeid(Tag).name();
        return newIndex;
    }

    /**
     * @brief Get the human-readable name of a tag type.
     * @tparam Tag The tag type.
     * @return C-string containing the tag’s type name.
     */
    template <typename Tag>
    static const char* getTagName()
    {
        return typeid(Tag).name();
    }

    /**
     * @brief Look up the name of a tag given its index.
     * @param index Numeric tag index.
     * @return Tag name if found, otherwise an empty string view.
     */
    static std::string_view getTagNameByIndex(size_t index)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = indexToName.find(index);
        return it != indexToName.end() ? std::string_view(it->second) : "";
    }

    /**
     * @brief Register multiple tag types at once.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    static void registerTags()
    {
        (getTagIndex<Tags>(), ...);
    }

    /**
     * @brief Unregister a specific tag type.
     * @tparam Tag The tag type to remove from the registry.
     */
    template <typename Tag>
    static void unregisterTag()
    {
        const std::type_index typeIdx(typeid(Tag));
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = tagIndices.find(typeIdx);
        if (it != tagIndices.end())
        {
            size_t index = it->second;
            tagIndices.erase(it);
            indexToName.erase(index);
        }
    }

    /**
     * @brief Remove all registered tags from the registry.
     */
    static void unregisterAllTags()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tagIndices.clear();
        indexToName.clear();
    }

    /**
     * @brief Get the total number of registered tags.
     * @return Number of tags currently stored.
     */
    static size_t tagCount()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return tagIndices.size();
    }

private:
    static inline std::unordered_map<std::type_index, size_t> tagIndices; ///< Map from tag type to index.
    static inline std::unordered_map<size_t, const char*> indexToName;    ///< Map from index to tag name.
    static inline std::mutex mutex_;                                      ///< Mutex for thread-safe access.
};
