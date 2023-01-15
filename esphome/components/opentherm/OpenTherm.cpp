/*
OpenTherm.cpp - OpenTherm Communication Library For Arduino, ESP8266
Copyright 2018, Ihor Melnyk
*/

#include "OpenTherm.h"
#include "esphome/core/log.h"

OpenTherm::OpenTherm(int inPin, int outPin, bool isSlave):
        status(OpenThermStatus::NOT_INITIALIZED),
        inPin(inPin),
        outPin(outPin),
        isSlave(isSlave),
        response(0),
        responseStatus(OpenThermResponseStatus::NONE),
        responseTimestamp(0),
        handleInterruptCallback(NULL),
        processResponseCallback(NULL)
{
}

void OpenTherm::begin(void(*handleInterruptCallback)(void), std::function<void(unsigned long, OpenThermResponseStatus)> &&processResponseCallback)
{
    pinMode(inPin, INPUT);
    pinMode(outPin, OUTPUT);
    if (handleInterruptCallback != NULL) {
        this->handleInterruptCallback = handleInterruptCallback;
        attachInterrupt(digitalPinToInterrupt(inPin), handleInterruptCallback, CHANGE);
    }
    activateBoiler();
    status = OpenThermStatus::READY;
    esphome::ESP_LOGD("opentherm", "initialized");
    this->processResponseCallback = processResponseCallback;
}

void OpenTherm::begin(void(*handleInterruptCallback)(void))
{
    begin(handleInterruptCallback, NULL);
}

bool ICACHE_RAM_ATTR OpenTherm::isReady()
{
    return status == OpenThermStatus::READY;
}

int ICACHE_RAM_ATTR OpenTherm::readState() {
    return digitalRead(inPin);
}

void OpenTherm::setActiveState() {
    digitalWrite(outPin, LOW);
}

void OpenTherm::setIdleState() {
    digitalWrite(outPin, HIGH);
}

void OpenTherm::activateBoiler() {
    setIdleState();
    delay(1000);
}

void OpenTherm::sendBit(bool high) {
    if (high) setActiveState(); else setIdleState();
    delayMicroseconds(500);
    if (high) setIdleState(); else setActiveState();
    delayMicroseconds(500);
}

bool OpenTherm::sendRequestAync(unsigned long request)
{
    //Serial.println("Request: " + String(request, HEX));
    noInterrupts();
    const bool ready = isReady();
    interrupts();

    if (!ready)
        return false;

    status = OpenThermStatus::REQUEST_SENDING;
    response = 0;
    responseStatus = OpenThermResponseStatus::NONE;

    sendBit(HIGH); //start bit
    for (int i = 31; i >= 0; i--) {
        sendBit(bitRead(request, i));
    }
    sendBit(HIGH); //stop bit
    setIdleState();

    status = OpenThermStatus::RESPONSE_WAITING;
    responseTimestamp = micros();
    return true;
}

unsigned long OpenTherm::sendRequest(unsigned long request)
{
    if (!sendRequestAync(request)) return 0;
    while (!isReady()) {
        process();
        yield();
    }
    return response;
}

bool OpenTherm::sendResponse(unsigned long request)
{
    status = OpenThermStatus::REQUEST_SENDING;
    response = 0;
    responseStatus = OpenThermResponseStatus::NONE;

    sendBit(HIGH); //start bit
    for (int i = 31; i >= 0; i--) {
        sendBit(bitRead(request, i));
    }
    sendBit(HIGH); //stop bit
    setIdleState();
    status = OpenThermStatus::READY;
    return true;
}

unsigned long OpenTherm::getLastResponse()
{
    return response;
}

OpenThermResponseStatus OpenTherm::getLastResponseStatus()
{
    return responseStatus;
}

