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


#include "Species.h"

double BERN::trapez(double x, double pMin, double oMin, double oMax, double pMax) {
    //Trapezoid function
    if (x < pMin || x > pMax) { // Outside pessimum
        return 0;
    } else if (x < oMin) {  // Left flank
        return (x - pMin) / (oMin - pMin);
    } else if (x > oMax) { // Right flank
        return (pMax - x) / (pMax - oMax);
    } else { //Optimum plateau
        return 1;
    }
}

double BERN::Species::possibility(const BERN::SiteVector &SiteConditions) const {
    double minValue=1;
    for (int i = 0; i < SiteVector::dims(); i++)
    {
        //Calculate the possibility for each parameter
        double poss=trapez(SiteConditions[i], pess.min[i], opt.min[i], opt.max[i], pess.max[i]);
        //Get the minimum
        minValue = std::min(poss, minValue);
    }
    return minValue;
}

BERN::Species::Species(int id_, const std::string &name_, const BERN::SiteVector &pessMin,
                       const BERN::SiteVector &optMin, const BERN::SiteVector &optMax, const BERN::SiteVector &pessMax)
        : id(id_), name(name_), pess({pessMin, pessMax}), opt({optMin, optMax})
{

}

BERN::Species::Species()
    : id(-1), name()
{

}
