echo off
del fs*.bin
set /p v=<../data/h4UI
echo Making version %v%

mklittlefs -p 256 -b 4096 -s 98304 -c ../data/ fs_1M.bin > nul
mklittlefs -p 256 -b 8192 -s 1024000 -c ../data/ fs_4M.bin > nul