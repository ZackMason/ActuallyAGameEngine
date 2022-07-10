// Author: Zackery Mason-Blaug
// Date: 2022-06-03
//////////////////////////////////////////////////////////


#include "Networking/packet.hpp"

#include <cstring>

void packet_t::set_data(const std::string& new_data){
    data = new_data;
    enet_packet_resize(packet, new_data.length() + 1);
    std::memset(packet->data, 0, new_data.length()+1);
    std::memcpy(packet->data, new_data.c_str(), new_data.length());
}