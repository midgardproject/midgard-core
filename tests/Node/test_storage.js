
var vows   = require('vows'),
    assert = require('assert');

var gir = require('gir');
gir.init();

var Midgard, gir, config, mgd;
Midgard = gir.load('Midgard');
Midgard.init();

var mgd = require('./midgard_connection');

console.log(mgd.cnc);

var suite = vows.describe('Midgard.Config');
suite.addBatch({
	'Create Storage' : {
		'Base' : {
			topic : function () {
				s = new Midgard.Storage();
				console.log(mgd.cnc);
				return s.__call__('create_base_storage', mgd.cnc);	
			},
			'created' : function (topic) {
				assert.isTrue (topic)
			}	
		}
	}
}).run();
