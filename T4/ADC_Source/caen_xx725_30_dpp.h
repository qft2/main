/**
 * \file caen_xx725_30_dpp.h
 * \author Christian Wysotzki
 * \date Apr 2017
 * \brief implementation for the CAEN 725/730 digitizer family with DPP-PSD firmware, based on V1740 implementation
 */
#pragma once

#include "caen_xx725_30.h"
#include "caen_xx725_30_dpp_board_agg.h"

/**
 * \class llbad_caen_xx725_30_dpp
 * \brief standard exception class for CAEN XX725_30 ADC with DPP-PSD firmware
 */
class llbad_caen_xx725_30_dpp : public virtual llexception {
	public:
	llbad_caen_xx725_30_dpp(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30_dpp:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp() throw() {};
};

/**
 * \class llbad_caen_xx725_30_dpp_not_supported
 * \brief exception class for CAEN XX725_30 ADC called function is not supported as the register have different interpretation depending on the firmware
 */
class llbad_caen_xx725_30_dpp_not_supported: public llbad_caen_xx725_30_dpp {
	public:
	llbad_caen_xx725_30_dpp_not_supported(std::string newtext="",int errnumber=0):
		llexception("caen_xx725_30_dpp:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_not_supported() throw() {};
};

/**
 * \class llbad_caen_xx725_30_dpp_invalid_channel
 * \brief exception class for CAEN XX725_30_DPP ADC requested channel number is invalid
 */
class llbad_caen_xx725_30_dpp_invalid_channel: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_dpp_invalid_channel(std::string newtext="",int errnumber=0):
		llexception("caen_xx725_30_dpp:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_invalid_channel() throw() {};
};

/**
 * \class llbad_caen_xx725_30_dpp_invalid_couple
 * \brief exception class for CAEN XX725_30 ADC requested couple number is invalid
 */
class llbad_caen_xx725_30_dpp_invalid_couple: public llbad_caen_xx725_30_dpp {
	public:
	llbad_caen_xx725_30_dpp_invalid_couple(std::string newtext="",int errnumber=0):
		llexception("caen_xx725_30_dpp:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_invalid_couple() throw() {};
};

/**
 * \class llbad_caen_xx725_30_dpp_vme_error
 * \brief exception class for CAEN XX725_30 ADC when VME communication fails
 */
class llbad_caen_xx725_30_dpp_vme_error: public llbad_caen_xx725_30_dpp {
	public:
	llbad_caen_xx725_30_dpp_vme_error(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30_dpp:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_vme_error() throw() {};
};

/**
 * \class llbad_caen_xx725_30_vme_error
 * \brief exception class for CAEN XX725_30 ADC if a VME-only function is called from a board with an other formfactor
 *
class llbad_caen_xx725_30_vme_only: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_vme_only(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_vme_only() throw() {};
};
*/


/**
* \class caen_xx725_30_dpp
* \brief implementation of 725/730 waveform digitizer family with DPP-PSD firmware    
*/
class caen_xx725_30_dpp : public caen_xx725_30 {
    private:
        ///Addresses of registers
        //_BC = broadcast
        enum RegAddr {
            READOUT_BUFFER          = 0x0000,
            CHN_MULTIPLIER          = 0x0100,   
            RECORD_LENGTH           = 0x1020, 
            RECORD_LENGTH_BC        = 0x8020,            
            EVENTS_PER_AGGREGATE    = 0x1034,
            EVENTS_PER_AGGREGATE_BC = 0x8034,
            PRE_TRIGGER             = 0x1038,
            PRE_TRIGGER_BC          = 0x8038,
            CFD_SETTINGS            = 0x103C,
            CFD_SETTINGS_BC         = 0x803C,
            FORCED_DATA_FLUSH       = 0x1040,
            FORCED_DATA_FLUSH_BC    = 0x8040,
            CHARGE_ZERO_SUPPR_THR   = 0x1044,
            CHARGE_ZERO_SUPPR_THR_BC= 0x8044,
            SHORT_GATE_WIDTH        = 0x1054,
            SHORT_GATE_WIDTH_BC     = 0x8054,
            LONG_GATE_WIDTH         = 0x1058,
            LONG_GATE_WIDTH_BC      = 0x8058,
            GATE_OFFSET             = 0x105C,
            GATE_OFFSET_BC          = 0x805C,
            TRIGGER_THRESHOLD       = 0x1060,
            TRIGGER_THRESHOLD_BC    = 0x8060,
            FIXED_BASELINE          = 0x1064,
            FIXED_BASELINE_BC       = 0x8064,
            SHAPED_TRIGGER_WIDTH    = 0x1070,
            SHAPED_TRIGGER_WIDTH_BC = 0x8070,
            TRIGGER_HOLD_OFF_WIDTH  = 0x1074,
            TRIGGER_HOLD_OFF_WIDTH_BC= 0x8074,
            THRESHOLD_PSD_CUT       = 0x1078,
            THRESHOLD_PSD_CUT_BC    = 0x8078,
            PUR_GAP_THRESHOLD       = 0x107C,
            PUR_GAP_THRESHOLD_BC    = 0x807C,
            DPP_ALGORITHM_CONTROL   = 0x1080,             
            DPP_ALGORITHM_CONTROL_BC= 0x8080, 
            DPP_ALGORITHM_CONTROL2  = 0x1084,             
            DPP_ALGORITHM_CONTROL2_BC= 0x8084,
            CHN_STATUS_DPP          = 0x1088, 
            AMC_FIRMWARE_DPP        = 0x108C,
            INDIVIDUAL_SW_TRIGGER   = 0x10C0,
            INDIVIDUAL_SW_TRIGGER_BC= 0x80C0,
            VETO_WIDTH              = 0x10D4,
            VETO_WIDTH_BC           = 0x80D4,
            BASELINE_FREEZE_TIME    = 0x10D8,
            BASELINE_FREEZE_TIME_BC = 0x80D8,            
            BOARD_CONFIG            = 0x8000, //TODO: 0x8004 (BitSet), 0x8008 BitClear           
            CHANNELS_SHUTDOWN       = 0x80BC,
            ACQ_CONTROL             = 0x8100, //TODO: just one bit[5] is different
            TIME_BOMB_DOWNCOUNTER   = 0x8158,
            DISABLE_EXTERNAL_TRIGGER= 0x817C,
            TRIGGER_VALIDATION_MASK = 0x8180,
            AGGREGATE_NUMBER_BLT    = 0xEF1C,

