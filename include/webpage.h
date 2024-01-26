#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>

<head>

    <style>
        .menu {
            position: absolute;
            display: inline-block;
            right: 20px;
            top: 15px;
            text-decoration: none;
        }

        html {
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
        }

        h1 {
            font-size: 1.5rem;
            color: white;
        }

        h2 {
            font-size: 1.5rem;
            font-weight: bold;
            color: #143642;
        }

        h3 {
            font-size: 1rem;
            font-weight: bold;
            color: #999999;
        }

        .topnav {
            overflow: hidden;
            background: linear-gradient(270deg, #246655, #4ea2cd, #cd954e);
            background-size: 600% 600%;

            animation: gradientAni 7s ease infinite;
        }

        @keyframes gradientAni {
            0% {
                background-position: 0% 50%
            }

            50% {
                background-position: 100% 50%
            }

            100% {
                background-position: 0% 50%
            }
        }

        body {
            margin: 0;
        }

        .content {
            padding: 30px;
            max-width: 600px;
            margin: 0 auto;
        }

        .card {
            background: linear-gradient(45deg, #d6fff5, #a4e1ff, #ffecd3);
            box-shadow: 5px 5px 20px 20px rgba(255, 255, 255, 0.5);
            padding-top: 10px;
            padding-bottom: 10px;
            padding-left: 10px;
        }

        .card2 {
            position: relative;
            text-align: center;
            width: 3em;
            height: 3em;
            border: 2px solid #000;
            border-radius: 2.5em;
            margin-left: 30px;
            margin-top: 10px;
        }

        .card2:after {
            content: '';
            position: absolute;
            top: .3em;
            left: 1em;
            width: 60%;
            height: 40%;
            border-radius: 60%;
            background-image: linear-gradient(rgba(255, 255, 255, 0.4), rgba(255, 255, 255, 0.1));
            background-position: 50% 100%;
        }

        #innerdiv {
            display: flex;
            flex-direction: row;
            flex-flow: wrap;
            justify-content: center;
            align-items: center;
        }

        .slider {
            -webkit-appearance: none;
            width: 90%;
            height: 15px;
            border-radius: 5px;
            background: #d3d3d3;
            outline: none;
            opacity: 0.7;
            -webkit-transition: .2s;
            transition: opacity .2s;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 25px;
            height: 50px;
            border-radius: 50%;
            background: #0f8b8d;
            cursor: pointer;
        }

        .slider::-moz-range-thumb {
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: #04AA6D;
            cursor: pointer;
        }

        .button {
            padding: 15px 50px;
            font-size: 24px;
            text-align: center;
            outline: none;
            color: #fff;
            background-color: #0f8b8d;
            border: none;
            border-radius: 5px;
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -khtml-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }

        /*.button:hover {background-color: #0f8b8d}*/
        .button:active {
            background-color: #0f8b8d;
            box-shadow: 2 2px #CDCDCD;
            transform: translateY(2px);
        }

        .state {
            font-size: 1.5rem;
            color: #8c8c8c;
            font-weight: bold;
        }

        #led {
            position: relative;
            width: 3em;
            height: 3em;
            border: 2px solid #000;
            border-radius: 2.5em;
            background-image: radial-gradient(farthest-corner at 50% 20%, #b30000 0%, #330000 100%);
            box-shadow: 0 0.5em 1em rgba(102, 0, 0, 0.3);
        }

        #led.ON {
            background-image: radial-gradient(farthest-corner at 50% 75%, red 0%, #990000 100%);
            box-shadow: 0 1em 1.5em rgba(255, 0, 0, 0.5);
        }

        #led:after {
            content: '';
            position: absolute;
            top: .3em;
            left: 1em;
            width: 60%;
            height: 40%;
            border-radius: 60%;
            background-image: linear-gradient(rgba(255, 255, 255, 0.4), rgba(255, 255, 255, 0.1));
            background-position: 50% 100%;
        }
    </style>

    <title>Iot Project</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta charset="utf-8">
    <link rel="shortcut icon" type="image/x-icon" href=%FAVICON%>
</head>

