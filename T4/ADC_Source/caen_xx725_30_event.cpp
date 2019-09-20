/*
 * =====================================================================================
 *
 *       Filename:  caen_xx725_30_event.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/02/2017 05:42:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Max Emde (mme), emde@physik.rwth-aachen.de
 *   Organization:  3rd Phys. Inst. B
 *
 * =====================================================================================
 */

#include <bitset>
#include <iostream>
#include <cstdio>
#include <algorithm>

#include "caen_xx725_30_event.h"

using namespace std;

caen_xx725_30_event::caen_xx725_30_event(): data(0), size(0), ownMemory(true){}

caen_xx725_30_event::caen_xx725_30_event(uint32_t* Data,uint32_t Size): data(Data), size(Size), ownMemory(false){}

caen_xx725_30_event::~caen_xx725_30_event(){
	if(data&&ownMemory) delete[] data;
}

void caen_xx725_30_event::setData(uint32_t* Data, uint32_t Size){
	if(size==0&&data==0){
		data = new uint32_t[Size];
		ownMemory=true;
	}else if(size!=Size&&data!=0){
		delete[] data;
		data = new uint32_t[Size];
		ownMemory=true;
	}
	size = Size;
	std::copy(Data, Data+size, data);
	return;
}
	

uint32_t* caen_xx725_30_event::setDataSize(const uint32_t Size){
	if(ownMemory){
		if(size==Size&&data){
			//std::fill(data,data+size,(uint32_t)(0));
			return data;
		}else if(data){
			delete[] data;
			data=0;
		}
		size=Size;
		data=new uint32_t[size];
		return data;
	}else{
		if(size>=Size) return data;
		else return 0;
	}
}

void caen_xx725_30_event::invalidate(){
	if(data) data[0]&=0xFFFFFFF; //set first 4 bits to 0
}

void caen_xx725_30_event_base::print(){
	printf("event memory address: %p, length: %x\n",getData(),getSize());
	for(size_t i=0;i<getSize()&&i<4;++i)
		printf("%x\n",getData()[i]);
	header testhead;
	testhead.A=0xA;
	testhead.eventSize=uint16_t(0xbeef);
	uint32_t* testptr = reinterpret_cast<uint32_t*>(&testhead);
	cout << "testheader: " << testptr[0] <<endl;
	return;
}

/////////////////////////////////
//static event functions


template<uint32_t size> void caen_xx725_30_static_event<size>::setData(uint32_t* Data, uint32_t Size){
	std::copy(Data, Data+min(size,Size), data);
	return;
}
	
template<uint32_t size> uint32_t* caen_xx725_30_static_event<size>::setDataSize(const uint32_t){
	return data;
}
template<uint32_t size> void caen_xx725_30_static_event<size>::invalidate(){
	data[0]&0xFFFFFFF; //set first 4 bits to 0
}


/////////////////////////////////
//event base functions


struct caen_xx725_30_event_base::header* caen_xx725_30_event_base::getHeader() const{
	return reinterpret_cast<header*> (getData());
}

uint8_t caen_xx725_30_event_base::getHeaderLength() const{
	return sizeof(header)/sizeof(uint32_t);
}
		
bool caen_xx725_30_event_base::isValid() const{
	return getHeader()->A==0xA;
}

uint32_t caen_xx725_30_event_base::getEventSize() const{
	return getHeader()->eventSize;
}

uint8_t caen_xx725_30_event_base::getBoardID() const{
	return getHeader()->boardID;
}

bool caen_xx725_30_event_base::getBoardFail() const{
	return getHeader()->boardFailed;
}

uint16_t caen_xx725_30_event_base::getPattern() const{
	return getHeader()->pattern;
}

uint16_t caen_xx725_30_event_base::getChannelMask() const{
	return getHeader()->getMask();
}

uint32_t caen_xx725_30_event_base::getEventCounter() const{
	return getHeader()->eventCounter;
}

