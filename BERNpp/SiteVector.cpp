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

#include <sstream>
#include "SiteVector.h"
#include <algorithm>

#define self (*this)

BERN::SiteType BERN::site_type;

std::istream& BERN::operator >>( std::istream& istr, BERN::SiteVector& sv )
{
    for (int i = 0; i < BERN::SiteVector::dims() ; i++)
        istr >> sv[i];
    return istr;
}
std::ostream& BERN::operator <<(std::ostream& ostr, const BERN::SiteVector& sv )
{
    for (int i = 0; i < BERN::SiteVector::dims() ; i++)
        ostr << site_type[i].Name << ": " << sv[i] << "\t";
    return ostr;
}


bool BERN::SiteVector::operator==(const BERN::SiteVector &sv) const {
    for (int i = 0; i < dims() ; i++)
        if (abs(self[i]-sv[i]) > site_type[i].error_tolerance()) return false;
    return true;
}

BERN::SiteVector BERN::SiteVector::operator+(const BERN::SiteVector &sv) const {
    SiteVector result;
    for (int i = 0; i < dims() ; i++)
        result[i] = self[i] + sv[i];
    return result;
}

BERN::SiteVector BERN::SiteVector::operator-(const BERN::SiteVector &sv) const {
    SiteVector result;
    for (int i = 0; i < dims() ; i++)
        result[i] = self[i] - sv[i];
    return result;
}

BERN::SiteVector BERN::SiteVector::operator*(double scalar) const {
    SiteVector result;
    for (int i = 0; i < dims() ; i++)
        result[i] = self[i] * scalar;
    return result;
}

BERN::SiteVector BERN::SiteVector::operator/(double scalar) const {
    SiteVector result;
    for (int i = 0; i < dims() ; i++)
        result[i] = self[i] / scalar;
    return result;
}

BERN::SiteVector BERN::SiteVector::center(const BERN::SiteVector &sv1, const BERN::SiteVector &sv2) {
    return (sv1 + sv2) * 0.5;
}

BERN::SiteVector::SiteVector()
: std::vector<double>(SiteVector::dims(), NaN) {

}

BERN::SiteVector::SiteVector(const BERN::SiteVector &src) = default;
BERN::SiteVector::SiteVector(const std::vector<double>& src)
: std::vector<double>(SiteVector::dims(), NaN)
{
    if (src.size() < this->dims()) {
        throw std::runtime_error("Too few values for a bern.SiteVector");
    } else if (src.size() > this->dims()) {
        throw std::runtime_error("Too many values for a bern.SiteVector");
    }
    std::copy(src.begin(), src.end(), this->begin());
}

BERN::SiteVector BERN::SiteVector::calc_accuracy() {
    SiteVector result;
    std::transform(
        site_type.begin(), site_type.end(),
        result.begin(),
        [](const SiteValue & var) -> double {return var.error_tolerance();}
    );
    return result;
}

BERN::SiteVector BERN::SiteVector::min(const BERN::SiteVector &sv1, const BERN::SiteVector &sv2)  {
    // Element wise min
    BERN::SiteVector res;
    std::transform(sv1.begin(), sv1.end(), sv2.begin(), res.begin(), [](double v1, double v2) -> double {return std::min(v1, v2);});
    return res;
}
BERN::SiteVector BERN::SiteVector::max(const BERN::SiteVector &sv1, const BERN::SiteVector &sv2)  {
    // Element wise max
    BERN::SiteVector res;
    std::transform(sv1.begin(), sv1.end(), sv2.begin(), res.begin(), [](double v1, double v2) -> double {return std::max(v1, v2);});
    return res;
}

std::string BERN::SiteVector::str() const {
    std::stringstream s;
    s << *this;
    return s.str();
}


double BERN::calculate_wetness_index(double accessible_field_capacity, double groundwater_table) {
    //Index for the groundwater table, assuming a gwt deeper than 15 dm is not relevant for plants
    double mG=std::max(0.0, 1- groundwater_table/1.5);
    //Index for the feasible field capacity, assuming more than 30 % are not relevant for plants
    double mK=std::min(1.0 ,std::max(0.0,accessible_field_capacity/30.0));
    //Combination of the indices, even a very high feasible field capacity can only push the moisture factor to 0.4 (highest value for not ground water influenced soils)
    return std::max(mG,mK*0.4);
}

BERN::SiteRange BERN::operator&(const BERN::SiteRange &left, const BERN::SiteRange &right) {
    BERN::SiteRange res;
    const size_t dims = left.min.dims();
    for (size_t dim=0; dim < dims; ++dim) {
        res.min[dim] = std::max(left.min[dim], right.min[dim]);
        res.max[dim] = std::min(left.max[dim], right.max[dim]);
    }
    return res;
}

BERN::SiteRange BERN::operator|(const BERN::SiteRange &left, const BERN::SiteRange &right) {
    BERN::SiteRange res;
    const size_t dims = left.min.dims();
    for (size_t dim=0; dim < dims; ++dim) {
        res.min[dim] = std::min(left.min[dim], right.min[dim]);
        res.max[dim] = std::max(left.max[dim], right.max[dim]);
    }
    return res;
}

size_t BERN::SiteType::find(const std::string &text_index) const {
    for (int i=0; i<this->size(); ++i) {
        if (text_index == self[i].Name) {
            return i;
        }
    }
    throw std::out_of_range(text_index + " is not a variable of " + this->str());
}

std::string BERN::SiteType::str() const {
    std::string res = "[";
    for (const auto & var: self) {
        res += var.Name + ",";
    }
    res[res.size() - 1] = ']';
    return res;
}

BERN::SiteVector BERN::SiteRange::center() const {
    return BERN::SiteVector::center(min, max);
}

bool BERN::SiteRange::contains(const BERN::SiteVector & site) const {
    for (int i=0; i<BERN::SiteVector::dims();++i) {
        if ((site[i] >= self.min[i]) && (site[i] <= self.max[i])) {
            return true;
        }
    }
    return false;
}

BERN::Possibility::operator bool() const {
    return value >= 0;
}


BERN::Possibility::Possibility(const SiteVector &_site, const double &_value)
        : value(_value), site(_site)
{}

std::string BERN::Possibility::str() const {
    std::stringstream s;
    s << "p = " << std::fixed << value << " @ " << site << "\n";
    return s.str();
}
