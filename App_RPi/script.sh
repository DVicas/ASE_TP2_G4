#!/bin/bash
if [[ $2 == "AP-STA-CONNECTED" ]]
then
  python3 updateEPROM.py $3
fi

if [[ $2 == "AP-STA-DISCONNECTED" ]]
then
  echo "someone has disconnected with mac id $3 on $1"
fi
