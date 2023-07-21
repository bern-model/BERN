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

#include "DataAccess.h"
#include <ostream>
#include <fstream>
#include <omp.h>


std::istream & BERN::operator>>(std::istream &istr, BERN::Species &spec) {
    std::string dummy;
    istr >> spec.id;
    std::getline(istr, dummy, '\t');
    std::getline(istr, spec.name, '\t');
    //std::getline(istr, dummy, '\t');
    istr >> spec.pess.min >> spec.opt.min >> spec.opt.max >> spec.pess.max;
    return istr;

}
bool skip_comment(std::istream& str) {
    //Commentary lines are marked with #
    //The column headers have to be comments to
    std::string dummy;
    char nextChar = str.peek();
    if (nextChar =='#') {
        getline(str, dummy);
        return true;
    }
    return false;
}


const BERN::SiteType& BERN::load_variables(const std::string& filename) {
    if (!site_type.empty()) {
        std::cerr << "Warning: Site type is not empty. Variables already populated, load aborted.\n";
    } else {
        //Open the file (exception handling needed)
        std::ifstream varFile;
        // specFile.exceptions(std::ios::failbit);
        varFile.open(filename);
        if (varFile) {
            //A place for something uninteresting
            SiteValue var;
            std::string dummy;
            size_t id=0;
            while (varFile.good() && !varFile.eof()) {
                if (!skip_comment(varFile)) {
                    var.id = id++;
                    std::getline(varFile, var.Name, '\t');
                    std::getline(varFile, var.LongName, '\t');
                    varFile >> var.min >> var.max;
                    std::getline(varFile, dummy);
                    site_type.push_back(var);
                }
            }
        } else {
            throw std::runtime_error(filename + " does not exist");
        }
    }
    return site_type;
}

std::ostream &BERN::operator<<(std::ostream &ostr, const BERN::Species &spec) {
    ostr << spec.name << " (" << spec.id << ")";
    return ostr;
}



BERN::Database::~Database() {
    for (auto com: _communities) {
        delete com.second;
    }
    for (auto spec: _species) {
        delete spec.second;
    }
}

BERN::Species &BERN::Database::species(int index) {
    return *_species[index];
}

BERN::Community &BERN::Database::community(int index) {
    return *_communities[index];
}

void BERN::Database::link(int comm_id, int spec_id) {
    auto comIt = _communities.find(comm_id);
    auto specIt = _species.find(spec_id);
    if (comIt!=_communities.end() && specIt!=_species.end()) {
        comIt->second->species.push_back(specIt->second);
    }

}

int BERN::Database::link_communities(std::string filename) {
    //Open the file (exception handling needed)
    std::ifstream relateFile;
    relateFile.open(filename.c_str());
    std::string dummy;
    int i=0;
    while (relateFile.good() && !relateFile.eof())
    {
        if (!skip_comment(relateFile))
        {
            int commId, specId, steady;
            relateFile >> commId >> specId >> steady;
            if (steady) {
                this->link(commId, specId);
                i++;
            }
            //Make sure to go to the end of the line
            getline(relateFile,dummy);

        }
    }
    relateFile.close();
    return i;

}

int BERN::Database::load_species(std::string filename) {
    //Open the file (exception handling needed)
    std::ifstream specFile;
    // specFile.exceptions(std::ios::failbit);
    specFile.open(filename);
    //A place for something uninteresting
    std::string dummy;
    while (specFile.good() && !specFile.eof())
    {
        if (!skip_comment(specFile)) {
            auto spec = new Species;
            specFile >> *spec;
            if (spec->id >= 0)
                _species[spec->id] = spec;
            else
                delete spec;
            std::getline(specFile, dummy);
        }
    }
    specFile.close();
    return _species.size();

}

int BERN::Database::load_communities(std::string filename) {
    //Open the file (exception handling needed)
    std::ifstream commFile;
    commFile.open(filename.c_str());
    //A place for something uninteresting
    std::string dummy;
    while (commFile.good() && !commFile.eof())
    {
        if (!skip_comment(commFile))
        {
            auto com = new BERN::Community;
            commFile >> com->id;
            dummy = commFile.get();
            std::getline(commFile, com->name, '\t');
            getline(commFile,dummy);
            _communities[com->id] = com;
        }
    }
    commFile.close();
    return _communities.size();
}

void BERN::Database::calculate_optima() const{

    std::vector<int> comm_ids;
    std::transform(_communities.begin(), _communities.end(), std::back_inserter(comm_ids),
                   [](auto & kv){return kv.first;}
   );
#pragma omp parallel for
    for (int i=0; i<comm_ids.size(); i++){
        try {
            int comm_id = comm_ids[i];
            const Community* comm = _communities.at(comm_id);
            comm->optimum();

        } catch (const std::runtime_error& e) {

        }
    }

}

std::vector<int> BERN::Database::community_ids() const {
    std::vector<int> res;
    res.reserve(community_size());
    for (const auto& it: _communities) {
        res.push_back(it.first);
    }
    return res;
}

std::vector<int> BERN::Database::species_ids() const {
    std::vector<int> res;
    res.reserve(species_size());
    for (const auto& it: _species) {
        res.push_back(it.first);
    }
    return res;
}
