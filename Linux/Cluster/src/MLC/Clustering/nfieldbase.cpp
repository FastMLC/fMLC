
//#include "stdafx.h"

#include "nfieldbase.h"

#include "toolbox_tl.h"

//	***************************************	TNFieldBase	***************************************

TNFieldBase::TNFieldBase(int32_t p_RecordId, const std::string & p_RecordName,  NFieldSubTypeEnum p_SubType)
:	m_RecordId(p_RecordId), 
	m_RecordName(p_RecordName),
	m_SubType(p_SubType), 
	m_bSrceReady(false),
	m_bRefReady(false)
{
}



TNFieldBase::TNFieldBase(const TNFieldBase & p_Srce)
:	m_RecordId(p_Srce.m_RecordId),
	m_RecordName(p_Srce.m_RecordName),
	m_SubType(p_Srce.m_SubType),
	m_bSrceReady(p_Srce.m_bSrceReady), 
	m_bRefReady(p_Srce.m_bRefReady)
{
}

TNFieldBase::TNFieldBase()
{
}

TNFieldBase::~TNFieldBase()
{
}

void 
TNFieldBase::SetRecordName(std::string & p_RecordName) {
	m_RecordName = p_RecordName;
}
void 
TNFieldBase::SetRecordId(uint32_t p_RecordId) {
	m_RecordId = p_RecordId;
}

