cd kernel

make clean
make DEBUG=0 USE_EF0=0
copy zerovsh_patcher.prx ../bin/zerovsh_patcher_ms0.prx
del -Rf *.prx *.elf

make clean
make DEBUG=0 USE_EF0=1
copy zerovsh_patcher.prx ../bin/zerovsh_patcher_ef0.prx
del -Rf *.prx *.elf

cd ../