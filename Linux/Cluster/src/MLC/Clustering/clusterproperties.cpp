
//#include "stdafx.h"

#include "clusterproperties.h"
#include <fstream>


ClusterProperties::ClusterProperties(void): m_Id(0)
{
	m_Pos.resize(3, 0.0);
	//	colors
	m_Properties.push_back(2);
	m_Properties.push_back(3);
	m_Properties.push_back(4);
}

ClusterProperties::ClusterProperties(const ClusterProperties & srce)
	: m_Id(srce.m_Id), m_Path(srce.m_Path), m_Pos(srce.m_Pos), m_Categories(srce.m_Categories), m_Properties(srce.m_Properties)
{
}


/* Typical content : (remove all CR, LF and TABS)
	"1050": {
		"Path": [ "0", "1050", "1050", "1050" ],
		"Coordinates": [ 0.66929479446010731, 0.39593615482773775, 1.0711315426202468 ],
		"Categories": [ "Saprochaete ingens", "Saprochaete ingens", "Saprochaete", "Dipodascaceae", "Saccharomycetales" ],
		"Properties": [ 6, 3, 0 ]
	}
*/

void
ClusterProperties::Save(std::ofstream & stream, uint32_t p_LevelNo)
{
	stream << "\r\n\t\"" << m_Id << "\": {";

	stream << "\r\n\t\t\"Path\": [ ";
	for (size_t i = 0; i < m_Path.size(); ++i) {
		if (i > 0) {
			stream << ", ";
		}
		stream << "\"" << m_Path[i] << "\"";
	}
	for (size_t i = m_Path.size(); i < p_LevelNo; ++i) {
		stream << ", \"" << m_Path.back() << "\"";
	}

	stream << " ], ";
	stream << "\r\n\t\t\"Coordinates\": [ " << m_Pos[0] << ", " << m_Pos[1] << ", " << m_Pos[2] << " ],";

	stream << "\r\n\t\t\"Categories\": [ ";
	for (size_t i = 0; i < m_Categories.size(); ++i) {
		if (i > 0) {
			stream << ", ";
		}
		stream << "\"" << m_Categories[i] << "\"";
	}
	stream << " ], ";

	stream << "\r\n\t\t\"Properties\": [ ";

	////	re-export the path for debugging the user interface
	//for (size_t i = 0; i < m_Path.size(); ++i) {
	//	if (i > 0) {
	//		stream << ", ";
	//	}
	//	stream << "\"" << m_Path[i] << "\"";
	//}
	//for (size_t i = m_Path.size(); i < p_LevelNo; ++i) {
	//	stream << ", \"" << m_Path.back() << "\"";
	//}

	int32_t idx = static_cast<int32_t>(m_Path.size()) - 2;
	if(idx >= 0 && idx < static_cast<int32_t>(m_Path.size())) {
		stream << m_Path[idx];
	}
	else {
		stream << 0;
	}

	for (size_t i = 0; i < m_Properties.size() - 1; ++i) {	//	TODO: use the real number of properties
		//if (i > 0) {
			stream << ", ";
		//}
		stream << m_Properties[i];
	}
	stream << " ]";

	stream << "\r\n\t}";
}


