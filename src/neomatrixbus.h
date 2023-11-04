#include <NeoPixelBusLg.h>
#include <NeoPixelAnimator.h>
//const uint16_t rgbmatrixleds=64;
//
#define matrix_righttoleft 0
#define matrix_testmode 1
#define zigzag 1
#define revzigzag 0
const uint8_t PanelWidth = 8;  // 8 pixel x 8 pixel matrix of leds
const uint8_t PanelHeight = 8;
const uint8_t TileWidth = 1;  // laid out in 4 panels x 2 panels mosaic
const uint8_t TileHeight = 1;
const uint16_t matrixount = PanelWidth * PanelHeight * TileWidth * TileHeight;
// instead of NeoPixelBus.h


typedef ColumnMajorAlternatingLayout MyPanelLayout;
// typedef ColumnMajorLayout MyPanelLayout;
// typedef RowMajorAlternatingLayout MyPanelLayout;
// typedef RowMajorLayout MyPanelLayout;

#define matrixsec  NeoGrbFeature //A three-element color in the order of Green, Red, and then Blue.This is used for SK6812(grb), WS2811, and WS2812.
//#define matrixsec  NeoRgbFeature //A three-element color in the order of Red, Green, and then Blue. Some older pixels used this. 
//#define matrixsec  NeoBgrFeature //A three-element color in the order of Blue, Red, and then Green.
//#define matrixmetod  NeoWs2812xMethod //WS2812a, WS2812b, WS2812c, etc The most compatible 
//#define matrixmetod Neo400KbpsMethod  // old slower speed standard that started this all
//#define matrixmetod NeoWs2812xInvertedMethod //(WS2812a, WS2812b, WS2812c, etc) Inverted
#ifdef ESP32
const uint8_t matrixpin= 25;
//#define ESP32_MATRIX_TASK
//****** ESP32 ******
#define matrixmetod NeoEsp32I2s0Ws2812xMethod //Uses the I2S 0 peripheral ESP32S2 only supports one I2S
//#define matrixmetod NeoEsp32I2s1Ws2812xMethod //Uses the I2S 1 peripheral 
//******
//Parallel Channels
//NOTE: The ESP32S2 only supports one i2s hardware bus. Thus you can only use the NeoEsp32I2s0*Method types.
//******
//#define matrixmetod NeoEsp32I2s0X8Ws2812xMethod //Uses the I2S 0 peripheral in 8 channel parallel mode
//#define matrixmetod NeoEsp32I2s0X16Ws2812xMethod //Uses the I2S 0 peripheral in 16 channel parallel mode
//#define matrixmetod NeoEsp32I2s1X8Ws2812xMethod //Uses the I2S 1 peripheral in 8 channel parallel mode
//#define matrixmetod NeoEsp32I2s1X16Ws2812xMethod //Uses the I2S 1 peripheral in 16 channel parallel mode
//******
//RTM
//******
//The RMT peripheral is used to send data to the NeoPixels. Thus, this method uses very little CPU for actually sending the data to NeoPixels but it requires an extra buffer for hardware to read from.
//NOTE: Even though the ESP32 has 8 channels of RMT hardware, using beyond 4 has shown to cause sending delays.
//NOTE: The ESP32S2 and ESP32S3 only supports 4 channels of RMT hardware, so only channels 0-3 are available.
//NOTE: The ESP32C3 only supports 2 channels of RMT hardware, so only channels 0 and 1 are available.
//NOTE: NeoEsp32Rmt# Methods supports any available pin below 34.
//#define matrixmetod NeoEsp32Rmt0Ws2812xMethod //Uses the RMT channel 0
//#define matrixmetod NeoEsp32Rmt1Ws2812xMethod //Uses the RMT channel 1
//#define matrixmetod NeoEsp32Rmt2Ws2812xMethod //Uses the RMT channel 2
//#define matrixmetod NeoEsp32Rmt3Ws2812xMethod //Uses the RMT channel 3
//#define matrixmetod NeoEsp32Rmt4Ws2812xMethod //Uses the RMT channel 4
//#define matrixmetod NeoEsp32Rmt5Ws2812xMethod //Uses the RMT channel 5
//#define matrixmetod NeoEsp32Rmt6Ws2812xMethod //Uses the RMT channel 6
//#define matrixmetod NeoEsp32Rmt7Ws2812xMethod //Uses the RMT channel 7
//#define matrixmetod NeoEsp32RmtNWs2812xMethod // Uses the RMT but allows the channel selection to be done at runtime. strip(rgbcount,rgb,pin,chanel)
//******
//BitBang not recomendet
//******
//#define matrixmetod NeoEsp32BitBangWs2812xMethod //The bit bang methods only support pins below 32
#else 
//****** ESP8266 ******
//The DMA methods will use GPIO3. 
//The UART1 methods will use GPIO2.
//The UART0 methods will use GPIO1.
//****** 
//DMA - no HW Serial 0 RX   
//#define matrixmetod NeoEsp8266DmaWs2812xMethod //you will not be able to recieve on Serial
//******
//UART
//#define matrixmetod NeoEsp8266Uart0Ws2812xMethod // GPIO1 no serial  
//#define matrixmetod NeoEsp8266Uart1Ws2812xMethod // GPIO2 no serial 1
//Due to using the hardware UART, the associated Serial or Serial1 objects can not be used. If you use UART1, Serial is still available and if you use UART0 then Serial1 is still available
//******
//Asynchronous UART
//#define matrixmetod NeoEsp8266AsycUart0Ws2812xMethod // GPIO1
#define matrixmetod NeoEsp8266AsyncUart1Ws2812xMethod // GPIO2
//#define matrixmetod NeoEsp8266AsyncUart0Ws2812Method // GPIO1
//#define matrixmetod NeoEsp8266AsyncUart1Ws2812Method // GPIO2

