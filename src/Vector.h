#pragma once

#include "TaggedPtr.h"
#include "VectorFwd.h"

#include <cassert>
#include <algorithm>

namespace SCONE
{
int8_t getHighestBit(size_t value);

namespace VectorDetails
{
namespace StorageDetails
{
template <typename T>
void destroy(T& t)
{
    t.~T();
}

template <typename Iterator>
void destroy(Iterator it, const Iterator& end)
{
    for (; it != end; ++it)
    {
        destroy(*it);
    }
}
} // namespace StorageDetails

template <typename T>
class MemoryOptimizedStorage final
{
public :
    using value_type = T;

public:
    MemoryOptimizedStorage() = default;

    MemoryOptimizedStorage(const MemoryOptimizedStorage&) = delete;
    MemoryOptimizedStorage operator =(const MemoryOptimizedStorage&) = delete;

    MemoryOptimizedStorage(MemoryOptimizedStorage&& other)
    {
        *this = std::move(other);
    }

    MemoryOptimizedStorage& operator=(MemoryOptimizedStorage&& other) noexcept
    {
        if (this != &other)
        {
            std::swap(_ptr, other._ptr);
            other.free();
        }
        return *this;
    }

    ~MemoryOptimizedStorage()
    {
        free();
    }

    void allocate(const size_t capacity)
    {
        assert(!_ptr);
        if (capacity <= std::numeric_limits<typename ShortData::Size>::max())
        {
            _ptr = operator new(sizeof(ShortData) + (capacity - 1U) * sizeof(T));
            auto* ptr = _ptr.getAs<ShortData>();
            ptr->capacity = static_cast<typename ShortData::Size>(capacity);
            ptr->size = 0U;
        }
        else
        {
            _ptr = operator new(sizeof(LongData) + (capacity - 1U) * sizeof(T));
            _ptr.setFlag(true);

            auto* ptr = _ptr.getAs<LongData>();
            ptr->capacity = static_cast<typename LongData::Size>(capacity);
            ptr->size = 0U;
        }
    }

    void free()
    {
        if (_ptr)
        {
            auto* it = data();
            StorageDetails::destroy(it, it + size());

            operator delete(_ptr.getAs<void>());
            _ptr = nullptr;
        }
    }

    uint32_t size() const
    {
        return _ptr ? (_ptr.hasFlag() ? _ptr.getAs<LongData>()->size : _ptr.getAs<ShortData>()->size) : 0U;
    }

    uint32_t capacity() const
    {
        return _ptr ? (_ptr.hasFlag() ? _ptr.getAs<LongData>()->capacity : _ptr.getAs<ShortData>()->capacity) : 0U;
    }

    T* data() const
    {
        return _ptr ? (_ptr.hasFlag() ? _ptr.getAs<LongData>()->data : _ptr.getAs<ShortData>()->data) : nullptr;
    }

    void advanceSize(ptrdiff_t value)
    {
        _ptr.hasFlag() ? _ptr.getAs<LongData>()->size += value : _ptr.getAs<ShortData>()->size += value;
    }

    void swap(MemoryOptimizedStorage& other)
    {
        _ptr.swap(other._ptr);
    }

private:
    template <typename SizeType>
    struct Data final
    {
        using Size = SizeType;

        SizeType size;
        SizeType capacity;
        T data[1];
    };

    using LongData = Data<uint32_t>;
    using ShortData = Data<uint16_t>;

private:
    TaggedPtr _ptr;
};

} // namespace VectorDetails

template <typename StorageType>
class Vector final
{
public:
    using value_type = typename StorageType::value_type;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = pointer;
    using const_iterator = const_pointer;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    Vector()
    {
    }

    Vector(const Vector& other)
        : Vector(std::begin(other), std::end(other))
    {
    }

    Vector& operator=(const Vector& other)
    {
        if (this != &other)
        {
            swap(Vector(other));
        }
    }

    Vector(Vector&&) noexcept(std::is_nothrow_move_constructible<StorageType>::value) = default;
    Vector& operator=(Vector&&) noexcept(std::is_nothrow_move_assignable<StorageType>::value) = default;

    Vector(std::initializer_list<value_type> list)
        : Vector(std::begin(list), std::end(list))
    {
    }

    template <typename Range>
    Vector(Range range)
        : Vector(std::begin(range), std::end(range))
    {
    }

    template <typename Iterator>
    Vector(Iterator begin, const Iterator end)
    {
        if (const auto size = std::distance(begin, end))
        {
            try
            {
                reallocate(size);

                for (auto it = this->begin(); begin != end; ++begin, ++it)
                {
                    new (it) value_type(*begin);
                    _storage.advanceSize(1);
                }
            }
            catch (...)
            {
                clear();
                throw;
            }
        }
    }

    value_type& operator[](size_t pos)
    {
        assert(pos < size());
        return _storage.data()[pos];
    }

    const value_type& operator[](size_t pos) const
    {
        assert(pos < size());
        return _storage.data()[pos];
    }

    value_type& front()
    {
        return (*this)[0U];
    }

    const value_type& front() const
    {
        return (*this)[0U];
    }

    value_type& back()
    {
        return (*this)[size() - 1U];
    }

    const value_type& back() const
    {
        return (*this)[size() - 1U];
    }

    iterator begin()
    {
        return _storage.data();
    }

    iterator end()
    {
        return begin() + size();
    }

