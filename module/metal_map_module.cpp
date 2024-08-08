#include "metal_map_module.h"
#include "module/metal_map_module_impl.h"
namespace rob_sys {
namespace metal_maps {

system::cRobsysModuleBase* cRobsysModuleMetalMaps::generate(const str::cStringArg& cfg) {
    system::cRobsysModuleBase* handle = new cRobsysModuleMetalMapsImpl(cfg);
    if(handle == nullptr) {
        LLOG(ERROR,"Alloc laserscan livox2 module impl results null");
    }
    else {
        SLOG(NOTICE)<<"\nModule ["<<handle->name()<<"] Created "
                  <<"\n>\tVersion ["<<handle->version()<<"]"
                  <<"\n>\tInfo: "<<handle->info()
                  <<" \n"<<handle->build_info();
    }
    return handle;
}
bool cRobsysModuleMetalMaps::release(system::cRobsysModuleBase*& handle) {
    if(handle!=nullptr){
        delete handle;
        handle=nullptr;
    }
    return true;
}

}//namespace metal_maps
}//namespace rob_sys
