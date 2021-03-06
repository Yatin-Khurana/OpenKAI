#ifndef OpenKAI_src_Autopilot_Rover_Rover_base_H_
#define OpenKAI_src_Autopilot_Rover_Rover_base_H_

#include "../../../Base/common.h"
#include "../../../Protocol/_Mavlink.h"
#include "../../../Control/PIDctrl.h"
#include "../../_ActionBase.h"
#include "_RoverCMD.h"

#define ROVER_N_MOTOR 16

namespace kai
{

struct ROVER_PWM_CHANNEL
{
	uint8_t	 m_iChan;
	uint16_t m_pwm;
	uint16_t m_H;
	uint16_t m_M;
	uint16_t m_L;
	float	 m_kDirection;	//+/-1.0
	float	 m_sDirection;	//+/-1.0

	void init(void)
	{
		m_iChan = 0;
		m_M = 1500;
		m_H = 2000;
		m_L = 1000;
		m_pwm = m_M;
		m_kDirection = 1.0;
		m_sDirection = 1.0;
	}

	uint16_t updatePWM(float nSpeed, float dSpeed)
	{
		float k = nSpeed * m_kDirection + dSpeed * m_sDirection;
		k = constrain<float>(k, -1.0, 1.0);

		if(k >= 0.0)
			m_pwm = m_M + ((float)(m_H - m_M)) * abs(k);
		else
			m_pwm = m_M - ((float)(m_M - m_L)) * abs(k);

		return m_pwm;
	}
};

class _Rover_base: public _ActionBase
{
public:
	_Rover_base();
	~_Rover_base();

	bool init(void* pKiss);
	int	 check(void);
	void update(void);
	bool draw(void);
	bool console(int& iY);

	void setSpeed(float nSpeed);
	void setTargetHdg(float hdg);
	void setTargetHdgDelta(float dHdg);
	void setPinout(uint8_t pin, uint8_t status);

private:
	void updatePWM(void);

public:
	_RoverCMD* m_pCMD;
	_Mavlink* m_pMavlink;
	PIDctrl* m_pPIDhdg;

	float m_hdg;
	float m_targetHdg;
	float m_nSpeed;		//-1.0 to 1.0
	float m_maxSpeed;	//in m/s
	vector<ROVER_PWM_CHANNEL> m_vPWM;
};

}
#endif
