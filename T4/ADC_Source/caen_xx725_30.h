/**
 * \file caen_xx725_30.h
 * \author Christian Wysotzki
 * \date Apr 2017
 * \brief base implementation for the CAEN 725/730 digitizer family, based on V1740 implementation
 */
#pragma once

#include <vme/vme_module.h>
#include <vme/vme_interface.h>
#include <vector>
#include <string>
#include "caen_xx725_30_ro_interface.h"
#include "caen_event_reader.h"


/**
 * \class llbad_caen_xx725_30
 * \brief standard exception class for CAEN XX725_30 ADC
 */
class llbad_caen_xx725_30: public virtual llexception {
	public:
	llbad_caen_xx725_30(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30() throw() {};
};


/**
 * \class llbad_caen_xx725_30_invalid_channel
 * \brief exception class for CAEN XX725_30 ADC requested channel number is invalid
 */
class llbad_caen_xx725_30_invalid_channel: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_invalid_channel(std::string newtext="",int errnumber=0):
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_invalid_channel() throw() {};
};

/**
 * \class llbad_caen_xx725_30_invalid_couple
 * \brief exception class for CAEN XX725_30 ADC requested couple number is invalid
 */
class llbad_caen_xx725_30_invalid_couple: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_invalid_couple(std::string newtext="",int errnumber=0):
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_invalid_couple() throw() {};
};

/**
 * \class llbad_caen_xx725_30_vme_error
 * \brief exception class for CAEN XX725_30 ADC when VME communication fails
 */
class llbad_caen_xx725_30_vme_error: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_vme_error(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_vme_error() throw() {};
};

/**
 * \class llbad_caen_xx725_30_vme_error
 * \brief exception class for CAEN XX725_30 ADC if a VME-only function is called from a board with an other formfactor
 */
class llbad_caen_xx725_30_vme_only: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_vme_only(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_vme_only() throw() {};
};

/**
 * \class llbad_caen_xx725_30_busy
 * \brief exception class for CAEN XX725_30 ADC if one or more channels are busy
 */
class llbad_caen_xx725_30_busy: public llbad_caen_xx725_30 {
	public:
	llbad_caen_xx725_30_busy(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_busy() throw() {};
};



/**
* \class caen_xx725_30 
* \brief base implementation for all devices of the waveform digitizer family 725/730, if you are running with a dpp firmware, use the according implementation \see caen_xx725_30_dpp
* ATTENTION: register have different interpretation if DPP-PSD firmware is installed on the device!
*/
class caen_xx725_30 : public vme_module, public caen_event_reader {
    private:
        ///Addresses of registers
        enum RegAddr : uint16_t {
            READOUT_BUFFER          = 0x0000,
            CHN_MULTIPLIER          = 0x0100,            
            CHN_INPUT_DYN_RANGE     = 0x1028,
            CHN_INPUT_DYN_RANGE_BC  = 0x8028, //broadcast write to all channels
            CHN_PULSE_WIDTH         = 0x1070,
            CHN_PULSE_WIDTH_BC      = 0x8070, //broadcast write to all channels
            CHN_TRG_THRESHOLD       = 0x1080,             
            CHN_TRG_THRESHOLD_BC    = 0x8080, //broadcast write to all channels
            CHN_STATUS              = 0x1088,
            CHN_AMC_FIRMWARE        = 0x108C,
            CHN_DC_OFFSET           = 0x1098,
            CHN_DC_OFFSET_BC        = 0x8098, //broadcast write to all channels
            CHN_ADC_TEMP            = 0x10A8,

            COUPLE_SELF_TRG_LOGIC   = 0x1084,
            COUPLE_SELF_TRG_LOGIC_BC= 0x8084, //broadcast write to all couples

            BOARD_CONFIG            = 0x8000,  //0x8004 (BitSet), 0x8008 BitClear
            BUFFER_ORGANIZATION     = 0x800C,
            CUSTOM_SIZE             = 0x8020, 

