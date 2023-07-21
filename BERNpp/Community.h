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
#ifndef Community_h__
#define Community_h__


#include "SiteVector.h"
#include "Species.h"
#include <vector>
#include <map>
#include <string>
namespace BERN {
	///@brief A class representing a community of species
	///
	///It has vector of species, some informations about the community and tools to calculate the niche of the Community
	class Community
	{
	public:
		typedef std::vector<const Species*> SpeciesVector;
        ///@brief The vector of species belonging to this community
        SpeciesVector species;

        ///Can be used to store additional information as key/value pairs
        std::map< std::string, std::string > extra_info;

    public:
		///@brief The unique ID
		///
		///Although not necessary, the following naming convention was used:
		///@li 2000..2999 Natural forest communities
		///@li 3000..3999 hayfields
		///@li 4000..4999 meadows
		///@li 5000..5999 Communities which may be easier meadows or hayfields
		///@li 6000..6999 Intensive used forests
		int id;
		std::string name;

    private:
        mutable Possibility optimumStorage;
		///@brief calculates the highest possibility value and populates the m_Optimum vector with the optimal site condition
        Possibility calculateOptimum() const;

	public:
		///Constructor (no species added)
		explicit Community(int id_=-1, const std::string &name_="");

		///@name Species of community
		//@{
		///Gets the number of species in the community
		size_t size() const {return species.size();}

        SiteRange envelope() const;
        SiteVector center() const;


		//@}
    public:
		///Returns the optimal site conditions of this community. It calculates the optimum if the current optimum is outdated
        BERN::Possibility optimum() const;
		///Returns the hypercube, where the possibility may be greater than 0. It calculates the envelope if the current envelope is outdated

		///Calculates the possibility of existence of this community at given site conditions
		/// @returns  The possibility, [0..1]
		/// @param SiteCondition The site conditions, for which the possibility is calculated
        double possibility(const SiteVector &SiteCondition) const;



	};
    class NoSpeciesError: public std::runtime_error {
    public:
        explicit NoSpeciesError(const Community& comm) : std::runtime_error(std::to_string(comm.id) + ": " + comm.name + " has no species") {}
    };

    /// Calculates the possibility for a group of communities at the same site. Uses OpenMP parallelisation, if available
    std::vector<double> possibility(std::vector<const Community*> comms, const SiteVector & site);

    /// Calculates the possibilities for every community in comms at every site in sites.
    /// Returns an array in the size comms.size() * sites.size().
    ///
    /// Result structure (for 3 communities and 5 sites):
    ///     p(c1,s1), p(c2,s1), p(c3,s1), p(c1,s2), p(c2,s2), p(c3,s2) ... p(c3,s5)
    ///
    ///
    /// possibility_matrix = np.array(possibility_matrix
    /// \param comms Communities
    /// \param sites Sites
    /// \return array in the size comms.size() * sites.size()
    std::vector<double> possibility_matrix(const std::vector<const Community*> & comms, const std::vector<SiteVector> & sites);

    /// Calculate the maximum possibility at a site
    double max_possibility(std::vector<const Community*> comms, const SiteVector & site);



}
#endif // Community_h__
