#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    // two_32: 2 ^ 32
    const uint64_t two_32 = static_cast<uint64_t>(2) << 31;

    // seqno = (isn + absolute_seqno) mod (2 ^ 32)
    return WrappingInt32(static_cast<uint32_t>((isn.raw_value() + n) % two_32));
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const uint64_t two_32 = static_cast<uint64_t>(2) << 31;

    // determine absoule_seqno mod (2^32)
    uint64_t absolute_seqno_mod = n.raw_value();
    if (n.raw_value() < isn.raw_value()) {
        absolute_seqno_mod += two_32;
    }
    absolute_seqno_mod -= isn.raw_value();

    // determine absolute_seqno's quotient by (2^32)
    uint64_t checkpoint_quotient = checkpoint / two_32;
    uint64_t absolute_seqno_candidate = absolute_seqno_mod + checkpoint_quotient * two_32;

    if (checkpoint_quotient >= 1) {
        // when absolute seqno's quotient == checkpoint's quotient - 1   (by 2^32)
        if (checkpoint - (absolute_seqno_candidate - two_32) < two_32 / 2) {
            return absolute_seqno_candidate - two_32;
        }
    }

    // when absolute seqno's quotient == checkpoint's quotient + 1   (by 2^32)
    if ((absolute_seqno_candidate + two_32) - checkpoint < two_32 / 2) {
        return absolute_seqno_candidate + two_32;
    }

    // when absolute seqno's quotient == checkpoint's quotient   (by 2^32)
    return absolute_seqno_candidate;
}
