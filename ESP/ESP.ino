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

static const char PROGMEM MANIFEST_JSON[] = R"rawliteral(
{
 "name": "myROVER",
  "short_name": "myrover",
  "description": "A simple rover controller app.",
  "display": "standalone",
  "scope": "/",
  "orientation":  "landscape"
}
)rawliteral";

static const char PROGMEM CHECK_SENSORS[] = R"rawliteral(<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
</head>
<body>


</body>
</html>
)rawliteral";
static const char PROGMEM INDEX_HTML[] = R"rawliteral(<!DOCTYPE html>
<!--
C-F Forward Motor A Returns C-F:OK
C-f forward Motor B
C-R Reverse A
C-r reverse B
C-X stop A
C-x stop B

A-A adjustPWM using A for Analog ADC battery voltage read. Returns H-255:OK, h-255:OK, L-255:OK, l-255:OK for PWMAMAX, PWMBMAX, PWMAMIN, PWMBMIN
A-D adjustPWM using D for Digital SMBUS battery voltage read. Returns H-255:OK, h-255:OK, L-255:OK, l-255:OK for PWMAMAX, PWMBMAX, PWMAMIN, PWMBMIN

S-255 sets PWMA to 255, returns S-255:OK
s-255 sets PWMB to 255, returns s-255:OK

b-V analog read battery voltage (mV) Returns v-13245:OK

B-V SMBUS battery voltage (mV) Returns V-13245:OK
B-I SMBUS Current (mA) Returns I--245:OK
B-C SMBUS Relative State of Charge (%) Returns c-45:OK
B-R SMBUS Remaining capacity (mAH) Returns R-3245:OK
B-T SMBUS Battery Temperature (deg C) Returns T-13.2:OK
B-F SMBUS Battery Full Capacity (mAH) Returns F-5245:OK
B-O SMBUS Battery LED OFF Returns O-OFF:OK
B-N SMBUS Battery LED ON Returns O-ON:OK

W-135 sets servo W to 135 degrees, returns W-135:OK
X-135 sets servo X to 135 degrees, returns X-135:OK
Y-135 sets servo Y to 135 degrees, returns Y-135:OK
Z-135 sets servo Z to 135 degrees, returns Z-135:OK

K-N LEDs turn on Returns K-N:OK
K-O LEDs turn off Returns K-O:OK

N-N Siren turn on Returns N-N:OK
N-O Siren turn off Returns N-O:OK






-->

<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="manifest" href="manifest.json">    
        <style>
        
        
  
/*        disable scrolling    */
            html{
margin:0;
padding:0;
overflow: hidden;
}
body{
position: absolute;
width:100%;
height:100%;
overflow: auto;
          margin:0px;
          background: black;
}
               
                
            
            * {
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                    }

               
/*
                input
                {
                    width:0px;
                    height:0px;
                    
                    
                }
                input:checked + .slider-knob
                {
                    background: green;
                      -webkit-transform: translateY(-26px);
                    -ms-transform: translateY(-26px);
                    transform: translateY(-26px);
                }
*/
            .sliderbuttons
            {
                position: absolute;
                bottom:0%;
                width:100%;
                height:20%;
            }
            .slidercolumn
            {
                width:49%;
                    height:100%;
                position: relative;
            }
            .fuelgaugecolumn
            {
                width:49%;
                height:100%;
                    
            }
                .individual-status-lights
                {
                    width:100%;
                    box-sizing: border-box;
                    height:33.3%;
                }
             /*     top 10%           */
                .notification-bar
                {
                    width:100%;
                    display:flex;
                    position: fixed;
                    height:10%;
                    background: white;
                }
            .selectables{
                width: 20%;
            }
            .console
            {
                width:20%;
                left:0px;
            }
            
            .numericals{
                border: solid;
                border-radius: 20%;
                width:10%;
            }
            /*                bottom 90%*/
                .half
                {
                    width:100%;
                    display:flex;
                    position: fixed;
                    height:45%;
                }
/*                position top half-10% away from top*/
                #top-half
                {
                    top:10%;
                }
/*                position bottom half -at bottom*/
                .bottom
                {
                    right:0;
                    bottom:0;
                }
/*                divide 5 even width columns*/
                .twenty
                {
                   width:20%;
                }
                .sixty
                {
                    width:60%;
                }
                .horizontalslider
                {
                    position: relative;
                    height:50%; 
                }
