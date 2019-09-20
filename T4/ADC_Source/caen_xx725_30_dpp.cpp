#include "caen_xx725_30_dpp.h"
#include <iostream> //cout
#include <iomanip>  //setw, setfill

using namespace std;

caen_xx725_30_dpp::caen_xx725_30_dpp(N_CHN n_channel, vme_interface *interface, vme_addr_t baseaddr)
 : caen_xx725_30(n_channel, interface, baseaddr) {
}

bool caen_xx725_30_dpp::isEventReady() {
    return getACQStatus().event_ready;
}
uint32_t caen_xx725_30_dpp::getNextEventSize() {
    return getEventSize();
}
vme_interface* caen_xx725_30_dpp::getInterface() {
    return caen_xx725_30::getInterface();
}
vme_addr_t caen_xx725_30_dpp::getEventReadoutAddress() {
    return getBaseAddress()+READOUT_BUFFER;
}
int caen_xx725_30_dpp::getMaxEventsPerBLT() {
    return getMaxNumberAggregatesPerBLT();
}
/**
* \param number will be interpreted as uint8_t
*/
void caen_xx725_30_dpp::setMaxEventsPerBLT(int number) {
    setMaxNumberAggregatesPerBLT(number);
}


/**
* sets record length for waveform acquisition
* in case of list mode, the record length is ignored
* a couple are two adjacent channel 2n and 2n+1 (e.g. couple0 = channel0 and channel1)
* [13:0]: Number of samples in the waveform according to the formula Ns = N * 8, where Ns is the
record length and N is the register value. For example, write N = 3 to acquire 24 samples.
Each sample corresponds to 4 ns for 725 series and 2 ns for 730 series.
*/
void caen_xx725_30_dpp::setCoupleRecordLength(int couple, uint16_t length) {
    checkIsValidCouple(couple);
    writeReg(RECORD_LENGTH+2*couple*CHN_MULTIPLIER, length);
}
void caen_xx725_30_dpp::setAllCouplesRecordLength(uint16_t length) {
    writeReg(RECORD_LENGTH_BC, length);
}
uint16_t caen_xx725_30_dpp::getCoupleRecordLength(int couple) {
    checkIsValidCouple(couple);
    return readReg(RECORD_LENGTH+2*couple*CHN_MULTIPLIER);
}

/**
* Set number of events per aggregate for a couple 
* a couple are two adjacent channel 2n and 2n+1 (e.g. couple0 = channel0 and channel1)
* bit[9:0] number of events
*/
void caen_xx725_30_dpp::setCoupleNumEventsPerAggregate(int couple, uint16_t number) {
    checkIsValidCouple(couple);
    writeReg(EVENTS_PER_AGGREGATE+2*couple*CHN_MULTIPLIER, number);
}
void caen_xx725_30_dpp::setAllCouplesNumEventsPerAggregate(uint16_t number) {
    writeReg(EVENTS_PER_AGGREGATE_BC, number);
}
uint16_t caen_xx725_30_dpp::getCoupleNumEventsPerAggregate(int couple) {
    checkIsValidCouple(couple);
    return readReg(EVENTS_PER_AGGREGATE+couple*CHN_MULTIPLIER);
}

/**
* [8:0] number of samples before the trigger in the waveform is saved into memory
*/
void caen_xx725_30_dpp::setChannelPreTrigger(int channel, uint16_t trigger) {
    checkIsValidChannel(channel);
    writeReg(PRE_TRIGGER+channel*CHN_MULTIPLIER, trigger);
}
void caen_xx725_30_dpp::setAllChannelsPreTrigger(uint16_t trigger) {
    writeReg(PRE_TRIGGER_BC, trigger);
}
uint16_t caen_xx725_30_dpp::getChannelPreTrigger(int channel) {
    checkIsValidChannel(channel);
    return readReg(PRE_TRIGGER+channel*CHN_MULTIPLIER);
}

/**
* sets CFD settings
*/
void caen_xx725_30_dpp::setChannelCFDSettings(int channel, cfd_settings settings) {
    checkIsValidChannel(channel);
    writeReg(CFD_SETTINGS+channel*CHN_MULTIPLIER, settings.raw);
}
void caen_xx725_30_dpp::setAllChannelsCFDSettings(cfd_settings settings) {
    writeReg(CFD_SETTINGS_BC, settings.raw);
}
caen_xx725_30_dpp::cfd_settings caen_xx725_30_dpp::getChannelCFDSettings(int channel) {
    checkIsValidChannel(channel);
    cfd_settings settings;
    settings.raw = readReg(CFD_SETTINGS+channel*CHN_MULTIPLIER);
    return settings;
}

