/*******************************************************************************
 * Update the system clock on the primary I/O page (index.htm)				   *
 * every two seconds (by default, can change refresh rate - see documentation) *
 *******************************************************************************/
function updateClock() {
	var clock = document.getElementById("clock");
	var url = "clockData.html";
	var req;
	var data;
	
	if( window.XMLHttpRequest ) {
		req = new XMLHttpRequest();
	}
	else if( window.ActiveXObject ) {
		req = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if( req != null ) {
		req.open('get',url);
		req.onreadystatechange = function() {
			if( req.readyState == 4 ) {
				if( req.status == 200 ) {
					data = req.responseText;
					console.log(data);
					clock.innerHTML = data;
					
				}
			}
		}
		req.send(null);
	}
}