/*                if height is not sufficient - make room for steering*/
                @media only screen and (max-height: 444px) {
                .horizontalslider
                {
                    position: relative;
                    height:10%; 
                }
                    }
                
                .slider-knob
                {
                    height:33.3%;
                    top:33.3%;
                    position: relative;
                    background: green;
                    transition: .4s;
                    border-radius: 1%;
                }
                .gearbuttons
                {
                    width:100%;
                    height:33.3%;
                    border-radius:20%;
                }
                .vertical3slider
                {
                position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        height:100%;
        margin:0 auto; 
                display: flex;

                }
                .vertical2slider
                {
                position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        height:100%;
        margin:0 auto;  
                }
                .verticalslider
                {
                position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        height:100%;
        margin:0 auto;  
                    display: flex;
                }
            .steering-container 
      {
                position: relative;
        z-index: 1;
                bottom:-50px;
        
      }
            .steeringbuttonswrapper
            {
                    position: absolute;
                    top: 21%;
                    left: 40%;
            }
      /*    CSS for Outer ring of joystick*/  
      .steering-outer-circle 
      {
        position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        width: 300px;
        height:300px;
        border-radius: 300px;
        margin:0 auto;
      }
      /*CSS for inner ring of joystick*/        
      .steering-inner-circle 
      {
        position:absolute;
        border: 0px;
        top: 50px;
        left:50px;
        /*opacity: 0.2;*/
        border: solid blueviolet;
        width: 250px;
        height:250px;
        border-radius: 250px;
      }
            .steering-knob 
      {
        position:absolute;
        border: solid blueviolet;
        top: -50px;
        left: 100px;
        background: blueviolet;
        /*opacity: 0.2;*/
        border: solid blueviolet;
        width: 100px;
        height:100px;
        border-radius: 100px;
      }
                
                .three-slider
                {
                    width:49%;
                    height:100%;
\
                }
                .three-lights
                {
                    width:49%;
                    height:100%;
                    background: red;
                 
                }
      </style>
      
      <title>myRover</title>
      </head>
  <body>
    
      <!--<img id="mjpeg_dest"src="/cam_pic_new.php" style="width:100%;"> -->
      <!--<div class="joystick-container" id="joystick-container">
        <div class="outer" id="outer"><div class="inner" id="inner"></div></div>
      </div>-->
      
        <div class="notification-bar" id="notification">
            <div class = "console" id = "mylocalconsole"></div>
            <div class="selectables">
                <label class="genericlabel">SmartBattery</label>
            <input type="checkbox" id = "isitasmartbattery" onchange="getbatteryinfo">
            </div>
            <div class="smbusnumericals numericals" id = "smbusbatteryI"></div>
            <div class="smbusnumericals numericals" id = "smbusbatteryV"></div>
            <div class="smbusnumericals numericals" id = "smbusbatteryT"></div>
            <div class="smbusnumericals numericals" id = "smbusbatteryc"></div>
            <div class="smbusnumericals numericals" id = "smbusbatteryR"></div>
            <div class="smbusnumericals numericals" id = "smbusbatteryF"></div>
            <div class="numericals" id = "analogbatteryV"></div>
            
        </div>    
        <div class ="half" id="top-half">
                <div class="twenty vertical3slider" id = "top-left-1">                                       
                                        <div class="slidercolumn" id="slidercolumnB"><button class = "sliderbuttons" id="speedcontrolB">
                    S
                    </button></div><div class="fuelgaugecolumn"><div class="fuelgaugecolumnstatus" id = "motorBspeed"></div></div>
                                        
                
            
            </div>
                <div class="twenty vertical3slider" id = "top-left-2" id="gearB">
            
                            <div class = "three-slider" id="gearBslider">
<!--                    <label><input type = "checkbox" id = "forward-checkbox">-->
<!--                    <div class="slider-knob" id = "gearsliderknob"></div>-->
<!--
                    <input type="checkbox" id="stop-checkbox">
                    <input type="checkbox"id = "reverse-checkbox">
                    </label>
