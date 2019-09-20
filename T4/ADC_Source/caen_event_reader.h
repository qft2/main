/**
* \file caen_event_reader.h
*/
#pragma once

#include "vme/vme_interface.h"
#include "caen_xx725_30_ro_interface.h"
#include "caen_xx725_30_event.h"
/**
 * \class llbad_caen_event_reader
 * \brief standard exception class
 */
class llbad_caen_event_reader: public virtual llexception {
	public:
	llbad_caen_event_reader(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_event_reader() throw() {};
};

class llbad_caen_event_reader_invalid_event: public virtual llexception {
	public:
	llbad_caen_event_reader_invalid_event(std::string newtext="",int errnumber=0) :
		llexception("caen_xx725_30:"+newtext,errnumber) {};
	virtual ~llbad_caen_event_reader_invalid_event() throw() {};
};

/**
* \class caen_event_reader
* \brief template pattern, implements read functions (like Read_Event_D32, etc.) to be used in subclasses
*/
class caen_event_reader {
	public:
		enum vmeAMs {
			AM_A24			=0x39,
			AM_A32			=0x09,
			AM_A32_BLT		=0x0B,
			AM_A32_MBLT		=0x08
		};	

		/***********************************************************/
		/*readout functions */     
		size_t ReadEventBuffer(uint32_t* data, size_t size, vmeAMs AM);

		size_t BlockTransfer_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize, bool MBLT);
		size_t BlockTransfer_many_events(caen_xx725_30_event_vector_base* EventVector, bool MBLT);

		void ReadEvent_D32(caen_xx725_30_ro_interface &event);
		size_t ReadEvent_D32(uint32_t* data, size_t size=0);

		void BlockTransfer_D32(caen_xx725_30_ro_interface &event);
		size_t BlockTransfer_D32(uint32_t* data, size_t size=0);

		void BlockTransfer_D64(caen_xx725_30_ro_interface &event);
		size_t BlockTransfer_D64(uint32_t* data, size_t size=0);

		size_t BlockTransfer_D32_many_events(caen_xx725_30_event_vector_base* EventVector);
		size_t BlockTransfer_D32_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize);
		size_t BlockTransfer_D64_many_events(caen_xx725_30_event_vector_base* EventVector);
		size_t BlockTransfer_D64_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize);

		/**********************************************************/
		/* functions which have to be implented in the sublcasses */
		virtual void start() = 0;
		virtual void stop() = 0;

		virtual bool isEventReady() = 0;
		virtual uint32_t getNextEventSize() = 0;
		virtual vme_interface* getInterface() = 0;

		///readout address (should include base address)
		virtual vme_addr_t getEventReadoutAddress() = 0;

		///buffer error in vme
		virtual bool isBerrEnabled() = 0;

		///maximum number of events per block transfer
		virtual int getMaxEventsPerBLT() = 0;
		virtual void setMaxEventsPerBLT(int number) = 0;
		virtual uint32_t readReg(uint16_t addr)=0;

	private:
		size_t searchBeginnings( uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize, uint32_t words_read);
		size_t readFirstWord(uint32_t* data);

};
