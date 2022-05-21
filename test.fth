\ forth test program

: sq dup * ;

: to0 dup 0 > if dup . 1 - recurse then ;

." Hello world!" cr
4 sq to0 cr
2 4 swap / . cr

0 if bye else 0 . cr then
