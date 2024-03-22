Assignment 2 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 6 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): []

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
[]

Implementation Challenges:
[]

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
