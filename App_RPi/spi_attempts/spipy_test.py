import spi

device_0 = spi.openSPI(device="/dev/spidev0.0",mode=0,speed=500000,bits=8,delay=0)

data_out = (0x06)
spi.transfer(device_0, data_out)


data_in = (0x00, 0x00, 0x00)
data_out = (0x02, 0x00, 0x34)

data_in = spi.transfer(device_0, data_out)

print(data_in)
