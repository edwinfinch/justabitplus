Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
						  fetchVersion();
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          console.log(e.type);
                          console.log(e.payload.temperature);
                          console.log("Appmessage");
                        });

// URL to your configuration page
var config_url = "http://edwinfinch.github.io/configscreen-jab_plus";

Pebble.addEventListener("showConfiguration", function(e) {
	var url = config_url;
	console.log("Opening configuration page: " + url);
	fetchVersion();
	Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
	if(e.response) {
		var values = JSON.parse(decodeURIComponent(e.response));

		for(key in values) {
			window.localStorage.setItem(key, values[key]);
		}

		Pebble.sendAppMessage(values,
			function(e) {
				console.log("Successfully sent options to Pebble");
			},
			function(e) {
				console.log("Failed to send options to Pebble.\nError: " + e.error.message);
			}
		);
	}
});