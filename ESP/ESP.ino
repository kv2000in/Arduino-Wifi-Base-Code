#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <string.h>
#include <ArduinoOTA.h>

#ifndef APSSID
#define APSSID "myRover"
#define APPSK  "revoRym123"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(8000);



static const char PROGMEM INDEX_HTML[] = R"rawliteral(<!DOCTYPE html>


<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
        
        body{
          margin:0px;
                    background: black;
        } 
      /*CSS for joystick*/        
      .joystick-container 
      {
        display: block;
        position: absolute;
        right: 10%;
        bottom: 5%;
        z-index: 1;
        
      }
      /*    CSS for Outer ring of joystick*/  
      .outer 
      {
        position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        width: 250px;
        height:250px;
        border-radius: 250px;
        margin:0 auto;
      }
      /*CSS for inner ring of joystick*/        
      .inner 
      {
        position:absolute;
        border: 0px;
        top: 75px;
        left: 75px;
        background: blueviolet;
        /*opacity: 0.2;*/
        border: solid blueviolet;
        width: 100px;
        height:100px;
        border-radius: 100px;
      }
      /*     CSS for refresh button when websocket closes*/
      .refresh
      {
        display: none;
      }
      .connected 
      {
        display: block;
        font-size: 40px;
        position: fixed;
        left:40%;
        top:45%;
        z-index: 1;
        color:red;
        
      } 
      
      /*    CSS for the MJPEG img display*/    
      img
      {
        width: 100%;
        display:block;   
        
      }    
      /*       CSS for X out button*/
      .xout
      {
        position: fixed;
        top: 0%;
        right: 0%;
        font-size: 32px;
        color: blueviolet;
        z-index: 1;
        border: 2px;
        padding-left: 10px;
        padding-right: 10px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: blueviolet;
        margin: 2px;
      }
      
      /*CSS for preset menu*/
      .preset-menu
      {
        display: none;
        position: fixed;
        width: 50%;
        min-width: 360px;
        z-index: 1;
        left: 45%;
        top: 10%;
      }
      .preset-menu-visible
      {
        display: block; 
        
      }
      /*   CSS for empty presets*/
      .preset-menu input
      {
        margin: 10px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: blueviolet;
        height: 30px;
        background: transparent;
        width: 40%;
        font-size: 24px;
        padding: 1px;
      }   
      /* CSS for presets with values*/
      .preset-menu [type="button"] 
      {
        margin: 15px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: #4CAF50;
        height: 36px;
        width: 40%;
        font-size: 24px;
        padding: 0px;
        background: slateblue;
        opacity: 0.7;
      } 
      
      
      /*Generic CSS for all the buttons (pan , tilt, settings, presets)*/
      .button
      {
        background-color: transparent;
        font-size: 25px;
        color: white;
        padding-left: 4px;
        padding-right: 4px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: blueviolet;
        margin: 2px;
      }
      /*CSS for the outer div wrapping the menus*/
      .dropdown-wrap
      {
        position: absolute;
        left: 5%;
        bottom: 25%; 
        display: inline-block;
        
      }
      .menu-wrap
      {
        display: block;
        position: absolute;
        min-width: 360px;
        z-index: 1;
      } 
      /*CSS for the "+" sign*/        
      .menu-button1
      {
        background-color: transparent;
        font-size: 25px;
        color: white;
        padding-left: 10px;
        padding-right: 10px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: blueviolet;
        margin: 2px;
      }
      /*CSS for the "-" sign*/
      .menu-button2
      {
        background-color: transparent;
        font-size: 25px;
        color: white;
        padding-left: 10px;
        padding-right: 10px;
        border-radius: 10px;
        border-style: solid;
        border-width: 4px;
        border-color: blueviolet;
        margin: 2px;
        /*display: none;*/
        display: block;
      }
      /*Hovering over dropdown div - show the menu-wrap div*/
      .dropdown-wrap:hover .menu-wrap
      {
        /*display: block;*/
      }
      /*Hovering over dropdown div - Hide the + button*/
      .dropdown-wrap:hover .menu-button1
      {
        /*display: none;*/
      }
      /*Hovering over dropdown div - Show the - button in place of + button*/
      .dropdown-wrap:hover .menu-button2
      {
        /* display: block;*/
        
      }
      /*Hovering over dropdown div - Hide the Joystick*/
      .dropdown-wrap:hover ~ #joystick-container 
      {
        /*display: none;*/
      }
      
      /*Generic class to hide visibility by toggling from javascript*/
      .hide
      {
        display: none;    
      }
      </style>
      
      <title>Camera</title>
      </head>
  <body>
    
