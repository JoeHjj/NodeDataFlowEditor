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
#include <vector>

/**
 * @brief Base class that provides tagging functionality via TagRegistry.
 *
 * Taggable allows derived classes to associate compile-time tag types
 * with objects. Tags are stored internally as bits in a `TagBitMask`
 * (std::bitset) for efficient comparison and filtering.
 *
 * Typical use cases include:
 * - Tagging node ports or objects with type or compatibility information.
 * - Quickly checking tag compatibility between objects.
 * - Copying or merging tag sets between instances.
 *
 * Example:
 * @code
 * struct FloatDataTag {};
 * struct TextureTag {};
 *
 * class Port : public Taggable {};
 *
 * Port p;
 * p.addTag<FloatDataTag>();
 *
 * if (p.hasTag<FloatDataTag>()) { ... }
 * @endcode
 */
class Taggable
{
public:
    /**
     * @brief Add a specific tag type to this object.
     * @tparam Tag The tag type to add (must be a class or struct).
     */
    template <typename Tag>
    void addTag() noexcept
    {
        tagMask.set(TagRegistry::getTagIndex<Tag>());
    }

    /**
     * @brief Remove a specific tag from this object.
     * @tparam Tag The tag type to remove.
     */
    template <typename Tag>
    void removeTag() noexcept
    {
        tagMask.reset(TagRegistry::getTagIndex<Tag>());
    }

    /**
     * @brief Toggle the presence of a tag (add if missing, remove if present).
     * @tparam Tag The tag type to toggle.
     */
    template <typename Tag>
    void toggleTag() noexcept
    {
        if (!hasTag<Tag>())
            addTag<Tag>();
        else
            removeTag<Tag>();
    }

    /**
     * @brief Check if this object has a specific tag.
     * @tparam Tag The tag type to check.
     * @return True if the tag is present.
     */
    template <typename Tag>
    [[nodiscard]] bool hasTag() const noexcept
    {
        return tagMask.test(TagRegistry::getTagIndex<Tag>());
    }

    /**
     * @brief Check if the object has *all* of the specified tags.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    [[nodiscard]] bool hasTags() const noexcept
    {
        return (... && hasTag<Tags>());
    }

    /**
     * @brief Check if the object has *any* of the specified tags.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    [[nodiscard]] bool hasAnyOfTags() const noexcept
    {
        return (... || hasTag<Tags>());
    }

    /**
     * @brief Check if the object has *none* of the specified tags.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    [[nodiscard]] bool hasNoneOfTags() const noexcept
    {
        return !hasAnyOfTags<Tags...>();
    }

    /**
     * @brief Add multiple tags at once.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    void addTags() noexcept
    {
        (addTag<Tags>(), ...);
    }

    /**
     * @brief Remove multiple tags at once.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    void removeTags() noexcept
    {
        (removeTag<Tags>(), ...);
    }

    /**
     * @brief Toggle multiple tags at once.
     * @tparam Tags Variadic list of tag types.
     */
    template <typename... Tags>
    void toggleTags() noexcept
    {
        (toggleTag<Tags>(), ...);
    }

    /**
     * @brief Clear all tags and reset the registry.
     *
     * Note: This resets the global TagRegistry as well.
     */
    void clearTags() noexcept
    {
        TagRegistry::unregisterAllTags();
        tagMask.reset();
    }

    /**
     * @brief Get the current tag bitmask.
     * @return Bitmask representing all assigned tags.
     */
    [[nodiscard]] TagBitMask getTagBitMask() const noexcept
    {
        return tagMask;
    }

    /**
     * @brief Replace the tag bitmask for this object.
     * @param mask New tag bitmask.
     */
    void setTagBitMask(const TagBitMask& mask) noexcept
    {
        tagMask = mask;
    }

    /**
     * @brief Copy all tags from another Taggable object.
     * @param other The source object to copy from.
     */
    void copyTagsFrom(const Taggable& other) noexcept
    {
        tagMask = other.getTagBitMask();
    }

    /**
     * @brief Merge tags from another object (logical OR).
     * @param other Source object.
     */
    void mergeTagsFrom(const Taggable& other) noexcept
    {
        tagMask |= other.getTagBitMask();
    }

    /**
     * @brief Move tags from another object (copy + clear source).
     * @param other Source object to move from.
     */
    void MoveTagsFrom(Taggable& other) noexcept
    {
        tagMask = other.getTagBitMask();
        other.tagMask.reset();
    }

    /**
     * @brief Swap tag bitmasks with another Taggable.
     * @param other Object to swap tags with.
     */
    void swapTagsWith(Taggable& other) noexcept
    {
        std::swap(tagMask, other.tagMask);
    }

