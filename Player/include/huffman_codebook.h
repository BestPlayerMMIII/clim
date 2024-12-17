#ifndef HUFFMAN_CODEBOOK_H
#define HUFFMAN_CODEBOOK_H

#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>
#include "printable.h"

/**
 * @class HuffmanCodebook
 * @brief Template for a Huffman codebook, with support for Printable and non-Printable types.
 */
template <typename T>
class HuffmanCodebook {
private:
    std::unordered_map<std::string, T> codebook;	///< Maps Huffman codes to values.

public:
    /**
     * @brief Inserts a value with its corresponding Huffman code.
     * @param code The Huffman code.
     * @param value The value to associate with the code.
     */
    void insert(const std::string& code, const T& value) {
        codebook[code] = value;
    }

    /**
     * @brief Retrieves the value associated with a Huffman code.
     * @param code The Huffman code.
     * @return The associated value.
     * @throws std::runtime_error If the code is not found.
     */
    const T& at(const std::string& code) const {
        auto it = codebook.find(code);
        if (it == codebook.end()) {
            throw std::runtime_error("Code not found in Huffman codebook");
        }
        return it->second;
    }

    /**
     * @brief Checks if a Huffman code exists in the codebook.
     * @param code The Huffman code.
     * @return True if the code exists, false otherwise.
     */
    bool contains(const std::string& code) const {
        return codebook.find(code) != codebook.end();
    }

    /**
     * @brief Returns the size of the codebook.
     * @return The number of entries in the codebook.
     */
    size_t size() const {
        return codebook.size();
    }

    /**
     * @brief Overloads the stream insertion operator for printing.
     * Specialized for types derived from Printable and other types.
     */
    friend std::ostream& operator<<(std::ostream& os, const HuffmanCodebook<T>& huffman_codebook) {
        if (huffman_codebook.size() == 0) {
            os << "{Empty Huffman Codebook}" << "\n";
            return os;
        }

        for (const auto& entry : huffman_codebook.codebook) {
            os << "Code: " << entry.first << " -> Value: ";
            huffman_codebook.print_value(os, entry.second);
            os << "\n";
        }
        return os;
    }

private:
    /**
     * @brief Prints a value. Specialized for Printable types.
     * @param os The output stream.
     * @param value The value to print.
     */
    template <typename U = T>
    typename std::enable_if<std::is_base_of<Printable, U>::value>::type
    print_value(std::ostream& os, const U& value) const {
        value.print(os);
    }

    /**
     * @brief Prints a value. Specialized for non-Printable types.
     * @param os The output stream.
     * @param value The value to print.
     */
    template <typename U = T>
    typename std::enable_if<!std::is_base_of<Printable, U>::value>::type
    print_value(std::ostream& os, const U& value) const {
        os << value;
    }
};

#endif	// HUFFMAN_CODEBOOK_H

