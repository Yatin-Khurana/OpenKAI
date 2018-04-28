/*
 * _TCPclient.h
 *
 *  Created on: August 8, 2016
 *      Author: yankai
 */

#ifndef OpenKAI_src_IO__TCPsocket_H_
#define OpenKAI_src_IO__TCPsocket_H_

#include "../Base/common.h"
#include "_IOBase.h"
#include "../Protocol/Peer.h"

#define N_BUF_IO 128
#define TIMEOUT_RECV_USEC 1000

namespace kai
{

class _TCPclient: public _IOBase
{
public:
	_TCPclient();
	virtual ~_TCPclient();

	bool init(void* pKiss);
	bool link(void);
	bool start(void);
	bool open(void);
	void close(void);
	void reset(void);
	bool draw(void);

private:
	void updateW(void);
	static void* getUpdateThreadW(void* This)
	{
		((_TCPclient*) This)->updateW();
		return NULL;
	}

	void updateR(void);
	static void* getUpdateThreadR(void* This)
	{
		((_TCPclient*) This)->updateR();
		return NULL;
	}

public:
	struct sockaddr_in m_serverAddr;
	string	m_strAddr;
	uint16_t m_port;

	bool m_bClient;
	int m_socket;
	uint32_t m_timeoutRecv;
};

}
#endif