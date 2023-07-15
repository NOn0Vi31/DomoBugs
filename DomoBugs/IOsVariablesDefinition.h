///////////////////////////////////////////////////// System IOs config
// ### 🚧 HAVE TO BE ADAPTED TO CAPACITIES YOU WANT ###
// Description:
//    Define here HW def for each IOs you want to use
//    
////////////////////////////////////////////////////
const int ActivationRelay_pin = 30;
const int DeActivationRelay_pin = 31;

const int MainState_pin = 22;
int MainState = 0;
const int FaultState_pin = 23;
int FaultState = 0;
const int AlertConnexion_pin = 24;
int AlertState = 0;
const int OptionRelay_pin = 25;
int OptionRelay = 0;

const int Battery_pin = A0;
float BatteryVoltage;
const int PowerSupply_pin = A1;
float PowerSupplyVoltage;
const int Zone1_pin = A2;
float Zone1Voltage;
const int Zone2_pin = A3;
float Zone2Voltage;
const int Zone3_pin = A4;
float Zone3Voltage;
const int Zone4_pin = A5;
float Zone4Voltage;
const int Zone5_pin = A6;
float Zone5Voltage;
const int Zone6_pin = A7;
float Zone6Voltage;
