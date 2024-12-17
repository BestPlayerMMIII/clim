#ifndef COLOR_H
#define COLOR_H

#include "printable.h"

#ifndef BYTE_TYPE
typedef unsigned char byte;
#endif

/**
 * @class Color
 * @brief Represents a color using RGB components.
 */
class Color : public Printable {
public:
    byte r, g, b;	///< Red, green, and blue components of the color.

    /**
     * @brief Default constructor initializes to black.
     * @param red Red component.
     * @param green Green component.
     * @param blue Blue component.
     */
    Color(byte red = 0, byte green = 0, byte blue = 0)
        : r(red), g(green), b(blue) {}

    /**
     * @brief Checks for equality between two colors.
     * @param other The other color to compare with.
     * @return True if the colors are equal, false otherwise.
     */
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b;
    }

    /**
     * @brief Implements the Printable interface for printing the color.
     * @param os The output stream to print to.
     */
    void print(std::ostream& os) const override {
        os << "(" << static_cast<int>(r) << ", " << static_cast<int>(g) << ", " << static_cast<int>(b) << ")";
    }
};

#endif	// COLOR_H

