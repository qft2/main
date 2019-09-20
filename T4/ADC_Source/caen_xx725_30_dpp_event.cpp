#include "caen_xx725_30_dpp_event.h"

caen_xx725_30_dpp_event::caen_xx725_30_dpp_event(uint32_t* buffer, caen_xx725_30_dpp_chn_agg* chn_agg)
 : data(buffer), chn_agg(chn_agg){	
}
int caen_xx725_30_dpp_event::getChannel() const {
	return 2*chn_agg->getCouple()+isOddChannel();
}

uint32_t caen_xx725_30_dpp_event::getFormat() const {
	return chn_agg->getChannelAggregateFormat().raw;
}

/**
* bits[13:0] sample
* bit[14] digital virtual probe 1
* bit[15] digital virtual probe 2
* \param[out] numSamples number of samples which are accessible
*/
uint16_t* caen_xx725_30_dpp_event::getSamples(int *numSamples) const {
	checkSamplesEnabled();
	//word for size, format and trigger time tag
	*numSamples = chn_agg->getChannelAggregateFormat().n_samples*4;
	return (uint16_t*)(data+1);
}

/**
* true for the odd channel, false for the even channel
* eventNum starts at 0
*/
bool caen_xx725_30_dpp_event::isOddChannel() const {
	return (*(data)) >> 31;
}
/**
* bits[30:0] trigger time tag
*/
uint32_t caen_xx725_30_dpp_event::getTriggerTimeTag() const {
	return (*(data)) & 0x7FFFFFFF;
}
/**
* throws an exception if extras are not enabled
*/
uint32_t caen_xx725_30_dpp_event::getExtras() const {
	checkExtrasEnabled();
	caen_xx725_30_dpp_chn_agg::chn_agg_format format = chn_agg->getChannelAggregateFormat();
	int offset = 1 + format.samples_en*format.n_samples*4;
	return *(data+offset);
}

uint16_t caen_xx725_30_dpp_event::getQLong() const {
	caen_xx725_30_dpp_chn_agg::chn_agg_format format = chn_agg->getChannelAggregateFormat();
	int offset = 1 + format.samples_en*format.n_samples*4 + format.extras_en;
	return (*(data+offset)) >> 16; 
}
/**
* eventNum starts at 0
*/
uint16_t caen_xx725_30_dpp_event::getQShort() const {
	caen_xx725_30_dpp_chn_agg::chn_agg_format format = chn_agg->getChannelAggregateFormat();
	int offset = 1 + format.samples_en*format.n_samples*4 + format.extras_en;
	return (*(data+offset)) & 0x7FFF;
}

/**
* Pile up rejected flag
*/
bool caen_xx725_30_dpp_event::isPUR() const {
	caen_xx725_30_dpp_chn_agg::chn_agg_format format = chn_agg->getChannelAggregateFormat();
	int offset = 1 + format.samples_en*format.n_samples*4 + format.extras_en;
	return (*(data+offset)) & (1 << 15);
}

void caen_xx725_30_dpp_event::checkSamplesEnabled() const {
	if(!chn_agg->getChannelAggregateFormat().samples_en) {
		llbad_caen_xx725_30_dpp_chn_agg("Samples are not enabled");
	}	
}

bool caen_xx725_30_dpp_event::hasExtras() const {
	return chn_agg->getChannelAggregateFormat().extras_en;
}

void caen_xx725_30_dpp_event::checkExtrasEnabled() const {
	if(!hasExtras()) {
		llbad_caen_xx725_30_dpp_chn_agg("Samples are not enabled");
	}
}