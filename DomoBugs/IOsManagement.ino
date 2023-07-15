
//////////////////////////////////////////////////// IOs initialization
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    This function set direction of specified IOs
//    and call update of system IOs states
//    (variables for inputs & state for Outputs).
//    
////////////////////////////////////////////////////
void IOsInit()
{
    pinMode(MainState_pin, INPUT);  // Used to read main state
    pinMode(FaultState_pin, INPUT);  // Used to read fault state
    pinMode(ActivationRelay_pin, OUTPUT); 
    pinMode(DeActivationRelay_pin, OUTPUT);
    pinMode(AlertConnexion_pin, OUTPUT);    

    UpdateIOsState();
}

//////////////////////////////////////////////////// IOs state update
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    This function 
//    1. read and update related variables for inputs
//    2. set related outputs to update
//    
////////////////////////////////////////////////////
void UpdateIOsState()
{

    MainState = digitalRead(MainState_pin);
    FaultState=digitalRead(FaultState_pin);
    digitalWrite(AlertConnexion_pin, AlertState);
    digitalWrite(OptionRelay_pin, OptionRelay);
    BatteryVoltage= analogRead(Battery_pin)*(5.0 / 1023.0);  // Used to read battery voltage
    PowerSupplyVoltage = analogRead(PowerSupply_pin)*(5.0 / 1023.0);  // Used to read PowerSupply voltage
    Zone1Voltage= analogRead(Zone1_pin)*(5.0 / 1023.0);  // Used to read zone 1 voltage
    Zone2Voltage= analogRead(Zone2_pin)*(5.0 / 1023.0);  // Used to read zone 2 voltage
    Zone3Voltage= analogRead(Zone3_pin)*(5.0 / 1023.0);  // Used to read zone 3 voltage
    Zone4Voltage= analogRead(Zone4_pin)*(5.0 / 1023.0);  // Used to read zone 4 voltage
    Zone5Voltage= analogRead(Zone5_pin)*(5.0 / 1023.0);  // Used to read zone 5 voltage
    Zone6Voltage= analogRead(Zone6_pin)*(5.0 / 1023.0);  // Used to read zone 6 voltage
}

//////////////////////////////////////////////////// IOs states display
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    This function 
//    1. print all variables states on rs232 com
//    
////////////////////////////////////////////////////
void PrintIOsState()
{
    Serial.print(F("\t\tMain state: ")); Serial.println(String(MainState));
    Serial.print(F("\t\tFault state: ")); Serial.println(String(FaultState));
    Serial.print(F("\t\tAlert state: ")); Serial.println(String(AlertState));
    Serial.print(F("\t\tOption relay state: ")); Serial.println(String(OptionRelay));
    Serial.print(F("\t\tBattery state: ")); Serial.println(String(BatteryVoltage));
    Serial.print(F("\t\tPower supply state: ")); Serial.println(String(PowerSupplyVoltage));
    Serial.print(F("\t\tZone 1 state: ")); Serial.println(String(Zone1Voltage));
    Serial.print(F("\t\tZone 2 state: ")); Serial.println(String(Zone2Voltage));
    Serial.print(F("\t\tZone 3 state: ")); Serial.println(String(Zone3Voltage));
    Serial.print(F("\t\tZone 4 state: ")); Serial.println(String(Zone4Voltage));
    Serial.print(F("\t\tZone 5 state: ")); Serial.println(String(Zone5Voltage));
    Serial.print(F("\t\tZone 6 state: ")); Serial.println(String(Zone6Voltage));
}

//////////////////////////////////////////////////// Relay pulse
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    This function 
//    1. make a relay pulse depending of function parameter
//    
////////////////////////////////////////////////////
void TrigRelayTransition(bool transition)
{
    if (transition == true)
    {
        digitalWrite(ActivationRelay_pin, HIGH);
        delay(1000);
        digitalWrite(ActivationRelay_pin, LOW);
    }
    else
    {
        digitalWrite(DeActivationRelay_pin, HIGH);
        delay(1000);
        digitalWrite(DeActivationRelay_pin, LOW);
    }
}