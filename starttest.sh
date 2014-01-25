#!//bin/bash

do_valg(){
	valgrind --tool=memcheck --demangle=yes --error-limit=no --track-origins=yes --show-reachable=yes --leak-check=full --suppressions=$ROOTSYS/etc/valgrind-root.supp  ./g4sim macros/run.mac > log 2>&1
}
do_make(){
	make
}

do_make &
wait
do_valg &
wait
vim log
