#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    map<uint32_t, std::pair<EthernetAddress, size_t>>::iterator iter;
    iter = _address_table.find(next_hop_ip);

    EthernetFrame frame = EthernetFrame();
    // valid mapping exist
    if (iter != _address_table.end()) {
        frame.header().type = EthernetHeader::TYPE_IPv4;
        frame.payload() = dgram.serialize();
        frame.header().src = _ethernet_address;
        frame.header().dst = iter->second.first;
        frames_out().push(frame);
        return;
    }

    // valid mapping not exist
    list<std::tuple<InternetDatagram, uint32_t, size_t>>::iterator delay_queue_iter;
    delay_queue_iter = _dgram_delay_queue.begin();
    // previous ARP request not time out
    while (delay_queue_iter != _dgram_delay_queue.end()) {
        if (get<1>(*delay_queue_iter) == next_hop_ip && get<2>(*delay_queue_iter) > _timer) {
            _dgram_delay_queue.push_back(make_tuple(dgram, next_hop_ip, _timer));
            return;
        }
        delay_queue_iter++;
    }
    // queueing, with 5seconds time out pending
    _dgram_delay_queue.push_back(make_tuple(dgram, next_hop_ip, _timer + 5 * 1000));

    // ARP request
    ARPMessage arp_request = ARPMessage();
    frame.header().type = EthernetHeader::TYPE_ARP;
    arp_request.opcode = arp_request.OPCODE_REQUEST;
    arp_request.sender_ip_address = _ip_address.ipv4_numeric();
    arp_request.sender_ethernet_address = _ethernet_address;
    arp_request.target_ip_address = next_hop_ip;
    frame.payload() = arp_request.serialize();
    frame.header().src = _ethernet_address;
    frame.header().dst = ETHERNET_BROADCAST;
    frames_out().push(frame);
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    // not for me
    if (frame.header().dst != _ethernet_address && frame.header().dst != ETHERNET_BROADCAST) {
        return std::nullopt;
    }

    // IPv4 received
    if (frame.header().type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram dgram = InternetDatagram();
        ParseResult parse_result = dgram.parse(frame.payload());
        if (parse_result == ParseResult::NoError) {
            return dgram;
        }
        return std::nullopt;
    }

    // ARP received
    ARPMessage recv_arp = ARPMessage();
    ParseResult parse_result = recv_arp.parse(frame.payload());
    if (parse_result != ParseResult::NoError) {
        return std::nullopt;
    }

    // add ip, ethernet address mapping (with 30seconds life)
    _address_table[recv_arp.sender_ip_address] =
        pair<EthernetAddress, size_t>(recv_arp.sender_ethernet_address, _timer + 30 * 1000);

    if (recv_arp.opcode == ARPMessage::OPCODE_REQUEST) {
        // have to reply about ARP request
        if (recv_arp.target_ip_address == _ip_address.ipv4_numeric()) {
            EthernetFrame send_frame = EthernetFrame();
            ARPMessage arp_request = ARPMessage();
            send_frame.header().type = EthernetHeader::TYPE_ARP;
            arp_request.opcode = arp_request.OPCODE_REPLY;
            arp_request.sender_ip_address = _ip_address.ipv4_numeric();
            arp_request.sender_ethernet_address = _ethernet_address;
            arp_request.target_ip_address = recv_arp.sender_ip_address;
            arp_request.target_ethernet_address = recv_arp.sender_ethernet_address;
            send_frame.payload() = arp_request.serialize();
            send_frame.header().src = _ethernet_address;
            send_frame.header().dst = recv_arp.sender_ethernet_address;
            frames_out().push(send_frame);
        }
    }

    // Check if any of the delayed dgrams can be sent
    list<std::tuple<InternetDatagram, uint32_t, size_t>>::iterator delay_queue_iter;
    delay_queue_iter = _dgram_delay_queue.begin();
    while (delay_queue_iter != _dgram_delay_queue.end()) {
        map<uint32_t, std::pair<EthernetAddress, size_t>>::iterator address_iter;
        address_iter = _address_table.find(get<1>(*delay_queue_iter));

        EthernetFrame send_frame = EthernetFrame();
        // if valid mapping exist
        if (address_iter != _address_table.end()) {
            send_frame.header().type = EthernetHeader::TYPE_IPv4;
            send_frame.payload() = get<0>(*delay_queue_iter).serialize();
            send_frame.header().src = _ethernet_address;
            send_frame.header().dst = address_iter->second.first;
            frames_out().push(send_frame);
            delay_queue_iter = _dgram_delay_queue.erase(delay_queue_iter);
        } else {
            delay_queue_iter++;
        }
    }

    return std::nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    _timer += ms_since_last_tick;

    map<uint32_t, std::pair<EthernetAddress, size_t>>::iterator iter;
    iter = _address_table.begin();
    while (iter != _address_table.end()) {
        if (iter->second.second < _timer) {
            iter = _address_table.erase(iter);
        } else {
            iter++;
        }
    }
}
