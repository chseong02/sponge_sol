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
    _time_since_last_segment_received = 0;
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
    if(_receiver.stream_out().eof() && !(_sender.stream_in().eof() && _sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2)){
        _linger_after_streams_finish = false;
    }
    if(!_linger_after_streams_finish && _is_satisfied_prereq()){
        _is_active = false;
    }
}

bool TCPConnection::active() const {
    return _is_active;
}

size_t TCPConnection::write(const string &data) {
    size_t written_length = _sender.stream_in().write(data);
    _sender.fill_window();
    _move_to_segments_out();

    return written_length;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _sender.tick(ms_since_last_tick);
    _move_to_segments_out();
    _time_since_last_segment_received += ms_since_last_tick;
    if(_time_since_last_segment_received >= 10 * _cfg.rt_timeout && _is_satisfied_prereq()){
        _is_active = false;
    }
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    _move_to_segments_out();
}

void TCPConnection::connect() {
    _sender.fill_window();
    _move_to_segments_out();
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

bool TCPConnection::_is_satisfied_prereq() const{
    if(_receiver.unassembled_bytes() == 0 && _receiver.stream_out().eof()){
        if(_sender.stream_in().eof() && _sender.bytes_in_flight() == 0){
            return true;
        }
    }
    return false;
}

void TCPConnection::_move_to_segments_out() {
    while(!_sender.segments_out().empty()){
        TCPSegment segment = _sender.segments_out().front();
        if(_receiver.ackno().has_value()){
            segment.header().ackno = _receiver.ackno().value();
        }
        _segments_out.push(segment);
        _sender.segments_out().pop();
    }
}
