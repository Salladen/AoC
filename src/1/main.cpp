#include "main.h"

#include <fstream>
#include <iostream>
#include <fmt/format.h>

#include "color_tools.h"
#include "arr_util.h"

using salad::Array;

int sorting_check(Array<int32_t>& (*sorting_algo)(Array<int32_t>&)) {
    int32_t arr_data[10] = {7, 5, -8, 9, 0, 6, 1, -8, 0, -10};
    Array<int32_t> arr = Array<int32_t>::from(arr_data, sizeof(arr_data) / sizeof(int32_t));

    std::string arr_size_str = salad::format_color("{" + salad::itos(arr.size) + ":f cyan}");
    std::cout << fmt::format("..:: Unsorted Array [{}] ::..", arr_size_str) << std::endl;
    for (int i = 0; i < arr.size; i++) {
        std::cout << fmt::format("{:+d} ", arr[i]);
    }
    std::cout << std::endl;

    Array<int32_t> sorted = sorting_algo(arr);
    arr_size_str = salad::format_color("{" + salad::itos(sorted.size) + ":f cyan}");
    std::cout << fmt::format("..:: Sorted Array [{}] ::..", arr_size_str) << std::endl;
    // Copy the array to avoid modifying the original
    for (int32_t* p = sorted.data; p != sorted.data + sorted.size; p++) {
        std::cout << fmt::format("{:+d} ", *p);
        if (p != sorted.data && *p < *(p-1)) {
            return 1;
        }
    }
    std::cout << std::endl;
    
    return 0;
}

int main(int argc, char** argv) {
    std::cout << fmt::format("<< Array Utility Test >>\n");
    if (const int res=salad::test() != 0) {
        return res;
    }
    std::cout << fmt::format("<< ------------------ >>\n") << std::endl;
    
    std::cout << fmt::format("<< Sorting Algorithm Test >>\n");

    // If there's a second argument, check if it's 'merge_sort', 'imerge_sort', or 'insertion_sort'
    Array<uint32_t>& (*usorting_algo)(Array<uint32_t>&) = nullptr;
    Array<int32_t>& (*isorting_algo)(Array<int32_t>&) = nullptr;
    if (argc > 2) {
        if (std::string(argv[2]) == "merge_sort") {
            usorting_algo = salad::merge_sort<uint32_t>;
            isorting_algo = salad::merge_sort<int32_t>;
        } else if (std::string(argv[2]) == "insertion_sort") {
            usorting_algo = salad::insertion_sort<uint32_t>;
            isorting_algo = salad::insertion_sort<int32_t>;
        } else if (std::string(argv[2]) == "imerge_sort") {
            usorting_algo = salad::merge_sort_iterative<uint32_t, 32>;
            isorting_algo = salad::merge_sort_iterative<int32_t, 32>;
        }
    }
    if (usorting_algo == nullptr || isorting_algo == nullptr) {
        std::cerr << fmt::format("Invalid sorting algorithm specified\nDefaulting to recursive merge_sort\n") << std::endl;
        usorting_algo = salad::merge_sort<uint32_t>;
        isorting_algo = salad::merge_sort<int32_t>;
    } else {
        std::cout << fmt::format("Using sorting algorithm: {}\n", argv[2]) << std::endl;
    }

    if (const int res=sorting_check(isorting_algo); res) {
        return res;
    }
    char* locs_path = argv[1];
    std::cout << fmt::format("<< ---------------------- >>\n", locs_path) << std::endl;
    
    std::ifstream locs;
    // Open the file and scan to the end to get the size
    locs.open(locs_path, std::ios::in | std::ios::ate);
    if (!locs.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    // Get the size of the file using the current position
    std::streamsize size = locs.tellg();
    locs.seekg(0, std::ios::beg);
    // Store the file contents in a buffer
    char data[size];
    locs.read(data, size);
    locs.close();
    
    std::cout << fmt::format("Read {} characters from file\n", size) << std::endl;

    uint8_t line_len = 0;
    for (const char c : data) {
        if (c == '\n') {
            break;
        }
        line_len++;
    }
    uint32_t line_count = size / (line_len+1);
    
    Array<uint32_t> note1 = Array<uint32_t>(line_count);
    Array<uint32_t> note2 = Array<uint32_t>(line_count);

    uint32_t* note1_caret = note1.data;
    uint32_t* note2_caret = note2.data;
    char* num_buffer = new char[line_len];
    size_t i = 0;
    uint8_t j = 0;
    for (char* caret = data; caret != data + size; caret++) {
        bool numeric = *caret >= '0' && *caret <= '9';
        bool last_iter = caret == data + size - 1;
        if (!numeric || last_iter) {
            num_buffer[j] = '\0';
            if (j == 0) {
                continue;
            }
            j = 0;
            *(i++ % 2 == 0 ? note1_caret++ : note2_caret++) = salad::stoi<uint32_t>(num_buffer);
            
           continue; 
        }

        num_buffer[j++] = *caret;
        j %= line_len;
    }

    note1 = salad::merge_sort_iterative<uint32_t>(note1);

    // Sort the second location notes
    note2 = salad::merge_sort_iterative<uint32_t>(note2);
    
    uint64_t diffs = 0;
    uint64_t similarity = 0;

    auto binary_search = salad::binary_search<uint32_t, size_t>;

    // Copy
    Array<uint32_t> note2_view = Array<uint32_t>::from(note2.data, note2.size);

    for (uint32_t* p1 = note1.data, *p2 = note2.data; p1 != note1.data + note1.size; p1++, p2++) {
        uint32_t l = *p1;
        uint32_t r = *p2;
        diffs += l > r ? l - r : r - l;
        
        // Find n occurrences of l in note2, both are sorted so binary search(note2, l) - binary search(note2, l+1) gives the number of occurrences
        size_t r_start = binary_search(note2_view, l);
        note2_view = note2_view[{r_start, note2_view.size}];
        size_t r_end = binary_search(note2_view, l+1);
        size_t n = r_end; // - r_start (omitted because r_start is always 0 in this case)
        similarity += l * n;
        if (n > 0)
            std::cout << fmt::format("{:d} appears {:d} times in note2\n", l, n);
        if (r_end < note2_view.size)
            note2_view = note2_view[{r_end, note2_view.size}];   // << SLICING, Fk yeah
    }
    std::cout << std::endl;

    
    std::cout << fmt::format("Sum of differences of location identifiers: {:d}\n", diffs);
    std::cout << fmt::format("Similarity score: {:d}\n", similarity) << std::endl;

    std::cout << fmt::format("We have copied {} bytes of the array, which is enough for {} int32's\n", salad::copies, salad::copies / sizeof(uint32_t)) << std::endl;
    delete[] num_buffer;
    return 0;
}