/**
* use this function to force the read of current incomplete aggregate (e.g. at the end of a run, or for low event rate)
*/
void caen_xx725_30_dpp::channelForceDataFlush(int channel) {
    checkIsValidChannel(channel);
    writeReg(FORCED_DATA_FLUSH, 1); //any write access causes a data flush
}
void caen_xx725_30_dpp::allChannelForceDataFlush() {
    writeReg(FORCED_DATA_FLUSH_BC, 1);
}

/**
* set a threshold in the spectrum (Qthr) to cut events with charge Qlong < Qthr
* 1 LSB corresponds to a specific value of charge which depends on the charge sensitivity value.
*/
void caen_xx725_30_dpp::setChannelChargeZeroSuppThreshold(int channel, uint16_t threshold) {    
    checkIsValidChannel(channel);
    writeReg(CHARGE_ZERO_SUPPR_THR, threshold);
}
void caen_xx725_30_dpp::setAllChannelsChargeZeroSuppThreshold(uint16_t threshold) {
    writeReg(CHARGE_ZERO_SUPPR_THR_BC, threshold);
}
uint16_t caen_xx725_30_dpp::getChannelChargeZeroSuppThreshold(int channel) {
    checkIsValidChannel(channel);    
    return readReg(CHARGE_ZERO_SUPPR_THR+channel*CHN_MULTIPLIER);
}

/**
* bit[11:0]: Number of samples for the Short Gate width. Each sample corresponds to 4 ns for 725 series and 2 ns for 730 series.
*/
void caen_xx725_30_dpp::setChannelShortGateWidth(int channel, uint16_t width) {
    checkIsValidChannel(channel);
    writeReg(SHORT_GATE_WIDTH+channel*CHN_MULTIPLIER, width);
}
void caen_xx725_30_dpp::setAllChannelsShortGateWidth(uint16_t width) {
    writeReg(SHORT_GATE_WIDTH_BC, width);
}
uint16_t caen_xx725_30_dpp::getChannelShortGateWidth(int channel) {
    checkIsValidChannel(channel);
    return readReg(SHORT_GATE_WIDTH+channel*CHN_MULTIPLIER);
}

/**
* bit[15:0]: Number of samples for the Long Gate width. Each sample corresponds to 4 ns for 725 series and 2 ns for 730 series.
*/
void caen_xx725_30_dpp::setChannelLongGateWidth(int channel, uint16_t width) {
    checkIsValidChannel(channel);
    writeReg(LONG_GATE_WIDTH+channel*CHN_MULTIPLIER, width);
}
void caen_xx725_30_dpp::setAllChannelsLongGateWidth(uint16_t width) {
    writeReg(LONG_GATE_WIDTH_BC, width); 
}
uint16_t caen_xx725_30_dpp::getChannelLongGateWidth(int channel) {
    checkIsValidChannel(channel);
    return readReg(LONG_GATE_WIDTH+channel*CHN_MULTIPLIER);
}

/**
* Number of samples of the Gate Offset. Each sample corresponds to 4 ns for 725 series and 2 ns for 730 series.
*/
void caen_xx725_30_dpp::setChannelGateOffset(int channel, uint8_t offset) {
    checkIsValidChannel(channel);
    writeReg(GATE_OFFSET+channel*CHN_MULTIPLIER, offset);
}
void caen_xx725_30_dpp::setAllChannelsGateOffset(uint8_t offset) {
    writeReg(GATE_OFFSET_BC, offset);
}
uint8_t caen_xx725_30_dpp::getChannelGateOffset(int channel) {
    checkIsValidChannel(channel);
    return readReg(GATE_OFFSET+channel*CHN_MULTIPLIER);
}

/**
* set trigger threshold value for leading edge discrimination
* bit[13:0]: number of LSB counts for the Trigger Threshold, where 1 LSB = 0.12 mV for 725 and 730 series with 2 Vpp input range, and 1 LSB = 0.03 mV for 725 and 730 series with 0.5 Vpp input range.
* The threshold is referred to the baseline level and can be used to trigger on Leading Edge Discrimina on or to arm the digital Constant Fraction Discrimination.
*/
void caen_xx725_30_dpp::setChannelTriggerThreshold(int channel, uint16_t value) {    
    checkIsValidChannel(channel);
    writeReg(TRIGGER_THRESHOLD+channel*CHN_MULTIPLIER, value);
}
void caen_xx725_30_dpp::setAllChannelsTriggerThreshold(uint16_t value) {    
    writeReg(TRIGGER_THRESHOLD_BC, value);
}         
uint16_t caen_xx725_30_dpp::getChannelTriggerThreshold(int channel) {
    checkIsValidChannel(channel);
    return readReg(TRIGGER_THRESHOLD+channel*CHN_MULTIPLIER);
}