    /**
     * @brief Return a list of tag names currently set on this object.
     */
    std::vector<std::string> tags() const noexcept
    {
        std::vector<std::string> result;
        TagBitMask mask = getTagBitMask();

        for (size_t i = 0; i < TagRegistry::tagCount(); ++i)
        {
            if (mask.test(i))
            {
                std::string_view tagName = TagRegistry::getTagNameByIndex(i);
                result.emplace_back(tagName);
            }
        }

        return result;
    }

protected:
    TagBitMask tagMask{}; ///< Bitmask storing the current tags for this object.
};

// ================================
// Free helper functions for Taggable types
// ================================

/**
 * @brief Add a tag to a Taggable object (free function version).
 */
template <typename Tag, typename T>
inline void
addTag(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template addTag<Tag>();
}

/**
 * @brief Remove a tag from a Taggable object.
 */
template <typename Tag, typename T>
inline void
removeTag(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template removeTag<Tag>();
}

/**
 * @brief Toggle a tag on a Taggable object.
 */
template <typename Tag, typename T>
inline void
toggleTag(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template toggleTag<Tag>();
}

/**
 * @brief Add multiple tags to a Taggable object.
 */
template <typename... Tags, typename T>
inline void
addTags(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template addTags<Tags...>();
}

/**
 * @brief Remove multiple tags from a Taggable object.
 */
template <typename... Tags, typename T>
inline void
removeTags(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template removeTags<Tags...>();
}

/**
 * @brief Toggle multiple tags on a Taggable object.
 */
template <typename... Tags, typename T>
inline void
toggleTags(T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_reference_t<T>>, "Object must inherit from Taggable");
    object.template toggleTags<Tags...>();
}

/**
 * @brief Check if an object has a specific tag.
 */
template <typename Tag, typename T>
inline bool
hasTag(const T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_cv_t<T>>, "Object must inherit from Taggable");
    return object.template hasTag<Tag>();
}

/**
 * @brief Check if an object has all specified tags.
 */
template <typename... Tags, typename T>
inline bool
hasTags(const T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_cv_t<T>>, "Object must inherit from Taggable");
    return object.template hasTags<Tags...>();
}

/**
 * @brief Check if an object has any of the specified tags.
 */
template <typename... Tags, typename T>
inline bool
hasAnyOfTags(const T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_cv_t<T>>, "Object must inherit from Taggable");
    return object.template hasAnyOfTags<Tags...>();
}

/**
 * @brief Check if an object has none of the specified tags.
 */
template <typename... Tags, typename T>
inline bool
hasNoneOfTags(const T& object)
{
    static_assert(std::is_base_of_v<Taggable, std::remove_cv_t<T>>, "Object must inherit from Taggable");
    return object.template hasNoneOfTags<Tags...>();
}

/**
 * @brief Check if two objects share at least one common tag.
 */
inline bool
haveAnyCommonTag(const Taggable& a, const Taggable& b)
{
    return (a.getTagBitMask() & b.getTagBitMask()).any();
}

/**
 * @brief Check if two objects have exactly the same tag set.
 */
inline bool
haveSameTags(const Taggable& a, const Taggable& b)
{
    return a.getTagBitMask() == b.getTagBitMask();
}

/**
 * @brief Check if all tags of @p b are present in @p a.
 */
inline bool
hasAllTagsOf(const Taggable& a, const Taggable& b)
{
    return (a.getTagBitMask() & b.getTagBitMask()) == b.getTagBitMask();
}

/**
 * @brief Copy all tags from source to destination.
 */
inline void
copyTagsFrom(Taggable& destination, const Taggable& source) noexcept
{
    destination.setTagBitMask(source.getTagBitMask());
}

/**
 * @brief Merge tags from source into destination (logical OR).
 */
inline void
mergeTagsFrom(Taggable& destination, const Taggable& source) noexcept
{
    destination.setTagBitMask(destination.getTagBitMask() | source.getTagBitMask());
}

/**
 * @brief Move tags from source to destination, clearing the source.
 */
inline void
moveTagsFrom(Taggable& destination, Taggable& source) noexcept
{
    destination.setTagBitMask(source.getTagBitMask());
    source.clearTags();
}

/**
 * @brief Swap the tag bitmasks between two taggable objects.
 */
inline void
swapTagsWith(Taggable& a, Taggable& b) noexcept
{
    TagBitMask temp = a.getTagBitMask();
    a.setTagBitMask(b.getTagBitMask());
    b.setTagBitMask(temp);
}
