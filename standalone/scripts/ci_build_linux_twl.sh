#!/bin/bash
meson setup build
meson configure -Dbuild_target=linux build
meson configure -Dsdk_version_major=5 build
meson configure -Dhardware_type=twl_hyb build
meson configure -Dstandalone=true build
meson compile -C build
