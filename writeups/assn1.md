Assignment 1 Writeup
=============

My name: Seong Chiho

My POVIS ID: chseong

My student ID (numeric): 20200423

This assignment took me about 12 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
## List Structure
Internet packets can arrive out of order, and in the process of reassemble them,
the sequence of characters can often change.
Therefore, a `list` data structure, which is relatively efficient 
in changing the position of items and changing items length, was chosen.

To facilitate fast search, List items are stored as 
`tuple<size_t, size_t, string>` type, consisting of 
`(front index, end index, substring)`, allowing for quick determination of 
the correct order without depending on the length of the string, based solely 
on the two indexes.

Furthermore, since there is no need to reorder within `data` 
(a contiguous String with correct order), I was intended to preserve 
that's original form and reuse the inputted String(in-order), instead of handling 
strings as different Charactors.

Items in `_staging_list` are stored in a way that ensures both the first (start)
and second (end) values of the item tuples are always increasing, both 
within item and between items. In other words, each item is guaranteed 
never to overlap and is always sorted. This approach ensures that the act of 
inserting values into the list serves the same function as sorting.

## Staging-Write Logic
### Staging
Given a `_staging_list` that is guaranteed to be sorted, iterate from the 
beginning and save 'substrings of data' that should be positioned 
before the current item (`iter`) directly in front of 
(the item pointed to by `iter`). After the iteration, the substrings that have 
not been saved are stored at the end of `_staging_list`. 
By doing this, sorting and data input are complete.

### Write
After staging, start iterating through `_staging_list` again from the beginning,
and continue inserting items from the `_staging_list` into `_output` 
until a substring item with a start index that cannot be concatenated 
(i.e., not immediately following the end of `_output`) is encountered.

Implementation Challenges:
## Implementation Challenges: Consider Overlapping String 
To ensure efficiency, I preserved the pre-sorted string that was input. 
At the same time, considering the overlapping input values, it was very 
challenging to store them without any overlapping parts. 

I simply identified all possible relationships between two ranges 
(the stored item(`iter`) and the item to be stored(`temp`)), which could 
only be six cases at most, and resolved it by handling each case accordingly. 

Additionally, dealing with potential overlap across multiple items was another 
difficult aspect. I managed this by only saving the part of the substring that 
is before the current item (`iter`) in range and postponing the rest 
to the next iteration.

Remaining Bugs:
## Remaining Bugs
I've passed all the tests. So hopefully, I think no remaining bugs in my code. 
But I am afraid about some unnecessary loop iteration. It is expected that 
there will be cases where the `for` statement is unnecessarily iterated once more 
due to the rather complicated relationship between `iter` and `temp`'s start, 
and end index cases.

- Optional: I had unexpected difficulty with:
    - It was difficult to know the location of the iterator when the iterator 
    was deleted from the `for` statement using itator and the index increased 
    by 2 in the next loop iter.

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
