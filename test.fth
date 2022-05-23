\ forth test program

: sq dup * ;

: to0 dup 0 > if dup . 1 - recurse then ;

." Hello world!" cr
4 sq to0 cr
2 4 swap / . cr

: 1to10 11 1 do i . loop cr ;
1to10

bye
