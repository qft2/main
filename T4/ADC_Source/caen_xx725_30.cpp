//#define DEBUG_XX725_30

#include "caen_xx725_30.h"
#include <iostream> //cout
#include <iomanip> //setw, setfill
#include <unistd.h> //usleep
#include <cstdio> //printf()

//#define DEBUG_XX725_30

using namespace std;

/**
\param interface use caencomm_interface if the device is connected via USB or CONET2
*/
caen_xx725_30::caen_xx725_30(caen_xx725_30::N_CHN n_channel, vme_interface *interface, vme_addr_t baseaddr):isInit(false) {
    if(n_channel != CHN8 && n_channel != CHN16) 
        { throw llbad_caen_xx725_30("The device does not support the requested number of channels", n_channel); }
    
    this->n_channel = n_channel;    
    ba = baseaddr;
    vme = interface;
    readRom = false;
}

/**
* call this function to initialize the device
* initializes the device and requests:
*    - configuration rom
*    - board info
* check the equipped number of channels
* calibrates the device
*/
void caen_xx725_30::init() {
    #ifdef DEBUG_XX725_30
	   cout << "Started initialization." << endl;
    #endif
    readConfigurationROM();

    info = getBoardInfo();

    checkEquippedChannel(info);
    
    calibrate();

    maxBltEvents= getMaxEventsPerBLT();
    #ifdef DEBUG_XX725_30
        cout << "finished initialization" << endl;
    #endif
	isInit=true;
}

/**
* read the complete configurationROM
*/
void caen_xx725_30::readConfigurationROM() {
    int ret;
	vme_claimer claim(vme,AM_A32);
    //manually increment by 4 between each configurationROM entry
    for(int i = 0; i < 21; i++) {
    	if ( (ret=vme->read32(ba+CONFIGURATION_ROM+i*4, &(rom.raw[i]), 1)) < 1 ){
		printf("ret=%x\n",ret);
    		throw llbad_caen_xx725_30_vme_error("VME read failed in readConfigurationROM()", ret); 
    	}
    }
    for(int i = 0; i < 3; i++) {
    	if ( (ret=vme->read32(ba+CONFIGURATION_ROM2+i*4, &(rom.raw[21+i]), 1)) < 1 ){
		printf("ret=%x\n",ret);
    		throw llbad_caen_xx725_30_vme_error("VME read failed in readConfigurationROM()", ret); 
    	}    
    }
}

caen_xx725_30::N_CHN caen_xx725_30::getNChannel() {
    return n_channel;
}

/**
* throws an exception if the channel is busy
*/
void caen_xx725_30::checkIsChannelBusy(int channel) {
    chn_status status = getChannelStatus(channel);
    if(status.dac_busy) //1 == busy
        {throw llbad_caen_xx725_30_busy("Operation not possible. Channel is busy: ", channel);}
}



/**
* throws an exception if at least one channel is busy
*/
void caen_xx725_30::checkIsAnyChannelBusy() {
    for(int i = 0; i < n_channel; i++) {
        chn_status status = getChannelStatus(i);
        if(status.dac_busy) {
            throw llbad_caen_xx725_30_busy("Operation not possible. At least one channel is busy.");
        }
    }
}

bool caen_xx725_30::isRunning() {
    return getACQStatus().run;
}

vme_interface* caen_xx725_30::getInterface() {
    return vme;
}

vme_addr_t caen_xx725_30::getBaseAddress() {
    return ba;
}

/**
* starts acquisition
* the board has to be in the software control mode (\see acq_control), ready for acqusition and must not be already running
*/
void caen_xx725_30::start() {
    acq_control control = getACQControl();
    if(control.mode != 0) {
        throw llbad_caen_xx725_30("start(): device is not in software control mode.");
    }
    
    //check if the device is in the right status
    acq_status status = getACQStatus();
    if(status.run) {
        throw llbad_caen_xx725_30("start(): device is already running.");   
    }
    if(!status.board_ready) {
        throw llbad_caen_xx725_30("start(): device is not ready for acqusition.");   
    }
    
    //start acquistion
    control.start=1;
    setACQControl(control);
}

/**
* stops the acquistion if the device is running
* throws an exception if the device is not running
*/
void caen_xx725_30::stop() {
    acq_status status = getACQStatus();
    if(!status.run) {
        throw llbad_caen_xx725_30("stop(): device is not running.");   
    }
    
    acq_control control = getACQControl();    
    if(control.mode != 0) {
        throw llbad_caen_xx725_30("stop(): device is not in software control mode.");
    }
    control.start=0;
    setACQControl(control);    
}

bool caen_xx725_30::isEventReady() {
    return getACQStatus().event_ready;
}

