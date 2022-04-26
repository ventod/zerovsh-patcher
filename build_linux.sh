cd kernel

make clean
make DEBUG=0 USE_EF0=0
cp zerovsh_patcher.prx ../bin/zerovsh_patcher_ms0.prx
rm -Rf *.prx *.elf

make clean
make DEBUG=0 USE_EF0=1
cp zerovsh_patcher.prx ../bin/zerovsh_patcher_ef0.prx
rm -Rf *.prx *.elf

cd ../