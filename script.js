var touchstartX;
var touchstartY;
var rect;
var p1;
var p2;
var x,y;
var bool_L =false;
var bool_R =false;
var bool_U =false;
var bool_D =false;
var bool_LU =false;
var bool_LD =false;
var bool_RU =false;
var bool_RD =false;
var preset_array=[];
function toggle_menu(){
	/*        Show and hide the menu on click*/
	document.getElementById("menu-wrap").classList.toggle("hide");
	document.getElementById("joystick-container").classList.toggle("hide");
	document.getElementById("btn_menu1").classList.toggle("hide");
	document.getElementById("btn_menu2").classList.toggle("hide");
}

function handle_presets(data){
	/*clear the current values of the preset array*/
	preset_array=[];
	/*    clear the input elements*/
	for (i=1;i<11;i++){
		document.getElementById("preset"+i).value=""; 
		document.getElementById("preset"+i).type="text"; 
		
	}
	
	/* Get the list of presets*/
	
	/*    Data comes as 1:HOME,2:DOOR,*/
	/* data.split(",") will be an array of "number:name" pairs*/
	/*number = data.split(",")[i].split(":")[0]*/
	
	
	for (i=0;i<(data.split(",").length-1);++i){
		var number=data.split(",")[i].split(":")[0];
		var name= data.split(",")[i].split(":")[1];
		/*Set the preset input value to preset name supplied by the server*/
		document.getElementById("preset"+number).value=name;
		/*    Add the preset name to preset array*/
		preset_array.push(name);
		/*Set the input with a name to type=button*/
		document.getElementById("preset"+number).type="button";
		
	}
}
function send_command(dir,speed){
	/*        This function sends a command to the server only if there is a change in the direction 
	 So, it prevents continuous sending of data while the joystick is being "touchmove"
	 Hence, if, in future - we want to implement "speed of movement" - based on distance of the joystick from center - this will not work. might have to go back to old ways of continuos send dir and speed on touch move.*/
	switch(dir)
	{
		case ("L"):
			bool_R =false;
			bool_U =false;
			bool_D =false;
			bool_LU =false;
			bool_LD =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_L)){
				bool_L=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("R"):
			bool_L =false;
			bool_U =false;
			bool_D =false;
			bool_LU =false;
			bool_LD =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_R)){
				bool_R=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("U"):
			bool_R =false;
			bool_L =false;
			bool_D =false;
			bool_LU =false;
			bool_LD =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_U)){
				bool_U=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("D"):
			bool_R =false;
			bool_U =false;
			bool_L =false;
			bool_LU =false;
			bool_LD =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_D)){
				bool_D=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("LU"):
			bool_R =false;
			bool_U =false;
			bool_D =false;
			bool_L =false;
			bool_LD =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_LU)){
				bool_LU=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("LD"):
			bool_R =false;
			bool_U =false;
			bool_D =false;
			bool_LU =false;
			bool_L =false;
			bool_RU =false;
			bool_RD =false;
			if (!(bool_LD)){
				bool_LD=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("RU"):
			bool_R =false;
			bool_U =false;
			bool_D =false;
			bool_LU =false;
			bool_LD =false;
			bool_L =false;
			bool_RD =false;
			if (!(bool_RU)){
				bool_RU=true;
				doSend(dir+"-"+speed);
			}
			break;
		case ("RD"):
			bool_R =false;
			bool_U =false;
			bool_D =false;
			bool_LU =false;
			bool_LD =false;
			bool_RU =false;
			bool_L =false;
			if (!(bool_RD)){
				bool_RD=true;
				doSend(dir+"-"+speed);
			}
			break;
		default:
			console.loh(dir+"-"+speed);
			break;
	}
	
	
}
function preset(id,val){
	var el=document.getElementById(id);
	
	/*    Send to server if value is not empty - to go to the preset*/
	if (!(val=="")){
		
		doSend("S-"+val);
		/*            If Pan or Tilt were in progress - it will be stopped by the server to go to the preset - so set the pan and tilt buttons to initial state.*/
		document.getElementById('btn_pan').innerHTML="PAN";
		
		document.getElementById('btn_tilt').innerHTML="TILT";  
		
	}
	else {
		
		/*    User is clicking an empty preset - Do u want to save this location as a preset?*/
		/*var preset_name=prompt(" ई वाला जगह के Save कर दिहल जाओ?" , id);*/
		var preset_name=prompt(" Save this location as preset" , id);
		if (preset_name==null || preset_name==""){
			/*  user cancelled the prompt*/
		} else {
			
			/*                Check if this name is in the preset array - if yes then tell user that the preset will be updated*/
			
			for (i=0;i<(preset_array.length);++i){
				
				if (preset_name==preset_array[i]){
					
					if (confirm("This will update the preset to current location")){
						
						doSend("s-"+preset_name);
						doSend("l-preset");
						
					}
				}
			}
			
			/* Save the preset */
			doSend("s-"+preset_name);
			/*               After sending the save command - request a new list to update the preset buttons*/
			doSend("l-preset");
		}
	}
}
function toggle_pan(){
	
	var current_btnpan_val = document.getElementById('btn_pan').innerHTML;
	if (current_btnpan_val=="PAN"){
		document.getElementById('btn_pan').innerHTML="Stop";
		/* To do - can set Pan speed by P-Speed*/
		doSend("P-1");
		
		
	}
	else if (current_btnpan_val=="Stop"){
		document.getElementById('btn_pan').innerHTML="PAN";
		doSend("p-1");
	}  
	
}

