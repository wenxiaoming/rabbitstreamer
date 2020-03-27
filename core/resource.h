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
#ifndef CORE_RS_RESOURCE_H_
#define CORE_RS_RESOURCE_H_

#include <memory>

template <class Type, class... Args>
inline typename std::enable_if<!std::is_array<Type>::value,
                               std::unique_ptr<Type>>::type
make_unique_ptr(Args &&... args) {
    return std::unique_ptr<Type>(new Type(std::forward<Args>(args)...));
}

template <class Type>
inline typename std::enable_if<std::is_array<Type>::value &&
                                   0 == std::extent<Type>::value,
                               std::unique_ptr<Type>>::type
make_unique_ptr(size_t size) {
    typedef typename remove_extent<Type>::type U;
    return std::unique_ptr<Type>(new U[size]());
}

template <class Type, class... Args>
typename std::enable_if<std::extent<Type>::value != 0, void>::type
make_unique_ptr(Args &&... args) = delete;

#endif