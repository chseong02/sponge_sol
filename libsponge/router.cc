#include "router.hh"

#include <iostream>

using namespace std;

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    DUMMY_CODE(route_prefix, prefix_length, next_hop, interface_num);
    // Your code here.
    _routes[pair<uint32_t, uint8_t>(route_prefix, prefix_length)] =
        pair<optional<Address>, size_t>(next_hop, interface_num);
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    DUMMY_CODE(dgram);
    // Your code here.
    uint8_t i;
    uint32_t dst = dgram.header().dst;

    std::map<std::pair<uint32_t, uint8_t>, std::pair<optional<Address>, size_t>>::iterator iter;
    for (i = 0; i <= 32; i++) {
        uint32_t dst_part = dst;
        for (int j = 0; j < i; j++) {
            dst_part = (dst_part / 2);
        }
        for (int j = 0; j < i; j++) {
            dst_part = dst_part * 2;
        }
        if ((iter = _routes.find(pair<uint32_t, uint8_t>(dst_part, 32 - i))) != _routes.end()) {
            pair<optional<Address>, size_t> route_info = iter->second;
            size_t interface_num = route_info.second;
            Address next_hop = route_info.first.has_value() ? route_info.first.value()
                                                            : Address::from_ipv4_numeric(dgram.header().dst);
            if (dgram.header().ttl == 0) {
                return;
            }
            InternetDatagram new_dgram = dgram;
            new_dgram.header().ttl -= 1;
            if (new_dgram.header().ttl == 0) {
                return;
            }
            interface(interface_num).send_datagram(new_dgram, next_hop);
            return;
        }
    }
    // drop dgram
    return;
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