            CONFIGURATION_ROM       = 0xF000,
            CONFIGURATION_ROM2      = 0xF080
        };

        /*enum vmeAMs {
			AM_A24			=0x39,
			AM_A32			=0x09,
			AM_A32_BLT		=0x0B,
			AM_A32_MBLT		=0x08
		};*/

public:
    #if __BYTE_ORDER == __BIG_ENDIAN
        #error("There is no definition of data structures for big endian byte order in CAEN_xx725_30_dpp.")
    #endif

    #if __BYTE_ORDER == __LITTLE_ENDIAN

        union chn_status_dpp {
            struct {
                uint32_t dummy1: 2;
                uint32_t dac_busy: 1;
                uint32_t calib_status: 1;
                uint32_t dummy2: 4;
                uint32_t adc_power_down: 1;
                uint32_t dummy3: 23; 
            };
            uint32_t raw;
        };

        union amc_firmware_dpp {
            struct {
                uint32_t revision: 8;
                uint32_t dpp_code: 8;
                uint32_t day_low: 4;
                uint32_t day_high: 4;
                uint32_t month: 4;
                uint32_t year: 4;
            };
            uint32_t raw;
        };

        union cfd_settings {
            struct {
                uint32_t delay: 8;
                uint32_t fraction: 2; //cfd fraction
                uint32_t ip_points: 2; //interpolation points
                uint32_t dummy: 20;
            };
            uint32_t raw;
        };

        union dpp_algorithm_control {
            struct {
                uint32_t q_sensitivity: 3;
                uint32_t dummy1: 1;
                uint32_t q_pedestal: 1; 
                uint32_t trg_counting: 1;
                uint32_t discrimination: 1;
                uint32_t pile_up: 1;
                uint32_t test_pulse: 1;
                uint32_t test_rate: 2;
                uint32_t dummy2: 5;
                uint32_t polarity: 1;
                uint32_t dummy3: 1;  
                uint32_t trigger_mode: 2;
                uint32_t base_mean: 3;
                uint32_t dummy4: 1;
                uint32_t self_trigger: 1;
                uint32_t charge_suppr: 1;
                uint32_t pile_up_rej: 1;
                uint32_t psd_cut_below: 1;
                uint32_t psd_cut_above: 1;
                uint32_t over_range: 1;
                uint32_t hysteresis: 1;
                uint32_t opp_polarity: 1;
            };
            uint32_t raw;
        };

