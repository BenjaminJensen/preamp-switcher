. ~/esp/esp-idf/export.sh
cd build/
esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o merged_qemu.bin @flash_args
~/esp/qemu/qemu/build/qemu-system-xtensa -nographic -s -S -machine esp32 -drive file=merged_qemu.bin,if=mtd,format=raw