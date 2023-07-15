
//////////////////////////////////////////////////// Constant to define default configuration file
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    This definition should contain:
//       - LocalServerIP
//       - LocalServerPort
//       - RemoteSystemIP
//       - RemoteSystemPort
//       - StateUpdateReq corresponding to IO local name, then remote url where info should be sent
//       - ...
//    
////////////////////////////////////////////////////
const char ConfigFileTemplate[] PROGMEM = ("SystemIP=192.168.1.xxx\nLocalServerPort=80\nRemoteSystemIP=192.168.1.xxx\nRemoteSystemPort=8080\nMainState,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nFaultState,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nOptionRelay,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nBatteryVoltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nPowerSupplyVoltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone1Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone2Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone3Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone4Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone5Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\nZone6Voltage,json.htm?type=command&param=switchlight&idx={remoteID}&switchcmd=\n");


//////////////////////////////////////////////////// Function reading and calling loading system configuration update function
// Description:
//    This function is called from initialization
//    function to load system configuration file and
//    call sub function updating related variables.
//    
////////////////////////////////////////////////////
bool SystemConfigurationsLoading()
{
    // Check & Load file
    if (CheckFileExist(ConfigFileName)) { //if file was found then
        Serial.print(F("\tSUCCESS - Found ")); Serial.print(ConfigFileName); Serial.println(F(" file."));
        
        // open file
        File32 FileStream;
        LoadFileFromSDCard(ConfigFileName, &FileStream);        // try to load System Config file
        
        // read file
        String ExtractedLine;
        while(FileStream.available()) {
            ExtractedLine = FileStream.readStringUntil('\n'); // read config
            LoadSystemConfiguration(ExtractedLine);
        }
        FileStream.close();
        return true;
    }
    else { //if file was not found then
          Serial.print(F("\tERROR - Can't find ")); Serial.print(ConfigFileName); Serial.println(F(" file!"));
          if (!CheckFileExist(ConfigFileName+".template")) {
              File32 FileStream;
              CreateFile(ConfigFileName+".template", &FileStream);
              FileStream.write(ConfigFileTemplate);
                      
              // close the file:
              FileStream.close();
              Serial.print(F("\tTemplate created. Please update it and rename it "));  Serial.println(ConfigFileName);
          }
          else
              Serial.print(F("\tPlease update template and rename it "));  Serial.println(ConfigFileName);

          return false;
    }
    return true;
}

//////////////////////////////////////////////////// Function loading system configuration
// Description:
//    This function is called from SystemConfigurationsLoading
//    function to load system configuration in corresponding
//    system variable.
//    
////////////////////////////////////////////////////
bool LoadSystemConfiguration(String ExtractedLine)
{
    String ExtractedConfig;
    String ExtractedConfigValue; 

    ExtractedConfig = ExtractedLine.substring(0, ExtractedLine.indexOf("="));

    if (ExtractedConfig == "SystemIP") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      for(int i=0; i<4; i++) { //Extract IP address into LocalServerIP array
        LocalServerIP[i]=ExtractedConfigValue.substring(0, ExtractedConfigValue.indexOf(".")).toInt();
        ExtractedConfigValue=ExtractedConfigValue.substring(ExtractedConfigValue.indexOf(".")+1);
      }
      Serial.print(F("\t\tSystemIP config found => "));
      ExtractedConfigValue = String(LocalServerIP[0]) + "." + String(LocalServerIP[1])+ "." + String(LocalServerIP[2])+ "." + String(LocalServerIP[3]);
      Serial.println(ExtractedConfigValue);
      return 1;
    }
    if (ExtractedConfig == "LocalServerPort") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      LocalServerPort=ExtractedConfigValue.toInt();
      Serial.print(F("\t\tLocalServerPort config found => "));
      Serial.println(LocalServerPort);
      return 1;
    }
    // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    if (ExtractedConfig == "AlertState") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      AlertState=ExtractedConfigValue.toInt();
      Serial.print(F("\t\tAlertState config found => "));
      Serial.println(AlertState);
      return 1;
    }    
    if (ExtractedConfig == "OptionRelay") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      OptionRelay=ExtractedConfigValue.toInt();
      Serial.print(F("\t\tOptionRelay config found => "));
      Serial.println(OptionRelay);
      return 1;
    }
    // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
    if (ExtractedConfig == "RemoteSystemIP") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      for(int i=0; i<4; i++) { //Extract IP address into RemoteSystemIP array
        RemoteSystemIP[i]=ExtractedConfigValue.substring(0, ExtractedConfigValue.indexOf(".")).toInt();
        ExtractedConfigValue=ExtractedConfigValue.substring(ExtractedConfigValue.indexOf(".")+1);
      }
      Serial.print(F("\t\tRemoteSystemIP config found => "));
      ExtractedConfigValue = String(RemoteSystemIP[0]) + "." + String(RemoteSystemIP[1])+ "." + String(RemoteSystemIP[2])+ "." + String(RemoteSystemIP[3]);
      Serial.println(ExtractedConfigValue);
      return 1;
    }
    if (ExtractedConfig == "RemoteSystemPort") {
      ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
      RemoteSystemPort=ExtractedConfigValue.toInt();
      Serial.print(F("\t\tRemoteSystemPort config found => "));
      Serial.println(RemoteSystemPort);
      return 1;
    }
    if (ExtractedConfig == "StateUpdateReq") {
      if (NumberOfDeclaredRequest < MaximumNumberOfRequest)
      {
        ExtractedConfigValue = ExtractedLine.substring(ExtractedLine.indexOf('=')+1);
        UpdateStateRequest[NumberOfDeclaredRequest].VariableName=ExtractedConfigValue.substring(0, ExtractedConfigValue.indexOf(","));
        UpdateStateRequest[NumberOfDeclaredRequest].RemoteURI=ExtractedConfigValue.substring(ExtractedConfigValue.indexOf(",")+1);
        UpdateStateRequest[NumberOfDeclaredRequest].RemoteURI.trim();
        Serial.print(F("\t\tStateUpdateReq ")); Serial.print(NumberOfDeclaredRequest); Serial.println(F(" found =>"));
        Serial.print(F("\t\t\t")); Serial.print(UpdateStateRequest[NumberOfDeclaredRequest].VariableName); Serial.print(F(": ")); Serial.println(UpdateStateRequest[NumberOfDeclaredRequest].RemoteURI);
        NumberOfDeclaredRequest++;
        return 1;
      } else {
          Serial.print(F("\t\t🔴 Too many StateUpdateReq declared => ")); Serial.print(NumberOfDeclaredRequest); Serial.print(F("/")); Serial.println(MaximumNumberOfRequest);
        return 0;
      }
    }

    // if config was not identified then
    Serial.print(F("\t\tUnknown or invalid config: ")); Serial.println(ExtractedLine);  
    return 0;   
}
