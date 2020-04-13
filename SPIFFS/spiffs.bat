echo off
del spiffs*.bin
set /p v=<../data/h4sv
echo Making version %v%
mkspiffs -c ../data/ spiffs_%v%_1M.bin > nul
mkspiffs -p 256 -b 8192 -s 0x0FA000 -c ../data/ spiffs_%v%_4M.bin > nul