-->
                    
                    <button class = "gearbuttons" id="forwardgearbuttonB" onclick='doSend("<C-f>")'>
                    F
                    </button>
                    
                    <button class = "gearbuttons" id="stopgearbuttonB" onclick='doSend("<C-x>")'>
                    X
                    </button>
                    <button class = "gearbuttons" id="reversegearbuttonB" onclick='doSend("<C-r>")'>
                    R
                    </button>

                    </div>
                    
                    <div class="three-lights" id="gear-lights">
                        <div class="individual-status-lights" id = "lightforwardB">F</div>
                        <div class="individual-status-lights" id = "lightstopB">X</div>
                        <div class="individual-status-lights" id = "lightreverseB">R</div>
                    </div>
            
            
            
            </div>
                
            <div class="twenty vertical3slider" id = "top-left-3">
             <button class = "gearbuttons" id="speedcontrol" onclick='doConnect()'>
                    Connect
                    </button>
            <button class = "gearbuttons" id="speedcontrol" onclick='doClose()'>
                    Close
                    </button>
            </div>
                <div class="twenty vertical2slider" id = "top-left-4"></div>
                <div class="twenty vertical2slider" id = "top-left-5"></div>
            </div>
            <div class ="half bottom" id="bottom-half">

                <div class="sixty" id = "bottom-middle"><div class="horizontalslider"></div>
                    <div class="steering-container" id="steering-container">
                <div class="steering-outer-circle" id="steering-outer-circle">
<!--                            <div class="steering-inner-circle" id="steering-inner-circle"></div>-->
                            <div class ="steering-knob" id="steering-knob"></div>
                            <div class = "steeringbuttonswrapper">
                            <button class ="genericbuttons" id = "trimset">Trim</button>
                          <button class ="genericbuttons" id = "gotomiddle">Mid</button>    
                        </div>
                            </div>
               </div>
                </div>
                <div class="twenty vertical3slider" id = "gearA">
                <div class = "three-slider" id="gearAslider">
<!--                    <label><input type = "checkbox" id = "forward-checkbox">-->
<!--                    <div class="slider-knob" id = "gearsliderknob"></div>-->
<!--
                    <input type="checkbox" id="stop-checkbox">
                    <input type="checkbox"id = "reverse-checkbox">
                    </label>
-->
                    
                    <button class = "gearbuttons" id="forwardgearbuttonA" onclick='doSend("<C-F>")'>
                    F
                    </button>
                    
                    <button class = "gearbuttons" id="stopgearbuttonA" onclick='doSend("<C-X>")'>
                    X
                    </button>
                    <button class = "gearbuttons" id="reversegearbuttonA" onclick='doSend("<C-R>")'>
                    R
                    </button>

                    </div>
                    
                    <div class="three-lights" id="gear-lights">
                        <div class="individual-status-lights" id = "lightforwardA">F</div>
                        <div class="individual-status-lights" id = "lightstopA">X</div>
                        <div class="individual-status-lights" id = "lightreverseA">R</div>
                    </div>
                </div>
                <div class="twenty verticalslider" id = "bottom-left">
                    <div class="slidercolumn" id="slidercolumnA"><button class = "sliderbuttons" id="speedcontrolA">
                    S
                    </button></div><div class="fuelgaugecolumn"><div class="fuelgaugecolumnstatus" id = "motorAspeed"></div></div>
                                        
                
                </div>
                
            </div>

    
    
    
    
    
  </body>
  
  <script>
        
 /*       For eventhandler passive support option, see here: https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
        */
        
        
        let passiveSupported = false;

        try {
    const options = {
    get passive() { // This function will be called when the browser
                    //   attempts to access the passive property.
      passiveSupported = true;
      return false;
    }
  };

  window.addEventListener("test", null, options);
  window.removeEventListener("test", null, options);
} catch(err) {
  passiveSupported = false;
}

        
        
        var PWMAMAX;
        var PWMBMAX;
        var PWMAMIN;
        var PWMBMIN;
        var PWMA;
        var PWMB;
        var PWMASTEPS = 5;
        var PWMBSTEPS = 5;
        var previousY =0;
        var smartbattery=false;
        var q = setInterval(getbatteryinfo, 10000); 


function getbatteryinfo()
        {
            smartbattery=document.getElementById("isitasmartbattery").checked;
            if(smartbattery){
                doSend("<B-V>");
                doSend("<B-I>");
                doSend("<B-T>");
                doSend("<B-C>");
                doSend("<B-R>");
                doSend("<B-F>");
                
            }
            else {
                doSend("<b-V>");
            }
        }
