const express = require('express');
const program = require('commander');
const lightstep = require('lightstep-tracer');
const opentracing = require('opentracing');
const TracedPromise = require('opentracing-tracedpromise').default;
const winston = require('winston');
const sqlite3 = require('sqlite3');
const common = require('./common');
const path = require('path');
const tracingMiddleware = require('./opentracing-express');

program.option('p, --port <n>', 'Port', parseInt)
    .option('r, --data_root <data_root>', 'Data Root')
    .option('a, --access_token <access_token>', 'Access Token')
    .parse(process.argv);

if (typeof program.data_root === 'undefined') {
  winston.error('no data_root given!');
  process.exit(1);
}

if (typeof program.port === 'undefined') {
  winston.error('no port given!');
  process.exit(1);
}

if (typeof program.access_token === 'undefined') {
  winston.error('no access_token given!');
  process.exit(1);
}

const databasePath = path.join(program.data_root, common.databaseName);
const db = new sqlite3.Database(databasePath);

const accessToken = program.access_token;
const tracer = new lightstep.Tracer(
    { access_token: accessToken, component_name: 'newspaper' });
opentracing.initGlobalTracer(tracer);

const app = express();
app.use(tracingMiddleware.middleware({ tracer }));
app.set('view engine', 'pug');
app.set('views', path.join(__dirname, '/views'));

function selectTopArticles(span) {
  const options = {
    childOf: span,
    tags: {
      component: 'SQLite',
    },
  };
  const now = (new Date()).getTime() / 1000;
  const stmtPattern = 'select uuid from articles where expiration > ? ' +
                      'order by priority, expiration limit 10';
  const stmt = db.prepare(stmtPattern);
  return new TracedPromise(options, stmtPattern, (resolve, reject) => {
    stmt.all(now, (err, rows) => {
      if (err) {
        winston.log('Unable to select!', {error : err});
        reject();
      } else {
        resolve(rows);
      }
    });
  });
}

function selectArticle(span, id) {
  const options = {
    childOf: span,
    tags: {
      component: 'SQLite',
    },
  };
  const stmtPattern = 'select headline, body from articles where uuid=?';
  const stmt = db.prepare(stmtPattern);
  return new TracedPromise(options, stmtPattern, (resolve, reject) => {
    stmt.get(id, (err, row) => {
      if (err) {
        winston.log('Unable to select!', {error : err});
        reject();
      } else if (row === undefined) {
        reject();
      } else {
        resolve(row);
      }
    });
  });
}

app.get('/header.html', (req, res) => {
  res.send('Books are great!');
});

app.get('/article/:articleId', (req, res) => {
  selectArticle(req.span, req.params.articleId)
      .then(
          (row) => {
            res.render('snippet', {headline : row.headline, short : row.body});
          },
          () => { res.status(500); });
});

app.get('/', (req, res) => {
  selectTopArticles(req).then(
      (rows) => {
        res.render('index', {
          headlines: rows.map((row) => `/article/${row.uuid}`),
        });
      },
      () => { res.status(500); });
});

app.listen(program.port,
           () => { winston.log('info', `Listening on ${program.port}`); });
