module.exports = {
  init: function (variant) {
    return new Promise(function(resolve, reject) {
      cordova.exec(resolve, reject, 'CordovaPluginScan', 'init', [variant]);
    });
  },

  cmd: function (cmd) {
    if (typeof cmd !== 'string') {
      throw new Error('window.Scan.cmd must be called with a single string parameter.');
    }

    return new Promise(function(resolve, reject) {
      cordova.exec(resolve, reject, 'CordovaPluginScan', 'cmd', [cmd]);
    });
  },

  output: function (success, error) {
    cordova.exec(success, error, 'CordovaPluginScan', 'output', []);
  },

  exit: function () {
    return new Promise(function(resolve, reject) {
      cordova.exec(resolve, reject, 'CordovaPluginScan', 'exit', []);
    });
  }
};