uint32_t caen_xx725_30::getNextEventSize() {
	uint32_t ret = getEventSize();
	if(ret==0) throw llbad_caen_xx725_30("getNextEventSize(): size is zero, no event in fifo!");
	return ret;
} 
vme_addr_t caen_xx725_30::getEventReadoutAddress() {
    return ba+READOUT_BUFFER;
}

int caen_xx725_30::getMaxEventsPerBLT() {
    return getMaxNumberEventsPerBLT();
}
void caen_xx725_30::setMaxEventsPerBLT(int number) {
    setMaxNumberEventsPerBLT(number);
}

bool caen_xx725_30::isBerrEnabled() {
    return getReadoutControl().vme_berr;
}

/**
* Sets the Dynamic Input Range for a channel
* \param range[0] = 0 -> 2 Vpp (default), range[0] = 1 -> 0.5 Vpp
*/
void caen_xx725_30::setChannelInputRange(int channel, uint8_t range) {
    checkIsValidChannel(channel);
    writeReg(CHN_INPUT_DYN_RANGE+channel*CHN_MULTIPLIER, range);
}
void caen_xx725_30::setAllChannelsInputRange(uint8_t range) {
    writeReg(CHN_INPUT_DYN_RANGE_BC, range);
}
/**
* gets the dynamic input range for a channel
*/
uint8_t caen_xx725_30::getChannelInputRange(int channel) {
    checkIsValidChannel(channel);      
    return readReg(ba+CHN_INPUT_DYN_RANGE+channel*CHN_MULTIPLIER);	
}

/**
*  sets the width of the pulse generated when the input signal on the channel crosses the threshold. The
value of this register is given in units of trigger clock (i.e. 8 ns).
NOTE: this setting is meaninful if bit[2] = 0 in the Self-Trigger Logic register.
NOTE: this register is implemented only from AMC FPGA firmware revision 0.2 on.
*/
void caen_xx725_30::setChannelPulseWidth(int channel, uint8_t width) {
    checkIsValidChannel(channel);
    checkAMCFirmware("setAllChannelsPulseWidth", getChannelAMCFirmware(channel), 0, 2);
    writeReg(CHN_PULSE_WIDTH+channel*CHN_MULTIPLIER, width);
}

void caen_xx725_30::setAllChannelsPulseWidth(uint8_t width) {
    checkAMCFirmware("setAllChannelsPulseWidth", getChannelAMCFirmware(0), 0, 2);
    writeReg(CHN_PULSE_WIDTH_BC, width);
}

/**
* \see setChannelPulseWidth
*/
uint8_t caen_xx725_30::getChannelPulseWidth(int channel) {
    checkIsValidChannel(channel);   
    checkAMCFirmware("getChannelPulseWidth", getChannelAMCFirmware(channel), 0, 2);
    return readReg(CHN_PULSE_WIDTH+channel*CHN_MULTIPLIER);
}

/**
* The channel is able to generate a self-trigger signal when the digi zed input pulse exceeds a configurable threshold Vth. This register allows to set Vth individually for each channel.
\param threshold [13:0] = Trigger threshold value in LSB (default = 0)
*/
void caen_xx725_30::setChannelTriggerThreshold(int channel, uint16_t threshold) {
    checkIsValidChannel(channel);
    writeReg(CHN_TRG_THRESHOLD+channel*CHN_MULTIPLIER, threshold);
}
void caen_xx725_30::setAllChannelsTriggerThreshold(uint16_t threshold) {
    writeReg(CHN_TRG_THRESHOLD_BC, threshold);
}
/**
* \see setChannelTriggerThreshold
*/
uint16_t caen_xx725_30::getChannelTriggerThreshold(int channel) {
    checkIsValidChannel(channel);  
    return readReg(CHN_TRG_THRESHOLD+channel*CHN_MULTIPLIER);
}

/**
* \param couple \see checkIsValidCouple
For a couple of channels of adjacent channels, this register sets the logic to generate the trigger request signal upon the self-triggers from the two channels of the couple. 
*/
void caen_xx725_30::setCoupleSelfTriggerLogic(int couple, couple_self_trigger_logic logic) { 
    checkAMCFirmware("setCoupleSelfTriggerLogic", getChannelAMCFirmware(2*couple), 0, 2);
    checkIsValidCouple(couple);
    writeReg(COUPLE_SELF_TRG_LOGIC+2*couple*CHN_MULTIPLIER, logic.raw);
}

void caen_xx725_30::setAllCouplesSelfTriggerLogic(couple_self_trigger_logic logic) {
    checkAMCFirmware("setAllCouplesSelfTriggerLogic", getChannelAMCFirmware(0), 0, 2);
    writeReg(COUPLE_SELF_TRG_LOGIC_BC, logic.raw);
}

