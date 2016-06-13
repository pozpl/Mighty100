Pebble.addEventListener('ready', function () {
    console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function () {
    var url = 'https://pozpl.github.io/pebble-config/mighty100.html';
    console.log('Showing configuration page: ' + url);

    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));

    // Send to the watchapp via AppMessage
    var dict = {
        'languageDict': configData.language_dict
    };

    // Send to watchapp
    Pebble.sendAppMessage(dict, function () {
        console.log('Send successful: ' + JSON.stringify(dict));
    }, function () {
        console.log('Send failed!');
    });
});

