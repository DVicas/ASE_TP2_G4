import smbus
import time

i2c_ch = 1
i2c_addr = 0x4D
temp = 0x00

bus = smbus.SMBus(i2c_ch)

f = open("temps.txt", "w")

for i in range(0,1):

    raw_val = bus.read_i2c_block_data(i2c_addr, temp, 2)
    final_temp = (raw_val[0] << 4) | (raw_val[1] >> 4)
    final_temp = final_temp*0.0625
    
    f.write(f"{final_temp} ")