//WARNING: With this method, Serial and Serial1 cannot be used due to the ISR not being shareable.
//******
//BitBang non wifimode
//#define matrixmetod NeoEsp8266BitBangWs2812xMethod //  pins 0-15

#endif

//NeoPixelBusLg<matrixsec, matrixmatrixmetod, NeoGammaTableMethod> neostring(rgbbusleds,rgbbuspin);
//NeoPixelBus<matrixsec, matrixmetod> neostring(rgbbusleds,rgbbuspin);


NeoMosaic <MyPanelLayout> mosaic(
    PanelWidth,
    PanelHeight,
    TileWidth,
    TileHeight);

#ifdef ESP32
NeoPixelBusLg<matrixsec, matrixmetod> neomatrix(matrixount ,matrixpin);
#else
NeoPixelBus<matrixsec, matrixmetod> neomatrix(matrixount);
#endif

#ifdef ESP32_MATRIX_TASK
xSemaphoreHandle semaphorematrix = NULL;
TaskHandle_t commit_taskmatrix;

void commitTaskProcedurematrix(void *arg)
{
    while (true)
    {
        while (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) != 1)
            ;
        neomatrix.Show();
        while (!neomatrix.CanShow())
            ;
        xSemaphoreGive(semaphorematrix);
    }
}

void commitmatrix()
{
    xTaskNotifyGive(commit_taskmatrix);
    while (xSemaphoreTake(semaphorematrix, portMAX_DELAY) != pdTRUE)
        ;
}

void init_taskmatrix()
{
    commit_taskmatrix = NULL;
    semaphorematrix = xSemaphoreCreateBinary();
    //disableCore1WDT();
    xTaskCreatePinnedToCore(
        commitTaskProcedurematrix,         /* Task function. */
        "ShowRunnerTaskMatrix",            /* name of task. */
        10480,                       /* Stack size of task */
        NULL,                        /* parameter of the task */
        4,                           /* priority of the task */
        &commit_taskmatrix,                /* Task handle to keep track of created task */
        1);                          /* pin task to core core_id */
}

#endif
void matrixpixel(int matrixpos, RgbColor color)
{
    if (zigzag) {
		int Y = matrixpos / 8;
			int X = matrixpos % 8;
			if (!revzigzag) {
				if (!(Y % 2)) {
					X = 7 - X;
				}
			}
			else {
				if ((Y % 2)) {
					X = 7 - X;
				}
			}
			matrixpos = Y * 8 + X;
		}

		neomatrix.SetPixelColor(matrixpos,color);
}



void neomatrixclear()
{
 		for (int mj = 0; mj < matrixount; mj++) {
			matrixpixel(mj, RgbColor(0, 0, 0));
		}
        //commit();

}

void  neomatrixbegin()
{
	neomatrix.Begin();
	#ifdef ESP32
	init_taskmatrix();
	#else
	neomatrix.Show();
	#endif
	if (matrix_testmode != 0)
	{	//neomatrix.SetLuminance(100);
		for (int mi = 0; mi < matrixount; mi++) {
		  matrixpixel(mi,RgbColor(random(0,255),random(0,255),random(0,255)));
		}
	/*	for (int mi = 0; mi < 64; mi++) {
		  matrixsetpixel(mi,RgbColor(random(0,255),random(0,255),random(0,255)));
		}*/
		
	}
	#ifdef ESP32
	commitmatrix();
	#else
	neomatrix.Show();
	#endif
	
}

void neomatrixread()
{
    //neomatrix.SetLuminance(255);

    
    
   	uint8_t mr;
	uint8_t mg;
	uint8_t mb;
	uint16_t mb1;
	uint16_t mb2;
	uint8_t mj;
	int mmode = 1;
	mmode = FlowSerialTimedRead();
	while (mmode > 0)
		{
			// Read all
			if (mmode == 1) {
				for (mj = 0; mj < matrixount; mj++) {
					mr = FlowSerialTimedRead();
					mg = FlowSerialTimedRead();
					mb = FlowSerialTimedRead();

					if (matrix_righttoleft == 1) {
						matrixpixel(matrixount - mj - 1, RgbColor(mr, mg, mb));
					}
					else {
						matrixpixel(mj, RgbColor(mr, mg, mb));
					}
				}
			}

			// partial led data
			else if (mmode == 2) {
				int mstartled = FlowSerialTimedRead();
				int mnumleds = FlowSerialTimedRead();

				for (mj = mstartled; mj < mstartled + mnumleds; mj++) {
					/*	if (ENABLE_BLUETOOTH == 0) {*/
					mr = FlowSerialTimedRead();
					mg = FlowSerialTimedRead();
					mb = FlowSerialTimedRead();

					if (matrix_righttoleft == 1) {
						matrixpixel(matrixount - mj - 1, RgbColor(mr, mg, mb));
					}
					else {
						matrixpixel(mj, RgbColor(mr, mg, mb));
					}
				}
			}

			// repeated led data
			else if (mmode == 3) {
				int mstartled = FlowSerialTimedRead();
				int mnumleds = FlowSerialTimedRead();

				mr = FlowSerialTimedRead();
				mg = FlowSerialTimedRead();
				mb = FlowSerialTimedRead();

				for (mj = mstartled; mj < mstartled + mnumleds; mj++) {
					if (matrix_righttoleft == 1) {
						matrixpixel(matrixount - mj - 1, RgbColor(mr, mg, mb));
					}
					else {
						matrixpixel(mj, RgbColor(mr, mg, mb));
					}
				}
			}

			mmode = FlowSerialTimedRead();
		}

		if (matrixount > 0) {
	    #ifdef ESP32
		commitmatrix();
		#else
		neomatrix.Show();
        #endif
		}
}