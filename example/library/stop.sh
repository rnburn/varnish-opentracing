#!/bin/bash

DATA_ROOT=$PWD/library-data
VARNISH_ROOT=$DATA_ROOT/varnish

while read pid; do
  kill $pid
done <$DATA_ROOT/backend_pids
rm $DATA_ROOT/backend_pids

while read pid; do
  kill $pid
done <$DATA_ROOT/varnish_pids
rm $DATA_ROOT/varnish_pids
