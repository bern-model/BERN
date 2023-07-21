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

#ifndef SiteVector_h__
#define SiteVector_h__


#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace BERN {
    const double NaN = std::numeric_limits<double>::quiet_NaN();

    struct SiteValue {
        std::string Name;
        std::string LongName;
        size_t id;
        double min;
        double max;
        double error_tolerance() const {
            return 1e-12 * (max - min);
        }

    };

    class SiteType: public std::vector<BERN::SiteValue> {
    public:
        size_t find(const std::string& text_index) const;
        std::string str() const;
    };

    extern SiteType site_type;

    ///A class holding a vector pointing at a position in the functional space of site properties.
    ///The site vector is based on a C-Array, hence extending the BERN model with more site dimensions is easy
    ///Extending BERN with another site properties X you have to
    ///- Extend the "Art" (species) table in the database with four fields XpessMin, XoptMin, XoptMax, XpessMax
    ///- Change the Macro DIMENSIONCOUNT to the appropriate number of dimensions
    ///- Adjust the @c calc_accuracy vector. This triggers the error of the Optimum calculation in the Community class, by parameter. 1/10000 of the range of that parameter is a fine value
    ///- Add a Get-Accessor @code double X() {return values[n]; } @endcode where n is DimensionCount-1 (Don't forget the DoxyGen comments)
    ///- Extend the Constructor by values
    class SiteVector : public std::vector<double> {
    public:
        SiteVector();
        SiteVector(const SiteVector& src);
        SiteVector(const std::vector<double>& src);


        ///@brief Returns true if all elements have the same value (may be wrongfully false in case of heavier mathematical calculations, floating point dilemma)
        bool operator ==(const SiteVector& sv) const;
        ///@brief Adds all elements of the two Vectors
        SiteVector operator +(const SiteVector& sv) const;
        ///@brief Substrates all elements of the right SiteVector2 from the left vector
        SiteVector operator -(const SiteVector& sv) const;
        ///@brief Multiplies all elements of the vector left by the scalar on the right side
        SiteVector operator *(double scalar) const;
        ///@brief Divide all elements of the vector left by the scalar on the right side
        SiteVector operator /(double scalar) const;

        std::string str() const;

        static size_t dims() {
            return site_type.size();
        }

        static SiteVector center(const SiteVector& sv1, const SiteVector& sv2);
        static SiteVector min(const SiteVector& sv1, const SiteVector& sv2);
        static SiteVector max(const SiteVector& sv1, const SiteVector& sv2);
        static SiteVector calc_accuracy();

        static const SiteType& type() {
            return site_type;
        }

    };

    /// This operator just parses a site vector
    std::istream & operator >>(std::istream& istr,BERN::SiteVector& sv);
    std::ostream& operator <<(std::ostream& ostr, const BERN::SiteVector& sv);


    struct SiteRange {
        BERN::SiteVector min;
        BERN::SiteVector max;
        BERN::SiteVector center() const;
        bool contains(const BERN::SiteVector&) const;
    };
    BERN::SiteRange operator&(const BERN::SiteRange& left, const BERN::SiteRange& right);
    BERN::SiteRange operator|(const BERN::SiteRange& left, const BERN::SiteRange& right);

    struct Possibility {
        ///@brief true if the optimum is already calculated
        operator bool() const;
        operator SiteVector() const {
            return site;
        }
        operator double() const {
            return value;
        }
        ///@brief The ecologic optimum of this community
        SiteVector site;
        ///The not normalized possibility at the optimum of this communits
        double value = NaN;
        Possibility() = default;
        Possibility(const SiteVector& site, const double & value);
        std::string str() const;
    };

    double calculate_wetness_index(double accessible_field_capacity, double groundwater_table);

}



#endif // SiteVector_h__
