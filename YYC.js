var XMLHttpRequest=require("xmlhttprequest").XMLHttpRequest;
var url = 'http://localhost:8086/write?db=mydb'
var val = 0;
var xhttp = new XMLHttpRequest();

xhttp.onreadystatechange=function()
{
	if(this.readyState == 4 && this.status==204){
		console.log('inserted');
	}else if(this.status>=300){
		console.log('insert Error');
	}
};
var mqtt = require('mqtt');
const options = {
	host : 'tailor.cloudmqtt.com',
	port : 26417,
	username: "nsiilwwe",
	password: "tAboZHax9-Ue",
	protocol: 'mqtts',
};
var client = mqtt.connect(options);
client.on('connect', function(){
	client.subscribe('cds');
	client.subscribe('ultsonic');
	console.log('Connected');
})
client.on('message', function(topic,message){
	xhttp.open("POST",url,true);
	if(topic.toString() == 'cds'){
		xhttp.send('cds value="'+message.toString()+'"')
	}
	if(topic.toString() == 'ultsonic'){
		xhttp.send('ultrasonic value="'+message.toString()+'"')
	}
	console.log('['+topic+'] : '+message.toString())
})
