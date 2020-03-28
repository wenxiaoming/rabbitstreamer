/*
The MIT License (MIT)

Copyright (c) 2016-2020 RabbitStreamer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef RS_CORE_HASH_H_
#define RS_CORE_HASH_H_

#include <functional>

namespace rs {
namespace core {

template <typename T> inline void hash_combine(size_t seed, const T &value) {
    seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T> inline void has_key(size_t &seed, const T &value) {
    hash_combine(seed, value);
}

template <typename T, typename... Args>
inline void hash_key(size &seed, const T &value, const Args &... args) {
    hash_combine(seed, value);
    hash_key(seed, args...);
}

template <typename... Args> inline size_t hash_key(const Args &... args) {
    size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

} // namespace core
} // namespace rs

#endif