/* included lib */
#include <SPI.h>
#include "SdFat.h"
#include <Ethernet.h>
#include <arduino-timer.h>

/* System configuration definition */
#include "DomoBugs.h"
#include "IOsVariablesDefinition.h"

///////////////////////////////////////////////////// Timers
auto timer = timer_create_default();
const PROGMEM long const_PeriodicRemoteUpdateTime = 10000;

// Used to interrupt periodic remote server update 
#define UnplanRemoteUpdate timer.cancel() 
// Used to trig periodic remote server update
#define PlanNextRemoteUpdate timer.in(const_PeriodicRemoteUpdateTime,RemoteUpdateFunction);

/////////////////////////////////////////////////////
const String ConfigFileName = "system.config";
const int c_SDInitRetryLimit = 3;

/////////////////////////////////////////////////////
const byte c_mac[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };  // MAC address from Ethernet shield sticker under board
byte LocalServerIP[] = {192, 168, 1, 254}; // IP address of local server
int LocalServerPort = 80; // server port for exchange
byte RemoteSystemIP[] = {192, 168, 1, 253}; // IP address of Remote server
int RemoteSystemPort = 8080; // Port to use for remote

/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void(* reset) (void) = 0; //function declared to @0 to simulate a SW reset

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// initialization
void setup()
{
    // --------------------------------------------------------------------------------
    ///////////////////////////////////////////////////// Init arduino outputs for shield   
    // Arduino communicates with both the W5100 and SD card using the SPI bus (through the ICSP header). 
    // For the Uno this is on digital:
    //     -> pins 10, 11, 12, and 13
    // For the Mega this is on digital:
    //     -> pins 50, 51, and 52
    // On both boards, pin 10 is used to select the W5100 and pin 4 for the SD card.
    // So these pins cannot be used for general I/O. 
    // On the Mega, the hardware SS pin, 53, is not used to select either the W5100 or the SD card, but it must be kept as an output or the SPI interface won't work.
    // Note that because the W5100 and SD card share the SPI bus, only one can be active at a time.
    // If you are using both peripherals in your program, this should be taken care of by the corresponding libraries.
    // If you're not using one of the peripherals in your program, however, you'll need to explicitly deselect it.
    // To do this with the SD card, set pin 4 as an output and write a high to it. 
    // For the W5100, set digital pin 10 as a high output.  
    // --------------------------------------------------------------------------------  
    // Pin 4 is dirrectly connected to SD SS pin
    pinMode(4, OUTPUT); 
    digitalWrite(4, HIGH);
   // Pin 10 is dirrectly connected to W5100 SS pin
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    // --------------------------------------------------------------------------------
    ///////////////////////////////////////////////////// Init serial com
    Serial.begin(250000);
    // Wait for USB Serial
    while (!Serial) {
      yield();
    }
    Serial.println(F("Serial COM initialized."));
    // --------------------------------------------------------------------------------
    ///////////////////////////////////////////////////// Init IOs
    Serial.println(F("IOs Initializing..."));
    IOsInit();
    Serial.println(F("\tIOs Initialized."));
    PrintIOsState();
    // --------------------------------------------------------------------------------
    ///////////////////////////////////////////////////// initialize SD card
    // Init Shield used
    EthernetShieldConfig();
    //delay(1000);

    ///////////////////////////////////////////////////// initialize SD card
    Serial.println(F("SD card Initializing..."));
    int SDInitCounter = 1;
    int SDInitState = 0;
    do{
      SDInitState = SDCardInitialization();
      if (!SDInitState) {
          Serial.print(F("\t#")); Serial.print(String(SDInitCounter)); Serial.println(F(" ERROR - SD card initialization failed!"));
      } else {
          Serial.println(F("\tSUCCESS - SD card initialized."));
      }
          
      SDInitCounter++;
    }while((SDInitCounter <= c_SDInitRetryLimit) && !SDInitState);
    if(!SDInitState){ delay(5000);  reset();}

    ///////////////////////////////////////////////////// Get system configuration
    Serial.println(F("Loading of system config..."));
    if(!SystemConfigurationsLoading()){ delay(5000);  reset();}

    ///////////////////////////////////////////////////// initialize Ethernet device
    Serial.println(F("Server initializing..."));
    if(!WebServerInit(c_mac, LocalServerIP))
    {
      IPAddress ServerIP; // Declared to return used IP address of local server
      //delay(5000);
      ServerIP = GetLocalServerAddress();
      Serial.print(F("\tSUCCESS - Server initialized at "));
      Serial.print(ServerIP[0],DEC); Serial.print("."); Serial.print(ServerIP[1],DEC); Serial.print("."); Serial.print(ServerIP[2],DEC); Serial.print("."); Serial.println(ServerIP[3],DEC);
    }
    else
        Serial.println(F("\tERROR - Server initialization failed!"));
    // --------------------------------------------------------------------------------  
    ///////////////////////////////////////////////////// Check server home page availability
    Serial.println(F("Server check..."));
    if (!CheckFileExist("home.html"))
        Serial.println(F("\tERROR - Can't find home.html file!"));
    else
        Serial.println(F("\tSUCCESS - Found home.html file."));
    
    Serial.println(F("Exploring of files..."));
    if (!ShowExistingFiles())
      Serial.println(F("Files explorer failed"));
    else
      Serial.println(F("Files explorer displayed!"));

    PlanNextRemoteUpdate;

    Serial.println(F("============ Initialization terminated! ============ "));
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Main
void loop()
{
    
    UpdateIOsState();
    GetAndManageClientRequest();
    
    // ---------------------------------------------- //
    timer.tick();
}
/////////////////////////////////////////////////////////////////////////////////////////
bool RemoteUpdateFunction(void *)
{
    UpdateRemoteServer();
    while(GetRemoteServerFeedback())
      delay(1);

    PlanNextRemoteUpdate;
    return true; // to run it periodically
}
