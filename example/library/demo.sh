#!/bin/bash
./start.sh
sleep 3
node node/client.js
node node/client.js
sleep 3
./stop.sh
