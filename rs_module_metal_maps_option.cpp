#include "rs_module_metal_maps_option.h"
#include "parsers/parsers.h"
namespace rob_sys {
namespace metal_maps {


int load_option_from_yaml(str::cStringArg str,cRobsysModuleMetalMapsImpl::option_t& opt) {
    opt._map_ids.clear();

    SLOG(INFO)<<"Loading srv["<<k_module_name<<"] option from: "<<str;
    YAML::Node node = YAML::LoadFile(str.c_str());
    if(node.IsNull()) {
        LLOG(ERROR,"Cannot load yaml filed");
        return ROBSYS_ERROR;
    }
    std::string config_name = node["config_name"].as<std::string>();
    if(config_name != k_module_name) {
        LLOG(ERROR,"Config file name [%s] load from yaml mismatched with srv name [%s]",config_name.c_str(),k_module_name);
        return ROBSYS_ERROR;
    }


    opt.publish_freq    = node["Publish_freq"]  .as<double>();
    opt.Srv_change_map  = node["change_map_srv"] .as<std::string>();
    opt.Pts_map_topic   = node["Pts_map_topic"]  .as<std::string>();
    opt.Grid_map_topic  = node["Grid_map_topic"] .as<std::string>();
    opt.Cost_map_topic  = node["Cost_map_topic"].as<std::string>();


    YAML::Node maps = node["maps"];

    if(maps.IsNull()) {
        LLOG(ERROR,"Map field not found in config");
        return ROBSYS_ERROR;
    }
    else if(!maps.IsSequence()) {
        LLOG(ERROR,"Map data shoud be sequence");
        return ROBSYS_ERROR;
    }
    for(size_t i =0; i<maps.size();++i) {
        auto m = maps[i];
        int id = m[0]["id"].as<int>();
        if(opt._map_ids.find(id) != opt._map_ids.end()) {
            LLOG(ERROR,"Multi map id [%d] found in config file",id);
            return ROBSYS_ERROR;
        }
        opt._map_ids[id] = m[1]["path"].as<std::string>();
    }
    for(auto it = opt._map_ids.begin();it!=opt._map_ids.end();++it) {
        if(!File::access_dir(it->second.c_str())) {
            LLOG(ERROR,"Map at id [%d] path [%s] does not exist",it->first,it->second.c_str());
            return ROBSYS_ERROR;
        }
    }
    return ROBSYS_SUCCESSED;
}
void show_option(const cRobsysModuleMetalMapsImpl::option_t& opt) {

    std::stringstream ss;
    ss<<"\nModule ["<<k_module_name<<"] option:"
      <<"Pub-freq:\n>\t" <<opt.publish_freq
      <<"Srv-change-map:\n>t"<<opt.Srv_change_map
      <<"Topic-pts :\n>\t" <<opt.Pts_map_topic
      <<"Topic-grid:\n>\t" << opt.Grid_map_topic
      <<"Topic-cost:\n>\t" <<opt.Cost_map_topic;

    for(auto it = opt._map_ids.begin();it!=opt._map_ids.end();++it) {
        ss<<"\nMaps:"
         <<"\n>Id  : "<<it->first
         <<"\n>Path: "<<it->second;
    }
    SLOG(INFO)<<ss.str();
}
}//namespace metal_maps
}//namespace rob_sys
