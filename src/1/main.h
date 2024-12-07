//
// Created by lin-salad on 12/6/24.
//

#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include "arr_util.h"

namespace salad {
    template<typename int_t = int32_t>
    int_t stoi (const std::string &str, const unsigned short base = 10) {
        int_t num = 0;
        bool negative = str[0] == '-';
        
        for (char* caret = const_cast<char*>(str.c_str()) + negative; *caret != '\0'; caret++) {
            char& c = *caret;
            if (c >= 'a' && c <= 'z') {
                c ^= 0x20;
            }
            
            if (c < '0' || (c > '9' && c < 'A')) {
                throw std::invalid_argument("invalid argument");
            }
            num = num * base + (c - (c <= '9' ? '0' : 'A' - 10)); 
        }
        return num * (negative ? -1 : 1);
    }

    template<typename int_t = int32_t>
    std::string itos (int_t num, const unsigned short base = 10) {
        std::string str{};
        while (num) {
            str.push_back(num % base + (num % base < 10 ? '0' : 'A' - 10));
            num /= base;
        }

        for (size_t i = 0; i < str.size() / 2; i++) {
            std::swap(str[i], str[str.size() - i - 1]);
        }
        return str;
    }

    template<typename T = int32_t, typename U = int64_t>
    U binary_search (const Array<T>& arr, T val) {
        int64_t left = 0;
        int64_t right = arr.size;
        while (left < right) {
            // Avoid overflow
            int64_t mid = left + (right - left) / 2;
            if (arr[mid] < val) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return left;
    }
    
    template<typename T = int32_t>
    Array<T>& merge (const Array<T>& a, const Array<T>& b, Array<T>& out) {
        T* res_caret = out.data;

        const T* a_end = a.data + a.size;
        const T* b_end = b.data + b.size;

        const T* a_caret = a.data;
        const T* b_caret = b.data;
        for (; a_caret != a_end && b_caret != b_end; res_caret++) {
            int32_t l = *a_caret;
            int32_t r = *b_caret;
            if (l < r) {
                *res_caret = l;
                a_caret++;
            } else {
                *res_caret = r;
                b_caret++;
            }
        }

        // Copy the remaining elements (the finished loop won't run because one of the arrays is empty)
        for (; a_caret != a_end; a_caret++, res_caret++) {
            *res_caret = *a_caret;
        }

        for (; b_caret != b_end; b_caret++, res_caret++) {
            *res_caret = *b_caret;
        }
        return out;
    }

    template<typename T = int32_t>
    Array<T>& merge_sort (Array<T>& arr) {
        if (arr.size <= 1) {
            return arr;
        }

        Array<T> arr_cpy = Array<T>(arr);
        Array<T> left = arr_cpy[{0, arr.size / 2}];
        left = merge_sort(left);
        Array<T> right = arr_cpy[{arr.size / 2, arr.size}];
        right = merge_sort(right);
        merge<T>(left, right, arr);
        return arr;
    }

    template<typename T = int32_t>
    Array<T>& insertion_sort (Array<T>& arr) {
        if (arr.size <= 1) {
            return arr;
        }

        Array<T> search_space = Array<T>::from(arr.data, arr.size); 
        for (size_t i = 1; i < arr.size; i++) {
            T key = arr[i];

            search_space = arr[{0, i}];
            const size_t dest = binary_search(search_space, key);
            for (size_t j = i; j > dest; j--) {
                arr[j] = arr[j - 1];
                arr[j - 1] = key;
            }
        }
        return arr;
    }
    
    template<typename T = int32_t, size_t K = 2>
    Array<T>& merge_sort_iterative (Array<T>& arr) {
        size_t chunk_size = K;
        
        if (arr.size <= 1) {
            return arr;
        } else if (arr.size <= chunk_size) {
            arr = insertion_sort(arr);
            return arr;
        }

        Array<T> arr_view = Array<T>::from(arr.data, arr.size);
        for (size_t i = 0; i < arr.size; i += chunk_size) {
            arr_view = arr[{i, i + chunk_size}];
            insertion_sort(arr_view);
        }

        size_t sublists = std::ceil(static_cast<double>(arr.size) / static_cast<double>(chunk_size));

        while (sublists > 1) {
            for (size_t i = 0; i < arr.size; i += chunk_size * 2) {
                arr_view = arr[{i, i + chunk_size * 2}];
                Array<T> left = arr_view[{0, chunk_size}];
                Array<T> right = arr_view[{chunk_size, chunk_size * 2}];
                
                Array<T> out_buffer = Array<T>(arr_view.size);
                merge<T>(left, right, out_buffer);
                std::memcpy(arr.data + i, out_buffer.data, sizeof(T) * arr_view.size);
                salad::copies += sizeof(T) * arr_view.size;
            }
            sublists = std::ceil(static_cast<double>(sublists) / 2.0f);
            chunk_size *= 2;
        }

        return arr;
    }
}

#endif //MAIN_H
