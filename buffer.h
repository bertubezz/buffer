#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

class Buffer
{
public:
    using value_type = std::byte;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    pointer _begin, _end, _endOfStorage;

public:
    // TODO: enum flags
    enum class Options
    {
        Uninitialize = 0x0,
        Initialize = 0x1,
        Shrink = 0x2,
        Default = Uninitialize
    };

    Buffer()
        : _begin(nullptr), _end(nullptr), _endOfStorage(nullptr)

    {
    }

    Buffer(uint64_t size, Options options = Options::Default)
        : _begin(new value_type[size]), _end(_begin + size), _endOfStorage(_end)
    {
        if (options == Options::Initialize)
            memset(_begin, 0, size);
    }

    explicit Buffer(std::string_view other)
    {
        assign(other);
    }

    Buffer(const Buffer& other)
    {
        assign(other);
    }

    Buffer(Buffer&& other) noexcept
        : Buffer()
    {
        swap(other);
    }

    ~Buffer()
    {
        if (_begin)
            delete[] _begin;
    }

    Buffer& operator=(const Buffer& other)
    {
        const auto newSize = other.size();
        if (capacity() < newSize)
        {
            const auto newStorage = new value_type[newSize];
            if (_begin)
                delete[] _begin;

            _begin = newStorage;
            _endOfStorage = _begin + newSize;
        }

        _end = _begin + newSize;
        memcpy(_begin, other._begin, newSize);

        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept
    {
        swap(other);
        return *this;
    }

    pointer data() noexcept { return _begin; }
    const_pointer data() const noexcept { return _begin; }

    std::string_view toString() const { return std::string_view(std::string_view::const_pointer(_begin), size()); }

    reference operator[](uint64_t index) noexcept { return _begin[index]; }
    const_reference operator[](uint64_t index) const noexcept { return _begin[index]; }

    reference front() noexcept { return *_begin; }
    const_reference front() const noexcept { return *_begin; }

    reference back() noexcept { return *(_end - 1); }
    const_reference back() const noexcept { return *(_end - 1); }

    iterator begin() noexcept { return _begin; }
    const_iterator begin() const noexcept { return _begin; }
    const_iterator cbegin() const noexcept { return _begin; }
    reverse_iterator rbegin() noexcept { return reverse_iterator(_end); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(_end); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(_end); }

    iterator end() noexcept { return _end; }
    const_iterator end() const noexcept { return _end; }
    const_iterator cend() const noexcept { return _end; }
    reverse_iterator rend() noexcept { return reverse_iterator(_begin); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(_begin); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(_begin); }

    uint64_t size() const noexcept { return _end - _begin; }
    uint64_t capacity() const noexcept { return _endOfStorage - _begin; }
    uint64_t max_size() const noexcept { return UINT64_MAX; }

    bool empty() const noexcept { return _begin == _end; }
    void clear() { _end = _begin; }

    void reserve(uint64_t newSize)
    {
        if (capacity() < newSize)
        {
            const auto oldSize = size();
            const auto newStorage = new value_type[newSize];
            memcpy(newStorage, _begin, oldSize);

            if (_begin)
                delete[] _begin;

            _begin = newStorage;
            _end = _begin + oldSize;
            _endOfStorage = _begin + newSize;
        }
    }

    void resize(uint64_t newSize, Options options = Options::Default)
    {
        const auto oldSize = size();

        if (newSize == 0 && options == Options::Shrink)
        {
            if (_begin)
                delete[] _begin;
            _begin = nullptr;
            _end = nullptr;
            _endOfStorage = nullptr;
            return;
        }

        if (options == Options::Shrink || capacity() < newSize)
        {
            const auto newStorage = new value_type[newSize];
            memcpy(newStorage, _begin, oldSize);

            if (_begin)
                delete[] _begin;
            _begin = newStorage;
            _endOfStorage = _begin + newSize;
        }

        _end = _begin + newSize;

        if (options == Options::Initialize && oldSize < newSize)
            memset(_begin + oldSize, 0, newSize - oldSize);
    }

    // T must implement `.data` and `.size`
    template<typename T>
    void assign(const T& other)
    {
        const auto newSize = other.size();

        if (newSize > capacity() || !_begin)
        {
            if (_begin)
                delete[] _begin;

            _begin = new value_type[newSize];
            _endOfStorage = _begin + newSize;
        }

        _end = _begin + newSize;
        memcpy(_begin, other.data(), newSize);
    }

    // T must implement `.data` and `.size`
    template<typename T>
    void insert(const_iterator position, const T& other)
    {
        const auto otherSize = other.size();
        const auto newSize = size() + otherSize;

        if (capacity() < newSize)
        {
            const auto newStorage = new value_type[newSize];

            memcpy(newStorage, _begin, position - _begin);
            memcpy(newStorage + (position - _begin) + otherSize,
                position, _end - position);

            if (_begin)
                delete[] _begin;
            _begin = newStorage;
            _endOfStorage = _begin + newSize;
        }
        else
        {
            memmove(position + otherSize, position, _end - position);
        }

        _end = _begin + newSize;
        memcpy(position, other.data(), otherSize);
    }

    iterator erase(const_iterator position)
    {
        auto begin = const_cast<iterator>(position);

        --_end;
        memmove(begin, begin + 1, (_end - begin));
        return begin;
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        auto begin = const_cast<iterator>(first);
        auto size = last - first;

        _end -= size;
        memmove(begin, begin + size, (_end - begin));
        return begin;
    }

    void swap(Buffer& other)
    {
        std::swap(_begin, other._begin);
        std::swap(_end, other._end);
        std::swap(_endOfStorage, other._endOfStorage);
    }

    friend void swap(Buffer& lhs, Buffer& rhs)
    {
        lhs.swap(rhs);
    }

    friend bool operator==(const Buffer& lhs, const Buffer& rhs)
    {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    friend bool operator !=(const Buffer& lhs, const Buffer& rhs)
    {
        return !(lhs == rhs);
    }
};