/**
* \param couple \see checkIsValidCouple
* \see setCoupleSelfTriggerLogic
*/
caen_xx725_30::couple_self_trigger_logic caen_xx725_30::getCoupleSelfTriggerLogic(int couple) {
    checkIsValidCouple(couple);    
    couple_self_trigger_logic logic;
    logic.raw = readReg(COUPLE_SELF_TRG_LOGIC+2*couple*CHN_MULTIPLIER);
	return logic;
}

/**
* gets the status information for one channel
*/
caen_xx725_30::chn_status caen_xx725_30::getChannelStatus(int channel) {
    checkIsValidChannel(channel);  
    chn_status status;     
	status.raw = readReg(CHN_TRG_THRESHOLD+channel*CHN_MULTIPLIER);
	return status;
}

void caen_xx725_30::printChannelStatus(chn_status chn_stat){
    cout << "Channel Status:\n";

    if(chn_stat.mem_full) {
       cout << "  Memory full\n";
    }
    if(chn_stat.mem_empty) {
       cout << "  Memory empty\n";
    }
    if(chn_stat.calib_status){
       cout << "  Calibration done\n";
    }
    if(chn_stat.adc_power_down){
       cout << "  DAC busy\n";
    }else{
       cout << "  DC offset updated\n";
    }
}

/**
* get AMC firmware revision for one channel
*/
caen_xx725_30::amc_firmware caen_xx725_30::getChannelAMCFirmware(int channel) {
    checkIsValidChannel(channel);
    amc_firmware firmware;
    firmware.raw = readReg(CHN_AMC_FIRMWARE+channel*CHN_MULTIPLIER);
    return firmware;
}

/**
* print the given AMC firmware on standard output
*/
void caen_xx725_30::printAMCFirmware(amc_firmware firmware) {
    cout << "revision: " << firmware.major << "." << firmware.minor << ", ";
    cout << setw(2) << setfill('0');
    cout << firmware.day << "." << firmware.month;
    cout << ".(" << 2000+firmware.year << " or " << 2016+firmware.year << ")\n"; 
}

/**
* \param offset DC offset Level (default: 0x8000)
* it is recommended to wait a few seconds before a new run    
*/
void caen_xx725_30::setChannelDCOffset(int channel, uint16_t offset) {   
    checkIsValidChannel(channel);
    checkIsChannelBusy(channel);
    writeReg(CHN_DC_OFFSET+channel*CHN_MULTIPLIER, offset);
}
/**
* it is recommended to wait a few seconds after setting channel DC offset before a new run
*/
void caen_xx725_30::setAllChannelsDCOffset(uint16_t offset) {
    checkIsAnyChannelBusy();
    writeReg(CHN_DC_OFFSET_BC, offset);
}

uint16_t caen_xx725_30::getChannelDCOffset(int channel) {
    checkIsValidChannel(channel);
    return readReg(CHN_DC_OFFSET+channel*CHN_MULTIPLIER);
}

/**
* gets ADC Chip temperature in degree celsius
*/
uint8_t caen_xx725_30::getChannelADCTemperature(int channel) {
    checkIsValidChannel(channel); 
    return readReg(CHN_ADC_TEMP+channel*CHN_MULTIPLIER);
}


/**
* sets the board config
* bit[0]: must be zero
* bit[1]: trigger overlap setting (default: 0)
* bit[2]: must be zero
* bit[3]: test pattern enable (default: 0)
* bit[4]: must be one
* bit[5]: must be zero
* bit[6]: self-trigger polarity (default: 0)
* bit[23,10,8:7]: must be zero
*/
void caen_xx725_30::setBoardConfig(board_config config) {    
    config.raw = (config.raw & 0x4A) | 0x10; //ensure zeros and one as stated in the manual
    writeReg(BOARD_CONFIG, config.raw);  
}

caen_xx725_30::board_config caen_xx725_30::getBoardConfig() {
    board_config config;
    config.raw = readReg(BOARD_CONFIG);
    return config;
}

/**
* set buffer organization, see manual for detailed description
* setting the buffer causes a software clear
*/
void caen_xx725_30::setBufferOrganization(uint8_t bufferCode) {    
    bufferCode = (bufferCode & 0xF);
    writeReg(BUFFER_ORGANIZATION, bufferCode);
}

uint8_t caen_xx725_30::getBufferOrganization() {
    return readReg(BUFFER_ORGANIZATION);
}

/**
* \param customSize: number of memory locations per event, 0=custom size disabled
*/
void caen_xx725_30::setCustomSize(uint32_t customSize) {
    if(isRunning()) {
        throw llbad_caen_xx725_30("setCustomSize(): not allowed while running.");
    }
    
    writeReg(CUSTOM_SIZE, customSize);
}
uint32_t caen_xx725_30::getCustomSize() {
    return readReg(CUSTOM_SIZE);
}

