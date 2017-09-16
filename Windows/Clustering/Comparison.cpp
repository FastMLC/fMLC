
#pragma once

#include "stdafx.h"
#include "boost_plugin.h"

#include "Comparison.h"

//	unused, but necessary for the std::map
TComparison::TComparison()
	: m_SrceIdx(0), m_RefIdx(0), m_Similarity(0.0)
{
}


TComparison::TComparison(uint32_t p_SrceIdx, uint32_t p_RefIdx, double p_Similarity)
	: m_SrceIdx(p_SrceIdx), m_RefIdx(p_RefIdx), m_Similarity(p_Similarity)
{
	m_Pos = {2.0, 2.0, 2.0};	//	just to recognize it
}

TComparison::~TComparison(void)
{
}

bool
TComparison::operator<(const TComparison & srce) const {
	if(m_Similarity == srce.m_Similarity) {
		return m_SrceIdx < srce.m_SrceIdx;
	}
	return (m_Similarity < srce.m_Similarity);
}

bool
TComparison::operator<=(const TComparison & srce) const {
	if (m_Similarity == srce.m_Similarity) {
		return m_SrceIdx <= srce.m_SrceIdx;
	}
	return (m_Similarity <= srce.m_Similarity);
}

bool
TComparison::operator>(const TComparison & srce) const {
	if (m_Similarity == srce.m_Similarity) {
		return m_SrceIdx > srce.m_SrceIdx;
	}
	return (m_Similarity > srce.m_Similarity);
}

bool
TComparison::operator>=(const TComparison & srce) const {
	if (m_Similarity == srce.m_Similarity) {
		return m_SrceIdx >= srce.m_SrceIdx;
	}
	return (m_Similarity >= srce.m_Similarity);
}

bool
TComparison::operator==(const TComparison & srce) const {
	return (m_Similarity == srce.m_Similarity);
}

bool
TComparison::operator!=(const TComparison & srce) const {
	return (m_Similarity != srce.m_Similarity);
}


//	note that the similarity may be slightly wrong, as Compare(srce, ref) may slightly differ from Compare(ref, srce)
void
TComparison::SwapSrceAndRef()
{
	uint32_t x = m_SrceIdx;
	m_SrceIdx = m_RefIdx;
	m_RefIdx = x;
}
