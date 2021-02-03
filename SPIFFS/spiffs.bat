echo off
del spiffs*.bin
set /p v=<../data/h4UI
echo Making version %v%
mkspiffs -c ../data/ spiffs_%v%_1M.bin > nul
mkspiffs -p 256 -b 8192 -s 0xFA000 -c ../data/ spiffs_%v%_4M.bin > nul
