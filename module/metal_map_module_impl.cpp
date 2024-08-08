#include "metal_map_module_impl.h"
#include "rs_module_metal_maps_option.h"
#include "ltimes/realtime/sleeper_const_rate.h"
#include "map_utility/map_file_format.h"
#include "robsys_pclbridge/robsys_pclbridge.h"
namespace rob_sys {
namespace metal_maps {
int map_unit_t::load(int id,const std::string& path) {
    LLOG(INFO,"Loading map [id: %d],from: %s",id,path.c_str());
    this->_costmap    = message::cMsgOccupMap::shared_ptr_t();
    this->_gridmap    = message::cMsgOccupMap::shared_ptr_t();
    this->_pointcloud = message::cMsgCloudsI3d::shared_ptr_t();
    if(id == -1) {
        LLOG(WARNING,"Clear maps");
        return ROBSYS_SUCCESSED;
    }

    this->_costmap   .reset(new message::cMsgOccupMap);
    this->_gridmap   .reset(new message::cMsgOccupMap);
    this->_pointcloud.reset(new message::cMsgCloudsI3d);
    std::string  costmap_path = path + "/"  +  k_map2d_name_cost;
    std::string  gridmap_path = path + "/"  +  k_map2d_name_occupied;
    std::string   ptsmap_path = path + "/"  +  k_map3d_name_pts;
    if(!File::access_file(costmap_path.c_str())) {
        LLOG(ERROR,"Costmap file [%s] does not exist",costmap_path.c_str());
        return ROBSYS_ERROR;
    }
    if(!File::access_file(gridmap_path.c_str())) {
        LLOG(ERROR,"Map file [%s] does not exist",gridmap_path.c_str());
        return ROBSYS_ERROR;
    }
    if(!File::access_file(ptsmap_path.c_str())) {
        LLOG(ERROR,"Map file [%s] does not exist",ptsmap_path.c_str());
        return ROBSYS_ERROR;
    }

    SLOG(INFO)<<"Loading costmap :"<<costmap_path;
    message::cPropOccupMap prop;
    if(!maps::map_format_png_t::load_occup(costmap_path, *_costmap,prop) ) {
        LLOG(ERROR,"Load grid map msg from [%s] failed",costmap_path.c_str());
        return false;
    }
    else {
        LLOG(INFO,"Grid map loaded,size [%d * %d],origin [%f %f %f]" ,
             _costmap->width(),_costmap->height(),_costmap->origin_x(),
             _costmap->origin_y(),_costmap->origin_theta());
    }

    SLOG(INFO)<<"Loading gridmap :"<<gridmap_path;
    if(!maps::map_format_png_t::load_occup(gridmap_path, *_gridmap,prop) ) {
        LLOG(ERROR,"Load grid map msg from [%s] failed",gridmap_path.c_str());
        return false;
    }
    else {
        LLOG(INFO,"Grid map loaded,size [%d * %d],origin [%f %f %f]"
             ,_gridmap->width(),_gridmap->height(),_gridmap->origin_x(),_gridmap->origin_y(),_gridmap->origin_theta());
    }
    SLOG(INFO)<<"Loading pts-map :"<<ptsmap_path;

    if(pclbridge::io::load_pcd(ptsmap_path,*_pointcloud) != ROBSYS_SUCCESSED) {
        LLOG(ERROR,"Load pointcloud map from %s failed",ptsmap_path.c_str());
        return false;
    }
    else {
        LLOG(INFO,"Pointcloud map loaded,point num [%ld]",_pointcloud->size());
    }
    _current_id = id;

    this->_costmap   ->id() = id;
    this->_gridmap   ->id() = id;
    this->_pointcloud->id() = id;

    return ROBSYS_SUCCESSED;
}


cRobsysModuleMetalMapsImpl::cRobsysModuleMetalMapsImpl(const str::cStringArg cfg)
    : _pub_map_pts(nullptr) ,_pub_map_grid(nullptr),_pub_map_cost(nullptr),
      _chg_map_srv(nullptr),
      _current_seq(0)
{

    if(load_option_from_yaml(cfg,_option) != ROBSYS_SUCCESSED) {
        LLOG(ERROR,"Load module [%s] option from yaml failed",k_module_name);
    }
    show_option(_option);

    _pub_map_pts = create_publisher<message::cMsgCloudsI3d>(_option.Pts_map_topic,3);
    if(_pub_map_pts == nullptr) {
        LLOG(FATAL,"Map herer");
    }
    _pub_map_grid=create_publisher<message::cMsgOccupMap,message::cPropOccupMap>(_option.Grid_map_topic);
    if(_pub_map_grid == nullptr) {
       LLOG(FATAL,"Create occupied map publisher results null");
    }
    _pub_map_cost=create_publisher<message::cMsgOccupMap,message::cPropOccupMap>(_option.Cost_map_topic);

    if(_pub_map_cost == nullptr) {
       LLOG(FATAL,"Create occupied map publisher results null");
    }
    _chg_map_srv = create_rpc_server(_option.Srv_change_map,&cRobsysModuleMetalMapsImpl::change_map_srv_callback,this);
}
cRobsysModuleMetalMapsImpl::~cRobsysModuleMetalMapsImpl() {

    if(_pub_map_pts) {
        delete _pub_map_pts;
    }
    if(_pub_map_grid) {
        delete _pub_map_grid;
    }
    if(_pub_map_cost) {
        delete _pub_map_cost;
    }
    if(_chg_map_srv) {
        delete _chg_map_srv;
    }
}

bool cRobsysModuleMetalMapsImpl::start() {
    _update_thread = new threads::cThreadPthread<std::function<void()>> (
                                       std::bind(&cRobsysModuleMetalMapsImpl::map_update,this),0,name()
                                                                           );

    if(_update_thread == nullptr) {
        LLOG(ERROR,"Create map thread failed");
        return false;
    }
    _stop_request =false;

    if(_update_thread->thread_start()!= ROBSYS_SUCCESSED) {
        LLOG(ERROR,"Start module [%s] update thread failed",name());
        delete _update_thread;
        _update_thread = nullptr;
        return false;
    }
    return true;
}
bool cRobsysModuleMetalMapsImpl::stop()  {
    return true;
}
bool cRobsysModuleMetalMapsImpl::is_normal()  {
    return true;
}
void cRobsysModuleMetalMapsImpl::map_update() {
    times::cSleeperConstRate rate(_option.publish_freq);
    while (true) {
        {
            threads::cLockGuard l(&_mtx);
            if(!_map_current) {
                continue;
            }
            if(!_map_current->_costmap
             ||!_map_current->_gridmap
             ||!_map_current->_pointcloud) {
                LLOG(ERROR,"Publish map failed,costmap gridmap or pointcloud map is null,this should not be happen");
                continue;
            }
            message::cMsgOccupMap::shared_ptr_t  costmap   =_map_current->_costmap;
            message::cMsgOccupMap::shared_ptr_t  gridmap   =_map_current->_gridmap;
            message::cMsgCloudsI3d::shared_ptr_t pointcloud=_map_current->_pointcloud;
            _pub_map_pts ->swap_publish( pointcloud);
            _pub_map_cost->swap_publish( costmap);
            _pub_map_grid->swap_publish( gridmap);
        }
        rate.sleep();
    }
}
int cRobsysModuleMetalMapsImpl::change_map_srv_callback(const message::cMsgObject6Dof& pose,int& res) {
    {
        threads::cLockGuard l(&_mtx);
        if(_map_current && _map_current->_current_id == pose.id()) {
            LLOG(ERROR,"Map change requested,but target id [%d] is same with current id [%d]"
                 ,pose.id(),(int)_map_current->_current_id);
            res = ROBSYS_SUCCESSED;
            return ROBSYS_SUCCESSED;
        }
    }

    std::map<int,std::string>::iterator it = _option._map_ids.find(pose.id());
    if(it == _option._map_ids.end()) {
        LLOG(ERROR,"Map id [%d] not found ,in map lists",pose.id());
        res= ROBSYS_ERROR;
        return ROBSYS_ERROR;
    }
    if(!_map_cache) {
        _map_cache.reset(new map_unit_t);
    }
    if(_map_cache->load(pose.id(),it->second) != ROBSYS_SUCCESSED) {
        LLOG(ERROR,"Load map failed");
        res= ROBSYS_ERROR;
        return ROBSYS_ERROR;
    }
    ++_current_seq;
    _map_cache->_costmap->seq()   =_current_seq;
    _map_cache->_gridmap->seq()   =_current_seq;
    _map_cache->_pointcloud->seq()=_current_seq;

    {
        threads::cLockGuard l(&_mtx);
        _map_current.swap(_map_cache);
    }
    res= ROBSYS_SUCCESSED;

    return ROBSYS_SUCCESSED;
}

}//namespace metal_maps
}//namespace rob_sys