            ADC_CALIBRATION         = 0x809C,
            ACQ_CONTROL             = 0x8100,
            ACQ_STATUS              = 0x8104,
            SOFTWARE_TRIGGER        = 0x8108,
            GLOBAL_TRIGGER_MASK     = 0x810C,
            FRONT_PANEL_MASK        = 0x8110,                    //Front Panel TRG-OUT (GPO) Enable Mask
            POST_TRIGGER            = 0x8114, 
            LVDS_I_O_DATA           = 0x8118,
            FRONT_PANEL_I_O_CONTROL = 0x811C,
            CHANNEL_ENABLE_MASK     = 0x8120, //Channel Enable Mask
            ROC_FIRMWARE            = 0x8124,
            EVENTS_STORED           = 0x812C,
            SET_MONITOR_DAC         = 0x8138,
            SOFTWARE_CLOCK_SYNC     = 0x813C,
            BOARD_INFO              = 0x8140,
            MONITOR_DAC_MODE        = 0x8144,
            EVENT_SIZE              = 0x814C, 
            FAN_SPEED_CONTROL       = 0x8168,
            BUFF_ALMOST_FULL_LEVEL  = 0x816C, //memory buff almost full level
            CONTROL_DELAY           = 0x8170, //Run/Start/Stop Delay
            BOARD_FAILURE_STATUS    = 0x8178,
            FRONT_PANEL_NEW_FEATURES= 0x81A0,
            CHANNELS_SHUTDOWN       = 0x81C0, 
            READOUT_CONTROL         = 0xEF00,
            READOUT_STATUS          = 0xEF04,
            BOARD_ID                = 0xEF08,
            MCST_BASE_ADDR_AND_CTRL = 0xEF0C,                     //MCST Base Address and Control
            RELOCATION_ADDR         = 0xEF10,
            INTERRUPT_STATUS_ID     = 0xEF14,
            INTERRUPT_EVENT_NUM     = 0xEF18,
            MAX_NUM_EVENTS_PER_BLT  = 0xEF1C, 
            SCRATCH                 = 0xEF20, 
            SOFTWARE_RESET          = 0xEF24,
            SOFTWARE_CLEAR          = 0xEF28,
            CONFIGURATION_RELOAD    = 0xEF34,
            CONFIGURATION_ROM       = 0xF000,
            CONFIGURATION_ROM2      = 0xF080
        };

    public:
	enum memory_size{
		SRAM640k = 640<<10,
		SRAM5M12 = 5120<<10
	};

	enum monitor_dac_mode{
		DAC_TRIGGER_MAJORITY = 0x0,
		DAC_TEST = 0x1,
		DAC_BUFFER_OCCUPANCY = 0x3,
		DAC_VOLTAGE_LEVEL = 0x4
	};

        

public:
    #if __BYTE_ORDER == __BIG_ENDIAN
        #error("There is no definition of data structures for big endian byte order in CAEN_xx725_30.")
    #endif

    #if __BYTE_ORDER == __LITTLE_ENDIAN

        union couple_self_trigger_logic {
            struct {
                uint32_t logic: 2;
                uint32_t signalGen: 1;
                uint32_t dummy: 29;
            };
            uint32_t raw;
        };

        union chn_status {
            struct {
                uint32_t mem_full: 1;
                uint32_t mem_empty: 1;
                uint32_t dac_busy: 1;
                uint32_t calib_status: 1;
                uint32_t dummy1: 4;
                uint32_t adc_power_down: 1;
                uint32_t dummy2: 23; 
            };
            uint32_t raw;
        };

        union amc_firmware {
            struct {
                uint32_t minor: 8;
                uint32_t major: 8;
                uint32_t day: 8;
                uint32_t month: 4;
                uint32_t year: 4;
            };
            uint32_t raw;
        };

        union board_config {
            struct {
                uint32_t dummy1: 1; //must be 0!
                uint32_t overlap: 1; //trigger overlap setting
                uint32_t dummy2: 1; //must be 0!
                uint32_t test_enabled: 1;
                uint32_t dummy3: 1; //must be 1!
                uint32_t dummy4: 1; //must be 0!
                uint32_t polarity: 1; //self trigger polarity
                uint32_t dummy5: 25; //must be 0
            };
            uint32_t raw;
        };

        union acq_control {
            struct {
                uint32_t mode: 2; //Start/Stop Mode selection
                uint32_t start: 1;
                uint32_t counting: 1;
                uint32_t dummy1: 1; 
                uint32_t mem_full: 1;
                uint32_t clk_src: 1;
                uint32_t dummy2: 1;
                uint32_t lvds_busy: 1;
                uint32_t lvds_veto: 1;
                uint32_t dummy3: 1;
                uint32_t lvds_run: 1;
            };
            uint32_t raw;
        };

