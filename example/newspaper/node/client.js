const request = require('request');
const winston = require('winston');

function readNewspaper() {
  request.get('http://localhost:8080/', (err) => {
    if (err) {
      winston.error('Could not view newspaper:', {error: err});
    }
  });
}

readNewspaper();