/**
* [13:0] value of fixed baseline in LSB counts
*/
void caen_xx725_30_dpp::setChannelFixedBaseline(int channel, uint16_t baselineValue) {
    checkIsValidChannel(channel);
    writeReg(FIXED_BASELINE+channel*CHN_MULTIPLIER, baselineValue);
}
void caen_xx725_30_dpp::setAllChannelsFixedBaseline(uint16_t baselineValue) {
    writeReg(FIXED_BASELINE_BC, baselineValue);
}
uint16_t caen_xx725_30_dpp::getChannelFixedBaseline(int channel) {
    checkIsValidChannel(channel);
    return readReg(FIXED_BASELINE+channel*CHN_MULTIPLIER);
}

/**
* [9:0] Shaped Trigger width in steps of 16 ns for 725 series and 8 ns for 730 series.
*/
void caen_xx725_30_dpp::setChannelShapedTriggerWidth(int channel, uint16_t width) {
    checkIsValidChannel(channel);
    writeReg(SHAPED_TRIGGER_WIDTH+channel*CHN_MULTIPLIER, width);
}
void caen_xx725_30_dpp::setAllChannelsShapedTriggerWidth(uint16_t width) {
    writeReg(SHAPED_TRIGGER_WIDTH_BC, width);
}
uint16_t caen_xx725_30_dpp::getChannelShapedTriggerWidth(int channel) {
    checkIsValidChannel(channel);
    return readReg(SHAPED_TRIGGER_WIDTH+channel*CHN_MULTIPLIER);
}

/**
* Trigger Hold-Off width expressed in steps of 16 ns for 725 series and 8 ns for 730 series.
*/
void caen_xx725_30_dpp::setChannelTriggerHoldOffWidth(int channel, uint16_t width) {
    checkIsValidChannel(channel);
    writeReg(TRIGGER_HOLD_OFF_WIDTH+channel*CHN_MULTIPLIER, width);
}
void caen_xx725_30_dpp::setAllChannelsTriggerHoldOffWidth(uint16_t width) {
    writeReg(TRIGGER_HOLD_OFF_WIDTH_BC, width);    
}
uint16_t caen_xx725_30_dpp::getChannelTriggerHoldOffWidth(int channel) {
    checkIsValidChannel(channel);
    return readReg(TRIGGER_HOLD_OFF_WIDTH+channel*CHN_MULTIPLIER);    
}

/**
* [9:0] Set the PSD threshold value. The desired value has to be multiplied by 1024. For example for a PSD threshold of 0.12, write 122 (= 0.12 * 1024).
*/
void caen_xx725_30_dpp::setChannelThresholdPSDCut(int channel, uint16_t cut) {
    checkIsValidChannel(channel);
    writeReg(THRESHOLD_PSD_CUT+channel*CHN_MULTIPLIER, cut);
}
void caen_xx725_30_dpp::setAllChannelsThresholdPSDCut(uint16_t cut) {
    writeReg(THRESHOLD_PSD_CUT_BC, cut);
}
uint16_t caen_xx725_30_dpp::getChannelThresholdPSDCut(int channel) {
    checkIsValidChannel(channel);
    return readReg(THRESHOLD_PSD_CUT+channel*CHN_MULTIPLIER);
}

/**
* bit[11:0] PUR-GAP value in LSB unit, where 1 LSB = 0.12 mV for 725 and 730 series with 2 Vpp input range, and 1 LSB = 0.03 mV for 725 and 730 series with 0.5 Vpp input range.
*/
void caen_xx725_30_dpp::setChannelPURGAPThreshold(int channel, uint16_t threshold) {
    checkIsValidChannel(channel);
    writeReg(PUR_GAP_THRESHOLD+channel*CHN_MULTIPLIER, threshold);
}
void caen_xx725_30_dpp::setAllChannelsPURGAPThreshold(uint16_t threshold) {
    writeReg(PUR_GAP_THRESHOLD_BC, threshold);
}
uint16_t caen_xx725_30_dpp::getChannelPURGAPThreshold(int channel) {
    checkIsValidChannel(channel);
    return readReg(PUR_GAP_THRESHOLD+channel*CHN_MULTIPLIER);
}

