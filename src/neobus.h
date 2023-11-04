#include <NeoPixelBusLg.h>
const uint16_t rgbbusleds=24;
#define _righttoleft 0
#define testmode 1

#define colorsec  NeoGrbFeature //A three-element color in the order of Green, Red, and then Blue.This is used for SK6812(grb), WS2811, and WS2812.
//#define colorsec  NeoRgbFeature //A three-element color in the order of Red, Green, and then Blue. Some older pixels used this. 
//#define colorsec  NeoBgrFeature //A three-element color in the order of Blue, Red, and then Green.
//#define metod  NeoWs2812xMethod //WS2812a, WS2812b, WS2812c, etc The most compatible 
//#define metod Neo400KbpsMethod  // old slower speed standard that started this all
//#define metod NeoWs2812xInvertedMethod //(WS2812a, WS2812b, WS2812c, etc) Inverted

#ifdef ESP32
const uint8_t rgbbuspin= GPIO_NUM_25;
//**** WIP ****
//ESP32 Task menaged in deep development need improwments 
//#define ESP32_STRIP_TASK 
// instead of NeoPixelBus.h

//****** ESP32 ******
#define metod NeoEsp32I2s0Ws2812xMethod //Uses the I2S 0 peripheral ESP32S2 only supports one I2S
//#define metod NeoEsp32I2s1Ws2812xMethod //Uses the I2S 1 peripheral 
//******
//Parallel Channels
//NOTE: The ESP32S2 only supports one i2s hardware bus. Thus you can only use the NeoEsp32I2s0*Method types.
//******
//#define metod NeoEsp32I2s0X8Ws2812xMethod //Uses the I2S 0 peripheral in 8 channel parallel mode
//#define metod NeoEsp32I2s0X16Ws2812xMethod //Uses the I2S 0 peripheral in 16 channel parallel mode
//#define metod NeoEsp32I2s1X8Ws2812xMethod //Uses the I2S 1 peripheral in 8 channel parallel mode
//#define metod NeoEsp32I2s1X16Ws2812xMethod //Uses the I2S 1 peripheral in 16 channel parallel mode
//******
//RTM
//******
//The RMT peripheral is used to send data to the NeoPixels. Thus, this method uses very little CPU for actually sending the data to NeoPixels but it requires an extra buffer for hardware to read from.
//NOTE: Even though the ESP32 has 8 channels of RMT hardware, using beyond 4 has shown to cause sending delays.
//NOTE: The ESP32S2 and ESP32S3 only supports 4 channels of RMT hardware, so only channels 0-3 are available.
//NOTE: The ESP32C3 only supports 2 channels of RMT hardware, so only channels 0 and 1 are available.
//NOTE: NeoEsp32Rmt# Methods supports any available pin below 34.
//#define metod NeoEsp32Rmt0Ws2812xMethod //Uses the RMT channel 0
//#define metod NeoEsp32Rmt1Ws2812xMethod //Uses the RMT channel 1
//#define metod NeoEsp32Rmt2Ws2812xMethod //Uses the RMT channel 2
//#define metod NeoEsp32Rmt3Ws2812xMethod //Uses the RMT channel 3
//#define metod NeoEsp32Rmt4Ws2812xMethod //Uses the RMT channel 4
//#define metod NeoEsp32Rmt5Ws2812xMethod //Uses the RMT channel 5
//#define metod NeoEsp32Rmt6Ws2812xMethod //Uses the RMT channel 6
//#define metod NeoEsp32Rmt7Ws2812xMethod //Uses the RMT channel 7
//#define metod NeoEsp32RmtNWs2812xMethod // Uses the RMT but allows the channel selection to be done at runtime. strip(rgbcount,rgb,pin,chanel)
//******
//BitBang not recomendet
//******
//#define metod NeoEsp32BitBangWs2812xMethod //The bit bang methods only support pins below 32
#else
//****** ESP8266 ******
//No hardware SERIAL WIFI ONLY
//The DMA methods will use GPIO3. 
//The UART1 methods will use GPIO2.
//The UART0 methods will use GPIO1.
//****** 
//DMA - no HW Serial 0 RX   
//#define metod NeoEsp8266DmaWs2812xMethod //you will not be able to recieve on Serial
//******
//UART
//#define metod NeoEsp8266Uart0Ws2812xMethod // GPIO1 no serial  
//#define metod NeoEsp8266Uart1Ws2812xMethod // GPIO2 no serial 1
//Due to using the hardware UART, the associated Serial or Serial1 objects can not be used. If you use UART1, Serial is still available and if you use UART0 then Serial1 is still available
//******
//Asynchronous UART
#define metod NeoEsp8266AsyncUart0Ws2812xMethod // GPIO1
//#define metod NeoEsp8266AsyncUart1Ws2812xMethod // GPIO2
//#define metod NeoEsp8266AsyncUart0Ws2812Method // GPIO1
//#define metod NeoEsp8266AsyncUart1Ws2812Method // GPIO2

