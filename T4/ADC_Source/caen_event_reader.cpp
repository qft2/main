#include "caen_event_reader.h"
#include <unistd.h>
#include <iostream>
#include <algorithm>

using namespace std;

//#define DEBUG_XX725_30

size_t caen_event_reader::ReadEventBuffer(uint32_t* data, size_t size, vmeAMs AM){
	if(size==0)
		throw llbad_caen_event_reader("ReadEventBuffer(): size 0 not allowed!");
	if(size>0xA00000)
		throw llbad_caen_event_reader("ReadEventBuffer(): size bigger than max buffer size of board!");
	if(AM==AM_A32||AM==AM_A32_BLT||AM==AM_A32_MBLT){
		int ret;
		vme_claimer claim(getInterface(), AM);
		if(AM!=AM_A32_MBLT){
			if ( (ret=getInterface()->read32(getEventReadoutAddress(), data, size, false)) < 0 )
				throw llbad_caen_event_reader("ReadEventBuffer()::read32 error",ret);
		}else{
			if ( (ret=getInterface()->read_multiplexed_32(getEventReadoutAddress(), reinterpret_cast <uint64_t*>(data), (size)/2+(isBerrEnabled()?1:0)-1, false)) < 0 )
				throw llbad_caen_event_reader("ReadEventBuffer()::read_multiplexed_32 error",ret);
#ifdef DEBUG_XX725_30
			for(size_t i=0;i<10;++i) cout << hex << *(p_start-i)<<endl;
#endif
			ret*=2;
		}
#ifdef DEBUG_XX725_30
		cout << "ReadEventBuffer(): ret=" <<ret << endl;
		for(uint8_t i=0;i<5;++i) cout << hex << data[i] <<endl;
#endif
		return ret;
	}
	else throw llbad_caen_event_reader("ReadEventBuffer():: Address modifier not suitable for event transfer",0);
	return 0;
}


size_t  caen_event_reader::ReadEvent_D32(uint32_t* data, size_t size) {
	if(!size){
		size = getNextEventSize();
	}
	size_t ret = ReadEventBuffer(data,size,AM_A32);
#ifdef DEBUG_XX725_30
	cout << "ReadEvent_D32(): ret=" <<ret << endl;
#endif
	return ret;
}
void caen_event_reader::ReadEvent_D32(caen_xx725_30_ro_interface &event) {
	size_t size = getNextEventSize();
	uint32_t* buffer = event.setDataSize(size+isBerrEnabled());
	size_t ret = ReadEvent_D32(buffer,size);

	if(ret==0){
#ifdef DEBUG_XX725_30
		std::cout << "no event in FIFO (event read failed)" << std::endl;
#endif
		event.invalidate();        
		return;
	}

	if(size>ret){ //we have not read the complete event
		throw llbad_caen_event_reader_invalid_event("ReadEvent_D32()::Did not block read complete Event!");
	}

	if(((*buffer) >> 28) != 0xA) {
		event.invalidate();
		throw llbad_caen_event_reader_invalid_event("ReadEvent_D32(): Event does not start with 0xA. Invalid event");    
	}

#ifdef DEBUG_XX725_30  
	if(size < ret){ //we have read too much data
		cout << "read " << ret << " words instead of " << size << endl;
	}
#endif
}

size_t caen_event_reader::readFirstWord(uint32_t* data){
	*data=readReg(getEventReadoutAddress());
	if((*data)>>28!=0xA) return 0;
	else return (*data)&0xFFFFFFF;
}

size_t caen_event_reader::BlockTransfer_D32(uint32_t* data, size_t size) {
	if(isBerrEnabled() && getMaxEventsPerBLT()!=1){        
		setMaxEventsPerBLT(1);
	}

	//read one additional double word to trigger BERR and end event readout if activated
	if(!size){
		size = getNextEventSize();
	}
	size_t ret = ReadEventBuffer(data,size,AM_A32_BLT);
#ifdef DEBUG_XX725_30
	cout << "BlockTransfer_D32(): ret=" <<ret << endl;
#endif
	return ret;
}

void caen_event_reader::BlockTransfer_D32(caen_xx725_30_ro_interface &event) {
	size_t size = getNextEventSize();
	uint32_t* buffer = event.setDataSize(size+isBerrEnabled());
	size_t ret = BlockTransfer_D32(buffer,size);

	if(ret==0){
#ifdef DEBUG_XX725_30
		std::cout << "no event in FIFO (event read failed)" << std::endl;
#endif
		event.invalidate();        
		return;
	}

	if(size>ret){ //we have not read the complete event
		throw llbad_caen_event_reader_invalid_event("BlockTransfer_D32()::Did not block read complete Event!");
	}

	if(((*buffer) >> 28) != 0xA) {
		event.invalidate();
		throw llbad_caen_event_reader_invalid_event("BlockTransfer_D32(): Event does not start with 0xA. Invalid event");    
	}

#ifdef DEBUG_XX725_30  
	if(size < ret){ //we have read too much data
		cout << "read " << ret << " words instead of " << size <<endl;
	}
#endif
}

