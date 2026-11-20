#ifndef LIBNTR_SIM_GUI_INTERNAL_HPP
#define LIBNTR_SIM_GUI_INTERNAL_HPP

namespace SIM::GUI {
void AppConfigInit();
void AppConfigMain(bool * openState);
void AppNetInit();
void AppNetMain(bool * openState);
void AppPadInit();
void AppPadMain(bool * openState);
void AppDebugMain(bool * openState);
void AppInfoMain(bool * openState);

}

#endif