        union dpp_algorithm_control2 {
            struct {
                uint32_t local_trigger_mode: 2;
                uint32_t en_local_trigger: 1;
                uint32_t dummy1: 1;
                uint32_t validation_mode: 2;
                uint32_t en_validation: 1;
                uint32_t dummy2: 1;
                uint32_t word_opt: 3;
                uint32_t smooth: 1;
                uint32_t smooth_factor: 4;
                uint32_t step: 2;
                uint32_t veto_src: 2;
                uint32_t dummy3: 4;
                uint32_t mark_saturated: 1;
                uint32_t dummy4: 7;
            };
            uint32_t raw;
        };

        union veto_width {
            struct {
                uint32_t extension: 16;
                uint32_t step_width: 2;
                uint32_t dummy: 14;                
            };
            uint32_t raw;
        };

        union board_config_dpp {
            struct {
                uint32_t auto_data_flush: 1; 
                uint32_t dummy1: 1; //must be 0
                uint32_t trigger_prop: 1; 
                uint32_t dummy2: 1; //must be 0
                uint32_t dummy3: 1; //must be 1!
                uint32_t dummy4: 3; //must be 0!
                uint32_t individual_trigger: 1; //must be 1
                uint32_t dummy6: 2; //must be 0
                uint32_t dual_trace: 1;
                uint32_t analog_probe_sel: 2;
                uint32_t dummy7: 2;
                uint32_t waveform_rec: 1;
                uint32_t extras_rec: 1;
                uint32_t time_stamp_rec: 1; //must be 1
                uint32_t charge_rec: 1; //must be 1
                uint32_t dummy8: 3; // must be 0
                uint32_t vir_probe_1: 3;
                uint32_t vir_probe_2: 3;
                uint32_t dummy9: 2; //must be 0
                uint32_t digital_trace: 1;
            };
            uint32_t raw;
        };

        union trigger_validation_mask {
            struct {
                uint32_t participants: 8;
                uint32_t op_mask: 2;
                uint32_t maj_level: 3;
                uint32_t dummy: 15;
                uint32_t lvds_global_trigger: 1;
                uint32_t lvds_idv_trigger: 1;
                uint32_t ext_trigger: 1;
                uint32_t sw_trigger: 1;
            };
            uint32_t raw;
        };

        union fp_lvds_new_features_dpp {
            struct {
                uint32_t config_3_0: 4;
                uint32_t config_7_4: 4;
                uint32_t config_11_8: 4;
                uint32_t config_15_12: 4;
                uint32_t what_copy: 1;
                uint32_t dummy: 15;
            };
            uint32_t raw;
        };
    #endif
        
    private:    
        caen_xx725_30_dpp(const caen_xx725_30_dpp& t); //hidden copy constructor
        caen_xx725_30_dpp& operator=(const caen_xx725_30_dpp& t); //hidden assignment operator

    public:
        caen_xx725_30_dpp(N_CHN n_channel, vme_interface *interface, vme_addr_t baseaddr);

    /*************************************************************************/
    //aggregate/event reading
        //void start();
        //void stop();       

    /***************************************************************************/
    //implementations for caen_event_reader
        bool isEventReady();
        uint32_t getNextEventSize();
        vme_interface* getInterface();
        vme_addr_t getEventReadoutAddress();
        int getMaxEventsPerBLT();
        void setMaxEventsPerBLT(int number);

    /*************************************************************************/
    //register manipulator
        void setCoupleRecordLength(int couple, uint16_t length);
        void setAllCouplesRecordLength(uint16_t length);
        uint16_t getCoupleRecordLength(int channel);

        void setCoupleNumEventsPerAggregate(int couple, uint16_t number);
        void setAllCouplesNumEventsPerAggregate(uint16_t number);        
        uint16_t getCoupleNumEventsPerAggregate(int channel);

        void setChannelPreTrigger(int channel, uint16_t trigger);
        void setAllChannelsPreTrigger(uint16_t trigger);
        uint16_t getChannelPreTrigger(int channel);

        void setChannelCFDSettings(int channel, cfd_settings settings);
        void setAllChannelsCFDSettings(cfd_settings settings);
        cfd_settings getChannelCFDSettings(int channel);
    
        void channelForceDataFlush(int channel);
        void allChannelForceDataFlush();

        void setChannelChargeZeroSuppThreshold(int channel, uint16_t threshold);
        void setAllChannelsChargeZeroSuppThreshold(uint16_t threshold);
        uint16_t getChannelChargeZeroSuppThreshold(int channel);

        void setChannelShortGateWidth(int channel, uint16_t width);
        void setAllChannelsShortGateWidth(uint16_t width);
        uint16_t getChannelShortGateWidth(int channel);

        void setChannelLongGateWidth(int channel, uint16_t width);
        void setAllChannelsLongGateWidth(uint16_t width);
        uint16_t getChannelLongGateWidth(int channel);

