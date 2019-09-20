#include "caen_xx725_30_dpp_chn_agg.h"
#include <cstdlib>
#include <iostream>

using namespace std;

/**
* \param buffer, buffer[0] states the channel aggregate size, and buffer[1] the format
*/
caen_xx725_30_dpp_chn_agg::caen_xx725_30_dpp_chn_agg(uint32_t *buffer, uint8_t couple) {
	data = buffer;
	this->couple = couple;
}

int caen_xx725_30_dpp_chn_agg::getNumberOfEvents() const {
	//first word states in bits[21:0] the channel aggregate size
	int size = getChnAggSize() - 2; //subtract size and format word
	if(size < 0)
		throw llbad_caen_xx725_30_dpp_chn_agg("Channel aggregate corrupt.");
	if(size == 0)
		return 0;
	
	int eventSize = getEventSize();
	if(eventSize == 0)
		throw llbad_caen_xx725_30_dpp_chn_agg("Event size of zero.");
	 
	div_t res = div(size, eventSize);
	if(res.rem != 0)
		throw llbad_caen_xx725_30_dpp_chn_agg("wrong event size.");
	return res.quot;
}

int caen_xx725_30_dpp_chn_agg::getCouple() const {
	return couple;
}

caen_xx725_30_dpp_chn_agg::chn_agg_format caen_xx725_30_dpp_chn_agg::getChannelAggregateFormat() const {
	chn_agg_format format;
	format.raw = data[1];
	return format;
}

caen_xx725_30_dpp_event caen_xx725_30_dpp_chn_agg::getEvent(int eventNum) {
	checkValidEventNum(eventNum);
	int offset = 2 + eventNum*getEventSize();
	return caen_xx725_30_dpp_event(data+offset, this);
}



/**
* size of one event in 32bit-words
*/
int caen_xx725_30_dpp_chn_agg::getEventSize() const {
	//calculate size of one event	
	chn_agg_format format = getChannelAggregateFormat();

	//divide by 4 to get the number of words (1 words = 2 samples)
	//charge_en and time_tag_en must be 1
	return format.n_samples * 4 + format.extras_en + format.charge_en + format.time_tag_en; 
}


/**
* get the total channel aggregate size in 32bit words
*/
uint32_t caen_xx725_30_dpp_chn_agg::getChnAggSize() const {
	return (*data) & 0x3FFFFF;
}

void caen_xx725_30_dpp_chn_agg::printFormat(chn_agg_format format) {
	cout << "CHANNEL AGGREGATE FORMAT: \n";
	cout << "n_samples = " << format.n_samples << '\n';
	cout << "dp1 = " << format.dp1 << '\n';
    cout << "dp2 = " << format.dp2 << '\n';
    cout << "ap = " << format.ap  << '\n';
    cout << "extras = " << format.extras << '\n';
    cout << "samples_en = " << format.samples_en << '\n';
    cout << "extras_en = " << format.extras_en << '\n';
    cout << "time_tag_en = " << format.time_tag_en << '\n';
    cout << "charge_en = " << format.charge_en << '\n';
    cout << "dual_trace_en = " << format.dual_trace_en  << endl;
}

void caen_xx725_30_dpp_chn_agg::checkValidEventNum(int eventNum) const {
	if(eventNum < 0 || eventNum >= getNumberOfEvents()) {
		throw llbad_caen_xx725_30_dpp_chn_agg("Event number out of range");
	}
}
