#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const {
    return _sender.stream_in().remaining_capacity();
 }

size_t TCPConnection::bytes_in_flight() const {
    return _sender.bytes_in_flight();
}

size_t TCPConnection::unassembled_bytes() const {
    return _receiver.unassembled_bytes();
}

size_t TCPConnection::time_since_last_segment_received() const {
    return _time_since_last_segment_received;
}

void TCPConnection::segment_received(const TCPSegment &seg) {
    if(seg.header().ack){
        _sender.ack_received(seg.header().ackno,seg.header().win);
    }
    if(seg.header().syn || seg.header().fin || !seg.header().ack){
        _receiver.segment_received(seg);
        TCPSegment segment = TCPSegment();
        TCPHeader header = TCPHeader();
        header.ack = true;
        header.seqno = _sender.next_seqno();
        if(_receiver.ackno().has_value()){
            header.ackno = _receiver.ackno().value();
        }
        segment.header() = header;
        _segments_out.push(segment);
    }
}

bool TCPConnection::active() const {
    return _is_active;
}

size_t TCPConnection::write(const string &data) {
    size_t written_length = _sender.stream_in().write(data);
    _sender.fill_window();
    while(!_sender.segments_out().empty()){
        TCPSegment segment = _sender.segments_out().front();
        if(_receiver.ackno().has_value()){
            segment.header().ackno = _receiver.ackno().value();
        }
        _segments_out.push(segment);
        _sender.segments_out().pop();
    }


    return written_length;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { DUMMY_CODE(ms_since_last_tick); }

void TCPConnection::end_input_stream() {}

void TCPConnection::connect() {
    _sender.fill_window();
    while(!_sender.segments_out().empty()){
        TCPSegment segment = _sender.segments_out().front();
        if(_receiver.ackno().has_value()){
            segment.header().ackno = _receiver.ackno().value();
        }
        _segments_out.push(segment);
        _sender.segments_out().pop();
    }
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";

            // Your code here: need to send a RST segment to the peer
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
