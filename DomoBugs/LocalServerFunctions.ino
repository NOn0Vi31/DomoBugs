// #define Debug_ConnectionState //➖
// #define Debug_ReceivedReq // 📣
// #define Debug_ReqDecoding //⚙️
// #define Debug_DecodedReq //🔎
// #define Debug_PostHttpReqHighlight //🔽🔼
// #define Debug_PageLoading // 🌐
// #define Debug_Json //🗞️
// #define Debug_ActionManagement //☑️

#define REQ_BUF_SZ   127 // size of buffer used to capture HTTP requests

EthernetServer LocalServer(LocalServerPort);  // server init with port for exchange
EthernetClient client;  // client init

// Variables for http req analysis
const String HttpMethod[] = {"GET", "POST", "HEAD"};
const int GET = 0, POST = 1;

struct HTTPRequest {
  String Method;
  String RequestURI;
  //String ContentType; //removed to save space
};


///////////////////////////////////////////////////// initialize Ethernet shield
// Description:
//    This function initialize Ethernet shield
//    depending of Arduino Mega 2560 HW.
//
/////////////////////////////////////////////////////
void EthernetShieldConfig()
{
    // On the Mega, the hardware SS pin, 53, is not used to select the Ethernet controller chip, but it must be kept as an output or the SPI interface won't work
    pinMode(53, OUTPUT);
    Ethernet.init(10);
 
}
///////////////////////////////////////////////////// initialize local server
// Description:
//    This function initialize local served with
//    specified mac address & IP.
//
/////////////////////////////////////////////////////
int WebServerInit(byte mac[6], byte EthernetAddress[4])
{
    Ethernet.begin(mac, EthernetAddress);
    LocalServer.begin();           // start to listen for clients
}
///////////////////////////////////////////////////// return local server address
IPAddress GetLocalServerAddress()
{
  return Ethernet.localIP();
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Get and manage client request
// Description:
//    This function is periodically called from main loop 
//    in order to get client request to the local server.
//    Function:
//       1. Read and extract required info from client request
//       2. Return an answer to client depending of his req
//    
////////////////////////////////////////////////////
void GetAndManageClientRequest()
{
    client = LocalServer.available();  // try to get client

    if (client) {  // got client?
        //UnplanRemoteUpdate;

            HTTPRequest HTTPRequestReceived;
    
            while (client.connected()) {
                if (client.available()) {   // client data available to read
                    #ifdef Debug_ConnectionState
                      Serial.println(F("➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖ Connection: opened")); // for debug
                    #endif

                    String ClientBuffer = client.readString(); // read line from client
                    #ifdef Debug_ReceivedReq
                      Serial.println(F("📣.📣.📣.📣.📣.📣.📣.📣.📣.📣.📣.📣.📣.📣.📣"));
                      Serial.print(ClientBuffer); // for debug
                      Serial.println(F(" ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗  ⁗ "));
                    #endif

                    HTTPReqMethodAndURIExtract(&HTTPRequestReceived, &ClientBuffer); // Extract HTTPReq info
                    #ifdef Debug_PostHttpReqHighlight
                      Serial.println(F("🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽.🔽")); // for debug
                    #endif

                    if(HTTPRequestReceived.Method == "GET") // if this is a GET browser http req then
                    {
                      if(HTTPRequestReceived.RequestURI.lastIndexOf("/json.htm") != 0) // if it is not a json req required then
                        SendRequestedFileToClient(&client, &HTTPRequestReceived); // respond to client by sending requested file
                      else
                      {
                        if(HTTPRequestReceived.RequestURI.lastIndexOf("/json.htm?get") == 0) // if json req is a get req then
                          SendStates(&client);
                        else
                        {
                          if(HTTPRequestReceived.RequestURI.lastIndexOf("/json.htm?set") == 0) // if json req is a set req then
                            GetActionToDoAndReturnFeedback(&client, &HTTPRequestReceived);
                        }
                      }
                    }
                    break;
                  
                } // end if (client.available())
            } // end while (client.connected())
            client.stop(); // close the connection
            
        #ifdef Debug_PostHttpReqHighlight
          Serial.println(F("🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼.🔼")); // for debug
        #endif
        #ifdef Debug_ConnectionState
          Serial.println(F("➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖➖ Connection: closed")); // for debug
        #endif
        
        //PlanNextRemoteUpdate;
    } // end if (client)
}

//////////////////////////////////////////////////// Load requested file from client, send it, and close the file
void SendRequestedFileToClient(EthernetClient *client, struct HTTPRequest *HTTPRequestReceived)
{
  // Check if files to load was precised, else set it to default home page
  if(HTTPRequestReceived->RequestURI=="/")  HTTPRequestReceived->RequestURI = "home.html";

  #ifdef Debug_PageLoading // Log what will be done
    Serial.print(F("\t🌐 "));
    Serial.print(HTTPRequestReceived->RequestURI);
    Serial.print(F(" file of type "));
    Serial.print(HTTPRequestReceived->RequestURI.substring(HTTPRequestReceived->RequestURI.lastIndexOf('.')+1));
    Serial.println(F(" will be sent"));
  #endif
  // Check & Load file
  File32 FileStream;
  LoadFileFromSDCard(HTTPRequestReceived->RequestURI, &FileStream);        // open web page file

  if (FileStream) { //if file was found then
      // send a standard http response header
      client->println("HTTP/1.1 200 OK");
      client->println("Content-type: text/" + HTTPRequestReceived->RequestURI.substring(HTTPRequestReceived->RequestURI.lastIndexOf('.')+1));
      client->println("Connection: Keep-Alive");
      client->println("Keep-Alive: max=2000, timeout=2000");
      client->println("");
      // send body: required file
      while(FileStream.available()) {
          client->write(FileStream.read()); // send web page to client
          //Serial.write(FileStream.read());
      }
      FileStream.close();
      #ifdef Debug_PageLoading
        Serial.print(F("\t🌐 "));
        Serial.print(HTTPRequestReceived->RequestURI);
        Serial.println(F(" file sent"));
      #endif
  }
  else //if file was not found then
  {
    client->println("HTTP/1.1 404 Not Found");
    client->println("Content-type: text/html");
    client->println("");
    client->println("<html>");
    client->println("<head><title>404 Not found</title></head>");
    client->println("<body>");
    client->println("<center><h1>404 Not found</h1></center>");
    client->println("<hr><center>openresty</center>");
    client->println("</body>");
    client->println("</html>");
    client->println("");
    #ifdef Debug_PageLoading
      Serial.print(F("\t🌐 "));
      Serial.print(HTTPRequestReceived->RequestURI);
      Serial.println(F(" file was not found"));
    #endif
  }
  client->println();
}
//////////////////////////////////////////////////// Function returning IOs states to client in a JSON format
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT FROM YOUR WEBSITE ###
// Description:
//    This function is called in case where client
//    send a json get request (considering in this case
//    client want to get IOs state).
//    So here, informations are formated in json form
//    before to be sent to clien.
//    Request & Feedback is generaly managed using a
//    java script updating informations on client browser.
//    
////////////////////////////////////////////////////
void SendStates(EthernetClient *client)
{
  #ifdef Debug_Json // Log what will be done
    Serial.println("\t🗞️ Send of state using json format");
  #endif
  // send http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Access-Control-Allow-Origin: *");
  client->println("Content-Type: application/json");
  client->println("Connection: Keep-Alive");
  client->println("Keep-Alive: max=2000, timeout=2000");
  client->println("");
  // send json
  client->println("{");
  client->print("\"MainState\":"); client->print(MainState); client->println(",");
  client->print("\"FaultState\":"); client->print(FaultState); client->println(",");
  client->print("\"AlertState\":"); client->print(AlertState); client->println(",");
  client->print("\"OptionRelay\":"); client->print(OptionRelay); client->println(",");
  client->print("\"BatteryVoltage\":"); client->print(BatteryVoltage); client->println(",");
  client->print("\"PowerSupplyVoltage\":"); client->print(PowerSupplyVoltage); client->println(",");
  client->print("\"Zone1Voltage\":"); client->print(Zone1Voltage); client->println(",");
  client->print("\"Zone2Voltage\":"); client->print(Zone2Voltage); client->println(",");
  client->print("\"Zone3Voltage\":"); client->print(Zone3Voltage); client->println(",");
  client->print("\"Zone4Voltage\":"); client->print(Zone4Voltage); client->println(",");
  client->print("\"Zone5Voltage\":"); client->print(Zone5Voltage); client->println(",");
  client->print("\"Zone6Voltage\":"); client->print(Zone6Voltage); client->println("");
  client->println("}");
  client->println("");
  client->println();
  
  #ifdef Debug_Json
    Serial.println(F("\t🗞️ json.htm sent"));
  #endif

}
//////////////////////////////////////////////////// Function getting action requested from client in a JSON format
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT FROM YOUR WEBSITE ###
// Description:
//    This function is called in case where client
//    send a json set request (considering in this case
//    client want to set an IO).
//    Function:
//      1. Decode received json request
//      2. Call a function executing the requested action
//      3. Return action result to client
//    Request & Feedback is generaly managed using a
//    java script updating informations on client browser.
//    
////////////////////////////////////////////////////
void GetActionToDoAndReturnFeedback(EthernetClient *client, struct HTTPRequest *HTTPRequestReceived)
{
  #ifdef Debug_Json // Log what will be done
    Serial.print(F("\t🗞️ Reqested action: "));
    Serial.println(HTTPRequestReceived->RequestURI.substring(HTTPRequestReceived->RequestURI.lastIndexOf('&')+1));
  #endif
  // do the action
  bool Status = ManageAction(HTTPRequestReceived->RequestURI.substring(HTTPRequestReceived->RequestURI.lastIndexOf('&')+1));
  
  // send http response header
  client->println("HTTP/1.1 200 OK");
  client->println("Access-Control-Allow-Origin: *");
  client->println("Content-Type: application/json");
  client->println("Connection: Keep-Alive");
  client->println("Keep-Alive: max=2000, timeout=2000");
  client->println("");
  // send json
  client->println("{");
  client->print("\"Status\":\""); if(Status) client->print("OK"); else client->print("NOK"); client->println("\"");
  client->println("}");
  client->println("");
  client->println();
  
  #ifdef Debug_Json
    Serial.print(F("\t🗞️ json.htm sent with status ")); if(Status) Serial.println(F("OK")); else Serial.println(F("NOK"));
  #endif

}
//////////////////////////////////////////////////// Function doing action requested from GetActionToDoAndReturnFeedback
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT FROM YOUR WEBSITE ###
// Description:
//    This function is called from GetActionToDoAndReturnFeedback
//    function to do decoded action from client.
//    
////////////////////////////////////////////////////
bool ManageAction(String ActiontoDo)
{
    if(ActiontoDo.lastIndexOf("=") > 0)
    {
      int StateToApply=ActiontoDo.substring(ActiontoDo.lastIndexOf('=')+1).toInt();
      ActiontoDo = ActiontoDo.substring(0, ActiontoDo.lastIndexOf("="));
      #ifdef Debug_ActionManagement
        Serial.print(F("\t☑️ Ouput requested: "));
        Serial.println(ActiontoDo);
      #endif
      if (ActiontoDo == "MainState")
      {
            #ifdef Debug_ActionManagement
              Serial.print(F("\t☑️ MainState = ")); Serial.println(StateToApply);
            #endif
            if(StateToApply)
              TrigRelayTransition(true);
            else
              TrigRelayTransition(false);

            return 1; //Passed
      }
      else
      {
        #ifdef Debug_ActionManagement
          Serial.println(F("\t☑️ no valid output identified "));
        #endif
      }
    }
    else
    {
      #ifdef Debug_ActionManagement
        Serial.println(F("\t☑️ no value identified "));
      #endif
    }
    return 0; //Failed
}
///////////////////////////////////////////////////// HTTPReqDecoding function
// Description:
//    This function is called from GetAndManageClientRequest 
//    function to do extract from client request:
//       1. Method
//       2. RequestURI
//    
////////////////////////////////////////////////////
void HTTPReqMethodAndURIExtract(struct HTTPRequest *HTTPRequestReceived, String *HTTPRequestStart)
{
  
  if(HTTPRequestStart->length() >= 14) //if request make sense
  {
    // Extract Method
    String Extract;
    Extract = HTTPRequestStart->substring(0,HTTPRequestStart->indexOf(' '));

    if(Extract.equals(HttpMethod[GET])||Extract.equals(HttpMethod[POST]))
    {
      HTTPRequestReceived->Method = Extract; //update method

      // Extract URI
      Extract = HTTPRequestStart->substring(HTTPRequestStart->indexOf(' ')+1);
      #ifdef Debug_ReqDecoding
        Serial.print(F("\t\t⚙️ ")); Serial.println(HTTPRequestStart->substring(0));
      #endif
      Extract = Extract.substring(0,Extract.indexOf(' '));
      #ifdef Debug_ReqDecoding
        Serial.print(F("\t\t⚙️ ")); Serial.println(Extract);
      #endif      
      HTTPRequestReceived->RequestURI = Extract; //update URI

      /*// Extract Content-type //removed to save space
      if(HTTPRequestStart->indexOf("Content-type: "))
      {
        Extract = HTTPRequestStart->substring(HTTPRequestStart->indexOf("Content-type: "));
        Extract = Extract.substring(Extract.indexOf(" ")+1,Extract.indexOf('\n')-1);
        HTTPRequestReceived->ContentType = Extract; //update ContentType
      }
      else
        HTTPRequestReceived->ContentType = "";*/
    }
  }
  #ifdef Debug_DecodedReq
    Serial.print(F("\t🔎 Method: ")); Serial.println(String(HTTPRequestReceived->Method));
    Serial.print(F("\t🔎 RequestURI: ")); Serial.println(String(HTTPRequestReceived->RequestURI));
    //Serial.print(F("\t🔎 Content-type: ")); Serial.println(String(HTTPRequestReceived->ContentType));
  #endif
}
