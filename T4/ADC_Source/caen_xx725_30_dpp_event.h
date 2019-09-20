#pragma once
#include <stdint.h>
#include "caen_xx725_30_dpp_chn_agg.h"

//forward declaration
class caen_xx725_30_dpp_chn_agg;

class caen_xx725_30_dpp_event {
private:
    uint32_t* data;  
	caen_xx725_30_dpp_chn_agg* chn_agg;
public:
    caen_xx725_30_dpp_event(uint32_t* buffer,caen_xx725_30_dpp_chn_agg* chn_agg);
    int getChannel() const;
    uint16_t* getSamples(int* numSamples) const;

    uint32_t getTriggerTimeTag() const;

	uint32_t getExtras() const;

	uint16_t getQLong() const;

	uint16_t getQShort() const;

	bool isOddChannel() const;   

	bool isPUR() const;  

    bool hasExtras() const;

	void checkSamplesEnabled() const;
	void checkExtrasEnabled() const;

    uint32_t getFormat() const;
};

