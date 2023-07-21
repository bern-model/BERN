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
#include "Community.h"
#include <cmath>
#include <stdexcept>
#include <omp.h>

using namespace std;
using namespace BERN;

// Integer power function, cheaper then pow
int ipow(int base, int exponent) {
    if (!exponent) { return 1;}
    int res = base;
    for (int i=1; i<exponent; ++i) {
        res *= base;
    }
    return res;
}

SiteVector Community::center() const {
    SiteRange inner_circle = this->envelope();
    for (auto spec: this->species) {
        inner_circle = inner_circle & spec->opt;
    }
    return inner_circle.center();
}

//Calculates the optimum (both position and value)
//Since the possibility function's derivate is not a continuous function, usual n-dimension numeric solutions for optimum problems are not suitable
//The optimum is calculated by trying in the n direction at specified step width. If no position with a higher possibility value is found,
//the step width is divided by 10, until the step width is smaller then SiteVector::CalcAccuracy
BERN::Possibility BERN::Community::calculateOptimum() const
{
    //Site near actual to test for higher possibility
    SiteVector  test;
    double
            curVal=0,
            bestVal,
            testVal=0;
    SiteVector curSite=this->center();
    //Each step in any dimension is a multiple of the calculation accuracy of that dimension (stored in SiteVector::CalcAccuracy)
    double stepWidthFactor = 1e10;
    //As long as the factor of the step width factor is greater than 1
    while (stepWidthFactor >= 1) {
        //Calculate the possibility at the actual site (no normalization, of course, since we are calculating the optimum by now)
        curVal = possibility(curSite);
        if (curVal > 1 - 1e-12) {
            break;
        }
        //An "in between" storage for the preliminary best neighbor
        SiteVector best = curSite;
        bestVal = curVal;
        //hasDir becomes true if a direction towards a higher possibility value is found. If it becomes false the step width is adjusted
        bool hasDir = false;
        //Calculate the step width vector for all site parameters
        SiteVector stepWidthVector = SiteVector::calc_accuracy() * stepWidthFactor;
        //Test in each dimension, 3 directions per dimension: left, no move, right
        //Possible combinations (including the actual position) is direction^dimensions
        size_t dims = SiteVector::dims();
        int combinations = ipow(3, int(dims));
        for (int i = 0; i < combinations; i++) {
            //If i is not pointing on the actual site conditions
            if (i != (combinations - 1) / 2) {
                //Populate the direction vector
                for (int d = 0; d < dims; d++)
                    //The function f(i)=floor(...) - 1 gets for each dimension d the direction part [-1,0,1] of i
                    test[d] = curSite[d] + ((int) (floor(i / ipow(3, d))) % 3 - 1) * stepWidthVector[d];
                //Calculate the possiblity at the test site
                testVal = this->possibility(test);
                //Is the test site the best neighbor until now?
                if (testVal > bestVal) {
                    //test is the best...
                    best = test;
                    bestVal = testVal;
                    //We've found a Direction for further search of the optimum
                    hasDir = true;
                }
            }
        }
        //A "better" direction found
        if (hasDir)
            //Take the best estimate as the new actual site condition
            curSite = best;
        else
            //Minimize the step width
            stepWidthFactor /= 10;
    } //While (stepWidthFactor>=1)

    //The optimum is found in the given accuracy, we can update the given references with the actual site and value
    return {curSite, curVal};
}

//Calculates the possibility measure of the species by the algebraic gamma operator
//under site conditions "SiteCondition". For better comparison of different communities the possibility is normalized by the optimal value
double BERN::Community::possibility(const SiteVector &SiteCondition) const
{
	//If one of the values of the SiteCondition vector is outside the niche intersection of species, return 0
    if (!envelope().contains(SiteCondition)) {
        return 0;
    }

	//Else calculate the possibility with the algebraic gamma operator
	double 
		A = 1,
		B = 1,
		//Hard coded, standard gamma for BERN (expert knowledge)
		gamma = 0.2;
	//Algebraic gamma operator
	for (const auto & spec: species)
	{
		double poss = spec->possibility(SiteCondition);
		A *= poss;
		B *= 1-poss;
	}
	A = pow(A, gamma);
	B = pow((1-B),(1-gamma));
	
    return (A * B);
}

Community::Community(int id_, const string &name_) :
        id(id_),name(name_)
{}


SiteRange Community::envelope() const {
    if (species.empty())
        throw BERN::NoSpeciesError(*this);
    SiteRange envelope = species[0] -> pess;
    for(const auto& spec: species)
    {
        envelope = envelope | spec->pess;
    }
    return envelope;

}

BERN::Possibility Community::optimum() const {
    if (!optimumStorage)
        optimumStorage = calculateOptimum();
    return optimumStorage;
}


/*
BERN::SiteVector BERN::Community::disharmonicAlphaPosition( double alpha,double tolerance )
{
	// Create site vectors p1 at optimum, p2 at disharmonic extreme for nutrients/acidity, pn as new position during iteration
	SiteVector 
		p1 = this->OptimalSiteConditions(),
		p2 = SiteVector(0.,0.,p1.FZ(),p1.DI(),p1.VZ()),
		pn;
	double // saves the possibility values for p1,p2,pn
		w1 = this->Possibility(p1),
		w2 = this->Possibility(p2),
		wn = 0.0;
	size_t maxiter = 1000;

	for (size_t i=0;i<maxiter;++i) {
		// Find center between p1 and p2
		pn = SiteVector::Center(p1,p2);
		// get value at half distance
		wn = this->Possibility(pn);
		// if wn is greater than alpha, use lower part of distance
		if (fabs(wn-alpha)<tolerance) 
			break;
		else if (wn>alpha) 
			p1 = pn;
		else // use upper part
			p2 = pn;
	}
	return pn;
}
*/

std::vector<double> BERN::possibility(std::vector<const Community *> comms, const SiteVector &site) {
    std::vector<double> res(comms.size());
#pragma omp parallel for
    for (int i=0; i<comms.size(); i++){
        try {
            res[i] = comms[i]->possibility(site);
        } catch (const std::runtime_error& e) {
            res[i] = NaN;
        }
    }
    return res;
}

double BERN::max_possibility(std::vector<const Community *> comms, const SiteVector &site) {
    double max = 0;
    for (int i=0; i<comms.size(); i++){
        try {
            max = std::max(max, comms[i]->possibility(site));

        } catch (const std::runtime_error& e) {

        }
    }
    return max;
}

std::vector<double> BERN::possibility_matrix(const vector<const Community *> &comms, const vector<SiteVector> &sites) {
    size_t nc = comms.size();
    size_t ns = sites.size();
    size_t ntot = nc * ns;
    std::vector<double> res(ntot);
#pragma omp parallel for
    for (int s=0; s < ns; s++) {
        for (int c = 0; c < comms.size(); c++) {
            size_t i = s * nc + c;
            try {
                res[i] = comms[c]->possibility(sites[s]);
            } catch (const std::runtime_error &e) {
                res[i] = NaN;
            }
        }
    }
    return res;
}


