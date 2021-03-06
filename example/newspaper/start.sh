#!/bin/bash
if [ -z $LIGHTSTEP_ACCESS_TOKEN ]
then
  echo "LIGHTSTEP_ACCESS_TOKEN must be set"
  exit -1
fi

DATA_ROOT=$PWD/newspaper-data
VARNISH_ROOT=$DATA_ROOT/varnish

mkdir -p $VARNISH_ROOT

node node/setup.js --data_root $DATA_ROOT

for i in {1..3}; do
  let port="3000+$i"
  node node/server.js --port $port --data_root $DATA_ROOT --access_token $LIGHTSTEP_ACCESS_TOKEN&
  echo $! >> $DATA_ROOT/backend_pids
done
node node/articleGenerator.js --data_root $DATA_ROOT&
echo $! >> $DATA_ROOT/backend_pids

cp varnish/newspaper.vcl $VARNISH_ROOT/newspaper.vcl
ENVS=`printenv`
for env in $ENVS
do
  IFS== read name value <<< "$env"
  sed -i -e "s|\${${name}}|${value}|g" $VARNISH_ROOT/newspaper.vcl
done
varnishd -F -a localhost:8080 -p vcc_allow_inline_c=on -p vcl_dir=$PWD/../../ -f $VARNISH_ROOT/newspaper.vcl -n $VARNISH_ROOT&
echo $! > $DATA_ROOT/varnish_pids
