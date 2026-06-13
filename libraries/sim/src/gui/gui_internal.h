#ifndef SIMULATOR_DEBUG_GUI_INTERNAL_H
#define SIMULATOR_DEBUG_GUI_INTERNAL_H

#include <stdbool.h>

void GUI_AppConfigMain(bool * p_open);
void GUI_AppG2Main(bool * p_open);
void GUI_AppNetMain(bool * p_open);
void GUI_AppOSMain(bool * p_open);
void GUI_AppPadMain(bool * p_open);

void GUI_AppNetInit();

#endif