/**
* starts the calibration for all channels on the board
* it is recommended to calibrate the channels after temperature has stabilized
* repeat the calibration after operating temperature variations or change of the clock settings
*/
void caen_xx725_30::calibrate() {
	#ifdef DEBUG_XX725_30
		cout << "Started calibration.\n";
	#endif
    
	//check channel DAC busy flag
    checkIsAnyChannelBusy();
	
    writeReg(ADC_CALIBRATION, 1); //any write access causes a calibration

    //poll status until calibration has finished:
    int maxPolls = 1000;
    int currPolls = 0;
    //loop over all channels, it could be that they need different times to calibrate
    for(int chn = 0; chn < n_channel; chn++) {
        bool calib_done = true;
        
        //poll as long they need to calibrate
        while(calib_done && (currPolls < maxPolls) ){
            chn_status status = getChannelStatus(chn);
            calib_done = status.calib_status;
           
            usleep(1000);
            currPolls++;
        } 
    }
    if(currPolls >= maxPolls)
        throw llbad_caen_xx725_30("calibrate(): calibration not finished");
	#ifdef DEBUG_XX725_30
		cout << "Finished calibration successfully.\n";
	#endif
}

/**
* sets the ACQ Control register
*/
void caen_xx725_30::setACQControl(acq_control control) {
    writeReg(ACQ_CONTROL, control.raw);
}

caen_xx725_30::acq_control caen_xx725_30::getACQControl() {
    acq_control control;
    control.raw = readReg(ACQ_CONTROL);
    return control;
}

/**
* retrieve the acquisition status
*/
caen_xx725_30::acq_status caen_xx725_30::getACQStatus() {    
    acq_status status;    
    status.raw = readReg(ACQ_STATUS);
    return status;
}

void caen_xx725_30::printACQStatus(acq_status acq_status) {
    cout << "AcquisitionStatus: \n";
    cout << "  Board: " << (acq_status.run ? "running" : "not running") << '\n';
    cout << "  Event: " << (acq_status.event_ready ? "ready" : "not ready")  << '\n';
    cout << "  Event: " << (acq_status.event_full ? "full" : "free")  << '\n';
    cout << "  Clock source: " << (acq_status.clk_src ? "external" : "internal")  << '\n';
    cout << "  PLL: " << (acq_status.bypass ? "bypassed" : "no bypass") << '\n';
    cout << "  PLL: " << (acq_status.unlock ? "locked" : "loss of lock") << '\n';
    cout << "  Board: " << (acq_status.board_ready ? "ready" : "not ready")  << '\n';
    
    cout << "  S-IN (VME) / GIP (NIM/DT) logic level: " << (acq_status.in_status ? "HIGH" : "LOW") << '\n';
    cout << "  TRG-IN logic level: " << (acq_status.trg_in_status ? "HIGH" : "LOW") << '\n';
    cout << "  Channels shutdown status, channels are: " << (acq_status.shutdown ? "in shutdown" : "ON") << '\n';

    //temperature status
    cout << "  Temperature limits exceeded:\n";
    cout << "    Chn 00-03: " << (acq_status.temp_status&0x1 ? "TRUE" : "FALSE") << '\n';
    cout << "    Chn 04-07: " << (acq_status.temp_status&0x1 ? "TRUE" : "FALSE") << '\n';
    cout << "    Chn 08-11: " << (acq_status.temp_status&0x1 ? "TRUE" : "FALSE") << '\n';
    cout << "    Chn 12-15: " << (acq_status.temp_status&0x1 ? "TRUE" : "FALSE") << '\n';

}

/**
* causes a software trigger generation
 which is propagated to all enabled channels of the board
*/
void caen_xx725_30::softwareTrigger() {
    writeReg(SOFTWARE_TRIGGER, 1); //any write access causes a trigger
}

/**
* sets the global trigger mask
* refer to the manual for details
*/
void caen_xx725_30::setGlobalTriggerMask(global_trigger_mask mask) {    
    writeReg(GLOBAL_TRIGGER_MASK, mask.raw);
}

caen_xx725_30::global_trigger_mask caen_xx725_30::getGlobalTriggerMask() {
    global_trigger_mask mask;
    mask.raw = readReg(GLOBAL_TRIGGER_MASK);
    return mask;
}

/**
* set front panel trg-out (GPO) enable mask
*/
void caen_xx725_30::setFrontPanelMask(front_panel_mask mask) {
    writeReg(FRONT_PANEL_MASK, mask.raw);
}

caen_xx725_30::front_panel_mask caen_xx725_30::getFrontPanelMask() {
    front_panel_mask mask;
    mask.raw = readReg(FRONT_PANEL_MASK);
    return mask;
}

void caen_xx725_30::setPostTrigger(uint32_t trigger) {
    writeReg(POST_TRIGGER, trigger);
}
uint32_t caen_xx725_30::getPostTrigger() {
    return readReg(POST_TRIGGER);
}

