const express = require('express');
const program = require('commander');
const lightstep = require('lightstep-tracer');
const opentracing = require('opentracing');
const winston = require('winston');
const tracingMiddleware = require('./opentracing-express');

program.option('p, --port <n>', 'Port', parseInt)
    .option('r, --data_root <data_root>', 'Data Root')
    .option('a, --access_token <access_token>', 'Access Token')
    .parse(process.argv);

if (typeof program.port === 'undefined') {
  winston.error('no port given!');
  process.exit(1);
}

if (typeof program.access_token === 'undefined') {
  winston.error('no access_token given!');
  process.exit(1);
}

const accessToken = program.access_token;
const tracer = new lightstep.Tracer(
    { access_token: accessToken, component_name: 'library' });
opentracing.initGlobalTracer(tracer);

const app = express();
app.use(tracingMiddleware.middleware({ tracer }));

app.get('/header.html', (req, res) => {
  res.send('Books are great!');
});

app.get('/', (req, res) => {
  res.send('<html><body><esi:include src="/header.html"/></body></html>');
});

app.listen(program.port,
           () => { winston.log('info', `Listening on ${program.port}`); });
