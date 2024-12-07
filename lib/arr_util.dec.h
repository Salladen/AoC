//
// Author: Salladen
// Date: 29/07/2024
// Project: Algos
//

#ifndef ARR_UTIL_DEC
#define ARR_UTIL_DEC
#include <tuple>
#include <cstddef> // size_t and some other types
#include <cstring>

namespace salad {
    static int copies = 0;
    template<typename T>
    struct Array {
        T* data;
        size_t size;
        bool owner;

        Array(T* data, size_t size, const bool owner=true) : data(data), size(size), owner(owner) {}
        explicit Array(size_t size) : data(new T[size]), size(size), owner(true) {
            std::memset(data, 0, sizeof(T) * size);
        }

        Array(const Array& other);
        ~Array();
    
        static Array from(T* data, size_t size) {
            // Don't take ownership of the data as it's not allocated by us
            return Array{data, size, false};
        }

        // Index operator
        T& operator[](size_t idx) const;
        // Slicing operator
        Array operator[](const std::tuple<size_t, size_t>& slice) const;
        Array& operator=(const Array& other);

        Array& operator=(Array&& other) noexcept;
    };
}

#endif //ARR_UTIL_DEC
