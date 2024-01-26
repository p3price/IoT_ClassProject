# IoT_Project
This project is the culmination of the course work completed during EGRT 357 (Internet of Things) at the University of Wisconsin Oshkosh department of Engineering and Engineering Technology.
My goal for this project was to create an installation piece that exists at the interconnection of Art and Technology. 
Using the knowledge I had gained thus for in my IoT course work as well as various electronic parts I already had at home, I built a piece that encourages interactivity and collaboration.

The final project outcome has little practical functionality and is nothing but a thing-a-ma-bob but the development of C++, HTML, and JavaScript code was no simple task to create such a piece.
I was able to deepen my understanding of IoT related functionality, particularily in websocket communication, which I see as a powerful tool for further IoT related projects.

This repository can be opened directly in VSCode and depends on the PlatformIO Extension to build the binary file to then transfer to the DOIT ESP32 Dev Board.

The key functionalites of the system are:
  1. Establish a webpage server on the ESP32 microcontroller
  2. Allow control of a simple Red LED, a Servo motor, and an array of RGB LEDs from the webpage
  3. Use number of connected clients as limit for how many RGB LEDs can be modified, thus encouraging multiple users to join together to create more detailed lighting displays
  4. All communication for event and message data is facilitated through WebSocket protocal

# Supplies
  1. Espressif ESP32 Microcontroller: https://makeradvisor.com/tools/esp32-dev-board-wi-fi-bluetooth/
  2. Red LED and appropriate resistor
  3. 128 x 64 OLED display: https://www.digikey.com/en/products/detail/universal-solder-electronics-ltd/OLED%2520128X64%25200.96%2522%2520I2C/16822116?utm_adgroup=&utm_source=google&utm_medium=cpc&utm_campaign=PMax%20Shopping_Product_Medium%20ROAS%20Categories&utm_term=&utm_content=&utm_id=go_cmp-20223376311_adg-_ad-__dev-c_ext-_prd-16822116_sig-EAIaIQobChMI-ojU3rL-ggMVcCStBh0mxg3YEAQYBCABEgLORvD_BwE&gad_source=1&gclid=EAIaIQobChMI-ojU3rL-ggMVcCStBh0mxg3YEAQYBCABEgLORvD_BwE
  4. NeoPixel Ring with resistor for data wire: https://www.adafruit.com/product/1463?gad_source=1&gclid=EAIaIQobChMI1JP3gLP-ggMVMAitBh1KOg1BEAQYASABEgIgYfD_BwE
  5. SG90 Servo: https://www.amazon.com/Micro-Helicopter-Airplane-Remote-Control/dp/B072V529YD
  6. Lensball: https://www.amazon.com/Original-Lensball-Microfiber-Photography-Accessory/dp/B076BQFDF8/ref=asc_df_B076BSWLPT/?tag=&linkCode=df0&hvadid=309822971200&hvpos=&hvnetw=g&hvrand=6346218967746506082&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9019412&hvtargid=pla-524046190968&mcid=bf68e83dc7b53b8282cb0e28dc64fbf4&ref=&adgrpid=70157059668&gclid=EAIaIQobChMI7Iz7rrP-ggMVBBqtBh1yfQzREAQYASABEgJyqfD_BwE&th=1

# Photo References
Front View
![288912034-308d8089-1547-49ee-93db-8e83fd5f6a76](https://github.com/p3price/IoT_Project/assets/126983543/fb455689-da67-4408-8efb-fe4fa5ec1802)
Rear View
![288911954-0691f4c8-169d-447b-aef6-20b2f785dd6c](https://github.com/p3price/IoT_Project/assets/126983543/4a837067-7f1b-49b2-89f8-1d18b6bfede2)
Servo Connection
![288912063-b8df5277-5776-4730-9164-bd1d55f0791c](https://github.com/p3price/IoT_Project/assets/126983543/facedc6a-74b6-4e3f-b8ca-83d2820621a5)
Webpage Screenshot
<img width="1036" alt="288912117-e18b5840-305b-4047-a942-07eb82b0a068" src="https://github.com/p3price/IoT_Project/assets/126983543/7ef31cc3-28dc-47bf-9d4b-5a1e9932eb88">
Operation Example
![288912165-ca4ccefe-ca3c-4fdf-ae57-2c00e42a148c](https://github.com/p3price/IoT_Project/assets/126983543/20c6cf3f-c88e-44de-9c67-f57ec2898ee3)
