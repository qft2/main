#include "caen_xx725_30_dpp_board_agg.h"
#include <iostream>

using namespace std;

caen_xx725_30_dpp_board_agg::caen_xx725_30_dpp_board_agg()
: data(NULL), dataSize(0) {	
}
caen_xx725_30_dpp_board_agg::~caen_xx725_30_dpp_board_agg() {
	if(dataSize != 0) {
    	delete[] data;
    }
    for(int i = 0; i < couples.size(); i++) {
    	delete couples[i];
    }
}

uint32_t* caen_xx725_30_dpp_board_agg::setDataSize(const uint32_t size) {
	if(size <= 0) {
		throw llbad_caen_xx725_30_dpp_board_agg("setDataSize(): invalid size: size <= 0.");
	}
	if(dataSize == size) {
		return data;
	}else if(data != NULL) {
		delete[] data;
	}
    data = new uint32_t[size];
    dataSize = size;
    valid = true;
    return data;
}

uint32_t caen_xx725_30_dpp_board_agg::getSize() const {
	checkIsNotInvalidated();
	return *data & 0xFFFFFFF;
}

caen_xx725_30_dpp_chn_agg* caen_xx725_30_dpp_board_agg::getChnAgg(const int couple) {
	checkIsNotInvalidated();
	checkIsValidCouple(couple);

	for(int i = 0; i < couples.size(); i++) {
		if(couples[i]->getCouple() == couple) {
			return couples[i];
		}
	}

	int offset = 4;
	int currCouple = 0;
	while(currCouple < couple) {
		if(hasCouple(currCouple)) {
			caen_xx725_30_dpp_chn_agg tmp(data+offset, couple);
			offset += tmp.getChnAggSize();
			#ifdef DEBUG_XX725_30
				cout << "Previous aggregate size: " << tmp.getChnAggSize() << endl;
				cout << "Offset (32bit words): " << offset << endl;
			#endif
		}
		currCouple++;		
	}	
	caen_xx725_30_dpp_chn_agg * chn_agg = new caen_xx725_30_dpp_chn_agg(data+offset, couple);
	couples.push_back(chn_agg);
	return chn_agg;
}

uint32_t caen_xx725_30_dpp_board_agg::getBoardID() const {
	checkIsNotInvalidated();
	return (*(data+1)) >> 27;
}
uint32_t caen_xx725_30_dpp_board_agg::getPattern() const {
	checkIsNotInvalidated();
	return ((*(data+1)) >> 8) & 0x7FFF;
}
uint32_t caen_xx725_30_dpp_board_agg::getCoupleMask() const {
	checkIsNotInvalidated();
	return (*(data+1)) & 0xFF;
}
uint32_t caen_xx725_30_dpp_board_agg::getAggregateCounter() const {
	checkIsNotInvalidated();
	return (*(data+2)) & 0x7FFFFF;
}
uint32_t caen_xx725_30_dpp_board_agg::getAggregateTimeTag() const {
	checkIsNotInvalidated();
	return *(data+3);
}
caen_xx725_30_dpp_event_iterator caen_xx725_30_dpp_board_agg::getEventIterator() {
	checkIsNotInvalidated();
	return caen_xx725_30_dpp_event_iterator(this);
}

bool caen_xx725_30_dpp_board_agg::isValid() const {
	return valid;
}

void caen_xx725_30_dpp_board_agg::invalidate() {
	if(data != NULL) {
		delete[] data;
		dataSize = 0;
	}
	valid = false;
}

bool caen_xx725_30_dpp_board_agg::hasCouple(const int couple) const {
	checkIsNotInvalidated();
	return getCoupleMask() & (1 << couple); 
}

void caen_xx725_30_dpp_board_agg::checkIsValidCouple(const int couple) const {
	//does not check if it is an 8 channel or 16 channel device
	if(couple < 0 || couple > 7) {
		throw llbad_caen_xx725_30_dpp_board_agg("Invalid couple.");
	}
}

void caen_xx725_30_dpp_board_agg::printRawHeader() {
	checkIsNotInvalidated();
	if(dataSize>3) {
		cout << hex << 	data[0] << '\n' << data[1] << '\n' << data[2] << '\n' << data[3] << dec << endl;
	}
}

void caen_xx725_30_dpp_board_agg::checkIsNotInvalidated() const {
	if(!isValid()) {
		throw llbad_caen_xx725_30_dpp_board_agg("Aggregate has been invalidated.");
	}
}