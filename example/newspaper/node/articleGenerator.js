const _ = require('lodash');
const winston = require('winston');
const sqlite3 = require('sqlite3');
const common = require('./common');
const program = require('commander');
const path = require('path');
const uuid = require('uuid/v1');

const kSubstitutions = {
  animal: ['lion', 'eagle', 'fox', 'gorilla', 'bear', 'pangolin', 'tarantula'],
  object: ['table', 'chair', 'bus', 'car', 'truck'],
  gender: ['male', 'female'],
  firstName: ['Sarah', 'Beth', 'April', 'Paris', 'Nicole', 'John', 'Joe'],
  lastName: ['Clark', 'Jones', 'Williams', 'Smith', 'Miller'],
  neighborhood: [
    'Fort Greene', 'DUMBO', 'Clinton Hill', 'Prospect Heights', 'Park Slope',
    'Crown Heights', 'Brooklyn Heights',
  ],
  singer:
      ['Bob Dylan', 'Guns N\' Roses', 'Eminem', 'Neil Young', 'Dire Straits'],
  famousPerson: [
    'Captain America', 'Abraham Lincoln', 'Balto', 'Thomas Jefferson',
    'Ulysses Grant', 'Miss Brooklyn',
  ],
  location:
      ['Prospect Park', 'Sunset Park', 'Barclays Center', 'Fort Greene Park'],
  occupation: [
    'plumber', 'carpenter', 'artist', 'street perfomer', 'farmer', 'barista',
  ],
};

const kTemplates =
    [
      {
        headline: '${Animal} Escapes from Brooklyn Zoo!',
        body: [
          'A ${gender} ${animal} escaped from Brooklyn Zoo after handlers ',
          'forgot to close the ${animal}\'s cage. The ${animal} wandered into ',
          '${neighborhood} scaring local residents.', '\n',
          'Long-time Brooklynite ${firstName} ${lastName} found the ',
          '${animal} under her ${object} and notified authorities.',
        ].join('')
      },
      {
        headline:
            'Glass Facade falls from newly built ${Neighborhood} High Rise!',
        body: [
          'The large piece of glass shattered on the sidewalk below. The FDNY ',
          'has closed the adjacent street and a team of engineers are ',
          'investigated what caused the incident.',
        ].join('')
      },
      {
        headline: '${Singer} holds concert in ${Location}',
        body: [
          'The concert is expected to draw large crowds and ticket costs have ',
          'already soared. The NYPD is cracking down on scalpers, who are ',
          'expected to make large profits.',
        ].join('')
      },
      {
        headline:
            'New Statue of ${FamousPerson1} to be constructed in ${Location}',
        body: [
          'The annoucement drew mixed reactions from residents. ${Occupation} ',
          '${firstName} ${lastName} said he would prefer to see a statue of ',
          '${famousPerson2}; but others have welcomed the development.',
        ].join('')
      },
      {
        headline:
          '${Neighborhood} Resident Arrested for Keeping Pet ${Animal}',
        body: [
          'Animal control was alerted to the situation after neighbor ',
          '${firstName} ${lastName} complained of loud noises. It is illegal ',
          'to keep ${animal}s under New York Exotic Pet Law.',
        ].join('')
      },
    ];

program.option('r, --data_root <data_root>', 'Data Root').parse(process.argv);

if (typeof program.data_root === 'undefined') {
  winston.error('no data_root given!');
  process.exit(1);
}

const databasePath = path.join(program.data_root, common.databaseName);
const db = new sqlite3.Database(databasePath);
db.run('PRAGMA journal_mode = WAL');
db.configure('busyTimeout', 15000);

function pickSubstitution(cls) {
  if (!(cls in kSubstitutions)) {
    return null;
  }
  const choices = kSubstitutions[cls];
  if (!choices.length) {
    return null;
  }
  return choices[_.random(choices.length - 1)];
}

function substitute(substitutions, template) {
  const blankRegExp = /\$\{([^0-9}]+)\d*\}/g;
  const matches = _.uniq(template.match(blankRegExp));
  return template.replace(blankRegExp, (match, cls_) => {
    match = match.toLowerCase();
    const cls = _.lowerFirst(cls_);
    let result;
    if (match in substitutions) {
      result = substitutions[match];
    } else {
      result = pickSubstitution(cls);
      substitutions[match] = result;
    }
    if (result == null) {
      winston.error(`Unable to generate substitution for ${cls}!`);
      process.exit(-1);
    }
    if (cls !== cls_) {
      return _.upperFirst(result);
    }
    return result;
  });
}

function generateArticle() {
  const template = kTemplates[_.random(kTemplates.length - 1)];
  const substitutions = {};
  return {
    headline: substitute(substitutions, template.headline),
    body: substitute(substitutions, template.body),
  };
}

function writeRandomArticle() {
  const article = generateArticle();
  const id = uuid();
  const priority = _.random(5);
  const expiration = ((new Date()).getTime() / 1000) + _.random(50);
  const stmt = db.prepare('insert into articles values (?, ?, ?, ?, ?)');
  stmt.run(id, priority, expiration, article.headline, article.body, (err) => {
    if (err) {
      winston.error(`Unable to insert article: ${err}!`);
    }
  });
}

let running = true;
function runWriter() {
  if (!running) {
    return;
  }
  writeRandomArticle();
  setTimeout(runWriter, _.random(5000));
}

runWriter();

function onExit() {
  running = false;
  db.close();
  process.exit(0);
}
process.on('SIGINT', onExit);
process.on('SIGTERM', onExit);