/**
* set channel algorithm control
*/
void caen_xx725_30_dpp::setChannelDPPAlgorithmControl(int channel, dpp_algorithm_control control) {
    checkIsValidChannel(channel);
    writeReg(DPP_ALGORITHM_CONTROL+channel*CHN_MULTIPLIER, control.raw);
}
void caen_xx725_30_dpp::setAllChannelsDPPAlgorithmControl(dpp_algorithm_control control) {
    writeReg(DPP_ALGORITHM_CONTROL_BC, control.raw);
}
caen_xx725_30_dpp::dpp_algorithm_control caen_xx725_30_dpp::getChannelDPPAlgorithmControl(int channel) {
    checkIsValidChannel(channel);
    dpp_algorithm_control control;
    control.raw = readReg(DPP_ALGORITHM_CONTROL+channel*CHN_MULTIPLIER);
    return control;
}

/**
* set channel algorithm control
* Bits[7:0](local_trigger_mode,  en_local_trigger, validation_mode, en_validation) are common to the couple (writing to the even channel writes also the odd and viceversa),
* while all other bits are individual (different settings on odd and even channels are allowed).
*/
void caen_xx725_30_dpp::setChannelDPPAlgorithmControl2(int channel, dpp_algorithm_control2 control) {
    checkIsValidChannel(channel);
    writeReg(DPP_ALGORITHM_CONTROL2+channel*CHN_MULTIPLIER, control.raw);
}
void caen_xx725_30_dpp::setAllChannelsDPPAlgorithmControl2(dpp_algorithm_control2 control) {
    writeReg(DPP_ALGORITHM_CONTROL2_BC, control.raw);
}
caen_xx725_30_dpp::dpp_algorithm_control2 caen_xx725_30_dpp::getChannelDPPAlgorithmControl2(int channel) {
    checkIsValidChannel(channel);
    dpp_algorithm_control2 control;
    control.raw = readReg(DPP_ALGORITHM_CONTROL2+channel*CHN_MULTIPLIER);
    return control;
}

caen_xx725_30_dpp::chn_status_dpp caen_xx725_30_dpp::getChannelStatusDPP(int channel) {
    checkIsValidChannel(channel);
    chn_status_dpp status;
    status.raw = readReg(CHN_STATUS_DPP+channel*CHN_MULTIPLIER);
    return status;
}

caen_xx725_30_dpp::amc_firmware_dpp caen_xx725_30_dpp::getChannelAMCFirmwareDPP(int channel) {
    checkIsValidChannel(channel);
    amc_firmware_dpp firmware;
    firmware.raw = readReg(AMC_FIRMWARE_DPP+channel*CHN_MULTIPLIER);
    return firmware;
}
void caen_xx725_30_dpp::printAMCFirmwareDPP(amc_firmware_dpp firmware) {
    cout << "revision: " << firmware.revision << '\n';
    cout << "dpp code: " << firmware.dpp_code << ", ";
    cout << setw(2) << setfill('0');
    cout << firmware.day_high << firmware.day_low << "." << firmware.month;
    cout << ". (" << 2000+firmware.year << " or " << 2016+firmware.year << ")\n"; 
}

/**
* Sends the Software Trigger to the individual channel n. This is not affected by the Trigger Valida on, i.e. the Individual Software Trigger can be issued also when coincidences are enabled without being affected.
*/
void caen_xx725_30_dpp::channelSoftwareTrigger(int channel) {
    checkIsValidChannel(channel);
    writeReg(INDIVIDUAL_SW_TRIGGER+channel*CHN_MULTIPLIER, 1);
}
void caen_xx725_30_dpp::allChannelsSoftwareTrigger() {
    writeReg(INDIVIDUAL_SW_TRIGGER_BC, 1);
}

/**
* sets the veto duration
*/
void caen_xx725_30_dpp::setChannelVetoWidth(int channel, veto_width width) {
    checkIsValidChannel(channel);
    writeReg(VETO_WIDTH+channel*CHN_MULTIPLIER, width.raw);
}
void caen_xx725_30_dpp::setAllChannelsVetoWidth(veto_width width) {
    writeReg(VETO_WIDTH_BC, width.raw);
}
caen_xx725_30_dpp::veto_width caen_xx725_30_dpp::getChannelVetoWidth(int channel) {
    veto_width width;
    width.raw = readReg(VETO_WIDTH+channel*CHN_MULTIPLIER);
    return width;
}