    const_iterator begin() const
    {
        return _storage.data();
    }

    const_iterator end() const
    {
        return begin() + size();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    bool empty() const
    {
        return size() == 0U;
    }

    size_t size() const
    {
        return _storage.size();
    }

    size_t capacity() const
    {
        return _storage.capacity();
    }

    void clear()
    {
        _storage.free();
    }

    void reserve(const size_t capacity)
    {
        if (capacity > this->capacity())
        {
            reallocate(capacity);
        }
    }

    void push_back(const value_type& value)
    {
        insertImpl(end(), value);
    }

    void push_back(value_type&& value)
    {
        insertImpl(end(), std::move(value));
    }

    template <class... Args>
    void emplace_back(Args&&... args)
    {
        push_back(T(std::forward<Args>(args)...));
    }

    void pop_back()
    {
        assert(size() > 0U);
        if (size() > 0U)
        {
            _storage.advanceSize(-1);
            VectorDetails::StorageDetails::destroy(*end());
        }
    }

    iterator insert(const_iterator pos, const value_type& value)
    {
        return insertImpl(pos, value);
    }

    iterator insert(const_iterator pos, value_type&& value)
    {
        return insertImpl(pos, std::move(value));
    }

    template <typename ForwardIt>
    iterator insert(const_iterator it, ForwardIt begin, const ForwardIt& end)
    {
        const auto dist = std::distance<const_iterator>(this->begin(), it);
        if (begin == end)
        {
            return this->begin() + dist;
        }

        const auto size = this->size();
        const auto srcDist = std::distance(begin, end);
        reserve(getNextCapacity(size + srcDist));
        // Restore "pos" after reallocation.
        auto pos = this->begin() + dist;

        const auto thisEnd = this->end();
        auto moveResultIt = thisEnd + srcDist;

        try
        {
            // Move existing data backward to create hole for new data.
            for (auto it = thisEnd; it != pos;)
            {
                --it;
                --moveResultIt;
                if (moveResultIt >= thisEnd)
                {
                    new (moveResultIt) value_type(std::move(*it));
                }
                else
                {
                    *(moveResultIt) = std::move(*it);
                }
            }

            // Insert new data
            for (; pos != thisEnd && begin != end; ++begin, ++pos)
            {
                *pos = *begin;
            }

            for (; begin != end; ++begin, ++pos)
            {
                new (pos) value_type(*begin);
                _storage.advanceSize(1);
            }
            _storage.advanceSize(size + srcDist - this->size());
        }
        catch (...)
        {
            VectorDetails::StorageDetails::destroy(std::max(this->end(), moveResultIt), thisEnd + srcDist);
            throw;
        }

        return pos - srcDist;
    }

    iterator erase(const_iterator it)
    {
        return erase(it, it + 1U);
    }

    iterator erase(const const_iterator it, const const_iterator endIt)
    {
        const auto dist = std::distance<const_iterator>(this->begin(), it);
        const auto result = this->begin() + dist;
        if (it != endIt)
        {
            auto first = result;
            auto end = first + std::distance(it, endIt);

            const auto endDataIt = this->end();
            first = moveData(end, endDataIt, first);

            _storage.advanceSize(std::distance(endDataIt, first));
            VectorDetails::StorageDetails::destroy(first, endDataIt);
        }

        return result;
    }

    bool operator==(const Vector& other) const
    {
        return size() == other.size() && std::equal(begin(), end(), other.begin());
    }

    bool operator!=(const Vector& other) const
    {
        return !(*this == other);
    }

    void swap(Vector& other)
    {
        _storage.swap(other._storage);
    }

private:
    template <typename T>
    iterator insertImpl(const_iterator it, T&& value)
    {
        const auto size = this->size();
        const auto dist = std::distance<const_iterator>(begin(), it);
        if (capacity() == size)
        {
            reserve(getNextCapacity(size + 1U));
        }
        auto pos = begin() + dist;

        const auto endIt = end();
        if (pos == endIt)
        {
            new (endIt) value_type(std::forward<T>(value));
            _storage.advanceSize(1);
        }
        else
        {
            auto it = endIt;
            new (endIt) value_type(std::move(*(--it)));
            _storage.advanceSize(1);

            // TODO: Optimize for POD types.
            while (it != pos)
            {
                auto& item = *(it--);
                item = std::move(*it);
            }
            *pos = std::forward<T>(value);
        }

        return pos;
    }

    void reallocate(const size_t capacity)
    {
        assert(capacity >= size());
        Vector tmp(std::move(*this));

        _storage.allocate(capacity);
        moveData(tmp.begin(), tmp.end(), begin());
    }

    value_type* moveData(value_type* first, value_type* end, value_type* result)
    {
        assert(first != result);
        // TODO: Optimize for POD types.
        const auto endDataIt = this->end();
        for (; first != end; ++first, ++result)
        {
            if (endDataIt <= result)
            {
                new (result) value_type(std::move(*first));
                _storage.advanceSize(1);
            }
            else
            {
                *result = std::move(*first);
            }
        }
        return result;
    }

    size_t getNextCapacity(const size_t size) const
    {
        size_t result = 0U;
        if (size > 0U)
        {
            result = (1U << (getHighestBit(size) + 1)) - 1;
        }
        return result;
    }

private:
    StorageType _storage;
};

} // namespace SCONE
