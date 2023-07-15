//#define Debug_PeriodicConnectionState // 🟫
//#define Debug_PeriodicReq // 📰
//#define Debug_PeriodicFeedback // 🗞️
//#define Debug_PeriodicStat // ℹ️

EthernetClient RemoteClientToUpdate;  // client init

#ifdef Debug_PeriodicStat
  // Variables to measure the speed
  unsigned long beginMicros, endMicros;
#endif

int RemoteStateRequestNumberToSend = 0;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Periodic request function
// Description:
//    This function open a connection with the remote
//    and call the function to make hhtp request that
//    will udpate the client.
//    
/////////////////////////////////////////////////////
void UpdateRemoteServer()
{
  #ifdef Debug_PeriodicConnectionState
    Serial.print(F("🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫 Connecting to ")); // for debug
    Serial.print(RemoteSystemIP[0],DEC); Serial.print("."); Serial.print(RemoteSystemIP[1],DEC); Serial.print("."); Serial.print(RemoteSystemIP[2],DEC); Serial.print("."); Serial.print(RemoteSystemIP[3],DEC);
    Serial.print(F(":"));
    Serial.println(RemoteSystemPort);
  #endif
  // if you get a connection, report back via serial:
  if (RemoteClientToUpdate.connect(RemoteSystemIP, RemoteSystemPort)) {
    #ifdef Debug_PeriodicConnectionState
      Serial.println(F("\tConnected ✅"));
    #endif
    // Make a HTTP request:
    SendStateToRemote();
  } else {
    // if you didn't get a connection to the server:
    #ifdef Debug_PeriodicConnectionState
      Serial.println(F("\tConnection failed ❌"));
    #endif
  }
  #ifdef Debug_PeriodicStat
    beginMicros = micros();
  #endif
}
///////////////////////////////////////////////////// Send request to remote
// Description:
//    This function make an http request with one of
//    informations to send to remote client.
//    Informations are sent one by one at each tick of
//    predefined timer, avoiding to stay too much time
//    lock in this function.
//    
/////////////////////////////////////////////////////
void SendStateToRemote()
{
    if(NumberOfDeclaredRequest > 0) { // if there is requests declared
      // Send current state req
      RemoteClientToUpdate.print("GET /");
      RemoteClientToUpdate.print(UpdateStateRequest[RemoteStateRequestNumberToSend].RemoteURI);
      RemoteClientToUpdate.print(ComputeStateToSend(RemoteStateRequestNumberToSend));
      RemoteClientToUpdate.println(" HTTP/1.1");
    } else { // No existing requests declared
      #ifdef Debug_PeriodicReq
        Serial.println(F("\t📰🔴 No request to send"));
      #endif
    }
    // Close connection
    RemoteClientToUpdate.print("Host: ");
    RemoteClientToUpdate.print(RemoteSystemIP[0],DEC); RemoteClientToUpdate.print("."); RemoteClientToUpdate.print(RemoteSystemIP[1],DEC); RemoteClientToUpdate.print("."); RemoteClientToUpdate.print(RemoteSystemIP[2],DEC); RemoteClientToUpdate.print("."); RemoteClientToUpdate.println(RemoteSystemIP[3],DEC);
    RemoteClientToUpdate.println("Connection: close");
    RemoteClientToUpdate.println();
    #ifdef Debug_PeriodicReq
      Serial.print(F("\t📰 Request: "));
      Serial.print(UpdateStateRequest[RemoteStateRequestNumberToSend].VariableName); Serial.print(F(", ")); Serial.print(UpdateStateRequest[RemoteStateRequestNumberToSend].RemoteURI); Serial.println(ComputeStateToSend(RemoteStateRequestNumberToSend));
    #endif

    // Update RemoteStateRequestNumberToSend to prepare next state req to send
    if(RemoteStateRequestNumberToSend<NumberOfDeclaredRequest)
      RemoteStateRequestNumberToSend++;
    else
      RemoteStateRequestNumberToSend=0;
}
///////////////////////////////////////////////////// ComputeStateToSend
// ### 🚧 HAVE TO BE ADAPTED TO CONTENT TO SEND ###
// Description:
//    This function format requested information, based
//    on the name of the information to send.
//    * State should return a On/Off info
//    * Relay should return a On/Off info
//    * Voltage should return a numeric value
//    * Current should return a numeric value
//    * Power should return a numeric value
//    
/////////////////////////////////////////////////////
String ComputeStateToSend(int RemoteStateRequestNumberToCompute)
{
      if (String("MainState") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
        if(MainState==1)
          return "On";
        else
          return "Off";
      }
      if (String("FaultState") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
        if(FaultState==1)
          return "On";
        else
          return "Off";
      }
      if (String("AlertState") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
        if(AlertState==1)
          return "On";
        else
          return "Off";
      }      
      if (String("OptionRelay") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
        if(OptionRelay==1)
          return "On";
        else
          return "Off";
      }
      if (String("BatteryVoltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(BatteryVoltage);
      }
      if (String("PowerSupplyVoltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(PowerSupplyVoltage);
      }
      if (String("Zone1Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone1Voltage);
      }
      if (String("Zone2Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone2Voltage);
      }
      if (String("Zone3Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone3Voltage);
      }
      if (String("Zone4Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone4Voltage);
      }
      if (String("Zone5Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone5Voltage);
      }
      if (String("Zone6Voltage") == UpdateStateRequest[RemoteStateRequestNumberToCompute].VariableName)
      {
          return String(Zone6Voltage);
      }
      return "";
}
///////////////////////////////////////////////////// Get Remote Server feedback
// Description:
//    This function get http feedback before to close
//    the connection.
//    🚧 Remote feedback (buffer) is currently 
//    untreated.
//    
/////////////////////////////////////////////////////
bool GetRemoteServerFeedback()
{
  unsigned long byteCount = 0;
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  int len = RemoteClientToUpdate.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    RemoteClientToUpdate.read(buffer, len);
    #ifdef Debug_PeriodicFeedback // 🗞️
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    #endif
    byteCount = byteCount + len;
  }

  // if the server's disconnected, stop the RemoteClientToUpdate:
  if (!RemoteClientToUpdate.connected()) {
    #ifdef Debug_PeriodicStat
      endMicros = micros();
    #endif
    RemoteClientToUpdate.stop();
    #ifdef Debug_PeriodicFeedback // 🗞️
      Serial.println(F("")); // to make a clean reporting at the end of recieved req displaying
    #endif    
    #ifdef Debug_PeriodicConnectionState
      Serial.println(F("🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫🟫 Connection closed")); // for debug
    #endif
    #ifdef Debug_PeriodicStat
      Serial.print(F("ℹ️ Received "));
      Serial.print(byteCount);
      Serial.print(F(" bytes in "));
      float seconds = (float)(endMicros - beginMicros) / 1000000.0;
      Serial.print(seconds, 4);
      float rate = (float)byteCount / seconds / 1000.0;
      Serial.print(F(", rate = "));
      Serial.print(rate);
      Serial.println(F(" kbytes/second"));
    #endif
    return false;
  } 
  else
    return true; 
}