void ICACHE_RAM_ATTR OpenTherm::handleInterrupt()
{
    if (isReady())
    {
        if (isSlave && readState() == HIGH) {
            status = OpenThermStatus::RESPONSE_WAITING;
        }
        else {
            return;
        }
    }

    unsigned long newTs = micros();
    if (status == OpenThermStatus::RESPONSE_WAITING) {
        if (readState() == HIGH) {
            status = OpenThermStatus::RESPONSE_START_BIT;
            responseTimestamp = newTs;
        }
        else {
            status = OpenThermStatus::RESPONSE_INVALID;
            responseTimestamp = newTs;
        }
    }
    else if (status == OpenThermStatus::RESPONSE_START_BIT) {
        if ((newTs - responseTimestamp < 750) && readState() == LOW) {
            status = OpenThermStatus::RESPONSE_RECEIVING;
            responseTimestamp = newTs;
            responseBitIndex = 0;
        }
        else {
            status = OpenThermStatus::RESPONSE_INVALID;
            responseTimestamp = newTs;
        }
    }
    else if (status == OpenThermStatus::RESPONSE_RECEIVING) {
        if ((newTs - responseTimestamp) > 750) {
            if (responseBitIndex < 32) {
                response = (response << 1) | !readState();
                responseTimestamp = newTs;
                responseBitIndex++;
            }
            else { //stop bit
                status = OpenThermStatus::RESPONSE_READY;
                responseTimestamp = newTs;
            }
        }
    }
}

void OpenTherm::process()
{
    noInterrupts();
    OpenThermStatus st = status;
    unsigned long ts = responseTimestamp;
    interrupts();

    if (st == OpenThermStatus::READY) return;
    unsigned long newTs = micros();
    if (st != OpenThermStatus::NOT_INITIALIZED && st != OpenThermStatus::DELAY && (newTs - ts) > 1000000) {
        status = OpenThermStatus::READY;
        responseStatus = OpenThermResponseStatus::TIMEOUT;
        if (processResponseCallback != NULL) {
            processResponseCallback(response, responseStatus);
        }
    }
    else if (st == OpenThermStatus::RESPONSE_INVALID) {
        status = OpenThermStatus::DELAY;
        responseStatus = OpenThermResponseStatus::INVALID;
        if (processResponseCallback != NULL) {
            processResponseCallback(response, responseStatus);
        }
    }
    else if (st == OpenThermStatus::RESPONSE_READY) {
        status = OpenThermStatus::DELAY;
        responseStatus = (isSlave ? isValidRequest(response) : isValidResponse(response)) ? OpenThermResponseStatus::SUCCESS : OpenThermResponseStatus::INVALID;
        if (processResponseCallback != NULL) {
            processResponseCallback(response, responseStatus);
        }
    }
    else if (st == OpenThermStatus::DELAY) {
        if ((newTs - ts) > 100000) {
            status = OpenThermStatus::READY;
        }
    }
}

bool OpenTherm::parity(unsigned long frame) //odd parity
{
    byte p = 0;
    while (frame > 0)
    {
        if (frame & 1) p++;
        frame = frame >> 1;
    }
    return (p & 1);
}

OpenThermMessageType OpenTherm::getMessageType(unsigned long message)
{
    OpenThermMessageType msg_type = static_cast<OpenThermMessageType>((message >> 28) & 7);
    return msg_type;
}

OpenThermMessageID OpenTherm::getDataID(unsigned long frame)
{
    return (OpenThermMessageID)((frame >> 16) & 0xFF);
}

unsigned long OpenTherm::buildRequest(OpenThermMessageType type, OpenThermMessageID id, unsigned int data)
{
    unsigned long request = data;
    if (type == OpenThermMessageType::WRITE_DATA) {
        request |= 1ul << 28;
    }
    request |= ((unsigned long)id) << 16;
    if (parity(request)) request |= (1ul << 31);
    return request;
}

unsigned long OpenTherm::buildResponse(OpenThermMessageType type, OpenThermMessageID id, unsigned int data)
{
    unsigned long response = data;
    response |= type << 28;
    response |= ((unsigned long)id) << 16;
    if (parity(response)) response |= (1ul << 31);
    return response;
}

bool OpenTherm::isValidResponse(unsigned long response)
{
    if (parity(response)) return false;
    byte msgType = (response << 1) >> 29;
    return msgType == READ_ACK || msgType == WRITE_ACK;
}

