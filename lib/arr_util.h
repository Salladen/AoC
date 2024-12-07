//
// Author: Salladen
// Date: 29/07/2024
// Project: Algos
//

#ifndef ARR_UTIL_H
#define ARR_UTIL_H
#include "arr_util.dec.h"
#include "functional"
#include <tuple>
#include <iostream>
#include <cstddef> // size_t and some other types
#include <cstring>
#include <fmt/format.h>

namespace salad {
    template <typename T>
    Array<T>::Array(const Array& other) : data(new T[other.size]), size(other.size), owner(true) {
        if (this != &other) {
            if (other.size < 1) {
                return;
            }

            salad::copies += other.size * sizeof(T);
            std::memcpy(this->data, other.data, sizeof(T) * other.size);
            return;
        }


        this->data = other.data; // Let them alias
    }

    template <typename T>
    T& Array<T>::operator[](std::size_t idx) const {
        return data[idx];   
    }

    template <typename T>
    Array<T> Array<T>::operator[](const std::tuple<std::size_t, std::size_t>& slice) const {
        std::size_t start = std::get<0>(slice);
        std::size_t end = std::get<1>(slice);
        if (end > this->size) {
            end = this->size;
        }

        start = start < end ? start : end;
        
        size_t new_size = end - start;
        return Array{this->data + start, new_size, false};
    }

    template <typename T>
    Array<T>& Array<T>::operator=(const Array& other) {
        {
            if (this != &other) {
                if (owner && data != nullptr) {
                    delete[] data;
                }

                data = new T[other.size];
                salad::copies += other.size * sizeof(T);
                std::memcpy(data, other.data, sizeof(T) * other.size);
                size = other.size;
                owner = true;
            }
            
            return *this;
        }
    }

    template<typename T>
    Array<T>& Array<T>::operator=(Array&& other) noexcept {
        {
            if (this != &other) {
                if (owner && data != nullptr) {
                    delete[] data;
                }

                data = other.data;
                size = other.size;
                owner = false;
            }
            
            return *this;
        }
    }

    template <typename T>
    Array<T>::~Array() {
        if (owner && data != nullptr) {
            delete[] data;
        }
    }

    inline int test() {
        using arrT = int;
        std::function<std::string(Array<arrT>&)> repr = [](Array<arrT>& a) {
            std::string repr;
            for (size_t i = 0; i < a.size; i++) {
                repr += std::to_string(a[i]);
                if (i < a.size - 1) {
                    repr += ", ";
                }
            }
            return repr;
        };
        
        arrT* arr = new arrT[6]{0, 1, 2, 3, 4, 5};
        
        auto* arr_data = new unsigned char[sizeof(arrT) * 6];
        memcpy(arr_data, arr, sizeof(arrT) * 6);
        delete[] arr;
        Array<arrT> a = Array<arrT>::from(reinterpret_cast<arrT*>(arr_data), 6);
        Array<arrT> a_splice = a[{1,4}];
        Array<arrT> a_copy = a;
        std::cout << "..:: Array Utility Test ::..\n"
                  << fmt::format("Copy doesn't alias: {:s}\n", a.data != a_copy.data)
                  << fmt::format("Array:\t\t {:s}\n", repr(a))
                  << fmt::format("Array[{:d}:{:d}]:\t {:s}\n", 1, 4, repr(a_splice))
                  << std::endl;
                  
        
        return 0;
    }
}
#endif //ARR_UTIL_H