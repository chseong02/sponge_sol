Assignment 2 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 6 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
## Program Structure and Design of the TCPReceiver and wrap/unwrap routines
### wrap/unwrap
To `wrap`, I simply used the relation `(isn + absolute_seqno) mod (2 ^ 32)`. 

For `unwrap`, I first subtracted `isn` from the received `n`(maybe + 2^32) 
to calculate `n`'s `absolute_seqno`'s remainder when divided by 2^32. 

I divided `checkpoint` by 2^32 to find the quotient. 
When dividing `absolute_seqno` by 2^32, the possible quotient could be the 
same as `checkpoint`'s quotient or +1/-1. This means there are three possible 
candidates. If the difference between `checkpoint` and `absolute_seqno` is 
closer than half of 2^32, which is 2^31, it can be guaranteed to be the closest. 
I implemented this mathmatical logic into the code in the same way.

### TCPReceiver
I also included and utilized `_have_SYN` and `_have_FIN` to indicate whether 
I've encountered those flags until now and `_icn`, a unique random number 
variable obtained from the SYN packet. 

`bytes_written`, the total index input into the Reassembler's `ByteStream` 
which is identical to the last assembled index (since it must be sorted to 
enter the ByteStream). This fact is used to calculate `window_size` and `ackno`.

Using `bytes_written` in many places helps to reduce the potential for 
inconsistency in information that could arise from managing unnecessary and 
duplicated states separately. I focused on maintaining a minimal state 
and not creating unnecessary ones.

The process in `segment_received` mainly consists of handling the SYN Flag, 
inserting the Payload, and processing the FIN Flag in that order.

Implementation Challenges:
## Implementation Challenges
Handling the index diffence +2 or -2 due to SYN and FIN flags was one of the 
most challenging parts of the implementation. 
For debugging purposes, I used `cout` to print out the values of various 
variables (`bytes_written`, `unwrap` result, etc.) during those specific situations. 
This way, I could check the output values when a test failed and debug accordingly.

I also made detailed variable name that even if variable names became long length,
their meanings were clear. (Ex. `length_wo_flag`)

The most crucial aspect, as mentioned in the `unexpected difficulty` part, 
was understanding the TCP Packet architecture. I had misunderstood that Flags 
could exist within the Payload, which was not the case. 
As a result, in SYN packets, the first index of the Payload could be different 
from the Sequence Number in the header.
Also, I have to consider FYN Flags index.

The problem was solved by considering all of the above.

Remaining Bugs:
## Remaining Bugs
I've passed all the tests. So I hope there are no bugs.

There might be a slight possibility of exceeding the upper range of uint64_t 
during operations at very long packet. For example, when unwrapping `WrappingInt32` 
to find the exact `absolute_seqno`, code investigates near the 2^32 region of 
the checkpoint. During this process, there's a chance it might temporarily exceed 
the upper limit, potentially causing issues.

But, This only happens for very long abnormal lengths(>=2^64),


- Optional: I had unexpected difficulty with: 
  - I had a tough time because I misunderstood the meaning of the Sequence Number
   header values across multiple packets and whether or not there was a Flag 
   present in the Payload. In the end, I realized my mistake and I was able to 
   complete the assignment. 

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
