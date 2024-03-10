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
    // DUMMY_CODE(data, index, eof);
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

    list<tuple<size_t, size_t, string>>::iterator iter;
    for (iter = _staging_list.begin(); iter != _staging_list.end(); iter++) {
        size_t temp_start = get<0>(temp);
        size_t temp_end = get<1>(temp);
        if (temp_start < get<0>(*iter)) {
            size_t remain_size = _capacity - unassembled_bytes() - _output.buffer_size();
            if (remain_size == 0) {
                break;
            }

            size_t max_end = min(temp_end,
                                 min(max(get<0>(*iter), static_cast<size_t>(1)) - 1,
                                     min(temp_start + remain_size - 1,
                                         _unassembled_start_index + _capacity - _output.buffer_size() - 1)));
            size_t next_start = get<1>(*iter) + 1;
            size_t next_end = temp_end;
            if (max_end >= temp_start) {
                _staging_list.insert(iter,
                                     make_tuple(temp_start,
                                                max_end,
                                                data.substr(temp_start - index, max_end - temp_start + 1)));
                _staging_size += max_end - temp_start + 1;
            }

            //}
            if (temp_end <= get<1>(*iter)) {
                break;
            }
            if (next_start > next_end) {
                break;
            }
            temp = make_tuple(next_start, next_end);
        } else if (temp_end <= get<1>(*iter)) {
            break;
        } else if (temp_end > get<1>(*iter)) {
            temp = make_tuple(max(get<1>(*iter) + 1, temp_start), temp_end);
        }
    }
    if (iter == _staging_list.end()) {
        size_t temp_start = get<0>(temp);
        size_t temp_end = get<1>(temp);
        size_t remain_size = _capacity - unassembled_bytes() - _output.buffer_size();
        if (remain_size != 0) {
            size_t max_end = min(
                temp_end,
                min(temp_start + remain_size - 1, _unassembled_start_index + _capacity - _output.buffer_size() - 1));
            _staging_list.push_back(
                make_tuple(temp_start, max_end, data.substr(temp_start - index, max_end - temp_start + 1)));
            _staging_size += max_end - temp_start + 1;
        }
    }
    for (iter = _staging_list.begin(); iter != _staging_list.end();) {
        if (get<0>(*iter) == _unassembled_start_index) {
            size_t size = get<1>(*iter) - get<0>(*iter) + 1;
            _output.write(move(get<2>(*iter)));
            _unassembled_start_index = get<1>(*iter) + 1;
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