/**
* for VME only
*/
void caen_xx725_30::setLVDSIOData(uint16_t data) {
    checkIsVME();
    writeReg(LVDS_I_O_DATA, data);
}
/**
* for VME only
*/
uint16_t caen_xx725_30::getLVDSIOData() {
    checkIsVME();
    return readReg(LVDS_I_O_DATA);
}

void caen_xx725_30::setFrontPanelIOControl(front_panel_io_control control) {    
    writeReg(FRONT_PANEL_I_O_CONTROL, control.raw);
}
caen_xx725_30::front_panel_io_control caen_xx725_30::getFrontPanelIOControl() {
    front_panel_io_control control;
    control.raw = readReg(FRONT_PANEL_I_O_CONTROL);
    return control;
}

/**
*/
void caen_xx725_30::setChannelEnableMask(uint16_t mask) {
    if(isRunning()) {
        throw llbad_caen_xx725_30("setChannelEnableMask(): not allowed while running()");
    }

    writeReg(CHANNEL_ENABLE_MASK, mask);
}
uint16_t caen_xx725_30::getChannelEnableMask() {
    return readReg(CHANNEL_ENABLE_MASK);
}

caen_xx725_30::roc_fpga_firmware caen_xx725_30::getROCFPGAFirmware() {
    roc_fpga_firmware firmware;
    firmware.raw = readReg(ROC_FIRMWARE);
    return firmware;
}

void caen_xx725_30::printROCFPGAFirmware(roc_fpga_firmware firmware) {
    cout << "ROC FPGA Firmware Revision: " << firmware.major << "." << firmware.minor << ", ";
    cout << setw(2) << setfill('0');
    cout << firmware.day << "." << firmware.month << "." << firmware.year << '\n';
}

/**
* the number of events currently stored in the Output Buffer
*/
uint32_t caen_xx725_30::getEventsStored() {
    return readReg(EVENTS_STORED);
}

/**
* for VME only
* sets the DAC value on the front panel MON/Sigma output LEMO
* bits[11:0]: value is multiplied by 0.244 mV    
*/
void caen_xx725_30::setMonitorDAC(uint16_t voltage) {
    checkIsVME();
    writeReg(SET_MONITOR_DAC, voltage);
}
/**
* for VME only
*/
uint16_t caen_xx725_30::getMonitorDAC() {
    checkIsVME();
    return readReg(SET_MONITOR_DAC);
}

/**
* for VME only
* forces PLL to re-align all clock outputs with the reference clock
* does a calibration afterwards
*/
void caen_xx725_30::softwareClockSync() {
    checkIsVME();
    writeReg(SOFTWARE_CLOCK_SYNC, 1); //any write access causes a software sync  

    calibrate();
}

caen_xx725_30::board_info caen_xx725_30::getBoardInfo() {
    board_info info;
    info.raw = readReg(BOARD_INFO);
    return info;
}

void caen_xx725_30::printBoardInfo(board_info info) {
    cout << "Board Info:\n";
    cout << "  family: ";
    if(info.family == 0x0E) {
        cout << "725\n";
    } else if(info.family == 0x0B) {
        cout << "730\n";
    } else {
        cout << "unknown\n";
    }
    cout << "  memory per chn: ";
    if(info.mem_size == 0x1) {
        cout << "640 kS\n";
    } else if(info.mem_size == 0x8) {
        cout << "5.12 MS\n";
    } else {
        cout << "unknown";
    }

    cout << "  equipped channel: " << ((int)info.equipped_channel) << std::endl;    
}

/**
* for VME only
* sets the output DAC mode of the MON/Sigma front panel LEMO
* options bits [2:0]:
* [000] = trigger majority mode
* [001] = test mode
* [010] = reserved
* [011] = buffer occupancy mode
* [100] = voltage level mode
* others reserved        
*/
void caen_xx725_30::setMonitorDACMode(uint8_t mode) {
    checkIsVME();
    writeReg(MONITOR_DAC_MODE, mode);
}
/**
* for VME only
*/
uint8_t caen_xx725_30::getMonitorDACMode() {
    checkIsVME();
    return readReg(MONITOR_DAC_MODE);
}

/**
* current available event size in 32-bit words. The value is updated after a complete readout of each event.
*/
uint32_t caen_xx725_30::getEventSize() {
    return readReg(EVENT_SIZE);
}

/**
* desktop boards only
* set the fan speed
* bit[3] = 0: automatic control (requires ROC FPGA firmware > 4.4); slow speed (for earlier revisions)
* bit[3] = 1: high speed    
*/
void caen_xx725_30::setFanSpeedControl(fan_speed_control control) {
    checkIsDesktop();
    writeReg(FAN_SPEED_CONTROL, control.raw);
}
/**
* desktop versions only
*/
caen_xx725_30::fan_speed_control caen_xx725_30::getFanSpeedControl() {    
    checkIsDesktop();
    fan_speed_control control;
    control.raw = readReg(FAN_SPEED_CONTROL);
    return control;
}