uint32_t caen_xx725_30_event_base::getTriggerTimeTag() const{
	return getHeader()->triggerTimeTag;
}

uint8_t caen_xx725_30_event_base::getNumberOfChannels() const{
	return (std::bitset<16>(getHeader()->getMask())).count();
}

uint32_t* caen_xx725_30_event_base::getChannelDataAddress(uint8_t Channel) const{
	std::bitset<16> mask = getHeader()->getMask();
	uint8_t offset=0;
	for(uint8_t i=0;i<Channel;++i)
		if(mask[i]) ++offset;
	return getData() + getHeaderLength() + offset*getChannelLength();
}

uint32_t caen_xx725_30_event_base::getWaveformLength() const{
	return getChannelLength()*2;	//2 samples per 32 bit word
}

uint32_t caen_xx725_30_event_base::getChannelLength() const{
	return (getEventSize()-getHeaderLength())/getNumberOfChannels();
}

void caen_xx725_30_event_base::copyChannelData(uint8_t Channel, uint16_t* Address) const{
	uint16_t* start = reinterpret_cast<uint16_t*>(getChannelDataAddress(Channel));
	uint16_t* end = start + getWaveformLength();
	std::copy(start,end,Address);
}

void caen_xx725_30_event_base::poke(){
	cout << "base event alive!"<<endl;
}

caen_xx725_30_event_pointer::caen_xx725_30_event_pointer(): data(0){}

caen_xx725_30_event_pointer::caen_xx725_30_event_pointer(uint32_t* Data): data(Data){}

uint32_t* caen_xx725_30_event_pointer::getData() const{
	return data;
}

void caen_xx725_30_event_pointer::setData(uint32_t* Data,uint32_t) {
	data = Data;
	return;
}


uint32_t caen_xx725_30_event_pointer::getSize() const {
	if(data) return getEventSize();
	else return 0;
}


void blah(){
	return;
}

caen_xx725_30_event_vector_base::caen_xx725_30_event_vector_base(uint32_t DataSize, uint32_t BegSize):
	data(new uint32_t[DataSize]), beginnings(new uint32_t*[BegSize+1]), dataSize(DataSize), begSize(BegSize), pVector(new vector<caen_xx725_30_event_base*>(BegSize,0))
{
	std::fill(beginnings,beginnings+begSize+1,(uint32_t*)(0));
	std::fill(data,data+dataSize,(uint32_t)(0));
}

void caen_xx725_30_event_vector_base::allocate(uint32_t size){
	for(vector<caen_xx725_30_event_base*>::iterator it=pVector->begin();it!=pVector->end();++it){
		*it=constructEvent();
	}
	return;
}

void caen_xx725_30_event_vector_base::updateVector(){
	for(uint32_t i=0;i<eventsRead;++i){
		pVector->at(i)->setData(beginnings[i],std::distance(beginnings[i],beginnings[i+1]));
	}
	return;
}

const std::vector<caen_xx725_30_event_base*>& caen_xx725_30_event_vector_base::getVector(){
	return *pVector;
}

caen_xx725_30_event_vector_base::~caen_xx725_30_event_vector_base(){
	for(vector<caen_xx725_30_event_base*>::iterator it = pVector->begin();it!=pVector->end();++it)
		delete *it;
	delete pVector;
}

caen_xx725_30_event_base* caen_xx725_30_event_vector::constructEvent(){
	return new caen_xx725_30_event;
}

caen_xx725_30_event_base* caen_xx725_30_event_pointer_vector::constructEvent(){
	return new caen_xx725_30_event_pointer;
}


template <uint32_t size>
caen_xx725_30_event_base* caen_xx725_30_static_event_vector<size>::constructEvent(){
	return new caen_xx725_30_static_event<size>;
}

void caen_xx725_30_event::poke(){
	cout << "event alive!"<<endl;
}

void caen_xx725_30_event_pointer::poke(){
	cout << "event pointer  alive!"<<endl;
}
