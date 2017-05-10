const express = require('express');
const program = require('commander');
const winston = require('winston');

program.option('p, --port <n>', 'Port', parseInt)
    .option('r, --data_root <data_root>', 'Data Root')
    .option('a, --access_token <access_token>', 'Access Token')
    .parse(process.argv);

if (typeof program.port === 'undefined') {
  winston.error('no port given!');
  process.exit(1);
}

const app = express();

app.get('/', (req, res) => {
  res.send('Hello, hello');
});

app.listen(program.port,
           () => { winston.log('info', `Listening on ${program.port}`); });
