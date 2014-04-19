var fetchedVersion = 0;

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

function fetchVersion() {
	if(fetchedVersion == 0){
		var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://edwinfinch.github.io/justabitplus", false);
	console.log("Getting latest watchapp and javascript version from: http://edwinfinch.github.io/justabitplus");
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        response = JSON.parse(req.responseText);
        var watchAppVersion;
        if (response > 0) {
			watchAppVersion = response,
			console.log("Latest watchapp version: " + watchAppVersion + ". Sending to pebble...");
          Pebble.sendAppMessage({
            "watchappver":watchAppVersion,
			});
			fetchedVersion = 1;
        }
		  else{
			  console.log("Version API error: No existing value in response");
		  }
      } else {
		  console.log("Error: could not connect");
      }
    }
  };
  req.send(null);
	}
}

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