#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>
#include "color.h"

/**
 * @struct Frame
 * @brief Represents a grid of colored pixels.
 */
struct Frame {
    std::vector<std::vector<Color>> pixels_grid; ///< 2D grid of pixels.
};

/**
 * @struct FlatFrame
 * @brief Represents a linear array of pixels.
 */
struct FlatFrame {
    std::vector<Color> pixels;	///< 1D array of pixels.

    /**
     * @brief Converts a FlatFrame into a Frame.
     * @param width Width of the target frame.
     * @param height Height of the target frame.
     * @return A 2D grid representation of the pixels.
     */
    Frame to_frame(const size_t width, const size_t height) const;
};

/**
 * @class FrameRenderer
 * @brief Handles the rendering of frames into a displayable format.
 */
class FrameRenderer {
public:
    /**
     * @brief Default constructor.
     */
    FrameRenderer() : width(0), height(0) {}

    /**
     * @brief Parameterized constructor.
     * @param width Width of the frame.
     * @param height Height of the frame.
     */
    FrameRenderer(const size_t width, const size_t height);

    /**
     * @brief Renders a frame as a colored string.
     * @param frame The frame to render.
     * @return A string representation of the frame.
     */
    std::string render_frame(const Frame& frame);

    /**
     * @brief Renders a flat frame as a colored string.
     * @param flat_frame The flat frame to render.
     * @return A string representation of the flat frame.
     */
    std::string render_flat_frame(const FlatFrame& flat_frame) {
        return render_frame(flat_frame.to_frame(width, height));
    }

private:
    size_t width, height;	///< Dimensions of the frame.
};

#endif	// FRAME_H

