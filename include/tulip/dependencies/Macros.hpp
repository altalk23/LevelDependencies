#pragma once

#include <Geode/DefaultInclude.hpp>

#ifdef GEODE_IS_WINDOWS
#if TULIP_DEPENDENCIES_EXPORTING
#define TULIP_DEPENDENCIES_DLL __declspec(dllexport)
#else
#define TULIP_DEPENDENCIES_DLL __declspec(dllimport)
#endif
#else
#define TULIP_DEPENDENCIES_DLL __attribute__((visibility("default")))
#endif