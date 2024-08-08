#include "rs_module_metal_maps.h"
#include "module/metal_map_module.h"
rob_sys::system::cRobsysModuleFactory<std::string>* create_module() {
    return new rob_sys::metal_maps::cRobsysModuleMetalMaps();
}