<body style="background-color: #143642;">
    <div class="menu">%AUTOCONNECT_MENU%</div>
    <div class="topnav">
        <h1>IoT Project</h1>
    </div>
    <div class="content">
        <div class="card">
            <h2>RGB LED Ring</h2>
            <p class="state">Number of Clients Connected: <span id="numFriends">%NUMFRIENDS%</span></p>
            <h3>Toggle through LED colors by clicking on them.</h3>
            <h3>Default color is Red and colors reset on client disconnect.</h3>
            <div id="innerdiv">
            </div>
            <p><button id="button2" class="button" onclick="sendColors()">Send Pattern</button></p>
            <h3>Once you are happy with your pattern click the button above.</h3>
            <h3>All other clients will receive pattern and the physical LEDs will be updated.</h3>
        </div>
        <br>
        <br>
        <br>
        <div class="card">
            <h2>Servo Control</h2>
            <h3>Servo range of motion is 0 to 180 degrees.</h3>
            <p class="state">Servo Angle in Degrees: <span id="servoPos">%SERVOPOS%</span></p>
            <p><input type="range" min="0" max="180" step="1" class="slider" id="slider" value="0"></p>
        </div>
        <br>
        <br>
        <br>
        <div class="card">
            <h2>Red Led</h2>
            <h3>Control for low-tech ON/OFF Red LED.</h3>
            <h3>Included for legacy and sentimental reasons.</h3>
            <div id="led" class="%STATE%"></div>
            <p class="state">State: <span id="state">%STATE%</span></p>
            <p><button id="button" class="button">Toggle</button></p>
        </div>
    </div>

    <script>
        var ws_Gateway = ("ws://" + location.hostname + ":3000/redled&servo");
        var Socket1;
        var ws2_Gateway = ("ws://" + location.hostname + ":3001/rgbleds");
        var Socket2;
        var numClients;
        window.addEventListener('load', onLoad);

        function initSocket1() {
            console.log('Trying to open a WebSocket connection for Red Led and Servo...');
            Socket1 = new WebSocket(ws_Gateway);
            Socket1.onopen = onOpen_Socket1;
            Socket1.onclose = onClose_Socket1;
            Socket1.onmessage = onMessage_Socket1;
        }

        function initSocket2() {
            console.log('Trying to open a WebSocket connection for RGB Leds...');
            Socket2 = new WebSocket(ws2_Gateway);
            Socket2.onopen = onOpen_Socket2;
            Socket2.onclose = onClose_Socket2;
            Socket2.onmessage = onMessage_Socket2;
        }

        function onOpen_Socket1(event) {
            console.log('Red LED and Servo Connection opened');
        }

        function onOpen_Socket2(event) {
            console.log('RGB Leds Connection opened');
        }

        function onClose_Socket1(event) {
            console.log('Red LED and Servo Connection closed');
            setTimeout(initSocket1, 2000);
        }

        function onClose_Socket2(event) {
            console.log('RGB Leds Connection closed');
            setTimeout(initSocket2, 2000);
        }

        function onMessage_Socket1(event) {
            var state;
            var angle;
            if (event.data == "ON") {
                state = "ON";
                document.getElementById('led').className = state;
                document.getElementById('state').innerHTML = state;
            }
            if (event.data == "OFF") {
                state = "OFF";
                document.getElementById('led').className = state;
                document.getElementById('state').innerHTML = state;
            }

            if ((parseInt(event.data, 10)) >= 0) {
                angle = event.data;
                document.getElementById('servoPos').innerHTML = angle;
                document.getElementById('slider').value = angle;
            }
            
            console.log(`Received a notification from ${event.origin}`);
            console.log(event);
        }

        function createLEDCard(LEDNum) {
            const LEDCard = document.createElement('button');
            LEDCard.setAttribute('class', 'card2');
            LEDCard.setAttribute('id', 'LEDCard' + LEDNum);
            LEDCard.setAttribute('style', 'background-color: red');
            LEDCard.setAttribute('onClick', 'changeColor('+LEDNum+')');
            LEDCard.textContent = LEDNum;
            return LEDCard;
        }

        function onMessage_Socket2(event) {
            var setColors;
            var div = document.getElementById('innerdiv');

            if ((parseInt(event.data, 10)) >= 1) {
                numClients = event.data;
                document.getElementById('numFriends').innerHTML = numClients;

                document.getElementById('innerdiv').innerHTML = "";
                for (let i = 1; i <= numClients; i ++) {
                    const LEDCard = createLEDCard(i);
                    div.appendChild(LEDCard);
                }
            }
            else {
                setColors = event.data;
                const seperatedColors = Array.from(setColors.split(","));

                for (let j = 0; j <= seperatedColors.length; j++) {
                    if (j == seperatedColors.length) {
                        break
                    }
                    else {
                        var k = j + 1;
                        document.getElementById('LEDCard'+k).style.backgroundColor = seperatedColors[j];
                    }
                }
            }

            console.log(`Received a notification from ${event.origin}`);
            console.log(event);
        }

        function onLoad(event) {
            initSocket1();
            initSocket2();
            initButton();
            initSlider();
        }

        function initButton() {
            document.getElementById('button').addEventListener('click', toggle);
        }

        function toggle() {
            Socket1.send('toggle');
        }

        function initSlider() {
            document.getElementById('slider').addEventListener('change', changeAngle);
        }

        function changeAngle() {
            Socket1.send(document.getElementById("slider").value);
        }

        function changeColor(numLED) {
            const colors = ["red", "orange", "yellow", "green", "blue", "purple"];
            var currentColor = document.getElementById('LEDCard' + numLED).style.backgroundColor;
            if (currentColor == colors[5]) {
                var nextColor = colors[0];
                document.getElementById('LEDCard' + numLED).style.backgroundColor = nextColor;
            }
            else {
                for (let i = 0; i < colors.length; i ++) {
                    if (currentColor == colors[i]) {
                        var nextColor = colors[i + 1];
                        document.getElementById('LEDCard' + numLED).style.backgroundColor = nextColor;
                    }
                }
            }
        }

        function sendColors() {
            var colorString = "";
            for (let j = 1; j <= numClients; j ++) {
                if (j == numClients) {
                    colorString += document.getElementById('LEDCard' + j).style.backgroundColor;
                }
                else {
                    colorString += document.getElementById('LEDCard' + j).style.backgroundColor + ",";
                }
            } 
            Socket2.send(colorString);
        }

    </script>
</body>

</html>

)rawliteral";
