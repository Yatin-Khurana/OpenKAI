/*
 * _SortingArm.cpp
 *
 *  Created on: May 28, 2019
 *      Author: yankai
 */

#include "_SortingArm.h"

namespace kai
{

_SortingArm::_SortingArm()
{
	m_pDet1 = NULL;
	m_pDet2 = NULL;
	m_nClass = 0;

	m_pSeq = NULL;
	m_classFlag = 0;
	m_rGripX.init();
	m_rGripY.init();
	m_rGripZ.init();
	m_actuatorX = "";
	m_actuatorZ = "";

	m_vROI.init();
	m_vROI.z = 1.0;
	m_vROI.w = 1.0;

}

_SortingArm::~_SortingArm()
{
}

bool _SortingArm::init(void* pKiss)
{
	IF_F(!this->_ThreadBase::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	pK->v("actuatorX",&m_actuatorX);
	pK->v("actuatorZ",&m_actuatorZ);
	pK->v("vROI", &m_vROI);

	pK->v("gripX", &m_rGripX);
	pK->v("gripY", &m_rGripY);
	pK->v("gripZ", &m_rGripZ);
	pK->a("dropPos", m_pDropPos, SB_N_CLASS);

	string iName;
	int i;

	int pClass[SB_N_CLASS];
	m_nClass = pK->a("classList", pClass, SB_N_CLASS);
	for (i = 0; i < m_nClass; i++)
		m_classFlag |= (1 << pClass[i]);

	string pActuator[16];
	int nA = pK->a("actuatorList", pActuator, 16);
	for (i = 0; i < nA; i++)
	{
		iName = pActuator[i];
		_ActuatorBase* pA = (_ActuatorBase*) (pK->root()->getChildInst(iName));
		IF_Fl(!pA, iName + " not found");
		m_vAB.push_back(pA);
	}

	iName = "";
	F_ERROR_F(pK->v("_Sequencer", &iName));
	m_pSeq = (_Sequencer*) (pK->root()->getChildInst(iName));
	IF_Fl(!m_pSeq, iName + " not found");

	iName = "";
	F_ERROR_F(pK->v("_SortingCtrlServer", &iName));
	m_pDet1 = (_SortingCtrlServer*) (pK->root()->getChildInst(iName));
	IF_Fl(!m_pDet1, iName + " not found");

	iName = "";
	F_INFO(pK->v("_DNNclassifier", &iName));
	m_pDet2 = (_DetectorBase*) (pK->root()->getChildInst(iName));

	return true;
}

bool _SortingArm::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		m_bThreadON = false;
		return false;
	}

	return true;
}

void _SortingArm::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		updateArm();

		this->autoFPSto();
	}
}

int _SortingArm::check(void)
{
	NULL__(m_pDet1, -1);

	return 0;
}

void _SortingArm::updateArm(void)
{
	IF_(check() < 0);
	if(m_pDet1->m_iState == SORT_STATE_OFF)
	{
		m_pSeq->m_bON = false;

		for(int i=0; i<m_vAB.size(); i++)
		{
			_ActuatorBase* pA = m_vAB[i];
			pA->moveToOrigin();
		}

		return;
	}

	m_pSeq->m_bON = true;

	vFloat4 vP,vS;
	vP.init(-1.0);
	vS.init(-1.0);
	vS.x = 1.0;

	OBJECT* pO;
	string cAction = m_pSeq->getCurrentActionName();

	if (cAction == "standby")
	{
		int i = 0;
		while((pO=m_pDet1->at(i++)))
		{
			IF_CONT(!pO->m_bVerified);
			IF_CONT(pO->m_bb.midY() < m_rGripY.x);
			IF_CONT(pO->m_bb.midY() > m_rGripY.y);
			pO->m_bb.y += m_rGripY.y;
			pO->m_bb.w += m_rGripY.y;

			SEQ_ACTION* pAction;
			SEQ_ACTUATOR* pSA;

			pAction = m_pSeq->getAction("descent");
			IF_CONT(!pAction);

			pSA = pAction->getActuator(m_actuatorX);
			IF_CONT(!pSA);
//			vP.x = (1.0 - pO->m_bb.midX()) * m_rGripX.len() + m_rGripX.x;
			vP.x = (m_vROI.z - pO->m_bb.midX())/m_vROI.width() * m_rGripX.len() + m_rGripX.x;
			vP.x = constrain<float>(vP.x, 0.0, 1.0);
			pSA->setTarget(vP, vS);

			pSA = pAction->getActuator(m_actuatorZ);
			IF_CONT(!pSA);
			vP.x = (pO->m_dist - m_rGripZ.x) / m_rGripZ.len();
			pSA->setTarget(vP, vS);

			pAction = m_pSeq->getAction("move");
			IF_CONT(!pAction);

			pSA = pAction->getActuator(m_actuatorX);
			IF_CONT(!pSA);
			vP.x = m_pDropPos[pO->m_topClass];
			pSA->setTarget(vP, vS);

			m_pSeq->wakeUp();
			return;
		}
	}
}

bool _SortingArm::draw(void)
{
	IF_F(!this->_ThreadBase::draw());
	Window* pWin = (Window*) this->m_pWindow;
	Mat* pMat = pWin->getFrame()->m();

	return true;
}

bool _SortingArm::console(int& iY)
{
	IF_F(!this->_ThreadBase::console(iY));

	string msg;

	return true;
}

}