        void setChannelGateOffset(int channel, uint8_t offset);
        void setAllChannelsGateOffset(uint8_t offset);
        uint8_t getChannelGateOffset(int channel);  

        void setChannelTriggerThreshold(int channel, uint16_t value);  
        void setAllChannelsTriggerThreshold(uint16_t value);          
        uint16_t getChannelTriggerThreshold(int channel);

        void setChannelFixedBaseline(int channel, uint16_t baselineValue);
        void setAllChannelsFixedBaseline(uint16_t baselineValue);
        uint16_t getChannelFixedBaseline(int channel);

        void setChannelShapedTriggerWidth(int channel, uint16_t width);
        void setAllChannelsShapedTriggerWidth(uint16_t width);
        uint16_t getChannelShapedTriggerWidth(int channel);

        void setChannelTriggerHoldOffWidth(int channel, uint16_t width);
        void setAllChannelsTriggerHoldOffWidth(uint16_t width);
        uint16_t getChannelTriggerHoldOffWidth(int channel);

        void setChannelThresholdPSDCut(int channel, uint16_t cut);
        void setAllChannelsThresholdPSDCut(uint16_t cut);
        uint16_t getChannelThresholdPSDCut(int channel);

        void setChannelPURGAPThreshold(int channel, uint16_t threshold);
        void setAllChannelsPURGAPThreshold(uint16_t threshold);
        uint16_t getChannelPURGAPThreshold(int channel);

        void setChannelDPPAlgorithmControl(int channel, dpp_algorithm_control control);
        void setAllChannelsDPPAlgorithmControl(dpp_algorithm_control control);
        dpp_algorithm_control getChannelDPPAlgorithmControl(int channel);

        void setChannelDPPAlgorithmControl2(int channel, dpp_algorithm_control2 control);
        void setAllChannelsDPPAlgorithmControl2(dpp_algorithm_control2 control);
        dpp_algorithm_control2 getChannelDPPAlgorithmControl2(int channel);

        chn_status_dpp getChannelStatusDPP(int channel);

        amc_firmware_dpp getChannelAMCFirmwareDPP(int channel);
        static void printAMCFirmwareDPP(amc_firmware_dpp firmware);
    
        void channelSoftwareTrigger(int channel);
        void allChannelsSoftwareTrigger();

        void setChannelVetoWidth(int channel, veto_width width);
        void setAllChannelsVetoWidth(veto_width width);
        veto_width getChannelVetoWidth(int channel);

        void setChannelBaselineFreezeTime(int channel, uint16_t time);
        void setAllChannelsBaselineFreezeTime( uint16_t time);
        uint16_t getChannelBaselineFreezeTime(int channel);

        void setBoardConfigDPP(board_config_dpp config);
        board_config_dpp getBoardConfigDPP();

        void setAggregateOrganization(uint8_t code);
        uint8_t getAggregateOrganization();

        void setChannelsShutdown(uint8_t shutdown);

        uint32_t getTimeBombDowncounter();

        void setDisableExternalTrigger(uint8_t reg);
        uint8_t getDisableExternalTrigger();

        void setCoupleTriggerValidationMask(int couple, trigger_validation_mask mask);
        trigger_validation_mask getCoupleTriggerValidationMask(int couple);

        void setFrontPanelLVDSNewFeatures(fp_lvds_new_features_dpp features_dpp);
        fp_lvds_new_features_dpp getFrontPanelLVDSNewFeatures();

        void setMaxNumberAggregatesPerBLT(uint8_t number);
        uint8_t getMaxNumberAggregatesPerBLT();

    /* following are overwritten for deactivation **/
        caen_xx725_30::amc_firmware getChannelAMCFirmware(int channel);
        void setBoardConfig(caen_xx725_30::board_config config);
        caen_xx725_30::board_config getBoardConfig();
        void setChannelPulseWidth(int channel, uint8_t width);
        void setAllChannelPulseWidth(uint8_t width);
        uint8_t getChannelPulseWidth(int channel);   
        void setCoupleSelfTriggerLogic(int couple, couple_self_trigger_logic logic);
        void setAllCoupleSelfTriggerLogic(couple_self_trigger_logic logic);
        couple_self_trigger_logic getCoupleSelfTriggerLogic(int couple);
        void setPostTrigger(uint32_t trigger);
        uint32_t getPostTrigger();
        //uint32_t getEventsStored();
        void setMemBufferAlmostFullLevel(uint16_t level);
        uint16_t getMemBufferAlmostFullLevel();
        void setMaxNumberEventsPerBLT(uint16_t maxNumber);
        uint16_t getMaxNumberEventPerBLT();
    /* end overwritten*/
};

