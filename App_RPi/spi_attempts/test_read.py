import spidev
import time
import RPi.GPIO as GPIO

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 500000
spi.mode = 0b00
spi.lsbfirst = False
#spi.no_cs = True
#spi.cshigh = False


#WREN COMMAND

spi.writebytes([0x06])
spi.writebytes([0x02, 0x01, 0x13])
    #time.sleep(1)


#Read status instruction
#print(spi.xfer([0x05]))


time.sleep(0.1)

#escrita 0x02
print(spi.readbytes(520))

#time.sleep(0.2)

#leitura 0x03
#print(spi.xfer([0x03, 0x00]))

#spi.writebytes(to_send)


spi.close()