/**
* the written value represents the number of buffer that must be full of data before to assert the BUSY signal
* if is set to 0, the ALMOST FULL is a FULL
*/
void caen_xx725_30::setMemBufferAlmostFullLevel(uint16_t level) {
    writeReg(BUFF_ALMOST_FULL_LEVEL, level);
}

uint16_t caen_xx725_30::getMemBufferAlmostFullLevel() {
    return readReg(BUFF_ALMOST_FULL_LEVEL);
}

/**
* set the delay in units of 8 ns
* for several boards in daisy chain this can be used to compensate for the propagation of the start(or stop) signal
*/
void caen_xx725_30::setRunStartStopDelay(uint32_t delay) {
    writeReg(CONTROL_DELAY, delay);
}
uint32_t caen_xx725_30::getRunStartStopDelay() {
    return readReg(CONTROL_DELAY);
}

/**
* check which kind of error occurred
*/
caen_xx725_30::board_failure_status caen_xx725_30::getBoardFailureStatus() {    
    board_failure_status status;
    status.raw = readReg(BOARD_FAILURE_STATUS);
    return status;
}
void caen_xx725_30::printBoardFailureStatus(board_failure_status status) {
    cout << "Board Failure Status: \n";
    if(status.internal_timeout != 0)
        cout << "Internal timeout occurred: " << status.internal_timeout <<"\n";
    if(status.lock_loss == 1)
        cout << "PLL Lock Loss occurred\n";
    if(status.temp_fail == 1)
        cout << "Temperature failure occurred (at least one channel is in over-temperature condition)\n";
    if(status.adc_power_down == 1)
        cout << "ADC power down occurred\n";
    cout << "Board failure status end.\n";    
}

/**
* for VME only
* are supported from ROC FPGA firmware revision 3.8 on
*/
void caen_xx725_30::setFrontPanelLVDSNewFeatures(fp_lvds_new_features features) {
    checkIsVME();
    checkROCFPGAFirmware("setFrontPanelLVDSNewFeatures", getROCFPGAFirmware(), 3, 8);
    writeReg(FRONT_PANEL_NEW_FEATURES, features.raw);
}
/**
* for VME only
*/
caen_xx725_30::fp_lvds_new_features caen_xx725_30::getFrontPanelLVDSNewFeatures() {    
    checkIsVME();
    checkROCFPGAFirmware("getFrontPanelLVDSNewFeatures", getROCFPGAFirmware(), 3, 8);
    fp_lvds_new_features features;
    features.raw = readReg(FRONT_PANEL_NEW_FEATURES);
    return features;    
}

/**
* switch on all the channels after they have been switched off by automatic shutodnw procedure
* bit[0] is forced to 1 while the board remains in the temperature protection condition.
* bit[0] = 0: no shutdown command is issued; = 1: a shutdown command is issued
*/
void caen_xx725_30::setChannelsShutdown(uint8_t shutdown) {
    writeReg(CHANNELS_SHUTDOWN, shutdown);
}

/**
* sets the readout control register (mainly for VME boards)
*/
void caen_xx725_30::setReadoutControl(readout_control control) {
    if(!isVME()) {//so for DT and NIM board
        control.vme_berr = 1; //must be 1
        control.vme_align64 = 0;
        control.vme_ba_reloc = 0;
        control.interrupt_release_mode = 0;        
    }
    writeReg(READOUT_CONTROL, control.raw);
    roControl.raw = readReg(READOUT_CONTROL);
}
caen_xx725_30::readout_control caen_xx725_30::getReadoutControl() {        
    if(!isInit) roControl.raw = readReg(READOUT_CONTROL);
    return roControl;
}
caen_xx725_30::readout_status caen_xx725_30::getReadoutStatus() {
    readout_status status;
    status.raw = readReg(READOUT_STATUS);
    return status;
}
void caen_xx725_30::printReadoutStatus(readout_status status) {
    cout << "Readout status: 0x" << hex << status.raw << dec << '\n';
    cout << "Event storage: " << (status.event_ready ? "event ready" : "no data ready") << '\n';
    cout << "Output buffer status: " << (status.out_buff_status ? "FULL" : "not FULL") << '\n';
    cout << "Bus error flag: " << (status.bus_error ? "error ocurred/transfer has been terminated" : "no error") << '\n';    
}

/**
* for VME only
* write GEO Address    
*/
void caen_xx725_30::setBoardID(uint8_t geo_address) {
    checkIsVME();
    writeReg(BOARD_ID, geo_address);
}
/**
* for VME only
*/
uint8_t caen_xx725_30::getBoardID() {
    checkIsVME();
    return readReg(BOARD_ID);
}

