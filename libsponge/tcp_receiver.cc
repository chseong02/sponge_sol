#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {

    TCPHeader header = seg.header();
    WrappingInt32 seqno = header.seqno; 
    if(!_have_SYN){
        if(!header.syn){
            return;
        }
        _have_SYN = true;
        _ackno = header.ackno;
        _icn = header.seqno;
        seqno = wrap(static_cast<uint64_t>(seqno.raw_value())+1,_icn);
    }
    // ignore overlap syn
    else if(header.syn){
        return;
    }
    uint64_t checkpoint = _reassembler.stream_out().bytes_written(); 
    _reassembler.push_substring(seg.payload().str().data(),unwrap(seqno,_icn, checkpoint)-1 ,header.fin);

    if(header.fin){
        _have_FIN = true;
    }
    if(_have_FIN && _reassembler.empty()){
        _reassembler.stream_out().end_input();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
    if(_have_SYN){
        if(_have_FIN&_reassembler.stream_out().input_ended())
        return wrap(_reassembler.stream_out().bytes_written()+2,_icn);
        return wrap(_reassembler.stream_out().bytes_written()+1,_icn);
    }
    return {};
}

size_t TCPReceiver::window_size() const {
    return _capacity - (_reassembler.stream_out().bytes_written() - _reassembler.stream_out().bytes_read());
}