        union acq_status {
            struct {
                uint32_t dummy1: 2;
                uint32_t run: 1; //0=stopped, 1=running
                uint32_t event_ready: 1;
                uint32_t event_full: 1;
                uint32_t clk_src: 1;
                uint32_t bypass: 1;
                uint32_t unlock: 1;
                uint32_t board_ready: 1;
                uint32_t dummy2: 6;
                uint32_t in_status: 1;
                uint32_t trg_in_status: 1;
                uint32_t dummy3: 2;
                uint32_t shutdown: 1;
                uint32_t temp_status: 4;
                uint32_t dummy4: 8;
            };
            uint32_t raw;
        };

        union global_trigger_mask {
            struct {
                uint32_t trigger_req: 8;
                uint32_t dummy1: 12;
                uint32_t maj_window: 4;
                uint32_t maj_level: 3;
                uint32_t dummy2: 2;
                uint32_t lvds_trigger: 1;
                uint32_t extern_trigger: 1;
                uint32_t sw_trigger: 1;
            };
            uint32_t raw;
        };

        union front_panel_mask {
            struct {
                uint32_t participants: 8;
                uint32_t logic: 2;
                uint32_t maj_level: 3;
                uint32_t dummy1: 16;
                uint32_t lvds_trigger: 1;
                uint32_t extern_trigger: 1;
                uint32_t sw_trigger: 1;
            };
            uint32_t raw;
        };

        union front_panel_io_control {
            struct {
                uint32_t el_level: 1;
                uint32_t lvds_enable: 1;
                uint32_t lvds_direction1: 1;
                uint32_t lvds_direction2: 1;
                uint32_t lvds_direction3: 1;
                uint32_t lvds_direction4: 1;
                uint32_t signal_config: 2;
                uint32_t new_features: 1;
                uint32_t pattern_latch: 1;
                uint32_t trg_in_control: 1;
                uint32_t trg_in_to_mezz: 1;
                uint32_t dummy1: 2;
                uint32_t force_trg_out: 1;
                uint32_t trg_out_mode: 1;
                uint32_t trg_out_sel: 2;
                uint32_t vp_sel: 2;
                uint32_t busy_unlock: 1;
                uint32_t pattern_config: 2;
                uint32_t dummy2: 9;
            };
            uint32_t raw;
        };

        union roc_fpga_firmware {
            struct {
                uint32_t minor: 8;
                uint32_t major: 8;
                uint32_t day: 8;
                uint32_t month: 4;
                uint32_t year: 4;

            };
            uint32_t raw;
        };

        union board_info {
            struct {
                uint32_t family: 8; //0x0E = 725, 0x0B = 730
                uint32_t mem_size: 8;//0x01 = 640kS mem per channel, 0x08 = 5.12 MS mem per channel
                uint32_t equipped_channel: 8; //0x10 = 16 equipped channel, 0x08 = 8 equipped channel
                uint32_t dummy1: 8;
            };  
            uint32_t raw;
        };

        union fan_speed_control {
            struct {
                uint32_t dummy1: 3; //must be 0
                uint32_t mode: 1; //0 = slow or automatic speed tuning, 1= high speed
                uint32_t dummy2: 2; //must be 1
                uint32_t dummy3: 26; //must be 0
            };
            uint32_t raw;
        };

        union board_failure_status {
            struct {
                uint32_t internal_timeout: 4;
                uint32_t lock_loss: 1;
                uint32_t temp_fail: 1;
                uint32_t adc_power_down: 1;
                uint32_t dummy: 25;
            };
            uint32_t raw;
        };

        union fp_lvds_new_features {
            struct {
                uint32_t config_3_0: 4;
                uint32_t config_7_4: 4;
                uint32_t config_11_8: 4;
                uint32_t config_15_12: 4;
                uint32_t dummy: 16;
            };
            uint32_t raw;
        };

        union readout_control {
            struct {
                uint32_t interrupt_level: 3; //VME interrupt level (vme only)
                uint32_t ol_interrupt_en: 1; // optical link interrupt enable
                uint32_t vme_berr: 1; //VME bus error
                uint32_t vme_align64: 1; //(vme only)
                uint32_t vme_ba_reloc: 1; //VME base address relocation
                uint32_t interrupt_release_mode: 1;
                uint32_t eBLT_en: 1; //(vme only) extended block transfer enable
                uint32_t dummy: 23;
            };
            uint32_t raw;
        };

