#pragma once

/**
* \brief common interface for event / aggregates for the readout
*/
class caen_xx725_30_ro_interface {
public:

	virtual ~caen_xx725_30_ro_interface() {};
	/**
	* \param size in number of 32bit words which has to be written
	* \return a pointer to the allocated memory of the given size
	*/
	virtual uint32_t* setDataSize(const uint32_t size) = 0;

	/**
	* is called when the readout procedure fails
	*/
	virtual void invalidate() = 0;
};