//WARNING: With this method, Serial and Serial1 cannot be used due to the ISR not being shareable.
//******
//BitBang non wifimode works whit serial but is slow

//#define metod NeoEsp8266BitBangWs2812xMethod //  pins 0-15


//NeoPixelBusLg<colorsec, metod, NeoGammaTableMethod> neostring(rgbbusleds,rgbbuspin);
//NeoPixelBus<colorsec, metod> neostring(rgbbusleds,rgbbuspin);
#endif
#ifdef ESP32
NeoPixelBusLg<colorsec, metod> neostring(rgbbusleds,rgbbuspin);
#else //IF BITBANG add ", GPIOX" after rgbusleds
NeoPixelBusLg<colorsec, metod> neostring(rgbbusleds);
#endif

#ifdef ESP32_STRIP_TASK
long msscommit;
xSemaphoreHandle semaphore = NULL;
TaskHandle_t commit_task;
TaskHandle_t commit_task_rgb;

void commitTaskProcedure(void *arg)
{
    while (true)
    {
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 1)
            ;
        neostring.Show();
        while (!neostring.CanShow())
            ;
        xSemaphoreGive(semaphore);
    }

}

void commit()
{
	

    xTaskNotifyGive(commit_task);
    while (xSemaphoreTake(semaphore, portMAX_DELAY) != pdTRUE)
        ;
	
}



void init_task()
{
    commit_task = NULL;
    semaphore = xSemaphoreCreateBinary();
    //disableCore1WDT();
    xTaskCreatePinnedToCore(
        commitTaskProcedure,         /* Task function. */
        "ShowRunnerTask",            /* name of task. */
        10000,                       /* Stack size of task */
        NULL,                        /* parameter of the task */
        7,                           /* priority of the task */
        &commit_task,                /* Task handle to keep track of created task */
        1);                          /* pin task to core core_id */
}

void loop2(void * ppd)
{
	for(;;){
		commit();
	}
}

void rgbloop()
{
	xTaskCreatePinnedToCore(
        loop2,         /* Task function. */
        "ShowRunnerTawsk",            /* name of task. */
        10000,                       /* Stack size of task */
        NULL,                        /* parameter of the task */
        6,                           /* priority of the task */
        &commit_task_rgb,                /* Task handle to keep track of created task */
        1);                          /* pin task to core core_id */
}
//testing core  1 set pixel

struct PixelData
{
	uint8_t r,g,b,p;
};
TaskHandle_t SetPixelTask;

void setpixeltask(void* dane)
{	
	PixelData pdt = *(PixelData *) dane;
	neostring.SetPixelColor(pdt.p, RgbColor(pdt.r, pdt.g, pdt.b));
	
	//vTaskDelete(SetPixelTask);
	vTaskDelete(NULL);
}




void setpixeldata(uint8_t r, uint8_t g, uint8_t b, uint8_t poz)
{	
	PixelData pd={r,g,b,poz};
	xTaskCreatePinnedToCore(setpixeltask, "SETPIXELCOLOR",1000,&pd,8,&SetPixelTask,1);
}

#endif

void  neobusbegin()
{
	neostring.Begin();
	//neomatrix.Begin();
	//neostring.Show();
	#ifdef ESP32_STRIP_TASK
    init_task();
	msscommit = millis();
	#else	
	neostring.Show();
	#endif

	if (testmode != 0)
	{	neostring.SetLuminance(155);
		for (int i = 0; i < rgbbusleds; i++) {
 		#ifdef ESP32_STRIP_TASK
		  setpixeldata(random(0,255),random(0,255),random(0,255),i);	
		#else  
		  neostring.SetPixelColor(i,RgbColor(random(0,255),random(0,255),random(0,255)));
		#endif 
		}
		delay(50);
	#ifdef ESP32_STRIP_TASK
	commit();
	#else
	neostring.Show();
	#endif
	}
neostring.SetLuminance(255);
}

