wait_halt
sleep 10
poll
mdw 0x0 100
flash probe 0

# erase all LPC2148 sectors (512k)
flash erase_sector 0 0 26
flash erase_check 0
mdw 0x0 100
