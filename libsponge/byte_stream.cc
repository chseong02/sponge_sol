#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { _capacity = capacity; }

size_t ByteStream::write(const string &data) {
    size_t append_length = remaining_capacity() <= data.length() ? remaining_capacity() : data.length();
    _buffer.append(data, 0, append_length);
    _written_length += append_length;
    return append_length;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if (len > buffer_size()) {
        // Does not call set_error due to the function being const.
        return std::string();
    }
    return _buffer.substr(0, len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    if (len > buffer_size()) {
        set_error();
        return;
    }
    _buffer.erase(0, len);
    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    if (len > buffer_size()) {
        set_error();
        return std::string();
    }
    string str = peek_output(len);
    pop_output(len);
    return str;
}

void ByteStream::end_input() { _is_ended = true; }

bool ByteStream::input_ended() const { return _is_ended; }

size_t ByteStream::buffer_size() const { return _buffer.length(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _written_length; }

size_t ByteStream::bytes_read() const { return bytes_written() - buffer_size(); }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