        union readout_status {
            struct {
                uint32_t event_ready: 1;
                uint32_t out_buff_status: 1;
                uint32_t bus_error: 1;
                uint32_t dummy: 29; //vme only
            };
            uint32_t raw;
        };

        union mcst_ba_addr_ctrl {
            struct {
                uint32_t addr: 8;
                uint32_t position: 2;
                uint32_t dummy: 22;
            };  
            uint32_t raw;
        };

    #endif //little_endian

        union configurationROM {
		    struct{
			    uint32_t checksum;          //starts with 0xF000
			    uint32_t checksum_length2;
			    uint32_t checksum_length1;
			    uint32_t checksum_length0;
			    uint32_t constant2;
			    uint32_t constant1;
			    uint32_t constant0;
			    uint32_t c_code;
			    uint32_t r_code;
			    uint32_t oui2;
			    uint32_t oui1;
			    uint32_t oui0;
			    uint32_t vers;
			    uint32_t formfactor;
			    uint32_t board1;
			    uint32_t board0;
			    uint32_t revis3;
			    uint32_t revis2;
			    uint32_t revis1;
			    uint32_t revis0;
                uint32_t flash;         //NR. 21, this word starts at 0xF050, 
                uint32_t sernum1;       //this one at 0xF080
			    uint32_t sernum0;
                uint32_t vcxo;
		    };
                
		    uint32_t raw[24];
	    };

        ///defines if the device has either 16 channel (VME only) or 8 channel
        enum N_CHN : uint8_t {
            CHN16   = 0x10, //only for VME boards possible
            CHN8    = 0x08        
        };
    private:
        N_CHN n_channel;  //number of channels for this device
        vme_interface *vme;
        vme_addr_t ba;
        board_info info;
        bool readRom; //have the ROM been read
        configurationROM rom;
        readout_control roControl;
	bool isInit;
	uint16_t maxBltEvents;

        caen_xx725_30(const caen_xx725_30& t); //hidden copy constructor
		caen_xx725_30& operator=(const caen_xx725_30& t); //hidden assignment operator

    public:
        caen_xx725_30(N_CHN n_channel, vme_interface *interface, vme_addr_t baseaddr);     

        void init();   

        void readConfigurationROM();

        N_CHN getNChannel();
       
        bool isRunning();

        vme_addr_t getBaseAddress();
        
    /***************************************************************************/
    //implementations for caen_event_reader
        virtual void start();
        virtual void stop();
        virtual bool isEventReady();
        virtual uint32_t getNextEventSize();
        virtual vme_interface* getInterface();
        virtual vme_addr_t getEventReadoutAddress();
        virtual int getMaxEventsPerBLT();
        virtual void setMaxEventsPerBLT(int number);
        virtual bool isBerrEnabled();

    /***************************************************************************/
    //register manipulator
        void setChannelInputRange(int channel, uint8_t range);
        void setAllChannelsInputRange(uint8_t range);
        uint8_t getChannelInputRange(int channel);

        virtual void setChannelPulseWidth(int channel, uint8_t width);
        virtual void setAllChannelsPulseWidth(uint8_t width);
        virtual uint8_t getChannelPulseWidth(int channel);
    
        virtual void setChannelTriggerThreshold(int channel, uint16_t threshold);
        virtual void setAllChannelsTriggerThreshold(uint16_t threshold);
        virtual uint16_t getChannelTriggerThreshold(int channel);

        virtual void setCoupleSelfTriggerLogic(int couple, couple_self_trigger_logic logic);
        virtual void setAllCouplesSelfTriggerLogic(couple_self_trigger_logic logic);
        virtual couple_self_trigger_logic getCoupleSelfTriggerLogic(int couple);

        chn_status getChannelStatus(int channel);
        static void printChannelStatus(chn_status stat);

        virtual amc_firmware getChannelAMCFirmware(int channel);
        static void printAMCFirmware(amc_firmware firmware);

        void setChannelDCOffset(int channel, uint16_t offset);
        void setAllChannelsDCOffset(uint16_t offset);
        uint16_t getChannelDCOffset(int channel);

