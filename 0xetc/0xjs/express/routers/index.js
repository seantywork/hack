let express = require('express');
let ctrlindex = require('../controllers');

module.exports = function () {

    let router = express.Router();
    
    router.get('/', ctrlindex.GetIndex);

    return router;
};