bool OpenTherm::isValidRequest(unsigned long request)
{
    if (parity(request)) return false;
    byte msgType = (request << 1) >> 29;
    return msgType == READ_DATA || msgType == WRITE_DATA;
}

void OpenTherm::end() {
    if (this->handleInterruptCallback != NULL) {
        detachInterrupt(digitalPinToInterrupt(inPin));
    }
}

const char *OpenTherm::statusToString(OpenThermResponseStatus status)
{
    switch (status) {
        case NONE:	return "NONE";
        case SUCCESS: return "SUCCESS";
        case INVALID: return "INVALID";
        case TIMEOUT: return "TIMEOUT";
        default:	  return "UNKNOWN";
    }
}

const char *OpenTherm::messageTypeToString(OpenThermMessageType message_type)
{
    switch (message_type) {
        case READ_DATA:	   return "READ_DATA";
        case WRITE_DATA:	  return "WRITE_DATA";
        case INVALID_DATA:	return "INVALID_DATA";
        case RESERVED:		return "RESERVED";
        case READ_ACK:		return "READ_ACK";
        case WRITE_ACK:	   return "WRITE_ACK";
        case DATA_INVALID:	return "DATA_INVALID";
        case UNKNOWN_DATA_ID: return "UNKNOWN_DATA_ID";
        default:			  return "UNKNOWN";
    }
}

//building requests

unsigned long OpenTherm::buildSetBoilerStatusRequest(bool enableCentralHeating, bool enableHotWater, bool enableCooling, bool enableOutsideTemperatureCompensation, bool enableCentralHeating2) {
    unsigned int data = enableCentralHeating | (enableHotWater << 1) | (enableCooling << 2) | (enableOutsideTemperatureCompensation << 3) | (enableCentralHeating2 << 4);
    data <<= 8;
    return buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Status, data);
}

unsigned long OpenTherm::buildSetBoilerTemperatureRequest(float temperature) {
    unsigned int data = temperatureToData(temperature);
    return buildRequest(OpenThermMessageType::WRITE_DATA, OpenThermMessageID::TSet, data);
}

unsigned long OpenTherm::buildGetBoilerTemperatureRequest() {
    return buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Tboiler, 0);
}

//parsing responses
bool OpenTherm::isFault(unsigned long response) {
    return response & 0x1;
}

bool OpenTherm::isCentralHeatingActive(unsigned long response) {
    return response & 0x2;
}

bool OpenTherm::isHotWaterActive(unsigned long response) {
    return response & 0x4;
}

bool OpenTherm::isFlameOn(unsigned long response) {
    return response & 0x8;
}

bool OpenTherm::isCoolingActive(unsigned long response) {
    return response & 0x10;
}

bool OpenTherm::isDiagnostic(unsigned long response) {
    return response & 0x40;
}

uint16_t OpenTherm::getUInt(const unsigned long response) {
    const uint16_t u88 = response & 0xffff;
    return u88;
}

float OpenTherm::getFloat(const unsigned long response) {
    const uint16_t u88 = getUInt(response);
    const float f = (u88 & 0x8000) ? -(0x10000L - u88) / 256.0f : u88 / 256.0f;
    return f;
}

unsigned int OpenTherm::temperatureToData(float temperature) {
    if (temperature < 0) temperature = 0;
    if (temperature > 100) temperature = 100;
    unsigned int data = (unsigned int)(temperature * 256);
    return data;
}

//basic requests

unsigned long OpenTherm::setBoilerStatus(bool enableCentralHeating, bool enableHotWater, bool enableCooling, bool enableOutsideTemperatureCompensation, bool enableCentralHeating2) {
    return sendRequest(buildSetBoilerStatusRequest(enableCentralHeating, enableHotWater, enableCooling, enableOutsideTemperatureCompensation, enableCentralHeating2));
}

bool OpenTherm::setBoilerTemperature(float temperature) {
    unsigned long response = sendRequest(buildSetBoilerTemperatureRequest(temperature));
    return isValidResponse(response);
}

