#include "caen_xx725_30_dpp_event_iterator.h"
#include <iostream>

using namespace std;

/**
* points to first valid event after construction
*/
caen_xx725_30_dpp_event_iterator::caen_xx725_30_dpp_event_iterator(caen_xx725_30_dpp_board_agg *agg) {
	aggregate = agg;
	behindLast = false;

	couple = 0;
	event = -1;
	//initialise to first valid event
	findNextEvent();
}

void caen_xx725_30_dpp_event_iterator::findNextEvent() {
	#ifdef DEBUG_XX725_30
		cout << "findNextEvent(): " << endl;
	#endif
	uint32_t mask = aggregate->getCoupleMask();

	bool found = false;	
	mask >>= couple;

	while(!found) {

		//find participating couple
		while(couple < 8 && !(mask & 1)) {
			mask >>= 1;
			couple++;
		}

		if(mask & 1) {
			curr = aggregate->getChnAgg(couple);
			if(curr->getNumberOfEvents() > 0) {
				event = 0;				
				found = true;
				#ifdef DEBUG_XX725_30
					cout << "Found next aggregate with " << curr->getNumberOfEvents() << " events" << endl;
				#endif

			} else {
				//should normally not be the case, that there are no events in a couple (just for forced flushed data)
				couple++;
				mask >>= 1;
			}

		} else {
			found = true;
			behindLast = true;
		}
	}
	#ifdef DEBUG_XX725_30
		cout << "end of findNextEvent()" << endl;
	#endif

}

caen_xx725_30_dpp_event_iterator caen_xx725_30_dpp_event_iterator::operator++(int) {
	if(isBehindLast()) {
		throw llbad_caen_xx725_30_dpp_event_iterator("Iterator is behind last element.");
	}

	if( (event+1) < curr->getNumberOfEvents()) {
		event++;
		return *this;
	}
	couple++;

	//find next valid channel aggregate
	findNextEvent();
	return *this;
}

caen_xx725_30_dpp_event caen_xx725_30_dpp_event_iterator::operator*() const {
	if(isBehindLast()) {
		throw llbad_caen_xx725_30_dpp_event_iterator("Iterator is behind last element.");
	}
	return curr->getEvent(event);
}

bool caen_xx725_30_dpp_event_iterator::isBehindLast() const {
	return behindLast;
}