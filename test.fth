: sq dup * ;

: to0 dup 0 > if dup . cr 1 - recurse then ;

4 sq to0
