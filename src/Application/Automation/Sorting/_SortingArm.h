/*
 * _SortingArm.h
 *
 *  Created on: May 28, 2019
 *      Author: yankai
 */

#ifndef OpenKAI_src_Automation__SortingArm_H_
#define OpenKAI_src_Automation__SortingArm_H_

#include "../../../Base/common.h"
#include "../../../Detector/_DetectorBase.h"
#include "_Sequencer.h"
#include "_SortingCtrlServer.h"

#define SB_N_CLASS 16

namespace kai
{

class _SortingArm: public _ThreadBase
{
public:
	_SortingArm(void);
	virtual ~_SortingArm();

	bool init(void* pKiss);
	bool start(void);
	bool draw(void);
	bool console(int& iY);
	int check(void);

private:
	void updateState(void);
	void updateArm(void);
	void update(void);
	static void* getUpdateThread(void* This)
	{
		((_SortingArm *) This)->update();
		return NULL;
	}

public:
	_SortingCtrlServer*	m_pDet1;
	_DetectorBase*	m_pDet2;
	_Sequencer*		m_pSeq;
	vector<_ActuatorBase*> m_vAB;

	int			m_nClass;
	float		m_pDropPos[SB_N_CLASS];
	uint64_t	m_classFlag;
	vFloat2		m_rGripX;	//grip region X, target bb midX will be re-mapped into this region
	vFloat2		m_rGripY;	//grip region Y
	vFloat2		m_rGripZ;	//vertical

	string		m_actuatorX;
	string		m_actuatorZ;

	vFloat4		m_vROI;
};

}
#endif
