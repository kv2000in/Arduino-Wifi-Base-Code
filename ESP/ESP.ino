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
      <style>
        
        body{
          margin:0px;
          background: black;
        } 
                * {
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                    }

                /*     top 10%           */
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
                .individual-status-lights
                {
                    width:100%;
                    box-sizing: border-box;
                    height:33.3%;
                }
                .notification-bar
                {
                    width:100%;
                    display:flex;
                    position: fixed;
                    height:10%;
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
                }
            .steering-container 
      {
                position: relative;
        z-index: 1;
                bottom:-25px;
        
      }
      /*    CSS for Outer ring of joystick*/  
      .steering-outer-circle 
      {
        position:relative;
        top: 0px;
        left: 0px;
        border: 2px solid blueviolet;
        align-self: center;
        width: 350px;
        height:350px;
        border-radius: 350px;
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
        top: -25px;
        left: 125px;
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
      
      <title>Camera</title>
      </head>
  <body>
    
      <!--<img id="mjpeg_dest"src="/cam_pic_new.php" style="width:100%;"> -->
      <!--<div class="joystick-container" id="joystick-container">
        <div class="outer" id="outer"><div class="inner" id="inner"></div></div>
      </div>-->
      
        <div class="notification-bar" id="notification">
            <div class="numericals" id = "smbusbatteryI"></div>
            <div class="numericals" id = "smbusbatteryV"></div>
            <div class = "console" id = "mylocalconsole"></div>
            <div class="numericals" id = "smbusbatteryT"></div>
            <div class="numericals" id = "smbusbatteryc"></div>
            <div class="numericals" id = "smbusbatteryR"></div>
            <div class="numericals" id = "smbusbatteryF"></div>
            <div class="numericals" id = "analogbatteryV"></div>
        </div>    
        <div class ="half" id="top-half">
                <div class="twenty vertical3slider" id = "top-left-1"></div>
                <div class="twenty vertical3slider" id = "top-left-2"></div>
                <div class="twenty vertical3slider" id = "top-left-3"></div>
                <div class="twenty vertical2slider" id = "top-left-4"></div>
                <div class="twenty vertical2slider" id = "top-left-5"></div>
            </div>
            <div class ="half bottom" id="bottom-half">

                <div class="sixty" id = "bottom-middle"><div class="horizontalslider"></div>
                    <div class="steering-container" id="steering-container">
                <div class="steering-outer-circle" id="steering-outer-circle"><div class="steering-inner-circle" id="steering-inner-circle"></div><div class ="steering-knob" id="steering-knob"></div></div>
               </div>
                </div>
                <div class="twenty vertical3slider" id = "gear">
                <div class = "three-slider" id="gear">
<!--                    <label><input type = "checkbox" id = "forward-checkbox">-->
<!--                    <div class="slider-knob" id = "gearsliderknob"></div>-->
<!--
                    <input type="checkbox" id="stop-checkbox">
                    <input type="checkbox"id = "reverse-checkbox">
                    </label>
-->
                    
                    <button class = "gearbuttons" id="forwardgearbutton" onclick='doSend("<C-F>")'>
                    F
                    </button>
                    
                    <button class = "gearbuttons" id="stopgearbutton" onclick='doSend("<C-X>")'>
                    X
                    </button>
                    <button class = "gearbuttons" id="reversegearbutton" onclick='doSend("<C-R>")'>
                    R
                    </button>

                    </div>
                    
                    <div class="three-lights" id="gear-lights">
                        <div class="individual-status-lights" id = "forward">F</div>
                        <div class="individual-status-lights" id = "stop">X</div>
                        <div class="individual-status-lights" id = "reverse">R</div>
                    </div>
                </div>
                <div class="twenty verticalslider" id = "bottom-left">
                                        <button class = "gearbuttons" id="speedcontrol" onclick='doSend("<S-"+PWMAMIN+">")'>
                    S
                    </button>
                
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
    var PWMAMAX;
        var PWMBMAX;
        var PWMAMIN;
        var PWMBMIN;
        var PWMA;
        var PWMB;

      
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
            PWMAMAX=mylocalvar.split("-")[1];
            break;
        //PWMBMAX
        case ("h"):
            PWMBMAX=mylocalvar.split("-")[1];
            break;
        //PWMAMIN    
        case ("L"):
            PWMAMIN=mylocalvar.split("-")[1];
            break;
        //PWMBMIN    
        case ("l"):
            PWMBMIN=mylocalvar.split("-")[1];
            break;
        //PWMA    
        case ("S"):
            PWMA=mylocalvar.split("-")[1];
            break;
        //PWMB    
        case ("s"):
            PWMB=mylocalvar.split("-")[1];
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
    }
    

  
websock = new WebSocket('ws://' + window.location.hostname + ':8000/');
//websock = new WebSocket('ws://192.168.1.150:8000/');
websock.onopen = function(evt) 
{ 
    console.log('websock open');  
    doSend("<A-A>"); //Send adjust PWM Digital on connecting                               
 }
                                               
                                        
websock.onclose = function(evt) 
{ 
    console.log('websock close'); 
    
}
websock.onerror = function(evt) 
{
    console.log(evt); 
}

websock.onmessage = function(evt) 
{ 
    console.log(evt); 
    console.log(evt.data);
    parse_incoming_websocket_messages(evt.data);
}
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