size_t caen_event_reader::BlockTransfer_D64(uint32_t* data, size_t size){
	if(isBerrEnabled() && (uint32_t)getMaxEventsPerBLT()!=1){        
		setMaxEventsPerBLT(1);
	}
	if(!size)
		size = getNextEventSize();
	size_t ret = ReadEventBuffer(data,size,AM_A32_MBLT);
	ret *= 2;
#ifdef DEBUG_XX725_30
	cout << "BlockTransfer_D64 ret=" <<ret << endl;
#endif
	return ret;
}

void caen_event_reader::BlockTransfer_D64(caen_xx725_30_ro_interface &event) {    
    size_t size = getNextEventSize();
    uint32_t* buffer = event.setDataSize(size+isBerrEnabled());
    size_t ret=BlockTransfer_D64(buffer,size);

    if(ret==0){
        #ifdef DEBUG_XX725_30
            std::cout << "no event in FIFO (event read failed)" << std::endl;
        #endif
        event.invalidate();        
        return;
    }
     
    if(size>ret){ //we have not read the complete event
        throw llbad_caen_event_reader_invalid_event("BlockTransfer_D64()::Did not block read complete Event!");
    }
    if(((*buffer) >> 28) != 0xA) {
        event.invalidate();
        throw llbad_caen_event_reader_invalid_event("BlockTransfer_D64(): Event does not start with 0xA. Invalid event");    
    }
    #ifdef DEBUG_XX725_30  
        if(size < ret){ //we have read too much data
            cout << "read " << ret << " words instead of " << size << endl;
        }
    #endif
}

/// search for event beginnings in a consecutive part of memory.
// One pointer in excess of the number of events is stored in the array of event beginnings to act as the "last" pointer of the array.
size_t caen_event_reader::searchBeginnings(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize, uint32_t words_read){
	uint32_t cur_evt=0;
	uint32_t* cur_ptr=data;
	std::fill(beginning,beginning+begsize,(uint32_t*)(0));
	while(cur_evt<=begsize){
		if((*cur_ptr)>>28==0xA){ // check for an A which marks the beginning of an event
			beginning[cur_evt]=cur_ptr;
#ifdef DEBUG_XX725_30 
			cout << "found beginning at " << std::distance(data,cur_ptr) <<endl;
#endif
			cur_ptr+=((*cur_ptr)&0xFFFFFFF);//lower 28 bits are the length of the event, so increase pointer by this amount
			if(cur_ptr>=data+min(datasize,words_read)+1){
		       	       break;
			}
			 ++cur_evt;
		}else{
			beginning[cur_evt]=cur_ptr;
			break;
		}
	}
#ifdef DEBUG_XX725_30 
	cout << "Total found beginnings: " <<cur_evt << endl;
	for(size_t i =0;i<10;++i) cout << *(beginning[cur_evt-1]+i)<<endl;
	cout << *(beginning[cur_evt])<<endl;
#endif
	return cur_evt; //return number of found event beginnings
}

size_t caen_event_reader::BlockTransfer_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize, bool MBLT){
	if(isBerrEnabled() && (uint32_t)getMaxEventsPerBLT()!=begsize){        
		setMaxEventsPerBLT(min(uint32_t( 1)<<0xA,begsize));
	}

	size_t ret = ReadEventBuffer(data,datasize,MBLT?AM_A32_MBLT:AM_A32_BLT);
#ifdef DEBUG_XX725_30
	cout << "BlockTransfer_many_events(): ret=" <<ret << endl;
#endif
	return searchBeginnings(data,beginning,datasize,begsize,ret);
}

size_t caen_event_reader::BlockTransfer_many_events(caen_xx725_30_event_vector_base* EventVector, bool MBLT){
#ifdef DEBUG_XX725_30
	cout << "datasize="<<EventVector->getDataSize()<<endl;
#endif
	EventVector->getEventsRead() = BlockTransfer_many_events(EventVector->getData(), EventVector->getBeginnings(), EventVector->getDataSize(), EventVector->getBegSize(), MBLT);
	EventVector->updateVector();
	return EventVector->getEventsRead();
}

size_t caen_event_reader::BlockTransfer_D32_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize){
	size_t ret = BlockTransfer_many_events(data,beginning,datasize,begsize,false);
#ifdef DEBUG_XX725_30
	cout << "BlockTransfer_D32_many_events(): ret=" <<ret << endl;
#endif
	return ret;
}

size_t caen_event_reader::BlockTransfer_D32_many_events(caen_xx725_30_event_vector_base* EventVector){
	return BlockTransfer_many_events(EventVector, false);
}

size_t caen_event_reader::BlockTransfer_D64_many_events(uint32_t* data, uint32_t** beginning, uint32_t datasize, uint32_t begsize){
	size_t ret = BlockTransfer_many_events(data,beginning,datasize,begsize,true);
#ifdef DEBUG_XX725_30
	cout << "BlockTransfer_D64_many_events(): ret=" <<ret << endl;
#endif
	return ret;
}

size_t caen_event_reader::BlockTransfer_D64_many_events(caen_xx725_30_event_vector_base* EventVector){
	return BlockTransfer_many_events(EventVector, true);
}
