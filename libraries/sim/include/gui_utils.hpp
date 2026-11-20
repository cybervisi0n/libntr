#ifndef LIBNTR_SIM_GUI_UTILS_HPP
#define LIBNTR_SIM_GUI_UTILS_HPP

#include <nitro/types.h>

namespace SIM::GUI {

// Returns the left-most X coordinate of a line "width" centered in the window
u32 CenteredXPx(u32 width);
// Returns the top-most Y coordinate of a line "height" centered in the window
u32 CenteredYPx(u32 height);
// Returns (0.0f-1.0f) percent of the window width in pixels
u32 PercentXPx(float percent);
// Returns (0.0f-1.0f) percent of the window height in pixels
u32 PercentYPx(float percent);


}

#endif
