import os
import sys

eprom = ["26:96:75:df:f8:bd", "26:65:ce:4a:c1:bc"] 

print(sys.argv[1])

if sys.argv[1] in eprom:
    print("LIGA LED VERDE")
else:
    print("LIGA LED VERMELHO")
