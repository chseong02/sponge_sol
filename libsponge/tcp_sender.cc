#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <algorithm>
#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _retransmission_timeout{retx_timeout} {}

uint64_t TCPSender::bytes_in_flight() const { return _tracked_count; }

void TCPSender::fill_window() {
    switch (_current_state()) {
        // Never sent SYN
        case _TCPSenderState::Closed: {
            _next_seqno += 1;
            TCPSegment segment = TCPSegment();
            TCPHeader header = TCPHeader();
            header.seqno = _isn;
            header.syn = true;
            header.ackno = next_seqno();
            segment.header() = header;
            _segments_out.push(segment);
            _tracking_segments.push_front(segment);
            _tracked_count += 1;
            _timer = 0;
            break;
        }

        case _TCPSenderState::SynSent:
            break;

        default: {
            // have remain window and (have remained str or fin flag)
            while (_remain_window_size > 0 &&
                   (!_stream.buffer_empty() || _current_state() == _TCPSenderState::SynAckedEof)) {
                uint16_t read_size =
                    min(_remain_window_size > _stream.buffer_size() ? _stream.buffer_size() : _remain_window_size,
                        TCPConfig::MAX_PAYLOAD_SIZE);

                TCPSegment segment = TCPSegment();
                if (read_size != 0) {
                    segment.payload() = _stream.read(read_size);
                }
                segment.header().seqno = next_seqno();
                // Even if you include payload, window remained & buffer eof
                if (_remain_window_size > read_size && _stream.eof()) {
                    segment.header().fin = true;
                    segment.header().ack = true;
                }
                segment.header().ackno = segment.header().seqno + segment.length_in_sequence_space();
                _segments_out.push(segment);

                // First in First out
                _tracking_segments.push_front(segment);
                _next_seqno += segment.length_in_sequence_space();
                _tracked_count += segment.length_in_sequence_space();
                _remain_window_size -= segment.length_in_sequence_space();
            }
        }
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t remain_tracked_length = 0;
    list<TCPSegment>::iterator iter;
    // latest -> oldest
    for (iter = _tracking_segments.begin(); iter != _tracking_segments.end(); iter++) {
        if (iter->header().ackno == ackno) {
            _tracking_segments.erase(iter, _tracking_segments.end());
            _consecutive_time_out_count = 0;
            _timer = 0;
            _retransmission_timeout = _initial_retransmission_timeout;
            break;
        }
        // segment that still has to be tracked.
        remain_tracked_length += iter->length_in_sequence_space();
    };

    // edge case for Test "Don't add FIN if this would make the segment exceed
    // the receiver's window"
    if (_tracked_count == remain_tracked_length && !_tracking_segments.empty() &&
        _tracking_segments.back().header().seqno.raw_value() < ackno.raw_value()) {
        return;
    }
    _tracked_count = remain_tracked_length;
    _window_size = window_size;
    // For Check Window Size Again
    _remain_window_size = _window_size == 0 ? 1 : _window_size;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if(bytes_in_flight()!=0){
        _timer += ms_since_last_tick;
    }
    if (_timer >= _retransmission_timeout && bytes_in_flight() != 0) {
        _segments_out.push(_tracking_segments.back());
        _timer = 0;
        if (_window_size != 0) {
            _retransmission_timeout += _retransmission_timeout;
        }
        _consecutive_time_out_count += 1;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_time_out_count; }

void TCPSender::send_empty_segment() {
    TCPSegment segment = TCPSegment();
    TCPHeader header = TCPHeader();
    // Not taking up space
    header.seqno = next_seqno();
    header.ackno = next_seqno();
    segment.header() = header;
    _segments_out.push(segment);
}

// in accordance with the given Assignment Specification
TCPSender::_TCPSenderState TCPSender::_current_state() const {
    if (next_seqno_absolute() == 0) {
        return _TCPSenderState::Closed;
    }
    if (next_seqno_absolute() > 0 && next_seqno_absolute() == bytes_in_flight()) {
        return _TCPSenderState::SynSent;
    }
    if (next_seqno_absolute() > bytes_in_flight() && !stream_in().eof()) {
        return _TCPSenderState::SynAcked;
    }
    if (stream_in().eof() && next_seqno_absolute() < stream_in().bytes_written() + 2) {
        return _TCPSenderState::SynAckedEof;
    }
    if (stream_in().eof() && next_seqno_absolute() == stream_in().bytes_written() + 2 && bytes_in_flight() > 0) {
        return _TCPSenderState::FinSent;
    }
    if (stream_in().eof() && next_seqno_absolute() == stream_in().bytes_written() + 2 && bytes_in_flight() == 0) {
        return _TCPSenderState::FinAcked;
    }
    return _TCPSenderState::Closed;
}
