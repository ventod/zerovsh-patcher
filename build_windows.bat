cd kernel
make clean
make DEBUG=0
copy zerovsh_patcher.prx ../bin
del -Rf *.prx *.elf zerovsh_upatcher.h
cd ../