/**
* for VME only
* configure for VME multicast cycles
*/
void caen_xx725_30::setMCSTBaseAddressAndControl(mcst_ba_addr_ctrl reg) {
    checkIsVME();
    writeReg(MCST_BASE_ADDR_AND_CTRL, reg.raw);
}
/**
* for VME only
*/
caen_xx725_30::mcst_ba_addr_ctrl caen_xx725_30::getMCSTBaseAddressAndControl(){    
    checkIsVME();
    mcst_ba_addr_ctrl reg;
    reg.raw = readReg(MCST_BASE_ADDR_AND_CTRL);
    return reg;
}

/**
* for VME only
* sets the VME Base Address of the module of relocation is enabled through register 0xEF00
*/
void caen_xx725_30::setRelocationAddress(uint16_t address) {
    checkIsVME();
    writeReg(RELOCATION_ADDR, address);
}
/**
* for VME only
*/
uint16_t caen_xx725_30::getRelocationAddress() {
    checkIsVME();
    return readReg(RELOCATION_ADDR);
}

/**
* for VME only
*/
void caen_xx725_30::setInterruptStatus_ID(uint32_t status_id) {
    checkIsVME();
    writeReg(INTERRUPT_STATUS_ID, status_id);
}
/**
* for VME only
*/
uint32_t caen_xx725_30::getInterruptStatus_ID() {
    checkIsVME();
    return readReg(INTERRUPT_STATUS_ID);
}

/**
* sets the number of events that causes an interrupt request. If interrupts are enabled, the module generates a request whenever it has stored in memory a Number of Events > INTERRUPT EVENT NUMBER
bits[9:0] INTERRUPT EVENT NUMBER
*/
void caen_xx725_30::setInterruptEventNumber(uint16_t ien) {
    writeReg(INTERRUPT_EVENT_NUM, ien);
}
uint16_t caen_xx725_30::getInterruptEventNumber() {
    return readReg(INTERRUPT_EVENT_NUM);
}

/**
* sets the maximum number of complete events which has to be transferred for each block transfer (via VME BLT/CBLT cycles, or block readout through USB or Optical Link).
*/
void caen_xx725_30::setMaxNumberEventsPerBLT(uint16_t maxNumber) {
    writeReg(MAX_NUM_EVENTS_PER_BLT, maxNumber);
    maxBltEvents = readReg(MAX_NUM_EVENTS_PER_BLT);
}
uint16_t caen_xx725_30::getMaxNumberEventsPerBLT() {
	if(!isInit) maxBltEvents = readReg(MAX_NUM_EVENTS_PER_BLT);
	return maxBltEvents;
}

/**
* resets all register to their default values
*/
void caen_xx725_30::softwareReset() {
    writeReg(SOFTWARE_RESET, 1); //any write access causes a software reset
    isInit=false;
    getReadoutControl();
    getMaxNumberEventsPerBLT();
    isInit=true;
}

/**
* all digitizer internal memories are cleared
*/
void caen_xx725_30::softwareClear() {
    writeReg(SOFTWARE_CLEAR, 1); //any write access causes a software clear
}

/**
* causes a software reset, a relaod of configuration ROM parameters and a PLL reconfiguration
*/
void caen_xx725_30::configurationReload() {
    writeReg(CONFIGURATION_RELOAD, 1); //any write access causes a configuration reload
    isInit=false;
    getReadoutControl();
    getMaxNumberEventsPerBLT();
    isInit=true;
}

caen_xx725_30::configurationROM caen_xx725_30::getConfigurationROM() {
    if(!readRom) //just checking one value, which should never be zero
        readConfigurationROM();
    return rom;
}

void caen_xx725_30::printConfigurationROM(configurationROM rom) {
    cout << "Configuration ROM:\n" << hex << showbase;
    cout << "Checksum: " << rom.checksum << '\n';
    cout << "Checksum Length2: " << rom.checksum_length2 << '\n';
    cout << "Checksum Length1: " << rom.checksum_length1 << '\n';
    cout << "Checksum Length0: " << rom.checksum_length0 << '\n';
    cout << "Constant2: " << rom.constant2 << '\n';
    cout << "Constant1: " << rom.constant1 << '\n';
    cout << "Constant0: " << rom.constant0 << '\n';
    cout << "C Code: " << rom.c_code << '\n';
    cout << "R Code: " << rom.r_code << '\n';
    cout << "IEEE OUI byte 2: " << rom.oui2 << '\n';
    cout << "IEEE OUI byte 1: " << rom.oui1 << '\n';
    cout << "IEEE OUI byte 0: " << rom.oui0 << '\n';
    cout << "Board version: " << rom.vers << '\n';
    cout << "Formfactor: " << rom.formfactor << '\n';
    cout << "Board ID byte 1: " << rom.board1 << '\n';
    cout << "Board ID byte 0: " << rom.board0 << '\n';
    cout << "PCB Revision byte 3: " << rom.revis3 << '\n';
    cout << "PCB Revision byte 2: " << rom.revis2 << '\n';
    cout << "PCB Revision byte 1: " << rom.revis1 << '\n';
    cout << "PCB Revision byte 0: " << rom.revis0 << '\n';
    cout << "Flash type: " << rom.flash << '\n';
    cout << "Board serial number byte 1: " << rom.sernum1 << '\n';
    cout << "Board serial number byte 0: " << rom.sernum0 << '\n';
    cout << "VCXO type: " << rom.vcxo << dec << endl;    
}

