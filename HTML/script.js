var connection = new WebSocket('ws://192.168.1.11:81/', ['arduino']);

connection.onopen = function() { connection.send('Connect ' + new Date()); };
connection.onerror = function(error) { console.log('WebSocket Error ', error); };
connection.onmessage = function(e) { console.log('Server: ', e.data); };

function sendRGB() {
    var r = parseInt(document.getElementById('r').value).toString(16);
    var g = parseInt(document.getElementById('g').value).toString(16);
    var b = parseInt(document.getElementById('b').value).toString(16);
    var num = '$' + document.getElementById('num').value;
    
    if (r.length < 2) { r = '0' + r; }
    if (g.length < 2) { g = '0' + g; }
    if (b.length < 2) { b = '0' + b; }
    
    var rgb = '#' + r + g + b;
    
    console.log('RGB: ' + rgb + '   NUM: ' + num);
    connection.send(rgb); 
    connection.send(num); 
  }

function updatePicker(picker) {
    //document.getElementById('hex-str').innerHTML = picker.toHEXString();
    //document.getElementById('rgb-str').innerHTML = picker.toRGBString();
    
    document.getElementById('r').value = picker.rgb[0];
    document.getElementById('g').value = picker.rgb[1];
    document.getElementById('b').value = picker.rgb[2];
    
    document.getElementById('picker_value').innerHTML =
        Math.round(picker.rgb[0]) + ', ' +
        Math.round(picker.rgb[1]) + ', ' +
        Math.round(picker.rgb[2]);

    sendRGB();
}