float OpenTherm::getBoilerTemperature() {
    unsigned long response = sendRequest(buildGetBoilerTemperatureRequest());
    return isValidResponse(response) ? getFloat(response) : 0;
}

float OpenTherm::getReturnTemperature() {
    unsigned long response = sendRequest(buildRequest(OpenThermRequestType::READ, OpenThermMessageID::Tret, 0));
    return isValidResponse(response) ? getFloat(response) : 0;
}

bool OpenTherm::setDHWSetpoint(float temperature) {
    unsigned int data = temperatureToData(temperature);
    unsigned long response = sendRequest(buildRequest(OpenThermMessageType::WRITE_DATA, OpenThermMessageID::TdhwSet, data));
    return isValidResponse(response);
}

float OpenTherm::getDHWTemperature() {
    unsigned long response = sendRequest(buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Tdhw, 0));
    return isValidResponse(response) ? getFloat(response) : 0;
}

float OpenTherm::getModulation() {
    unsigned long response = sendRequest(buildRequest(OpenThermRequestType::READ, OpenThermMessageID::RelModLevel, 0));
    return isValidResponse(response) ? getFloat(response) : 0;
}

float OpenTherm::getPressure() {
    unsigned long response = sendRequest(buildRequest(OpenThermRequestType::READ, OpenThermMessageID::CHPressure, 0));
    return isValidResponse(response) ? getFloat(response) : 0;
}

