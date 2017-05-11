const request = require('request');

function listBooks() {
  request.get('http://localhost:8080/', function(err, resp, body) {
    if (err) {
      console.log(err);
    }
  });
}

listBooks();
