#ifndef STRING_HPP
#define STRING_HPP

#include "stl/types.hpp"
#include "stl/algorithms.hpp"
#include "stl/vector.hpp"
#include <utility>

namespace std
{
    uint64_t str_len(const char *a);

    template <typename CharT>
    struct basic_string
    {
    public:
        typedef CharT *iterator;
        typedef const CharT *const_iterator;

    private:
        size_t _size;
        size_t _capacity;
        CharT *_data;

    public:
        basic_string() : _size(0), _capacity(1), _data(new CharT[1])
        {
            _data[0] = '\0';
        }

        basic_string(const CharT *s) : _size(str_len(s)), _capacity(_size + 1), _data(new CharT[_capacity])
        {
            std::copy_n(_data, s, _capacity);
        }

        explicit basic_string(size_t __capacity) : _size(0), _capacity(__capacity), _data(new CharT[_capacity])
        {
            _data[0] = '\0';
        }

        basic_string(const basic_string &rhs) : _size(rhs._size), _capacity(rhs._capacity), _data(new CharT[_capacity])
        {
            std::copy_n(_data, rhs._data, _size + 1);
        }

        basic_string &operator=(const basic_string &rhs)
        {
            if (this != &rhs)
            {
                if (_capacity < rhs._capacity || !_data)
                {
                    if (_data)
                    {
                        delete[] _data;
                    }

                    _capacity = rhs._capacity;
                    _data = new CharT[_capacity];
                }

                _size = rhs._size;
                std::copy_n(_data, rhs._data, _size + 1);
            }

            return *this;
        }

        basic_string(basic_string &&rhs) : _size(rhs._size), _capacity(rhs._capacity), _data(rhs._data)
        {
            rhs._size = 0;
            rhs._capacity = 0;
            rhs._data = nullptr;
        }

        basic_string &operator=(basic_string &&rhs)
        {
            if (_data)
            {
                delete[] _data;
            }

            _size = rhs._size;
            _capacity = rhs._capacity;
            _data = rhs._data;

            rhs._size = 0;
            rhs._capacity = 0;
            rhs._data = nullptr;

            return *this;
        }

        ~basic_string()
        {
            if (_data)
            {
                delete[] _data;
            }
        }

        void clear()
        {
            _size = 0;
            _data[0] = '\0';
        }

        void pop_back()
        {
            _data[--_size] = '\0';
        }

        basic_string operator+(CharT c) const
        {
            basic_string copy(*this);

            copy += c;

            return move(copy);
        }

        basic_string &operator+=(CharT c)
        {
            if (!_data || _capacity <= _size + 1)
            {
                _capacity = _capacity ? _capacity * 2 : 1;
                auto new_data = new CharT[_capacity];

                if (_data)
                {
                    std::copy_n(new_data, _data, _size);
                    delete[] _data;
                }

                _data = new_data;
            }

            _data[_size] = c;
            _data[++_size] = '\0';

            return *this;
        }

        size_t size() const
        {
            return _size;
        }

        size_t capacity() const
        {
            return _capacity;
        }

        bool empty() const
        {
            return !_size;
        }

        const CharT *c_str() const
        {
            return _data;
        }

        CharT &operator[](size_t i)
        {
            return _data[i];
        }

        const CharT &operator[](size_t i) const
        {
            return _data[i];
        }

        bool operator==(const CharT *s) const
        {
            if (size() != str_len(s))
            {
                return false;
            }

            for (size_t i = 0; i < size(); ++i)
            {
                if (_data[i] != s[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator==(const basic_string &rhs) const
        {
            if (size() != rhs.size())
            {
                return false;
            }

            for (size_t i = 0; i < size(); ++i)
            {
                if (_data[i] != rhs._data[i])
                {
                    return false;
                }
            }

            return true;
        }

        iterator begin()
        {
            return iterator(&_data[0]);
        }

        iterator end()
        {
            return iterator(&_data[_size]);
        }

        const_iterator begin() const
        {
            return const_iterator(&_data[0]);
        }

        const_iterator end() const
        {
            return const_iterator(&_data[_size]);
        }
    };

    typedef basic_string<char> string;

    uint64_t parse(const char *str);
    uint64_t parse(const char *str, const char *end);
    uint64_t parse(const string &str);

    vector<string> split(const string &s);

}
#endif
