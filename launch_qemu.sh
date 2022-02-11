. ~/esp/esp-idf/export.sh
cd build/
esptool.py --chip esp32 merge_bin --fill-flash-size 4MB -o merged_qemu.bin @flash_args
if [ -o gdb ]
  then
	~/esp/qemu/build/qemu-system-xtensa -nographic -s -S -machine esp32 -drive file=merged_qemu.bin,if=mtd,format=raw -serial tcp::8880,server,nowait -serial tcp::8881,server,nowait -serial tcp::8882,server,nowait
else 
  ~/esp/qemu/build/qemu-system-xtensa -nographic -machine esp32 -drive file=merged_qemu.bin,if=mtd,format=raw -serial mon:stdio -serial tcp::8881,server,nowait -serial tcp::8882,server,nowait
fi
#~/esp/qemu/build/qemu-system-xtensa -nographic -s -S -machine esp32 -drive file=merged_qemu.bin,if=mtd,format=raw -serial tcp::8880,server,nowait -serial tcp::8881,server,nowait -serial tcp::8882,server,nowait
#~/esp/qemu/build/qemu-system-xtensa -nographic -s -S -machine esp32 -drive file=merged_qemu.bin,if=mtd,format=raw