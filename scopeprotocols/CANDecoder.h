/**
	@file
	@author Andrés MANELLI
	@brief Declaration of CANDecoder
 */

#ifndef CANDecoder_h
#define CANDecoder_h

#include "../scopehal/ProtocolDecoder.h"

class CANSymbol
{
public:
	enum stype
	{
		TYPE_SOF,
		TYPE_SID,
		TYPE_RTR,
		TYPE_IDE,
		TYPE_R0,
		TYPE_DLC,
		TYPE_DATA,
		TYPE_CRC,
		TYPE_IDLE
	};

	CANSymbol(stype t, uint8_t *data, size_t size)
	 : m_stype(t)
	{
		for (uint8_t i = 0; i < size; i++)
		{
			m_data.push_back(data[i]);
		}
	}

	stype m_stype;
	std::vector<uint8_t> m_data;

	bool operator== (const CANSymbol& s) const
	{
		return (m_stype == s.m_stype) && (m_data == s.m_data);
	}
};

typedef OscilloscopeSample<CANSymbol> CANSample;
typedef CaptureChannel<CANSymbol> CANCapture;

class CANDecoder : public ProtocolDecoder
{
public:
	CANDecoder(std::string color);

	virtual void Refresh();
	virtual ChannelRenderer* CreateRenderer();
	virtual bool NeedsConfig();

	static std::string GetProtocolName();
	virtual void SetDefaultName();

	virtual bool ValidateChannel(size_t i, OscilloscopeChannel* channel);

	PROTOCOL_DECODER_INITPROC(CANDecoder)

protected:
	std::string m_tq;
	std::string m_bs1, m_bs2;

	// Nominal Bit Time, in ns
	unsigned int m_nbt;
};

#endif