unsigned char OpenTherm::getFault() {
    return ((sendRequest(buildRequest(OpenThermRequestType::READ, OpenThermMessageID::ASFflags, 0)) >> 8) & 0xff);
}
const char *OpenTherm::messageIdToString(OpenThermMessageID message_id) {
    switch (message_id) {
        case Status: return "Status:"; // flag8 / flag8  Master and Slave Status flags.
            case TSet: return "TSet:"; // f8.8  Control setpoint  ie CH  water temperature setpoint (°C)
            case MConfigMMemberIDcode: return "MConfigMMemberIDcode:"; // flag8 / u8  Master Configuration Flags /  Master MemberID Code
            case SConfigSMemberIDcode: return "SConfigSMemberIDcode:"; // flag8 / u8  Slave Configuration Flags /  Slave MemberID Code
            case Command: return "Command:"; // u8 / u8  Remote Command
            case ASFflags: return "ASFflags:"; // / OEM-fault-code  flag8 / u8  Application-specific fault flags and OEM fault code
            case RBPflags: return "RBPflags:"; // flag8 / flag8  Remote boiler parameter transfer-enable & read/write flags
            case CoolingControl: return "CoolingControl:"; // f8.8  Cooling control signal (%)
            case TsetCH2: return "TsetCH2:"; // f8.8  Control setpoint for 2e CH circuit (°C)
            case TrOverride: return "TrOverride:"; // f8.8  Remote override room setpoint
            case TSP: return "TSP:"; // u8 / u8  Number of Transparent-Slave-Parameters supported by slave
            case TSPindexTSPvalue: return "TSPindexTSPvalue:"; // u8 / u8  Index number / Value of referred-to transparent slave parameter.
            case FHBsize: return "FHBsize:"; // u8 / u8  Size of Fault-History-Buffer supported by slave
            case FHBindexFHBvalue: return "FHBindexFHBvalue:"; // u8 / u8  Index number / Value of referred-to fault-history buffer entry.
            case MaxRelModLevelSetting: return "MaxRelModLevelSetting:"; // f8.8  Maximum relative modulation level setting (%)
            case MaxCapacityMinModLevel: return "MaxCapacityMinModLevel:"; // u8 / u8  Maximum boiler capacity (kW) / Minimum boiler modulation level(%)
            case TrSet: return "TrSet:"; // f8.8  Room Setpoint (°C)
            case RelModLevel: return "RelModLevel:"; // f8.8  Relative Modulation Level (%)
            case CHPressure: return "CHPressure:"; // f8.8  Water pressure in CH circuit  (bar)
            case DHWFlowRate: return "DHWFlowRate:"; // f8.8  Water flow rate in DHW circuit. (litres/minute)
            case DayTime: return "DayTime:"; // special / u8  Day of Week and Time of Day
            case Date: return "Date:"; // u8 / u8  Calendar date
            case Year: return "Year:"; // u16  Calendar year
            case TrSetCH2: return "TrSetCH2:"; // f8.8  Room Setpoint for 2nd CH circuit (°C)
            case Tr: return "Tr:"; // f8.8  Room temperature (°C)
            case Tboiler: return "Tboiler:"; // f8.8  Boiler flow water temperature (°C)
            case Tdhw: return "Tdhw:"; // f8.8  DHW temperature (°C)
            case Toutside: return "Toutside:"; // f8.8  Outside temperature (°C)
            case Tret: return "Tret:"; // f8.8  Return water temperature (°C)
            case Tstorage: return "Tstorage:"; // f8.8  Solar storage temperature (°C)
            case Tcollector: return "Tcollector:"; // f8.8  Solar collector temperature (°C)
            case TflowCH2: return "TflowCH2:"; // f8.8  Flow water temperature CH2 circuit (°C)
            case Tdhw2: return "Tdhw2:"; // f8.8  Domestic hot water temperature 2 (°C)
            case Texhaust: return "Texhaust:"; // s16  Boiler exhaust temperature (°C)
            case TdhwSetUBTdhwSetLB: return "TdhwSetUBTdhwSetLB"; // s8 / s8  DHW setpoint upper & lower bounds for adjustment  (°C)
            case MaxTSetUBMaxTSetLB: return "MaxTSetUBMaxTSetLB"; // s8 / s8  Max CH water setpoint upper & lower bounds for adjustment  (°C)
            case HcratioUBHcratioLB: return "HcratioUBHcratioLB"; // s8 / s8  OTC heat curve ratio upper & lower bounds for adjustment
            case TdhwSet: return "TdhwSet"; // f8.8  DHW setpoint (°C)    (Remote parameter 1)
            case MaxTSet: return "MaxTSet:"; // f8.8  Max CH water setpoint (°C)  (Remote parameters 2)
            case Hcratio: return "Hcratio:"; // f8.8  OTC heat curve ratio (°C)  (Remote parameter 3)
            case RemoteOverrideFunction: return "RemoteOverrideFunction"; // flag8 / -  Function of manual and program changes in master and remote room setpoint.
            case OEMDiagnosticCode: return "OEMDiagnosticCode"; // u16  OEM-specific diagnostic/service code
            case BurnerStarts: return "BurnerStarts:"; // u16  Number of starts burner
            case CHPumpStarts: return "CHPumpStarts:"; // u16  Number of starts CH pump
            case DHWPumpValveStarts: return "DHWPumpValveStarts:"; // u16  Number of starts DHW pump/valve
            case DHWBurnerStarts: return "DHWBurnerStarts:"; // u16  Number of starts burner during DHW mode
            case BurnerOperationHours: return "BurnerOperationHours:"; // u16  Number of hours that burner is in operation (i.e. flame on)
            case CHPumpOperationHours: return "CHPumpOperationHours:"; // u16  Number of hours that CH pump has been running
            case DHWPumpValveOperationHours: return "DHWPumpValveOperationHours:"; // u16  Number of hours that DHW pump has been running or DHW valve has been opened
            case DHWBurnerOperationHours: return "DHWBurnerOperationHours:"; // u16  Number of hours that burner is in operation during DHW mode
            case OpenThermVersionMaster: return "OpenThermVersionMaster:"; // f8.8  The implemented version of the OpenTherm Protocol Specification in the master.
            case OpenThermVersionSlave: return "OpenThermVersionSlave:"; // f8.8  The implemented version of the OpenTherm Protocol Specification in the slave.
            case MasterVersion: return "MasterVersion:"; // u8 / u8  Master product version number and type
            case SlaveVersion: return "SlaveVersion:"; // u8 / u8  Slave product version number and type
            default:
            return "Unknown message";
    }
}
