LD_LIBRARY_PATH=$PWD/lib valgrind  --tool=memcheck --trace-children=yes --tool=drd --trace-alloc=yes --log-file=valgrind-%p.log  bin/libOrionDecoder 
