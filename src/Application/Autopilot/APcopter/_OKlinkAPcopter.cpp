#include "_OKlinkAPcopter.h"

namespace kai
{
_OKlinkAPcopter::_OKlinkAPcopter()
{
	m_vPos.init();
	m_tPos = 0;
}

_OKlinkAPcopter::~_OKlinkAPcopter()
{
}

bool _OKlinkAPcopter::init(void* pKiss)
{
	IF_F(!this->_OKlink::init(pKiss));
	Kiss* pK = (Kiss*)pKiss;

	return true;
}

bool _OKlinkAPcopter::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		LOG(ERROR) << "Return code: "<< retCode;
		m_bThreadON = false;
		return false;
	}

	return true;
}

void _OKlinkAPcopter::update(void)
{
	while (m_bThreadON)
	{
		if(!m_pIO)
		{
			this->sleepTime(USEC_1SEC);
			continue;
		}

		if(!m_pIO->isOpen())
		{
			this->sleepTime(USEC_1SEC);
			continue;
		}

		this->autoFPSfrom();

		while(readCMD())
		{
			handleCMD();
			m_nCMDrecv++;
		}

		this->autoFPSto();
	}
}

void _OKlinkAPcopter::handleCMD(void)
{
	int16_t x,y,z;
	switch (m_recvMsg.m_pBuf[1])
	{
	case OKLINK_POS:
		x = unpack_int16(&m_recvMsg.m_pBuf[3], false);
		y = unpack_int16(&m_recvMsg.m_pBuf[5], false);
		z = unpack_int16(&m_recvMsg.m_pBuf[7], false);
		m_vPos.x = ((float)x)*0.001;
		m_vPos.y = ((float)y)*0.001;
		m_vPos.z = ((float)z)*0.001;
		m_tPos = m_tStamp;
		LOG_I("Pos=("+f2str(m_vPos.x)+","+f2str(m_vPos.y)+","+f2str(m_vPos.z)+")");
		break;
	default:
		break;
	}

	m_recvMsg.reset();
}

void _OKlinkAPcopter::setPos(vFloat3 vP)
{
	NULL_(m_pIO);
	IF_(!m_pIO->isOpen());

	m_pBuf[0] = OKLINK_BEGIN;
	m_pBuf[1] = OKLINK_POS;
	m_pBuf[2] = 6;

	pack_uint16(&m_pBuf[3], (int16_t)(vP.x * 1000), false);
	pack_uint16(&m_pBuf[5], (int16_t)(vP.y * 1000), false);
	pack_int16(&m_pBuf[7], (int16_t)(vP.z * 1000), false);

	m_pIO->write(m_pBuf, OKLINK_N_HEADER + 6);
}

bool _OKlinkAPcopter::draw(void)
{
	IF_F(!this->_OKlink::draw());
	Window* pWin = (Window*) this->m_pWindow;

	string msg;
	pWin->tabNext();
	pWin->tabPrev();

	return true;
}

bool _OKlinkAPcopter::console(int& iY)
{
	IF_F(!this->_OKlink::console(iY));
	string msg;

	return true;
}

}
