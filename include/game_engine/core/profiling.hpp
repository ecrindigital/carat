#pragma once

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>

#define AXOLOTL_ZONE ZoneScoped
#define AXOLOTL_ZONE_NAMED(name) ZoneScopedN(name)
#define AXOLOTL_FRAME FrameMark
#define AXOLOTL_PLOT(name, value) TracyPlot(name, value)

#else

#define AXOLOTL_ZONE
#define AXOLOTL_ZONE_NAMED(name)
#define AXOLOTL_FRAME
#define AXOLOTL_PLOT(name, value)

#endif
