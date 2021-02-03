echo off
del fs*.bin
set /p v=<../data/h4UIv
echo Making version %v%

mklittlefs -c ../data/ fs_1M.bin > nul
mklittlefs -p 256 -b 8192 -s 1024000 -c ../data/ fs_4M.bin > nul