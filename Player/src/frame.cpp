#include "frame.h"
#include <stdexcept>
#include <sstream>  // For constructing strings

// Converts a flat vector of pixels into a 2D grid (Frame) of specified dimensions.
Frame FlatFrame::to_frame(const size_t width, const size_t height) const {
    size_t num_pixels = pixels.size();

    // Validate that the number of pixels matches the grid dimensions.
    if (num_pixels != width * height) {
        throw std::invalid_argument(
            "The number of pixels (" + std::to_string(num_pixels) + 
            ") does not match the specified dimensions (" +
            std::to_string(width) + " x " + std::to_string(height) + ").");
    }

    // Initialize the resulting frame with a grid of empty rows.
    Frame frame;
    frame.pixels_grid = std::vector<std::vector<Color>>(height);

    std::vector<Color> frame_row;  // Temporary vector to store one row of the frame.

    // Loop through each pixel in the flat vector.
    for (size_t pixel = 0; pixel < num_pixels; pixel++) {
        // Add the current pixel to the temporary row.
        frame_row.push_back(pixels[pixel]);

        // Check if the row is complete (i.e., it has reached the specified width).
        if ((pixel + 1) % width == 0) {
            // Assign the completed row to the corresponding grid row.
            frame.pixels_grid[(pixel + 1) / width - 1] = frame_row;
            frame_row.clear();  // Clear the temporary row for the next row.
        }
    }

    return frame;  // Return the constructed 2D frame.
}

// Constructor for FrameRenderer initializes the frame's dimensions.
FrameRenderer::FrameRenderer(const size_t width, const size_t height)
: width(width), height(height) {}

// Renders the frame (2D grid of pixels) into a string representation with ANSI color codes.
std::string FrameRenderer::render_frame(const Frame& frame) {
    std::ostringstream oss;

    // Loop through each row in the frame grid.
    for (const auto& row : frame.pixels_grid) {
        // Loop through each pixel in the row.
        for (const auto& pixel : row) {
            // Append the ANSI color code for the pixel's RGB color.
            oss << "\033[48;2;" << static_cast<int>(pixel.r) << ";" 
                << static_cast<int>(pixel.g) << ";" 
                << static_cast<int>(pixel.b) << "m ";
        }
        // Reset formatting after the row and add a newline.
        oss << "\033[0m\n";
    }

    return oss.str();  // Return the complete string representation of the frame.
}

