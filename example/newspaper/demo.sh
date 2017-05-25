#!/bin/bash
./start.sh
sleep 3
node node/client.js
sleep 2
node node/client.js
sleep 6
node node/client.js
sleep 3
./stop.sh
