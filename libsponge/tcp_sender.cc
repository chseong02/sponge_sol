#include "tcp_sender.hh"

#include "tcp_config.hh"

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
    , _retransmission_timeout{retx_timeout} {
    _next_seqno += 1;
    TCPSegment segment = TCPSegment();
    TCPHeader header = TCPHeader();
    header.seqno = _isn;
    header.syn = true;
    header.ackno = next_seqno();
    segment.header() = header;
    _segments_out.push(segment);
    _tracking_segments.push(segment);
    _tracked_count += 1;
    _timer = 0;
}

uint64_t TCPSender::bytes_in_flight() const { return _tracked_count; }

void TCPSender::fill_window() {}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) { DUMMY_CODE(ackno, window_size); }

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    switch (_current_state()) {
        case _TCPSenderState::Closed: {
            _next_seqno += 1;
            TCPSegment segment = TCPSegment();
            TCPHeader header = TCPHeader();
            header.seqno = _isn;
            header.syn = true;
            header.ackno = next_seqno();
            segment.header() = header;
            _segments_out.push(segment);
            _tracking_segments.push(segment);
            _tracked_count += 1;
            _timer = 0;
            break;
        }
        default:
            break;
    }
    _timer += ms_since_last_tick;
}

unsigned int TCPSender::consecutive_retransmissions() const { return {}; }

void TCPSender::send_empty_segment() {}

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
