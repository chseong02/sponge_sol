#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    _time_since_last_segment_received = 0;
    _receiver.segment_received(seg);

    if (seg.header().rst) {
        _is_active = false;
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
    }

    // receive ack except before syn
    if (_sender.next_seqno_absolute() != 0 && seg.header().ack) {
        _sender.ack_received(seg.header().ackno, seg.header().win);
        _sender.fill_window();
        _move_to_segments_out();
    }

    // receive payload or SYN, FIN flag
    if (seg.length_in_sequence_space() > 0) {
        // SYN
        if (_sender.next_seqno_absolute() == 0) {
            connect();
            return;
        }
        // default ack
        if (_segments_out.empty()) {
            _sender.send_empty_segment();
        }

        _move_to_segments_out();
    }

    // alive ping
    if (_receiver.stream_out().eof() &&
        !(_sender.stream_in().eof() && _sender.next_seqno_absolute() == _sender.stream_in().bytes_written() + 2)) {
        _linger_after_streams_finish = false;
    }

    // passive close
    if (!_linger_after_streams_finish && _is_satisfied_prereq()) {
        _is_active = false;
    }
}

bool TCPConnection::active() const { return _is_active; }

size_t TCPConnection::write(const string &data) {
    size_t written_length = _sender.stream_in().write(data);
    _sender.fill_window();
    _move_to_segments_out();

    return written_length;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _sender.tick(ms_since_last_tick);
    _time_since_last_segment_received += ms_since_last_tick;

    // active close
    if (_time_since_last_segment_received >= 10 * _cfg.rt_timeout && _is_satisfied_prereq()) {
        _is_active = false;
    }

    // resent max overflow reset
    if (_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) {
        TCPSegment segment = TCPSegment();
        segment.header().rst = true;
        _segments_out.push(segment);
        _is_active = false;
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        _linger_after_streams_finish = false;
        return;
    }
    _move_to_segments_out();
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
            TCPSegment segment = TCPSegment();
            segment.header().rst = true;
            _segments_out.push(segment);
            _is_active = false;
            _sender.stream_in().set_error();
            _receiver.stream_out().set_error();
            _linger_after_streams_finish = false;
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

// check condition about clean shutdown
bool TCPConnection::_is_satisfied_prereq() const {
    if (_receiver.unassembled_bytes() == 0 && _receiver.stream_out().eof()) {
        if (_sender.stream_in().eof() && _sender.bytes_in_flight() == 0) {
            return true;
        }
    }
    return false;
}

// move seg _sender.segments_out --> _segments_out
// when have ackno, always ack
void TCPConnection::_move_to_segments_out() {
    while (!_sender.segments_out().empty()) {
        TCPSegment segment = _sender.segments_out().front();
        if (_receiver.ackno().has_value()) {
            segment.header().ack = true;
            segment.header().ackno = _receiver.ackno().value();
            segment.header().win = _receiver.window_size();
        }
        _segments_out.push(segment);
        _sender.segments_out().pop();
    }
}
