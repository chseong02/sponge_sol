#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity){
    _capacity = capacity;
}

size_t ByteStream::write(const string &data) {
    DUMMY_CODE(data);
    return {};
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    DUMMY_CODE(len);
    return {};
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { DUMMY_CODE(len); }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    DUMMY_CODE(len);
    return {};
}

void ByteStream::end_input() {
    _isEnded=true;
}

bool ByteStream::input_ended() const {
    return _isEnded;
}

size_t ByteStream::buffer_size() const {
    return _buffer.length();
}

bool ByteStream::buffer_empty() const {
    return _buffer.empty();
}

bool ByteStream::eof() const {
    return input_ended() && buffer_empty();
}

size_t ByteStream::bytes_written() const { return {}; }

size_t ByteStream::bytes_read() const { return {}; }

size_t ByteStream::remaining_capacity() const {
    return _capacity - buffer_size();
}
