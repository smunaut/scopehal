/***********************************************************************************************************************
*                                                                                                                      *
* ANTIKERNEL v0.1                                                                                                      *
*                                                                                                                      *
* Copyright (c) 2012-2020 Andrew D. Zonenberg                                                                          *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Implementation of SCPISocketTransport
 */

#include "scopehal.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

SCPISocketTransport::SCPISocketTransport(string hostname, unsigned short port)
	: m_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	, m_hostname(hostname)
	, m_port(port)
{
	LogDebug("Connecting to SCPI oscilloscope at %s:%d\n", hostname.c_str(), port);

	if(!m_socket.Connect(hostname, port))
	{
		LogError("Couldn't connect to socket\n");
		return;
	}
	if(!m_socket.DisableNagle())
	{
		LogError("Couldn't disable Nagle\n");
		return;
	}
}

SCPISocketTransport::~SCPISocketTransport()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actual transport code

bool SCPISocketTransport::SendCommand(string cmd)
{
	LogTrace("Sending %s\n", cmd.c_str());
	string tempbuf = cmd + "\n";
	return m_socket.SendLooped((unsigned char*)tempbuf.c_str(), tempbuf.length());
}

string SCPISocketTransport::ReadReply()
{
	//FIXME: there *has* to be a more efficient way to do this...
	char tmp = ' ';
	string ret;
	while(true)
	{
		if(!m_socket.RecvLooped((unsigned char*)&tmp, 1))
			break;
		if( (tmp == '\n') || (tmp == ';') )
			break;
		else
			ret += tmp;
	}
	LogTrace("Got %s\n", ret.c_str());
	return ret;
}

void SCPISocketTransport::SendRawData(size_t len, const unsigned char* buf)
{
	m_socket.SendLooped(buf, len);
}

void SCPISocketTransport::ReadRawData(size_t len, unsigned char* buf)
{
	m_socket.RecvLooped(buf, len);
}