<!--    <div class="refresh" id="refresh" onclick='location.reload()'>Refresh</div>-->
    <div class="xout" onclick='window.close()'>X</div>
    <div class="container-fluid text-center liveimage">
      <!--<div style="position:relative;"> -->
        
        <!--             onmouseover= 'toggle_menu_btn()' onmouseout='toggle_menu_btn()'-->
        <div class ="dropdown-wrap" id="dropdown-wrap"><button class ="menu-button1" id="btn_menu1" onclick='toggle_menu()' >+</button><button class ="menu-button2 hide" id="btn_menu2" onclick='toggle_menu()' >-</button>
          <div class = "menu-wrap hide" id = "menu-wrap">
            <button class ="button"  id="btn_pan" onclick = 'toggle_pan()'>PAN</button><button id="btn_tilt" class ="button" onclick = 'toggle_tilt()'>TILT</button><button class ="button" onclick= 'toggle_menu_btn()' >Presets</button> <br><button class ="button" onclick='window.location="/webcam.php"'>Settings</button>            <div id = "menu_preset"class="preset-menu">
              
              <input type="text" id = preset1 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                <input type="text" id = preset2 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                  <input type="text" id = preset3 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                    <input type="text" id = preset4 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                      <input type="text" id = preset5 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                        <input type="text" id = preset6 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                          <input type="text" id = preset7 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                            <input type="text" id = preset8 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                              <input type="text" id = preset9 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                                <input type="text" id = preset10 onclick="preset(this.id,this.value)" ontouchstart='touchstart(this.id,this.value)' ontouchend='touchend()'>
                                  </div> 
          </div>        
        </div>
        <!--<img id="mjpeg_dest"src="/cam_pic_new.php" style="width:100%;"> -->
          <div class="joystick-container" id="joystick-container">
            <div class="outer" id="outer"><div class="inner" id="inner"></div></div>
          </div>
          
          
          </div>
      
    
    
    
    
  </body>
  
  <script>
    
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
//                              location.reload();
                              /*        alert("reloading"+counter);
                               counter++;*/
                            } else {
                              
//                              websock.close();    
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
  </script>
</html>


)rawliteral";




void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  Serial1.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial1.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial1.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      //itoa( cm, str, 10 );
      //  webSocket.sendTXT(num, str, strlen(str));
      }
      break;
    case WStype_TEXT:
    {
      Serial1.printf("[%u] get Text: %s\r\n", num, payload);
      //Send whatever comes on the WS to Atmega.
      char *mystring = (char *)payload;
      Serial.print(mystring);
 
    }
     break;
    case WStype_BIN:
      Serial1.printf("[%u] get binary length: %u\r\n", num, length);
      break;
    default:
      Serial1.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

void handleRoot()
{
   server.send(200, "text/html", "try index.html");
   
}
void handleIndex()
{
  //server.send_P(200, "text/html", INDEX_HTML);
   server.sendHeader("Cache-Control","max-age=604800");
   server.send(200, "text/html", INDEX_HTML);
   
}

void handleNotFound()
{
String message = "File Not Found\n\n";
message += "URI: ";
message += server.uri();
message += "\nMethod: ";
message += (server.method() == HTTP_GET)?"GET":"POST";
message += "\nArguments: ";
message += server.args();
message += "\n";
for (uint8_t i=0; i<server.args(); i++){
message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
}
server.send(404, "text/plain", message);
}

/*

//String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)
//String getContentType(String filename) { // convert the file extension to the MIME type
//  if (filename.endsWith(".html")) return "text/html";
//  else if (filename.endsWith(".css")) return "text/css";
//  else if (filename.endsWith(".js")) return "application/javascript";
//  else if (filename.endsWith(".ico")) return "image/x-icon";
//  return "text/plain";
//}


bool handleFileRead(String path) { // send the right file to the client (if it exists)
  //Serial1.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  //String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    //size_t sent = server.streamFile(file, contentType); // And send it to the client
    size_t sent = server.streamFile(file, "text/plain"); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  //Serial1.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

*/
/*******************Serial Read Functions ************************/
//Serial Read stuff

const byte numChars = 32;
char receivedChars[numChars];



void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 ) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                webSocket.sendTXT(0,receivedChars,ndx);
                Serial1.println(receivedChars);
                Serial.println(receivedChars);
                ndx = 0;
                
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}




/*******************Serial Read Functions ************************/

void setup()
{ 


  Serial1.begin(115200);
  delay(10);
  Serial.begin(57600);
  //Serial1.setDebugOutput(true);

  Serial1.println();
  Serial1.println();
  Serial1.println();

  for(uint8_t t = 4; t > 0; t--) {
    Serial1.printf("[SETUP] BOOT WAIT %d...\r\n", t);
    Serial1.flush();
    delay(1000);
   //Serial.print("<f-10000>");
  }
/***************** AP mode*******************/
  Serial1.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  WiFi.printDiag(Serial1);
  IPAddress myIP = WiFi.softAPIP();
  Serial1.print("AP IP address: ");
  Serial1.println(myIP);
  
/***********************************************/

/*****************Client Mode******************/
 /*
  WiFiMulti.addAP(ssid, password);

  while(WiFiMulti.run() != WL_CONNECTED) {
    Serial1.print(".");
    delay(100);
  }

  Serial1.println("");
  Serial1.print("Connected to ");
  Serial1.println(ssid);
  Serial1.print("IP address: ");
  Serial1.println(WiFi.localIP());
  */
/**********************************************/
  
  server.on("/", handleRoot);
  server.on("/index.html",handleIndex);
  server.onNotFound(handleNotFound);
//  server.onNotFound([]() {                              // If the client requests any URI
//    if (!handleFileRead(server.uri()))                  // send it if it exists
//      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
//  });

  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


/* ************SPIFFS********************* */
//  if (SPIFFS.begin()){Serial1.println("file system mounted");};
//
//  //Open the "Save.txt" file and check if we were saving before the reset happened
//  File q = SPIFFS.open("/Save.txt", "r");
//  if (q.find("Y")){saveData=true;}
//  q.close();

 /*********************************************/
 
 /* ************OTA********************* */
  
    // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
   
   ArduinoOTA.onStart([]() {
    Serial1.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial1.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial1.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial1.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial1.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial1.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial1.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial1.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial1.println("End Failed");
  });
  ArduinoOTA.begin();

/****************************************************/  
 
  
  }



void loop()
{
  webSocket.loop();
  server.handleClient();
  ArduinoOTA.handle();
  recvWithStartEndMarkers();
}
