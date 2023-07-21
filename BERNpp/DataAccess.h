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

#ifndef DataAccess_h__
#define DataAccess_h__

#include <iostream>
#include <string>
#include <memory>
#include "SiteVector.h"
#include "Species.h"
#include "Community.h"
#include "Site.h"


namespace BERN {
    /// This operator just parses a site vector
    std::istream & operator >>(std::istream& istr,BERN::Species& spec);

    ///Writes a Site vector to a stream (Space limited numbers)
    std::ostream& operator <<(std::ostream& ostr, const BERN::Species& spec);

    class Database {
    private:
        std::map<int, Species*> _species;
        std::map<int, Community*> _communities;
    public:
        ~Database();
        Species& species(int index);
        Community& community(int index);;
        size_t  species_size() const {
            return _species.size();
        }
        size_t community_size() const {
            return _communities.size();
        }


        void link(int comm_id, int spec_id);

        int load_species(std::string filename);
        int load_communities(std::string filename);
        int link_communities(std::string filename);
        void calculate_optima() const;
        std::vector<int> community_ids() const;
        std::vector<int> species_ids() const;


    };

    const BERN::SiteType& load_variables(const std::string & filename);


}
#endif // DataAccess_h__
