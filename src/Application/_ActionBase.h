
#ifndef OpenKAI_src_Autopilot_ActionBase_H_
#define OpenKAI_src_Autopilot_ActionBase_H_

#include "../Base/common.h"
#include "../Base/_ThreadBase.h"
#include "Mission/_MissionControl.h"

#define N_ACTIVEMISSION 128

namespace kai
{

class _ActionBase: public _ThreadBase
{
public:
	_ActionBase();
	~_ActionBase();

	virtual bool init(void* pKiss);
	virtual void update(void);
	virtual bool draw(void);
	virtual bool bActive(void);
	virtual bool bMissionChanged(void);
	virtual int check(void);

public:
	_MissionControl* m_pMC;
	std::vector<int> m_vActiveMission;
	int m_iLastMission;
	bool m_bMissionChanged;

	int64_t m_tStamp;
	int64_t m_dTime;
	int		m_iPriority;

};

}
#endif
