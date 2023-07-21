// BERN-model
//
// A static model to calculate the potential biodiversity at given environmental factors
// (c) 2023 by IBE – Ingenieurbüro Dr. Eckhof GmbH, https://www.eckhof.de/unternehmen.html
// Written by Philipp Kraft, Justus-Liebig-Universität, 2007 - 2023
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// For the usage of this model a database of species and plant communities is needed.
// The database is usually in the same repository as this code, but covered by another, less free licence

#ifndef Site_h__
#define Site_h__

#include "SiteVector.h"
#include <string>
#include <strstream>
#include <map>
namespace BERN {
	///Class that represents named site conditions, e.g. different measured or modelled plots, or time series of a single plot
	class SiteState
	{
	private:
		///The site conditions of this state
		SiteVector m_SiteVector;
		std::string m_Name;
		int m_ID;
	protected:
	public:
		///@returns The conditions of this site state
		const BERN::SiteVector& SiteConditions() const { return m_SiteVector; }
		///@returns the name of this site state state (e.g. Name of plot)
		std::string Name() const { return m_Name; }
		///@param val the name of this site state state (e.g. Name of plot)
		void Name(std::string val) { m_Name = val; }
		///@returns the unique ID of this plot (if not given on initialization the number of created site states will be used), for time series of a single plot, this may be a year
		int ID() const { return m_ID; }

		///Complete constructor
		SiteState(int Id, const std::string& _Name, SiteVector& siteconditions) : m_ID(Id), m_Name(_Name), m_SiteVector(siteconditions) {}
		///Constructor with automatic name generation
		SiteState(int Id, SiteVector& siteconditions): m_ID(Id), m_SiteVector(siteconditions)
		{
			std::strstream strstr;
			strstr << m_ID;
			m_Name=strstr.str();
		}
		///The STL-Map of this type
		typedef std::map<int,SiteState*> Map;
		///Adds this to a given SiteState::Map
		void AddToMap(Map& map) {map.insert(std::make_pair(this->ID(),this));}

	};
}

#endif // Site_h__
