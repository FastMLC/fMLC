
#pragma once

#include <boost_plugin.h>
#include <double_vector.h>

//	a class containing the information needed for export to the display tools developped by Sonia
class ClusterProperties
{
public:
	ClusterProperties(void);
	explicit ClusterProperties(const ClusterProperties & srce);
	void Save(std::ofstream & stream, uint32_t p_LevelNo);

	uint32_t					m_Id;					//	the sequence ID
	vector<uint32_t>		m_Path;				//	the sequence ID path, numerical
	double_vector			m_Pos;				//	the coordinates, relative to the parent position
	vector<std::string>	m_Categories;		//	the taxonomic path, as a list of strings
	vector<uint32_t>		m_Properties;		//	three integers giving the dot color
};


