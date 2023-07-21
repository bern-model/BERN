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

#ifndef Species_h__
#define Species_h__

#include <string>
#include "SiteVector.h"
#include <map>
#include <algorithm>
namespace BERN {

    double trapez(double x,double pMin,double oMin,double oMax,double pMax);
	
	///@brief A class to characterize the niche width of a species.
	///
	///The niche of a species is defiened by a n-dimensional (currently 5 dim) trapezoid possibility distribution function (fuzzy constraint of existence)
	///It is defined by 4 points in functional space, Minimum of pessimum (pessMin), minimum of optimum (optMin), maximum of optimum (optMax) and maximum of pessimum
	///The optimum conditions (possibility value (pi) = 1) are in a range between optMin and optMax, while non optimal but feasible condition are represented by a linear
	///interpolation between pi=0 at pessMin / pessMax and the optimum range
	class Species
	{
	public:
		///@brief ID of the species
		int id;
		///@brief Name of species (usually the scientific name)
		std::string name;
        ///@brief The absolute minimum of the niche pessimum
        SiteRange pess;
        SiteRange opt;


    public:
		///Constructor
		/// @param id @copydoc ID
		/// @param name @copydoc Name
		/// @param pessMin @copydoc PessMin
		/// @param optMin @copydoc OptMin
		/// @param optMax @copydoc OptMax
		/// @param pessMax @copydoc PessMax
		Species(int id,const std::string& name,const SiteVector& pessMin, const SiteVector& optMin, const SiteVector& optMax,const SiteVector& pessMax);

        Species();

		///@brief Returns the possibility value at given site conditions
		///
		///According to Liebig's minimum law, the minimum of the possibilities for each single site parameter is returned
		///@returns The minimum possibility for each site condition
		double possibility(const SiteVector& SiteConditions) const;
		

	};

}

#endif // Species_h__
