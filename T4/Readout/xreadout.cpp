#include "vme/CAEN/XX725_30_ADC/caen_xx725_30.h"
#include "vme/CAEN/caencomm_interface.h"
#include "exceptions.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <sstream>


using namespace std;
uint16_t abs(uint16_t q){
	if(q<0){return -q;}
	return q;
}
int main(int argc, char* argv[]) {
    
	int usbDeviceNumber;

	if(argc > 1) {
		usbDeviceNumber = atoi(argv[1]);
	} else {
		usbDeviceNumber = 0;
		printf("%s\n", "Using usbDeviceNumber = 0");
	}

	//open interface and add device
	caencomm_interface* interface = new caencomm_interface();

	vme_addr_t addr = 0x00010000;

	try {
		interface->add_device(CAENComm_USB, addr, usbDeviceNumber, 0);
	} catch(exception &e) {
		printf("Exception while adding device to interface:\n");
		printf("%s\n", e.what());
		exit(1);
	}

	caen_xx725_30* device = new caen_xx725_30(caen_xx725_30::CHN8, interface, addr);

	device->softwareClear();
	device->softwareReset();
	usleep(1e6);

	device->calibrate();
	usleep(1e6);
	
	device->setMaxEventsPerBLT(1);
	
	// Board config
	{
		printf("device->getBoardConfig() = %x\n", device->getBoardConfig().raw);
	}
	
	// Event size
	{
		device->setBufferOrganization(0x8);
		printf("device->getBufferOrganization() = %x\n", device->getBufferOrganization());
	}
	
	// enable channel 0
	{
		uint16_t mask = 0x1;
		device->setChannelEnableMask(mask);
		printf("device->getChannelEnableMask() = %x\n", device->getChannelEnableMask());
	}

	// check that external trigger is enabled
	{
		caen_xx725_30::global_trigger_mask gltrmk;
		gltrmk = device->getGlobalTriggerMask();

		if (!(gltrmk.raw & 0x40000000)) {
			gltrmk.raw &= ~0x80000000;
			gltrmk.raw |= 0x40000000;
			device->setGlobalTriggerMask(gltrmk);
		}
		
		printf("device->getGlobalTriggerMask() = %x\n", device->getGlobalTriggerMask().raw);
	}
	
	// set front panel LEMO to NIM
	{
        	caen_xx725_30::front_panel_io_control frpaio;
        	frpaio = device->getFrontPanelIOControl();
            frpaio.el_level = 0; //0 = NIM IO level, 1 = TTL IO level
        	device->setFrontPanelIOControl(frpaio);
        	
		
		printf("device->getFrontPanelIOControl() = %x\n", device->getFrontPanelIOControl().raw);
        }
        
        // set position of trigger
        {
		device->setPostTrigger(0);
		
		printf("device->getPostTrigger() = %d\n", device->getPostTrigger());
	}
		printf("starting measurement\n");
		cout<<mi<<endl;
		
	// start measurement
	try {
		device->start();
	} catch(llexception &e) {
		printf("Exception while starting device:\n");
		printf("%s\n", e.what());
	}

	usleep(1e4);

	

	
	
	/*s s << mi;
	string str = ss.str();
	cout<<(str.c_str())<<endl;
	ofstream io("test2/file"+str+".txt");*/

	const uint16_t uplim=10000;		
	printf("waiting for event\n");

	// wait for next event (trigger)
	while(!device->isEventReady()) {
		usleep(1e3);
	}

	
	caen_xx725_30_event event[uplim];
	for(int i=0;i<uplim;i++){
		device->BlockTransfer_D32(event[i]);

		cout<<(i)<<endl;
		/*printf("event.getEventSize() = %d\n", event.getEventSize());
		printf("event.getChannelLength() = %d\n", event.getChannelLength());
		printf("event.getChannelMask() = %d\n", event.getChannelMask());
		printf("event.getEventCounter() = %d\n", event.getEventCounter());
		printf("event.getTriggerTimeTag() = %d\n", event.getTriggerTimeTag());
		printf("event.getNumberOfChannels() = %d\n", event.getNumberOfChannels());
		printf("event.getWaveformLength() = %d\n", event.getWaveformLength());*/



		while(!device->isEventReady()) {
			usleep(1);
		}
	}
	for(int i=0;i<uplim;i++){
		stringstream ss;
		ss << i;
		string str = ss.str();
		cout<<(str.c_str())<<endl;
		ofstream io("test6/file"+str+".txt");
		for (uint j = 0; j < event[i].getChannelLength(); j++) {
					stringstream ss;

			uint16_t ac=(uint16_t) (event[i].getChannelDataAddress(0)[j] & 0x3FFF);
// 			std::cout<<ac<<std::endl;
			//read first sample in word
			io << ac << endl;
		}
		io.close();
	}


// 		io.close();

	device->stop();
	delete device;
	delete interface;

	return 0;
}