/**
* [9:0] Baseline freeze me expressed in steps of 16 ns for 725 and 8 ns for 730. Default value is 2.
*/
void caen_xx725_30_dpp::setChannelBaselineFreezeTime(int channel, uint16_t time) {
    checkIsValidChannel(channel);
    writeReg(BASELINE_FREEZE_TIME+channel*CHN_MULTIPLIER, time);
}
void caen_xx725_30_dpp::setAllChannelsBaselineFreezeTime( uint16_t time) {
    writeReg(BASELINE_FREEZE_TIME_BC, time);
}
uint16_t caen_xx725_30_dpp::getChannelBaselineFreezeTime(int channel) {
    return readReg(BASELINE_FREEZE_TIME+channel*CHN_MULTIPLIER);
}

/** 
* setBoardConfigDPP
*/
void caen_xx725_30_dpp::setBoardConfigDPP(board_config_dpp config) {      
    config.dummy1 = 0; //must be 0               
    config.dummy2 = 0;//must be 0
    config.dummy3 = 1; //must be 1!
    config.dummy4 = 0; //must be 0!    
    config.dummy6 = 0; //must be 0
    config.individual_trigger = 1;
    config.time_stamp_rec = 1; //must be 1
    config.charge_rec = 1; //must be 1
    config.dummy8 = 0; // must be 0
    config.dummy9 = 0; //must be 0*/
    writeReg(BOARD_CONFIG, config.raw);    
}
caen_xx725_30_dpp::board_config_dpp caen_xx725_30_dpp::getBoardConfigDPP() {
    board_config_dpp config;
    config.raw = readReg(BOARD_CONFIG);
    return config;
}

/**
* sets the aggregate organization (same as setBufferOrganization)
*/
void caen_xx725_30_dpp::setAggregateOrganization(uint8_t code) {
    setBufferOrganization(code);
}
uint8_t caen_xx725_30_dpp::getAggregateOrganization() {
    return getBufferOrganization();
}

/**
* switch on all the channels after they have been switched off by automatic shutodnw procedure
* bit[0] is forced to 1 while the board remains in the temperature protection condition.
* bit[0] = 0: no shutdown command is issued; = 1: a shutdown command is issued
*/
void caen_xx725_30_dpp::setChannelsShutdown(uint8_t shutdown) {
    writeReg(CHANNELS_SHUTDOWN, shutdown);
}

/**
* if this value is constant the DPP firmware is licensed, otherweise it decreases with time (firmware stops working 30 min after power-on)
*/
uint32_t caen_xx725_30_dpp::getTimeBombDowncounter() {
   return readReg(TIME_BOMB_DOWNCOUNTER);
}

/**
* Disables TRG-IN connector and any functionality related to TRG-IN,
* [0]   = 0 -> enabled, 
        = 1 -> disabled
*/
void caen_xx725_30_dpp::setDisableExternalTrigger(uint8_t reg) {
    writeReg(DISABLE_EXTERNAL_TRIGGER, reg);
}
uint8_t caen_xx725_30_dpp::getDisableExternalTrigger() {
    return readReg(DISABLE_EXTERNAL_TRIGGER);
}

/**
*/
void caen_xx725_30_dpp::setCoupleTriggerValidationMask(int couple, trigger_validation_mask mask) {
    checkIsValidCouple(couple);
    writeReg(TRIGGER_VALIDATION_MASK+4*couple, mask.raw);
}
caen_xx725_30_dpp::trigger_validation_mask caen_xx725_30_dpp::getCoupleTriggerValidationMask(int couple) {
    checkIsValidCouple(couple);
    trigger_validation_mask mask;
    mask.raw = readReg(TRIGGER_VALIDATION_MASK+4*couple);
    return mask;
}

/**
*/
void caen_xx725_30_dpp::setFrontPanelLVDSNewFeatures(fp_lvds_new_features_dpp features_dpp) {
    caen_xx725_30::fp_lvds_new_features features;
    features.raw = features_dpp.raw;
    ((caen_xx725_30*)this)->setFrontPanelLVDSNewFeatures(features);
}
caen_xx725_30_dpp::fp_lvds_new_features_dpp caen_xx725_30_dpp::getFrontPanelLVDSNewFeatures() {
    caen_xx725_30::fp_lvds_new_features features = ((caen_xx725_30*)this)->getFrontPanelLVDSNewFeatures();
    fp_lvds_new_features_dpp features_dpp;
    features_dpp.raw = features.raw;
    return features_dpp;
}

