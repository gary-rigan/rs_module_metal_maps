#ifndef ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_H
#define ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_H
#include "rs_module_metal_maps_config.h"

#include "robsys_system/module/rs_module.h"
#include "robsys_system/datacenter_local/datacenter_local.h"
namespace rob_sys {
namespace metal_maps {

class cRobsysModuleMetalMaps:public system::cRobsysModuleFactory<std::string>
{
public:
    cRobsysModuleMetalMaps() =default;
    ~cRobsysModuleMetalMaps() override = default;
public:
    system::cRobsysModuleBase* generate(const str::cStringArg& cfg) override;
    bool release(system::cRobsysModuleBase*& module) override;
public:
    std::string key() override {return k_module_key;}
};

}//namespace metal_maps
}//namespace rob_sys
#endif//ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_H