/*void neobusread32(void* dsds)
{
   // neostring.SetLuminance(255);

    
    
   	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint16_t b1;
	uint16_t b2;
	uint8_t j;
	int mode = 1;
	mode = FlowSerialTimedRead();
	while (mode > 0)
		{
			// Read all
			if (mode == 1) {
				for (j = 0; j < rgbbusleds; j++) {
					r = FlowSerialTimedRead();
					g = FlowSerialTimedRead();
					b = FlowSerialTimedRead();

					if (_righttoleft == 1) {
						//setpixeldata(r,g,b,rgbbusleds - j - 1);
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
					}
					else {
						//setpixeldata(r,g,b,j);
						neostring.SetPixelColor(j, RgbColor(r, g, b));
					}
				}
			}

			// partial led data
			else if (mode == 2) {
				int startled = FlowSerialTimedRead();
				int numleds = FlowSerialTimedRead();

				for (j = startled; j < startled + numleds; j++) {
					
					r = FlowSerialTimedRead();
					g = FlowSerialTimedRead();
					b = FlowSerialTimedRead();

					if (_righttoleft == 1) {
						//setpixeldata(r,g,b,rgbbusleds - j - 1);
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
					}
					else {
						//setpixeldata(r,g,b,j);
						neostring.SetPixelColor(j, RgbColor(r, g, b));
					}
				}
			}

			// repeated led data
			else if (mode == 3) {
				int startled = FlowSerialTimedRead();
				int numleds = FlowSerialTimedRead();

				r = FlowSerialTimedRead();
				g = FlowSerialTimedRead();
				b = FlowSerialTimedRead();

				for (j = startled; j < startled + numleds; j++) {
					if (_righttoleft == 1) {
						//setpixeldata(r,g,b,rgbbusleds - j - 1);
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
					}
					else {
						//setpixeldata(r,g,b,j);
						neostring.SetPixelColor(j, RgbColor(r, g, b));
					}
				}
			}

			mode = FlowSerialTimedRead();
		}

		if (rgbbusleds > 0) {
	    	#ifdef ESP32_STRIP_TASK
			//commit();
			#else
			neostring.Show();
			//vTaskDelete(SetPixelTask);
			#endif
		}
//		neostring.Show();
	
}*/

void neobusread()
{
   // neostring.SetLuminance(255);
	//setCpuFrequencyMhz(240);

    uint8_t r;
	uint8_t g;
	uint8_t b;
	uint16_t b1;
	uint16_t b2;
	uint8_t j;
	int mode = 1;
	mode = FlowSerialTimedRead();
	while (mode > 0)
		{
			// Read all
			if (mode == 1) {
				for (j = 0; j < rgbbusleds; j++) {
					r = FlowSerialTimedRead();
					g = FlowSerialTimedRead();
					b = FlowSerialTimedRead();

					if (_righttoleft == 1) {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,rgbbusleds - j - 1);
						#else
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
						#endif
					}
					else {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,j);
						#else
						neostring.SetPixelColor(j, RgbColor(r, g, b));
						#endif
					}
				}
			}

			// partial led data
			else if (mode == 2) {
				int startled = FlowSerialTimedRead();
				int numleds = FlowSerialTimedRead();

				for (j = startled; j < startled + numleds; j++) {
					/*	if (ENABLE_BLUETOOTH == 0) {*/
					r = FlowSerialTimedRead();
					g = FlowSerialTimedRead();
					b = FlowSerialTimedRead();

					if (_righttoleft == 1) {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,rgbbusleds - j - 1);
						#else
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
						#endif
					}
					else {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,j);
						#else
						neostring.SetPixelColor(j, RgbColor(r, g, b));
						#endif
					}
				}
			}

			// repeated led data
			else if (mode == 3) {
				int startled = FlowSerialTimedRead();
				int numleds = FlowSerialTimedRead();

				r = FlowSerialTimedRead();
				g = FlowSerialTimedRead();
				b = FlowSerialTimedRead();

				for (j = startled; j < startled + numleds; j++) {
					if (_righttoleft == 1) {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,rgbbusleds - j - 1);
						#else
						neostring.SetPixelColor(rgbbusleds - j - 1, RgbColor(r, g, b));
						#endif
					}
					else {
						#ifdef ESP32_STRIP_TASK
						setpixeldata(r,g,b,j);
						#else
						neostring.SetPixelColor(j, RgbColor(r, g, b));
						#endif
					}
				}
			}

			mode = FlowSerialTimedRead();
		}

		if (rgbbusleds > 0) {
	    	#ifdef ESP32_STRIP_TASK
			commit();
			#else
			neostring.Show();
			#endif
		}
	
}