function toggle_tilt(){
	
	var current_btntilt_val = document.getElementById('btn_tilt').innerHTML;
	if (current_btntilt_val=="TILT"){
		document.getElementById('btn_tilt').innerHTML="Stop";
		doSend("T-1");
	}
	else if (current_btntilt_val=="Stop"){
		document.getElementById('btn_tilt').innerHTML="TILT";
		doSend("t-1");
	}    
	
}

var onlongtouch; 
var timer;
var touchduration = 750; //length of time we want the user to touch before we do something

function touchstart(id,val) {
	timer = setTimeout(onlongtouch, touchduration,id,val); 
	
}

function touchend() {
	
		//stops short touches from firing the event
	if (timer)
		clearTimeout(timer); // clearTimeout, not cleartimeout..
}

onlongtouch=function(id,val) { //do something 
	
	/*    long press but val is empty - send to the preset function*/
	if (val==""){
		preset(id,val);
	} else {
		/* so, val is not empty -u long pressed- do you want to delete this preset or you want to update?*/
		
		/* var preset_name=prompt(" ई वाला जगह के Delete कर दिहल जाओ?" , val);*/
		var preset_name=prompt("Delete this preset" , val);
		if (preset_name==null || preset_name==""){
			/*  user cancelled the prompt*/
		} else {
			
			/* Save the preset */
			doSend("d-"+preset_name);
			/*               After sending the save command - request a new list to update the preset buttons*/
			/*  Set the deleted input element back to type text*/
			/* document.getElementById(id).type="text";*/
			/*       This didn't work -so when hadnling the list  - set all inputs to text*/
			
			doSend("l-preset");
		}
		
	}
	
};


window.addEventListener('focus', function() {
						foo(true);
						},{once:true}, false);

window.addEventListener('blur', function() {
						foo(false);
						}, {once:true}, false); 

/*    var counter=1;*/
function foo(bool) {
	if (bool){
			//															location.reload();
		/*        alert("reloading"+counter);
		 counter++;*/
	} else {
		
			//															websock.close();    
	}
}
function toggle_menu_btn(){
	
	var element = document.getElementById("menu_preset");
	element.classList.toggle("preset-menu-visible");
	
}
var draggable = document.getElementById('inner');
var outer = document.getElementById('outer');
draggable.addEventListener('touchstart', function(event) {
						   var touch = event.targetTouches[0];
						   
						   rect = draggable.getBoundingClientRect();
						   //Start point center of the inner circle
						   touchstartX=rect.left+((rect.right -rect.left)/2);
						   touchstartY=rect.top+((rect.bottom -rect.top)/2);
						   p1={
						   x:touchstartX,
						   y:touchstartY
						   };
						   //console.log(touchstartX,touchstartY,rect.left,rect.right,rect.top,rect.bottom);
						   }, false);

