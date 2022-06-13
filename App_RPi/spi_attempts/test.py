import spidev
import time
import RPi.GPIO as GPIO

spi = spidev.SpiDev(0, 0) # create spi object connecting to /dev/spidev0.1
spi.max_speed_hz = 250000 # set speed to 250 Khz

GPIO.setmode(GPIO.BCM)
GPIO.setup(22, GPIO.OUT)

GPIO.output(22, GPIO.HIGH)





#try:
                    # endless loop, press Ctrl+C to exit
    #spi.writebytes([0x03, 0x00])

    #spi.xfer([

    #print(spi.readbytes(1)) # write one byte
    #GPIO.output(8, GPIO.LOW)
    #time.sleep(0.1) # sleep for 0.1 seconds 
    #GPIO.output(8, GPIO.HIGH)
#finally:
spi.close() # always close the port before exit
