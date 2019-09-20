/**
* \file caen_xx725_30_dpp_board_agg.h
* \author Christian Wysotzki
* \date Apr 2017
* \brief base implementation for boards aggregates/events of the 725/730 digitizer
*/

#pragma once
#include "caen_xx725_30_dpp_chn_agg.h"
#include "caen_xx725_30_dpp_event_iterator.h"
#include "caen_xx725_30_ro_interface.h"
#include <exceptions.h>
#include <memory>
#include <vector>


/**
 */
class llbad_caen_xx725_30_dpp_board_agg: public virtual llexception {
	public:
	llbad_caen_xx725_30_dpp_board_agg(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30_dpp_event:"+newtext,errnumber) {};
	virtual ~llbad_caen_xx725_30_dpp_board_agg() throw() {};
};

//forward declaration
class caen_xx725_30_dpp_event_iterator;

/**
* \class caen_xx725_30_dpp_board_agg
* \brief implementation of board aggregates (see DPP-PSD firmware documentation)
*/
class caen_xx725_30_dpp_board_agg : public caen_xx725_30_ro_interface {   
private:
    uint32_t* data;
    uint32_t dataSize;
    bool valid;
    ///stores the channel aggregates and deletes them on destruction
    std::vector<caen_xx725_30_dpp_chn_agg*> couples;

    
public:
    caen_xx725_30_dpp_board_agg();
    ~caen_xx725_30_dpp_board_agg();

    caen_xx725_30_dpp_chn_agg* getChnAgg(const int couple);

    ///allocates a buffer with the given size and returns the pointer to the first element
    uint32_t* setDataSize(const uint32_t size);

    void invalidate();

    uint32_t getBoardID() const;
    uint32_t getPattern() const;
    uint32_t getCoupleMask() const;
    uint32_t getAggregateCounter() const;
    ///aggregate time tag has no physical interpretation
    uint32_t getAggregateTimeTag() const;

    caen_xx725_30_dpp_event_iterator getEventIterator();

    /**
    * total size of board aggregate in 32bit words
    */
    uint32_t getSize() const;

    bool isValid() const;
     
    /**
    * true if the couple is included in this board aggregate
    */
    bool hasCouple(const int couple) const;

    //check functions throw exceptions
    void checkIsValidCouple(const int couple) const;
    void checkIsNotInvalidated() const;

    void printRawHeader();
};