/**
* sets the maximum number of complete events which has to be transferred for each block transfer (via VME BLT/CBLT cycles, or block readout through USB or Optical Link).
*/
void caen_xx725_30_dpp::setMaxNumberAggregatesPerBLT(uint8_t number) {
    writeReg(AGGREGATE_NUMBER_BLT, number);
}
uint8_t caen_xx725_30_dpp::getMaxNumberAggregatesPerBLT() {
    return readReg(AGGREGATE_NUMBER_BLT);
}

/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setChannelPulseWidth(int channel, uint8_t width) {
    throw llbad_caen_xx725_30_dpp_not_supported("function setChannelPulseWidth() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setAllChannelPulseWidth(uint8_t width) {
    throw llbad_caen_xx725_30_dpp_not_supported("function setAllChannelPulseWidth() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
uint8_t caen_xx725_30_dpp::getChannelPulseWidth(int channel) {
    throw llbad_caen_xx725_30_dpp_not_supported("function getChannelPulseWidth() is not supported.");
}

/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setCoupleSelfTriggerLogic(int couple, couple_self_trigger_logic logic){
    throw llbad_caen_xx725_30_dpp_not_supported("function setCoupleSelfTriggerLogic() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setAllCoupleSelfTriggerLogic(couple_self_trigger_logic logic){
    throw llbad_caen_xx725_30_dpp_not_supported("function setAllCoupleSelfTriggerLogic() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
caen_xx725_30_dpp::couple_self_trigger_logic caen_xx725_30_dpp::getCoupleSelfTriggerLogic(int couple){
    throw llbad_caen_xx725_30_dpp_not_supported("function getCoupleSelfTriggerLogic() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setPostTrigger(uint32_t trigger) {
    throw llbad_caen_xx725_30_dpp_not_supported("function setPostTrigger() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
uint32_t caen_xx725_30_dpp::getPostTrigger() {
    throw llbad_caen_xx725_30_dpp_not_supported("function getPostTrigger() is not supported.");
}

/**
* not supported with dpp-psd firmware
*/
void caen_xx725_30_dpp::setMemBufferAlmostFullLevel(uint16_t level){
    throw llbad_caen_xx725_30_dpp_not_supported("function setMemBufferAlmostFullLevel() is not supported.");
}
/**
* not supported with dpp-psd firmware
*/
uint16_t caen_xx725_30_dpp::getMemBufferAlmostFullLevel(){
    throw llbad_caen_xx725_30_dpp_not_supported("function getMemBufferAlmostFullLevel() is not supported.");
}
/**
* not supported with dpp-psd firmware;
* \see setMaxNumberAggregatesPerBLT
*/
void caen_xx725_30_dpp::setMaxNumberEventsPerBLT(uint16_t maxNumber){
    throw llbad_caen_xx725_30_dpp_not_supported("function setMaxNumberEventsPerBLT() is not supported.");
}
/**
* not supported with dpp-psd firmware
* \see getMaxNumberAggregatesPerBLT
*/
uint16_t caen_xx725_30_dpp::getMaxNumberEventPerBLT(){
    throw llbad_caen_xx725_30_dpp_not_supported("function getMaxNumberEventPerBLT() is not supported.");
}
/**
* not supported with dpp-psd firmware
* \see setBoardConfig(board_config_dpp)
*/
void caen_xx725_30_dpp::setBoardConfig(caen_xx725_30::board_config config) {
    throw llbad_caen_xx725_30_dpp_not_supported("setBoardConfig() with paramter board_config is not supported.");
}
/**
* not supported with dpp-psd firmware
* \see board_config_dpp getBoardConfig()
*/
caen_xx725_30::board_config caen_xx725_30_dpp::getBoardConfig() {
    throw llbad_caen_xx725_30_dpp_not_supported("getBoardConfig() with return value board_config is not supported.");    
}

/**
* not supported with dpp-psd firmware
* \see getChannelAMCFirmwareDPP
*/
caen_xx725_30::amc_firmware caen_xx725_30_dpp::getChannelAMCFirmware(int channel) {
    throw llbad_caen_xx725_30_dpp_not_supported("getChannelAMCFirmware() is not supported.");    
}