function parse_incoming_websocket_messages(data){
    //Data is S-255:OK or S-255:FAIL or C-F:OK or C-F:FAIL
    //Determine if it is OK response or FAIL response, splice it on ":", log it if it is FAIL, proceed if it is OK
    var myResponse=data.split(":")[1];
    var returningdata=data.split(":")[0];
    switch(myResponse){
            case ("FAIL"):
            document.getElementById("mylocalconsole").innerHTML=data;
            break;
            case("OK"):
            var returningdata=data.split(":")[0];
            //Now find the first character and do something
            response_based_on_first_char(returningdata);
            default:
            document.getElementById("mylocalconsole").innerHTML=data;
            break;
            
    }
    
}

function response_based_on_first_char(mylocalvar){
    //mylocalvar = S-255 or C-F or H-135 etc , see above
    switch(mylocalvar.split("-")[0])
    {
        //PWMAMAX
        case ("H"):
            PWMAMAX=Number(mylocalvar.split("-")[1]);
            break;
        //PWMBMAX
        case ("h"):
            PWMBMAX=Number(mylocalvar.split("-")[1]);
            break;
        //PWMAMIN    
        case ("L"):
            PWMAMIN=Number(mylocalvar.split("-")[1]);
            break;
        //PWMBMIN    
        case ("l"):
            PWMBMIN=Number(mylocalvar.split("-")[1]);
            break;
        //PWMA    
        case ("S"):
            PWMA=Number(mylocalvar.split("-")[1]);
            break;
        //PWMB    
        case ("s"):
            PWMB=Number(mylocalvar.split("-")[1]);
            break;
        //Commands    
        case ("C"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Servo W angle    
        case ("W"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Servo X angle
        case ("X"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Servo Y angle
        case ("Y"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Servo Z angle    
        case ("Z"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Battery Smbus Voltage    
        case ("V"):
            document.getElementById("smbusbatteryV").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery ADC analog Voltage    
        case ("v"):
            document.getElementById("analogbatteryV").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery smbus current
        case ("I"):
            document.getElementById("smbusbatteryI").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery smbus remaining capacity (mAH)
        case ("R"):
            document.getElementById("smbusbatteryR").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery smbus remaining charge (%)    
        case ("c"):
            document.getElementById("smbusbatteryc").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery smbus temperature (degC)    
        case ("T"):
            document.getElementById("smbusbatteryT").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery smbus full charge capacity (mAH)    
        case ("F"):
            document.getElementById("smbusbatteryF").innerHTML=mylocalvar.split("-")[1];
            break;
        //Battery SMBUS LED on or off    
        case ("O"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Truck light Leds
        case ("K"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
        //Truck siren
        case ("N"):
            document.getElementById("mylocalconsole").innerHTML=mylocalvar;
            break;
            
        default:
        document.getElementById("mylocalconsole").innerHTML=mylocalvar;
        break;
    }
}

var steeringanlgesteps =30; 
var previoussentangle=0;
function handleOrientation(event) {
  var y = event.gamma;
    var z = event.alpha;
     var x = event.beta;
    //console.log("alpha: " + z + "\n");
     //document.getElementById("mylocalconsole").innerHTML = "alpha: " + z + "\n";
    //Device has moved more than the step
    if(Math.abs(y-previousY)>steeringanlgesteps){
       
    document.getElementById("mylocalconsole").innerHTML = "<W-"+(y+90)+">";  
    //previousY=y;
    }
    
    
}

/*    with x1 and y1 being the coordinates of the center of a circle, (x-x1)^2+(y-y1)^2 = radius^2
    so for any given value of x, y = sqrt(radius^2-(x-x1)^2)+y1*/
        

            
    var draggable = document.getElementById('steering-knob');
  var outer = document.getElementById('steering-outer-circle');
  var touchstartX;
    var touchstartY;
    //get center coordinates of the steering-outer-circle
    var centerofsteeringcircleX;
    var centerofsteeringcircleY;
    var rect1 = outer.getBoundingClientRect();
    var rect;
    var myX;
    var myY;
    var myoffsetfromcontainerX;
    var myoffsetfromcontainerY;
    var mymaxY;
    var myangle;
    centerofsteeringcircleX = rect1.left+((rect1.right -rect1.left)/2);
    centerofsteeringcircleY= rect1.top+((rect1.bottom -rect1.top)/2);
        
            draggable.addEventListener('touchstart', function(event) {
                 var touch = event.targetTouches[0];

                 rect = draggable.getBoundingClientRect();
                 //Start point -center of steering knob
                 touchstartX=rect.left+((rect.width)/2);
                 touchstartY=rect.top+((rect.height)/2);
                 rect1=outer.getBoundingClientRect();
                                //don't want the steering knob to go below horizon. Find the lowermost allowable steeringknob center Y pixel. it should be steering-knob's radius away from the bottom of the steering container
                                mymaxY=window.innerHeight-(rect.height)/2;
//                                myoffsetfromcontainerX=rect.left-rect1.left;
//                                myoffsetfromcontainerY=rect.top-rect1.top;
                myoffsetfromcontainerX = draggable.offsetLeft;
                myoffsetfromcontainerY=draggable.offsetTop;
                

//                 console.log("touchstartx="+touchstartX+", touchstartY="+touchstartY+", myoffsetX="+myoffsetfromcontainerX+", myoffsetY="+myoffsetfromcontainerY+", mymaxY="+mymaxY);
                 }, passiveSupported
                               ? { passive: true } : false);

                 draggable.addEventListener('touchmove', function(event) {
                              var touch = event.targetTouches[0];
                                                            //get the x point of touch;
                                                            myX = touch.pageX;
                                                            myY=centerofsteeringcircleY-Math.sqrt((150*150)-(Math.pow((myX-centerofsteeringcircleX),2)));
                                                            
                                                            
                                   
                                        
                                   //check if falling below horizon
                                   if (myY<mymaxY)
                                                                {
                                             draggable.style.left = myoffsetfromcontainerX+(myX-touchstartX)+'px';
                            draggable.style.top = myoffsetfromcontainerY+(myY-touchstartY)+'px';
                                                                                //calculate angle off center at current position
                                   myangle=(135-Math.abs(parseInt(Math.atan2(myY - centerofsteeringcircleY, myX - centerofsteeringcircleX) * 180 / Math.PI)))*2;
                       
                                                             
                                                                }

                                    
                                   if(Math.abs(myangle-previoussentangle)>steeringanlgesteps){
                                       doSend("<W-"+myangle+">"); 
                                       previoussentangle=myangle;
                                       
                                   }
/*                                                            console.log("myX="+myX);
                                                            console.log("myY="+myY);
                                                            console.log("touchstartX="+touchstartX);
                                                            console.log("touchstartY="+touchstartY);
                                                            console.log("knob left="+(rect.left+(myX-touchstartX)));
                                                            console.log("knob top="+(rect.top+(myY-touchstartY)));
                                                            console.log(myangle);

*/

                              event.preventDefault();
                              }, passiveSupported
                               ? { passive: true } : false);
                
            draggable.addEventListener('touchend', function(event) {
                        //console.log("touch ended");
                        },passiveSupported
                               ? { passive: true } : false);          

/*
works on chrome 52 on android (GalaxyS5). I think beyond chrome 60 - needs window.isSecureContext to be true - for access to sensors
secure context means everything has to be ssl. Doesn't work even if I save the file locally on the phone and just connect to websocket. On the other hand, saving it locally on the computer and using it - becomes a secure context and sensors can be accessed. Yes, macbook has orientation sensors too.
*/
        
window.addEventListener('deviceorientation', handleOrientation);

var mysliderA = document.getElementById('speedcontrolA');
var mysliderB = document.getElementById('speedcontrolB');

//sliderbasefunction returns at what percent of total slidable height -is the slider at
function sliderbasefunction(event,whocalledme,whichelement){
var mytotalheight;
var myminY;
var mymaxY;
var mytouchY;  
var myouterboundrect=document.getElementById('slidercolumn'+whichelement).getBoundingClientRect();
//var myslider = document.getElementById('speedcontrol'+whichelement);
var mysliderrect=whocalledme.getBoundingClientRect();
mytotalheight= myouterboundrect.height;
myminY = myouterboundrect.top+(mysliderrect.height/2);
mymaxY = myminY+(mytotalheight-mysliderrect.height);
var myslidertouch = event.targetTouches[0];
                 mytouchY = myslidertouch.pageY;
            if (mytouchY >= myminY && mytouchY <= mymaxY)
                {

                //move whocalledme to mytouchY
                
                var mypercentageslide = ((mymaxY-mytouchY)/(mymaxY-myminY))*100;
                var mystylefromtop;
                if ((80-mypercentageslide)>0){mystylefromtop = 80-mypercentageslide;} else {mystylefromtop = 0;}
                whocalledme.style.top=mystylefromtop+'%';
                                     //console.log("mytouchY="+mytouchY+" myminY="+myminY+" mymaxy="+mymaxY+" mypercentageslide ="+mypercentageslide+" style from top % ="+mystylefromtop);
                
                
                return Math.round(mypercentageslide);
                }
    else {
        //sliding out of bounds
        return -1;
        
    }
    
}


/*mysliderA.addEventListener('touchstart',function(event){

        console.log("Touch start");
        },passiveSupported
                               ? { passive: true } : false);*/
        
var mypreviousPWMAsent = 0;
var mypreviousPWMBsent = 0;
var valuetosendA =0;
var valuetosendB =0;
mysliderA.addEventListener('touchmove',function(event){
   var whatpercentslide = sliderbasefunction(event,this,"A");
    if ((whatpercentslide>0) && iswebsocketconnected){
     valuetosendA= Math.round(PWMAMIN+((PWMAMAX-PWMAMIN)*(whatpercentslide/100)));
     //console.log("whatpercentslide = "+whatpercentslide+ "valuetosend ="+valuetosend+"PWMAMAX = "+PWMAMAX+ "PWMAMIN = "+PWMAMIN);
   if (Math.abs(valuetosendA-mypreviousPWMAsent)>Number(PWMASTEPS)){
    
        
    doSend("<S-"+valuetosendA+">");
    //console.log("<S-"+valuetosend+">");
    mypreviousPWMAsent=valuetosendA;
    }
        
    }
},passiveSupported
                               ? { passive: true } : false);

    
        
mysliderB.addEventListener('touchmove',function(event){
   var whatpercentslide = sliderbasefunction(event,this,"B");
    if ((whatpercentslide>0) && iswebsocketconnected){
     valuetosendB= Math.round(PWMBMIN+((PWMBMAX-PWMBMIN)*(whatpercentslide/100)));
     //console.log("whatpercentslide = "+whatpercentslide+ "valuetosend ="+valuetosend+"PWMAMAX = "+PWMAMAX+ "PWMAMIN = "+PWMAMIN);
   if (Math.abs(Number(valuetosendB)-Number(mypreviousPWMBsent))>Number(PWMBSTEPS)){
    
        
    doSend("<s-"+valuetosendB+">");
    //console.log("<S-"+valuetosend+">");
    mypreviousPWMBsent=valuetosendB;
    }
        
    }
},passiveSupported
                               ? { passive: true } : false);     
        
        
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
  /*
    //if holding the steering knob for longer than 750 ms, send the current angle. 
  onlongtouch=doSend("<W-"+myangle+">");*/
        

    
var websock;
var iswebsocketconnected=false;
function doConnect()
  {
       // websock = new WebSocket('ws://' + window.location.hostname + ':8000/');
        websock = new WebSocket('ws://192.168.4.1:8000/');
        websock.onopen = function(evt) 
            { 
                console.log('websock open'); 
                iswebsocketconnected=true;
                if(smartbattery)
                    {
                        doSend("<A-D>"); //Send adjust PWM Digital on connecting }
                    } 
                else 
                    {
                        doSend("<A-A>"); //Send adjust PWM  on connecting }
                                  
                    }
            }
                                               
                                        
        websock.onclose = function(evt) 
            { 
            iswebsocketconnected=false;
            console.log('websock close'); 
    
            }
        websock.onerror = function(evt) 
            {
            console.log(evt); 
            }

        websock.onmessage = function(evt) 
            { 
            console.log(evt); 
            //console.log(evt.data);
            parse_incoming_websocket_messages(evt.data);
            }
  }

function doClose()
{
    websock.close();
}
function doSend(message)
{


    if(iswebsocketconnected==true)
    {
        if(websock.readyState==websock.OPEN){
        websock.send(message);
    
        
        }
        else {
            
            console.log("websocket is in an indeterminate state");
        }
    }
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
   server.sendHeader("Cache-Control","max-age=604800");
   server.send(200, "text/html", INDEX_HTML);
   
}
void handleIndex()
{
  //server.send_P(200, "text/html", INDEX_HTML);
   server.sendHeader("Cache-Control","max-age=604800");
   server.send(200, "text/html", INDEX_HTML);
   
}


void handleManifest()
{
   server.sendHeader("Cache-Control","max-age=604800");
   server.send(200, "application/json", MANIFEST_JSON);
   
}
void handleCheck()
{
   server.sendHeader("Cache-Control","max-age=604800");
   server.send(200, "text/html", CHECK_SENSORS);
   
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
  server.on("/manifest.json",handleManifest);
  server.on("/check.html",handleCheck);
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