draggable.addEventListener('touchmove', function(event) {
						   var touch = event.targetTouches[0];
						   
						   //Distance between 2 coordinates
						   if (Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2)) < 125) {
						   // Place element where the finger is
						   draggable.style.left = touch.pageX-(rect.left-25)+'px';
						   draggable.style.top = touch.pageY-(rect.top-25) +'px';
						   x = touch.pageX;
						   y = touch.pageY;
						   
						   p2={
						   x:x,
						   y:y
						   };
						   
						   /*Instead of sending a command with every touch move  - check a boolean to see if that "direction" was already sent - If so, then don't send it until there is a change.
							This will disable the ability to implement "speed" of movement in future.
							Speed of movement based of distance of joystick isn't implemented on the server side yet.'*/
						   var angle=parseInt(Math.atan2(p2.y - p1.y, p2.x - p1.x) * 180 / Math.PI);
						   switch (true){
						   case (angle >-22.5 && angle < 22.5):  
						   /*console.log("Moving Right");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   /*doSend("R-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("R",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   break;
						   case (angle >22.5 && angle < 67.5):  
						   /* doSend("RD-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("RD",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   
						   /*
							console.log("Moving Right and Down");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>67.5 && angle < 112.5):  
						   /*doSend("D-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("D",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving Down");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>112.5 && angle < 157.5):  
						   /* doSend("LD-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("LD",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving Left and Down");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>157.5 && angle < 180)||(angle>-180 &&angle <-157.5):  
						   /* doSend("L-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("L",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving left");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>-157.5 && angle < -112.5):  
						   /*doSend("LU-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("LU",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving Left and Up");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>-112.5 && angle < -67.5):  
						   /*doSend("U-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("U",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving Up");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   case (angle>-67.5 && angle < -22.5):  
						   /* doSend("RU-"+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));*/
						   send_command("RU",parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))));
						   /*
							console.log("Moving Right and up");
							console.log("Distance from center is "+parseInt(Math.sqrt(Math.pow(Math.abs(touch.pageX-touchstartX),2) + Math.pow(Math.abs(touch.pageY-touchstartY),2))))
							*/
						   break;
						   default:
						   console.log("unknown angle");
						   break;
						   }
						   
						   
						   
						   
						   event.preventDefault();}
						   }, false);
draggable.addEventListener('touchend', function(event) {
						   var touch = event.targetTouches[0];
						   
						   // Place element where the finger is
						   draggable.style.left = '75px';
						   draggable.style.top = '75px';
						   //Send stop
						   doSend("X-X");
						   
						   /*    Set all the booleans to false*/
						   bool_L =false;
						   bool_U =false;
						   bool_D =false;
						   bool_LU =false;
						   bool_LD =false;
						   bool_RU =false;
						   bool_RD =false;
						   bool_R  =false;
						   /* Since on the server side - Sending X-X will stop loop_pan and loop_tilt
							Therefore - change the pan/tilt buttons to PAN/TILT from STOP
							No need to check if the buttons are STOP or not
							*/ 
						   
						   document.getElementById('btn_pan').innerHTML="PAN";
						   
						   document.getElementById('btn_tilt').innerHTML="TILT";      /*
																					   //Angle in radians
																					   console.log(Math.atan2(p2.y - p1.y, p2.x - p1.x));  
																					   */
						   /*  
							//Angle in degrees
							console.log(Math.atan2(p2.y - p1.y, p2.x - p1.x) * 180 / Math.PI);
							*/
						   /*
							if (x<touchstartX){console.log("moving left");} else {console.log("moving right");}
							if (y<touchstartY){console.log("moving up");} else {console.log("moving down");}          
							*/
						   
						   event.preventDefault();
						   }, false);


websock = new WebSocket('ws://' + window.location.hostname + ':8000/');
	//websock = new WebSocket('ws://192.168.1.150:8000/');
websock.onopen = function(evt) { console.log('websock open');  
	
	
};
websock.onclose = function(evt) { console.log('websock close'); 
	
	document.getElementById("refresh").classList.toggle("connected");
};
websock.onerror = function(evt) { console.log(evt); };
websock.onmessage = function(evt) { console.log(evt); console.log(evt.data);
	
	handle_presets(evt.data);
};
function doSend(message)
{
	/*console.log("sent: " + message + '\n');*/
	/* writeToScreen("sent: " + message + '\n'); */
	websock.send(message);
}
