cd kernel
make clean
make DEBUG=0
cp zerovsh_patcher.prx ../bin
rm -Rf *.prx *.elf
cd ../