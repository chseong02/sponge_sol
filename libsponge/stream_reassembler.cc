#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    list<tuple<size_t, size_t, string>>::iterator iter;
    size_t data_length = data.length();

    if (eof) {
        _have_eof = true;
        _eof_index = index + data_length;
    }
    if (data_length <= 0) {
        _eof_process();
        return;
    }
    if ((index + data_length) < _unassembled_start_index) {
        _eof_process();
        return;
    }

    size_t temp_start_index = max(_unassembled_start_index, index);
    tuple<size_t, size_t> temp = make_tuple(temp_start_index, index + data_length - 1);
    size_t unassembled_max_index = (_unassembled_start_index - _output.buffer_size()) + (_capacity - 1);

    for (iter = _staging_list.begin(); iter != _staging_list.end(); iter++) {
        size_t temp_start = get<0>(temp);
        size_t temp_end = get<1>(temp);
        size_t iter_start = get<0>(*iter);
        size_t iter_end = get<1>(*iter);
        if (temp_start < iter_start) {
            size_t max_end = min({temp_end, iter_start - 1, unassembled_max_index});
            size_t next_start = iter_end + 1;
            size_t next_end = temp_end;
            if (max_end >= temp_start) {
                size_t substring_length = max_end - temp_start + 1;
                _staging_list.insert(
                    iter, make_tuple(temp_start, max_end, data.substr(temp_start - index, substring_length)));
                _staging_size += substring_length;
            }
            if (next_start > next_end) {
                break;
            }
            temp = make_tuple(next_start, next_end);
        } else if (temp_end <= iter_end) {
            break;
        }
        // if (temp_end > iter_end)
        else {
            temp = make_tuple(max(iter_end + 1, temp_start), temp_end);
        }
    }
    if (iter == _staging_list.end()) {
        size_t temp_start = get<0>(temp);
        size_t temp_end = get<1>(temp);
        size_t max_end = min(temp_end, unassembled_max_index);

        if (temp_start <= max_end) {
            size_t substring_length = max_end - temp_start + 1;
            _staging_list.push_back(make_tuple(temp_start, max_end, data.substr(temp_start - index, substring_length)));
            _staging_size += substring_length;
        }
    }
    for (iter = _staging_list.begin(); iter != _staging_list.end();) {
        size_t iter_start = get<0>(*iter);
        size_t iter_end = get<1>(*iter);
        if (iter_start == _unassembled_start_index) {
            size_t size = iter_end - iter_start + 1;
            _output.write(move(get<2>(*iter)));
            _unassembled_start_index = iter_end + 1;
            iter = _staging_list.erase(iter);
            _staging_size -= size;
        } else {
            break;
        }
    }
    _eof_process();
}

size_t StreamReassembler::unassembled_bytes() const { return _staging_size; }

bool StreamReassembler::empty() const { return _staging_size == 0; }

void StreamReassembler::_eof_process() {
    if (_have_eof && _unassembled_start_index >= _eof_index) {
        _output.end_input();
        _staging_list.clear();
        _staging_size = 0;
    }

    return;
}
