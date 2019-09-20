#pragma once
#include "caen_xx725_30_dpp_board_agg.h"
#include "exceptions.h"

//forward declaration
class caen_xx725_30_dpp_board_agg;

class llbad_caen_xx725_30_dpp_event_iterator: public virtual llexception {
    public:
    llbad_caen_xx725_30_dpp_event_iterator(std::string newtext="",int errnumber=0) :
        llexception("caen_xx725_30_dpp_event_iterator:"+newtext,errnumber) {};
    virtual ~llbad_caen_xx725_30_dpp_event_iterator() throw() {};
};

/**
* \class caen_xx725_30_dpp_event_iterator
* \brief iterates over all events in one board aggregate
*/
class caen_xx725_30_dpp_event_iterator {
private:
    caen_xx725_30_dpp_board_agg *aggregate;
    caen_xx725_30_dpp_chn_agg *curr;
    int couple;
    int event;
    bool behindLast;
public:
    caen_xx725_30_dpp_event_iterator(caen_xx725_30_dpp_board_agg *agg);

    caen_xx725_30_dpp_event_iterator operator++(int);

    ///access to event
    caen_xx725_30_dpp_event operator*() const;

    ///if the increment operator is called while pointing to the last increment, behindLast is true
    bool isBehindLast() const;
private:
    void findNextEvent();
};
