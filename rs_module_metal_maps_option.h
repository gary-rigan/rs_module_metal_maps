#ifndef ROBSYS_MODULE_METAL_MAPS_RS_MODULE_METAL_MAPS_OPTION_H
#define ROBSYS_MODULE_METAL_MAPS_RS_MODULE_METAL_MAPS_OPTION_H
#include "module/metal_map_module_impl.h"
namespace rob_sys {
namespace metal_maps {

extern int load_option_from_yaml(str::cStringArg str,cRobsysModuleMetalMapsImpl::option_t&);
extern void show_option(const cRobsysModuleMetalMapsImpl::option_t&);
}//namespace metal_maps
}//namespace rob_sys
#endif//ROBSYS_MODULE_METAL_MAPS_RS_MODULE_METAL_MAPS_OPTION_H
