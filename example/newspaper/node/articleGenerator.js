const _ = require('lodash');
const winston = require('winston');

const kSubstitutions = {
  animal: ['lion', 'eagle', 'fox', 'gorilla', 'bear', 'pangolin', 'tarantula'],
  object: ['table', 'chair', 'bus', 'car', 'truck'],
  gender: ['male', 'female'],
  firstName: ['Sarah', 'Beth', 'April', 'Paris', 'Nicole', 'John', 'Joe'],
  lastName: ['Clark', 'Jones', 'Williams', 'Smith', 'Miller'],
  neighborhood: [
    'Fort Greene', 'DUMBO', 'Clinton Hill', 'Prospect Heights', 'Park Slope',
    'Crown Heights', 'Brooklyn Heights'
  ],
  singer:
      ['Bob Dylan', 'Guns N\' Roses', 'Eminem', 'Neil Young', 'Dire Straits'],
  famousPerson: [
    'Captain America', 'Abraham Lincoln', 'Balto', 'Thomas Jefferson',
    'Ulysses Grant', 'Miss Brooklyn'
  ],
  location:
      ['Prospect Park', 'Sunset Park', 'Barclays Center', 'Fort Greene Park'],
  occupation: [
    'plumber', 'carpenter', 'artist', 'street perfomer', 'farmer', 'barista'
  ]
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
          '${animal} under her ${object} and notified authorities.'
        ].join('')
      },
      {
        headline:
            'Glass Facade falls from newly built ${Neighborhood} High Rise!',
        body: [
          'The large piece of glass shattered on the sidewalk below. The FDNY ',
          'has closed the adjacent street and a team of engineers are ',
          'investigated what caused the incident.'
        ].join('')
      },
      {
        headline: '${Singer} holds concert in ${Location}',
        body: [
          'The concert is expected to draw large crowds and ticket costs have ',
          'already soared. The NYPD is cracking down on scalpers, who are ',
          'expected to make large profits.'
        ].join('')
      },
      {
        headline:
            'New Statue of ${FamousPerson1} to be constructed in ${Location}',
        body: [
          'The annoucement drew mixed reactions from residents. ${Occupation} ',
          '${firstName} ${lastName} said he would prefer to see a statue of ',
          '${famousPerson2}; but others have welcomed the development.'  
        ].join('')
      },
      {
        headline:
          '${Neighborhood} Resident Arrested for Keeping Pet ${Animal}',
        body: [
          'Animal control was alerted to the situation after neighbor ',
          '${firstName} ${lastName} complained of loud noises. It is illegal ',
          'to keep ${animal}s under New York Exotic Pet Law.'
        ].join('')
      }
    ]

    function pickSubstitution(cls) {
      if (!(cls in kSubstitutions)) return null;
      const choices = kSubstitutions[cls];
      if (!choices.length) return null;
      return choices[_.random(choices.length - 1)];
    }

function substitute(substitutions, template) {
  const blankRegExp = /\$\{([^0-9}]+)\d*\}/g;
  const matches = _.uniq(template.match(blankRegExp));
  return template.replace(blankRegExp, (match, cls_) => {
    match = match.toLowerCase();
    cls = _.lowerFirst(cls_);
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
    if (cls === cls_)
      return result;
    else
      return _.upperFirst(result);
  });
}

function generateArticle() {
  const template = kTemplates[_.random(kTemplates.length-1)];
  const substitutions = {};
  return {
    headline: substitute(substitutions, template.headline),
    body: substitute(substitutions, template.body)
  };
}

console.log(generateArticle());