        uint8_t getChannelADCTemperature(int channel);

        virtual void setBoardConfig(board_config config);
        virtual board_config getBoardConfig();

        void setBufferOrganization(uint8_t bufferCode);
        uint8_t getBufferOrganization();

        void setCustomSize(uint32_t customSize);
        uint32_t getCustomSize();

        void calibrate();

        void setACQControl(acq_control control);
        acq_control getACQControl();

        acq_status getACQStatus();
        static void printACQStatus(acq_status status);

        void softwareTrigger();

        void setGlobalTriggerMask(global_trigger_mask mask);
        global_trigger_mask getGlobalTriggerMask();

        void setFrontPanelMask(front_panel_mask mask);
        front_panel_mask getFrontPanelMask();

        virtual void setPostTrigger(uint32_t trigger);
        virtual uint32_t getPostTrigger();

        void setLVDSIOData(uint16_t data);
        uint16_t getLVDSIOData();

        void setFrontPanelIOControl(front_panel_io_control control);
        front_panel_io_control getFrontPanelIOControl();

        void setChannelEnableMask(uint16_t mask);
        uint16_t getChannelEnableMask();

        roc_fpga_firmware getROCFPGAFirmware();
        static void printROCFPGAFirmware(roc_fpga_firmware firmware);

        virtual uint32_t getEventsStored();

        void setMonitorDAC(uint16_t voltage);
        uint16_t getMonitorDAC();

        void softwareClockSync();

        board_info getBoardInfo();
        static void printBoardInfo(board_info info);

        void setMonitorDACMode(uint8_t mode);
        uint8_t getMonitorDACMode();

        uint32_t getEventSize();

        void setFanSpeedControl(fan_speed_control control);
        fan_speed_control getFanSpeedControl();

        virtual void setMemBufferAlmostFullLevel(uint16_t level);
        virtual uint16_t getMemBufferAlmostFullLevel();

        void setRunStartStopDelay(uint32_t delay);
        uint32_t getRunStartStopDelay();

        board_failure_status getBoardFailureStatus();
        static void printBoardFailureStatus(board_failure_status status);

        void setFrontPanelLVDSNewFeatures(fp_lvds_new_features features);
        fp_lvds_new_features getFrontPanelLVDSNewFeatures();

        virtual void setChannelsShutdown(uint8_t shutdown);

        void setReadoutControl(readout_control control);
        readout_control getReadoutControl();

        readout_status getReadoutStatus();
        static void printReadoutStatus(readout_status status);

        void setBoardID(uint8_t geo_address);
        uint8_t getBoardID();

        void setMCSTBaseAddressAndControl(mcst_ba_addr_ctrl reg);
        mcst_ba_addr_ctrl getMCSTBaseAddressAndControl();

        void setRelocationAddress(uint16_t address);
        uint16_t getRelocationAddress();

        void setInterruptStatus_ID(uint32_t status_id);
        uint32_t getInterruptStatus_ID();

        void setInterruptEventNumber(uint16_t ien);
        uint16_t getInterruptEventNumber();

        virtual void setMaxNumberEventsPerBLT(uint16_t maxNumber);
        virtual uint16_t getMaxNumberEventsPerBLT();

        void softwareReset();
        void softwareClear();
        void configurationReload();

        configurationROM getConfigurationROM();
        static void printConfigurationROM(configurationROM rom);


        bool isVME();
        bool isNIM();
        bool isDT();
        //following more protected like        
        //functions with "check" throw an exception
        void checkIsValidChannel(int channel);
        void checkIsValidCouple(int couple);
        void checkIsVME();
        void checkIsDesktop();
        void checkIsChannelBusy(int channel);
        void checkIsAnyChannelBusy();
        uint32_t readReg(uint16_t addr);
        void writeReg(uint16_t addr, uint32_t ptr);
        void checkAMCFirmware(const std::string caller, const amc_firmware firmware,const int leastMajor,const int leastMinor);
        void checkROCFPGAFirmware(const std::string caller, const roc_fpga_firmware firmware,const int leastMajor,const int leastMinor);
	uint32_t calculateEventSize(uint8_t bufferOrganisation, uint8_t nLoc, uint8_t nChannels, uint32_t memory_size=SRAM640k);

    private:
        void checkEquippedChannel(board_info info);
};
