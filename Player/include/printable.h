#ifndef PRINTABLE_H
#define PRINTABLE_H

#include <iostream>

/**
 * @class Printable
 * @brief Interface for objects that can be printed to an output stream.
 */
class Printable {
public:
    virtual ~Printable() = default;

    /**
     * @brief Pure virtual method for printing the object.
     * @param os The output stream to print to.
     */
    virtual void print(std::ostream& os) const = 0;
};

/**
 * @brief Overloads the stream insertion operator for Printable objects.
 * @param os The output stream.
 * @param obj The Printable object.
 * @return The output stream.
 */
inline std::ostream& operator<<(std::ostream& os, const Printable& obj) {
    obj.print(os);
    return os;
}

#endif	// PRINTABLE_H

