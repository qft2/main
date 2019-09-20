/*
 * =====================================================================================
 *
 *       Filename:  caen_xx725_30_event.h
 *
 *    Description:  event class for CAEN xx1725_30 standard firmware
 *
 *        Version:  1.0
 *        Created:  06/02/2017 05:30:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Max Emde (mme), emde@physik.rwth-aachen.de
 *   Organization:  3rd Phys. Inst. B
 *
 * =====================================================================================
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

#include "caen_xx725_30_ro_interface.h"

class caen_xx725_30_event_base{
	public:
		struct header{
			uint32_t eventSize	:28;
			uint32_t A		: 4;
			uint32_t channelMaskLow	: 8;
			uint32_t pattern	:16;
			uint32_t reserved	: 2;
			uint32_t boardFailed	: 1;
			uint32_t boardID	: 5;
			uint32_t eventCounter	:24;
			uint32_t channelMaskHigh: 8;
			uint32_t triggerTimeTag :32;
			uint32_t getMask() const{
				return channelMaskLow+(channelMaskHigh<<8);
			}
		};
	public:

		virtual uint32_t* getData() const =0;
		virtual uint32_t getSize() const =0;

		struct header* getHeader() const;
		uint8_t getHeaderLength() const;
		virtual ~caen_xx725_30_event_base(){}
		virtual void setData(uint32_t* Data, uint32_t Size) = 0;
		bool isValid() const;
		uint32_t getEventSize() const;
		uint8_t getBoardID() const;
		bool getBoardFail() const;
		uint16_t getPattern() const;
		uint16_t getChannelMask() const;
		uint32_t getEventCounter() const;
		uint32_t getTriggerTimeTag() const;
		uint8_t getNumberOfChannels() const;
		uint32_t* getChannelDataAddress(uint8_t Channel) const;
		uint32_t getChannelLength() const;
		uint32_t getWaveformLength() const;
		void copyChannelData(uint8_t Channel, uint16_t* Address) const;
		void print();
		virtual void poke();
};

class caen_xx725_30_event: public caen_xx725_30_ro_interface, public caen_xx725_30_event_base{
	private:
		uint32_t* data;
		uint32_t size; ///< size in 32bit words
		bool ownMemory;
	public:
		caen_xx725_30_event();
		caen_xx725_30_event(uint32_t* Data, uint32_t Size);
		virtual ~caen_xx725_30_event();
		uint32_t* getData() const {return data;}
		uint32_t getSize() const {return size;}
		virtual uint32_t* setDataSize(const uint32_t Size);
		void setData(uint32_t* Data, uint32_t Size);
		virtual void invalidate();
		virtual void poke();
};

template <uint32_t size>
class caen_xx725_30_static_event: public caen_xx725_30_ro_interface, public caen_xx725_30_event_base{
	private:
		uint32_t data[size];
	public:
		caen_xx725_30_static_event(){}
		virtual ~caen_xx725_30_static_event(){}
		uint32_t* getData() const {return data;}
		uint32_t getSize() const {return size;}
		virtual uint32_t* setDataSize(const uint32_t );
		virtual void invalidate();
		uint32_t getSize(){return size;}
		void setData(uint32_t* Data, uint32_t Size);
};

class caen_xx725_30_event_pointer: public caen_xx725_30_event_base{
	private:
		uint32_t* data;
	public:
		caen_xx725_30_event_pointer();
		caen_xx725_30_event_pointer(uint32_t* Data);
		uint32_t* getData() const;
		uint32_t getSize() const;
		void setData(uint32_t* Data, uint32_t);
		virtual void poke();
};

class caen_xx725_30_event_vector_base{
	private:
		uint32_t* data;
		uint32_t** beginnings;
		uint32_t dataSize;
		uint32_t begSize;
		uint32_t eventsRead;
		std::vector<caen_xx725_30_event_base*>* pVector;
	protected:
		virtual caen_xx725_30_event_base* constructEvent() = 0;
		virtual void allocate(uint32_t size=0);
	public:
		caen_xx725_30_event_vector_base(uint32_t DataSize, uint32_t BegSize);
		virtual ~caen_xx725_30_event_vector_base();
		const std::vector<caen_xx725_30_event_base*>& getVector();
		void updateVector();
		void clear();
		uint32_t* getData() const {return data;}
		uint32_t** getBeginnings() const {return beginnings;}
		const uint32_t& getDataSize() const {return dataSize;}
		const uint32_t& getBegSize() const {return begSize;}
		uint32_t& getEventsRead() {return eventsRead;}
};

class caen_xx725_30_event_vector: public caen_xx725_30_event_vector_base{
	public:
		caen_xx725_30_event_vector(uint32_t DataSize, uint32_t BegSize): caen_xx725_30_event_vector_base(DataSize,BegSize){
			allocate(BegSize);
		}
		virtual ~caen_xx725_30_event_vector(){}
	protected:
		virtual caen_xx725_30_event_base* constructEvent();
};

class caen_xx725_30_event_pointer_vector: public caen_xx725_30_event_vector_base{
	public:
		caen_xx725_30_event_pointer_vector(uint32_t DataSize, uint32_t BegSize): caen_xx725_30_event_vector_base(DataSize,BegSize){
			allocate(BegSize);
		}
		virtual ~caen_xx725_30_event_pointer_vector(){}
	protected:
		virtual caen_xx725_30_event_base* constructEvent();
};

template <uint32_t size>
class caen_xx725_30_static_event_vector: public caen_xx725_30_event_vector_base{
	public:
		caen_xx725_30_static_event_vector(uint32_t DataSize, uint32_t BegSize): caen_xx725_30_event_vector(DataSize,BegSize){
			allocate(BegSize);
		}

		virtual ~caen_xx725_30_static_event_vector(){}
	protected:
		virtual caen_xx725_30_event_base* constructEvent();
};