/**
* true for formfactor VME64 or VME64X
*/
bool caen_xx725_30::isVME() {
    return ((rom.formfactor&0xF) == 0x0 || (rom.formfactor&0xF) == 0x1);
}
/**
* true for formfactor NIM
*/
bool caen_xx725_30::isNIM() {
    return (rom.formfactor&0xF) == 0x3;
}
/**
* true for formfactor Desktop
*/
bool caen_xx725_30::isDT() {
    return (rom.formfactor&0xF) == 0x2;
}

/**
* helper function: checks if the given channel number is in the valid range [0, N_CHN-1]
* throws an exception in case of an invalid channel number
*/
void caen_xx725_30::checkIsValidChannel(int channel) {
    if(channel < 0 || channel >= n_channel) {
        throw llbad_caen_xx725_30_invalid_channel("channel out of range", channel);
    }
}

/**
\param couple number of the couple: 0=channel0+channel1, 1=channel2+channel3
* helper function: checks if the given couple number is in the valid range [0, N_CHN/2-1]
* throws an exception otherwise
*/
void caen_xx725_30::checkIsValidCouple(int couple) {
    if(couple < 0 || couple >= n_channel/2) {
        throw llbad_caen_xx725_30_invalid_couple("couple out of range", couple);
    }
}

/**
* \param addr like enum RegAddr (without baseaddress)
*/
uint32_t caen_xx725_30::readReg(uint16_t addr) { 
    int ret;
    uint32_t ptr;   
    vme_claimer claim(vme,AM_A32);
    if ( (ret=vme->read32(ba+addr, &ptr, 1)) < 1 )
        { throw llbad_caen_xx725_30_vme_error("read error",ret); }
    return ptr;
}
/**
* \param addr like enum RegAddr (without baseaddress)
*/
void caen_xx725_30::writeReg(uint16_t addr, uint32_t ptr) {
    int ret;
    vme_claimer claim(vme,AM_A32);
    if ( (ret=vme->write32(ba+addr, &ptr, 1)) < 1 )
        { throw llbad_caen_xx725_30_vme_error("write error",ret); }
}

/**
* check if the formfactor is VME64 or VME64X
* throws an exception otherwise
*/
void caen_xx725_30::checkIsVME() {
    if(!isVME())  {
        throw llbad_caen_xx725_30_vme_only("The called function is accessible from VME boards only.");
    }
}

/**
* checks if the formfactor is of type desktop
* throws an exception otherwise
*/
void caen_xx725_30::checkIsDesktop() {
    if( (rom.formfactor&0xFF) != 0x02) {
        throw llbad_caen_xx725_30("The called function is accessible from desktop versions only.");
    }
}

/**
* checks if the equipped channel number corresponds to the number configured channels
* throws an exception if they mismatch
*/
void caen_xx725_30::checkEquippedChannel(board_info info) {
    if(info.equipped_channel != (uint32_t)n_channel)
        {throw llbad_caen_xx725_30("The number of channels recognized by the board is too low: ", info.equipped_channel); }
}

void caen_xx725_30::checkAMCFirmware(const string caller,const amc_firmware firmware,const int leastMajor,const int leastMinor) {
    if(leastMajor > firmware.major || ( (leastMajor == firmware.major) && firmware.minor < leastMinor) )
        throw llbad_caen_xx725_30("firmware version is too low to call: "+caller);
}

void caen_xx725_30::checkROCFPGAFirmware(const string caller, const roc_fpga_firmware firmware,const int leastMajor,const int leastMinor) {
    if(leastMajor > firmware.major || ( (leastMajor == firmware.major) && firmware.minor < leastMinor) )
        throw llbad_caen_xx725_30("firmware version is too low to call: "+caller);
}

//return theoretical number of 32 bit words per event for a given setting
uint32_t caen_xx725_30::calculateEventSize(uint8_t bufferOrganisation, uint8_t nLoc, uint8_t nChannels, uint32_t memorySize){
	return 4+(min(nLoc?nLoc*10:memorySize,memorySize/(1<<bufferOrganisation)-10))/2*nChannels;
}
