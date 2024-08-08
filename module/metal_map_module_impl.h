#ifndef ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_IMPL_H
#define ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_IMPL_H
#include "rs_module_metal_maps_config.h"


#include "robsys_system/module/rs_module.h"
#include "robsys_system/datacenter_local/datacenter_local.h"

#include "data/message.h"

namespace rob_sys {
namespace metal_maps {


struct map_unit_t {
public:
    map_unit_t()
        : _current_id(-1)
    {}
public:
    std::atomic<int> _current_id;
    message::cMsgOccupMap::shared_ptr_t _costmap;
    message::cMsgOccupMap::shared_ptr_t _gridmap;
    message::cMsgCloudsI3d::shared_ptr_t _pointcloud;
public:
    int load(int id,const std::string&);
};

class cRobsysModuleMetalMapsImpl: public system::cRobsysModuleBase,
                                public datacenter::cDataCenterLocal
{
public:
    struct option_t {
        std::map<int,std::string> _map_ids;

        double publish_freq;
        std::string Srv_change_map;
        std::string Pts_map_topic;
        std::string Grid_map_topic;
        std::string Cost_map_topic;
    };
public:
    cRobsysModuleMetalMapsImpl(const str::cStringArg);
    ~cRobsysModuleMetalMapsImpl() override;
public:
    bool start() override;
    bool stop() override;
    bool is_normal() override;
public:
    const char* name()    override { return k_module_name;}
    const char* version() override { return "v1.0.0";}
    const char* info()    override { return k_module_name;}
private:
    void map_update();
    int change_map_srv_callback(const message::cMsgObject6Dof&,int&);
private:
    option_t _option;
private:
    std::atomic<bool> _stop_request;

    datacenter::cDataCenterLocalPublisher<message::cMsgCloudsI3d>*      _pub_map_pts;
    datacenter::cDataCenterLocalPublisher<message::cMsgOccupMap,message::cPropOccupMap>* _pub_map_grid;
    datacenter::cDataCenterLocalPublisher<message::cMsgOccupMap,message::cPropOccupMap>* _pub_map_cost;

    datacenter::cRpcLocalServer<message::cMsgObject6Dof,int>* _chg_map_srv;

    threads::cThreadPthread<std::function<void()>>* _update_thread;
private:
    threads::cMutex _mtx;
    std::shared_ptr<map_unit_t> _map_current GUARDED_BY(_mtx);
    std::shared_ptr<map_unit_t> _map_cache   GUARDED_BY(_mtx);
    int64_t _current_seq;
};


}//namespace metal_maps
}//namespace rob_sys
#endif//ROBSYS_MODULE_METAL_MAPS_MODULE_METAL_MAP_MODULE_IMPL_H
