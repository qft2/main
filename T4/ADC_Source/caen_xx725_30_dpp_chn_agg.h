#pragma once
#include "caen_xx725_30_dpp_event.h"
#include <stdint.h>
#include <exceptions.h>


/**
 * \class llbad_caen_xx725_30_dpp_chn_agg
 * \brief standard exception class for CAEN xx725_30 Event class
 */
class llbad_caen_xx725_30_dpp_chn_agg: public virtual llexception {
	public:
	llbad_caen_xx725_30_dpp_chn_agg(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30_dpp_chn_agg:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_chn_agg() throw() {};
};

//forward declaration
class caen_xx725_30_dpp_event;

/**
* \class caen_xx725_30_dpp_chn_agg
* \brief data structure for readout of 725/730 with DPP-PSD firmware, is also called board aggregate   
* a channel aggregate holds events of a couple of two channels
*/
class caen_xx725_30_dpp_chn_agg {

public:
    #if __BYTE_ORDER == __BIG_ENDIAN
        #error("There is no definition of data structures for big endian byte order in CAEN_xx725_30 channel aggregate.")
    #endif
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        union chn_agg_format {
            struct {
                uint32_t n_samples: 16;
                uint32_t dp1: 3;
                uint32_t dp2: 3;
                uint32_t ap: 2;
                uint32_t extras: 3;
                uint32_t samples_en: 1;
                uint32_t extras_en: 1;
                uint32_t time_tag_en: 1;
                uint32_t charge_en: 1;
                uint32_t dual_trace_en: 1;        
            };
            uint32_t raw;
        };
    #endif
        
private:
    uint32_t *data;     
    uint8_t couple;

public:    
    caen_xx725_30_dpp_chn_agg(uint32_t *buffer, uint8_t couple);
    
    ///number of events in channel aggregate
    int getNumberOfEvents() const;
    int getCouple() const;
    chn_agg_format getChannelAggregateFormat() const;
    caen_xx725_30_dpp_event getEvent(const int eventNum);     
    
    ///size of one event in 32bit words
    int getEventSize() const;  

    ///size of this channel aggregate
    uint32_t getChnAggSize() const;      

    void checkValidEventNum(const int eventNum) const;

    static void printFormat(chn_agg_format format);
};
