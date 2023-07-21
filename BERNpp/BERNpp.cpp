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

#include <iostream>
#include <string>
#include <list>
#include "DataAccess.h"
#include "SiteVector.h"
#include "Species.h"
#include "Community.h"
#include "Site.h"
#include <memory>
#define DllExport __declspec(dllexport)
class BERNData
{
public:
	typedef std::auto_ptr<BERN::Species::Map> SpecMapPointer;
	typedef std::auto_ptr<BERN::Community::Map> CommMapPointer;
	SpecMapPointer species;
	CommMapPointer communities;
	BERN::Species * GetSpecies(int id) { 
		BERN::Species::Map::iterator it=species->find(id);
		if (it!=species->end())
			return it->second;
		else
			return 0;
	}
	BERN::Community * GetCommunity(int id) 
	{ 
		BERN::Community::Map::iterator it=communities->find(id);
		if (it!=communities->end())
			return it->second;
		else
			return 0;
	}
	BERNData() : species(new BERN::Species::Map),communities(new BERN::Community::Map) {}
	void SetData(std::auto_ptr<BERN::Species::Map> spec,std::auto_ptr<BERN::Community::Map> comm)
	{
		species=spec;
		communities=comm;
	}
};
struct result
{
	int id;
	double possibility;
	bool operator<(const result & cmp) {return possibility<cmp.possibility;}
	result(int ID,double Poss) : id(ID),possibility(Poss) {}
};
typedef std::list<result> resultqueue;
extern "C"
{
	DllExport void * CreateBERNData()
	{
		return (void *)(new BERNData);
	}
	DllExport void DeleteBERNData(void * data)
	{
		BERNData *bern=(BERNData*)(data);
		delete bern;
	}
	DllExport int AddSpecies(void * data, int id, char * name,
		double BSmin,double BSoptmin,double BSoptmax,double BSmax,
		double CNmin,double CNoptmin,double CNoptmax,double CNmax,
		double FZmin,double FZoptmin,double FZoptmax,double FZmax,
		double WBmin,double WBoptmin,double WBoptmax,double WBmax,
		double VZmin,double VZoptmin,double VZoptmax,double VZmax
		)
	{
		using namespace  BERN;
		BERNData *bern=(BERNData*)(data);
		SiteVector sv_min(BSmin,CNmin,FZmin,WBmin,VZmin);
		SiteVector sv_optmin(BSoptmin,CNoptmin,FZoptmin,WBoptmin,VZoptmin);
		SiteVector sv_optmax(BSoptmax,CNoptmax,FZoptmax,WBoptmax,VZoptmax);
		SiteVector sv_max(BSmax,CNmax,FZmax,WBmax,VZmax);
		Species * spec=new Species(id,name,"",sv_min,sv_optmin,sv_optmax,sv_max);
		(*bern->species)[id]=spec;
		return int(bern->species->size());
	}
	DllExport int AddCommunity(void * data,int id,char * name,int landuse,int ecovalue)
	{
		using namespace  BERN;
		BERNData *bern=(BERNData*)(data);
		Community* comm=new Community(id,name,(landuse & 1)!=0,(landuse & 2) != 0, (landuse & 4) != 0,ecovalue);
		(*bern->communities)[id]=comm;
		return int(bern->communities->size());
	}
	DllExport int AddSpeciesToCommunity(void * data,int comm_id,int spec_id)
	{
		using namespace  BERN;
		BERNData *bern=(BERNData*)(data);
		Community * comm=bern->GetCommunity(comm_id);
		Species * spec=bern->GetSpecies(spec_id);
		if (comm==0 || spec==0)
		{
			return 0;
		}
		comm->AddSpecies(spec);
		return int(comm->CountSpecies());
	}
	DllExport char * SpeciesName(void * data, int id) 
	{
		BERNData *bern=(BERNData*)(data);
		std::string name=bern->GetSpecies(id)->Name();
		char * res=new char[name.size()];
		name.copy(res,name.size());
		return res;
	}
	DllExport void * LoadFromASCII(char * SpeciesFile,char * CommunityFile, char * relate_file)
	{
		using namespace std;
		using namespace BERN;
		BERNData::SpecMapPointer spec(DataAccess::ReadSpeciesMap(SpeciesFile));
		BERNData::CommMapPointer comm(DataAccess::ReadCommunityMap(CommunityFile));
		DataAccess::RelateCommunitiesWithSpecies(*comm,*spec,relate_file);
		BERNData *bern=new BERNData;
		bern->SetData(spec,comm);
		return (void *)(bern);
	}
	DllExport double SpeciesPoss(void * data,int spec_id,double BS,double CN,double FZ, double WB, double VZ)
	{
		BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
		BERNData *bern=(BERNData*)(data);
		BERN::Species * spec=bern->GetSpecies(spec_id);
		if (spec==0) return -1.0;
		return spec->Possibility(sv);
	}
	DllExport double CommunityPoss(void* data,int comm_id,double BS,double CN,double FZ, double WB, double VZ)
	{
		BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
		BERNData *bern=(BERNData*)(data);
		BERN::Community * comm=bern->GetCommunity(comm_id);
		if (comm==0) return -1.0;
		return comm->Possibility(sv);
	}
	DllExport int SpeciesCount(void * data)
	{
		BERNData *bern=(BERNData*)(data);
		return int(bern->species->size());
	}
	DllExport int CommunityCount(void * data)
	{
		BERNData *bern=(BERNData*)(data);
		return int(bern->communities->size());
	}
	DllExport int CommunityOptimum(void * data,int id,double * BS,double * CN,double * FZ, double * WB,double * VZ)
	{
		BERNData * bern=(BERNData*)(data);
		BERN::Community * comm=bern->GetCommunity(id);
		if (comm)
		{
			BERN::SiteVector opt=comm->OptimalSiteConditions();
			*BS=opt.BS();
			*CN=opt.CN();
			*FZ=opt.FZ();
			*WB=opt.DI();
			*VZ=opt.VZ();
			return 1;
		}
		else
			return 0;
	}
	DllExport int FeasibleSpecies(void * data,double BS,double CN,double FZ, double WB, double VZ,int * idArray,double * possArray)
	{
		BERNData *bern=(BERNData*)(data);
		resultqueue spec_result;
		for(BERN::Species::Map::iterator it = bern->species->begin(); it != bern->species->end(); ++it)
		{
			BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
			BERN::Species&  spec=*it->second;
			double poss=spec.Possibility(sv);
			if (poss>0) spec_result.push_back(result(spec.ID(),poss));
		}
		spec_result.sort();
		int i=0;
		for(resultqueue::reverse_iterator it = spec_result.rbegin(); it != spec_result.rend(); ++it)
		{
		   idArray[i]=it->id;
			 possArray[i]=it->possibility;
			 ++i;
		}
		return i;
	}
	DllExport int BestSpecies(void * data,double BS,double CN,double FZ, double WB, double VZ)
	{
		BERNData *bern=(BERNData*)(data);
		double maxposs=0;
		int res=0;
		for(BERN::Species::Map::iterator it = bern->species->begin(); it != bern->species->end(); ++it)
		{
			BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
			BERN::Species&  spec=*it->second;
			double poss=spec.Possibility(sv);
			if (poss>maxposs) 
			{
				res=it->first;
				maxposs=poss;
			}
		}
		return res;
	}
	DllExport int BestCommunity(void * data,double BS,double CN,double FZ, double WB, double VZ)
	{
		BERNData *bern=(BERNData*)(data);
		double maxposs=0;
		int res=0;
		for(BERN::Community::Map::iterator it = bern->communities->begin(); it != bern->communities->end(); ++it)
		{
			BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
			BERN::Community&  comm=*it->second;
			double poss=comm.Possibility(sv);
			if (poss>maxposs) 
			{
				res=it->first;
				maxposs=poss;
			}
		}
		return res;
	}
	DllExport int FeasibleCommunities(void * data,double BS,double CN,double FZ, double WB, double VZ,int * idArray,double * possArray)
	{
		BERNData * bern=(BERNData*)(data);
		resultqueue comm_result;
		for(BERN::Community::Map::iterator it = bern->communities->begin(); it != bern->communities->end(); ++it)
		{
			BERN::SiteVector sv(BS,CN,FZ,WB,VZ);
			BERN::Community&  comm=*it->second;
			double poss=comm.Possibility(sv);
			if (poss>0) comm_result.push_back(result(comm.ID(),poss));
		}
		comm_result.sort();
		int i=0;
		for(resultqueue::reverse_iterator it = comm_result.rbegin(); it != comm_result.rend(); ++it)
		{
			idArray[i]=it->id;
			possArray[i]=it->possibility;
			++i;
		}
		return i;
	}
}
#ifdef _CONSOLE
int main(int argc, char* argv[])
{
	using namespace std;
	using namespace BERN;
	//Loads species from the example ASCII-files (the ASCII files are not free)
	Species::Map& species = *DataAccess::ReadSpeciesMap("Art.txt");
	//Loads communities from the example ASCII-files (the ASCII files are not free)
	Community::Map& communities = *DataAccess::ReadCommunityMap("Gesellschaft.txt");
	//Builds the relation between communities and species
	DataAccess::RelateCommunitiesWithSpecies(communities,species,"hat_art.txt");
	cout.precision(4);
	//Display the number of species and communities loaded
	cout << species.size() << " Species, " << communities.size() << " Communities" << endl;

	cout << "Example 1: Show the species with their niches and the resulting optimum" << endl;
	//Gets a community (ID 2010 = 
	Community& com = *communities[2010];
	//For each species of com
	for(Community::SpeciesVector::const_iterator it = com.begin(); it != com.end(); ++it)
	{
		//Display niche of each species
		Species& spec = **it;
	  cout << spec.ID() << " - " << spec.Name() << endl;
		cout << spec.PessMin() << endl;
		cout << spec.PessMax() << endl;
	}
	//Display name of com
	cout << com.Name() << endl;
	//Display niche of com
	cout << com.EnvelopeMin() << endl;
	cout << com.EnvelopeCenter() << endl;
	cout << com.EnvelopeMax() << endl;
	//Display optimum condition for com
	cout << "Opt: " << com.OptimalSiteConditions() << endl;
	
	//Example 2: Display the optimum conditions for all communities
	cout << endl << "Example 2: Display the optimum conditions for all communities" << endl;
	for(Community::Map::const_iterator it = communities.begin(); it != communities.end(); ++it)
	{
		Community& com=*it->second;
		cout << com.ID() << " - " << com.Name() << "(" << com.CountSpecies() << "): " << endl;
		cout << "Center: " << com.EnvelopeCenter() << endl;
		cout << "opt.  : " << com.OptimalSiteConditions() << endl;
	}

	//Example 3 & 4 are a little bit nonsense if the whole community collection is used
	//Build a new map with only the natural forest communities
	Community::Map& natForestCommunities = *new Community::Map;
	for(Community::Map::const_iterator it = communities.begin(); it != communities.end(); ++it)
	{
		Community& com=*it->second;
		if (com.IsForestCommunity() && com.EcologicValue() == 5)
			com.AddToMap(natForestCommunities);
	}


	//Example 3: Find possibility of communities for specified site states
	cout << endl << "Example 3: Find possibility of communities for specified site states" << endl;

	//Load site states
	SiteState::Map& states = *DataAccess::ReadSiteStates("SiteStates.txt");
	//Cycle through all site states
	for(SiteState::Map::const_iterator sIt = states.begin(); sIt != states.end(); ++sIt)
	{
		//Set a reference to the current site state, for better readability
		SiteState& state = *sIt->second;
		//Flag if any community for these site conditions are found
		bool comFound=false;
		//Cycle through all communities
    for(Community::Map::const_iterator cIt = natForestCommunities.begin(); cIt != natForestCommunities.end(); ++cIt)
    {
			//Set a reference to the current community, for better readability
			Community& com = *cIt->second;
			//Get the possibility of the current community at the current site state
			double poss = com.Possibility(state.SiteConditions());
			//If a the current community can exist at the current site state
			if (poss>0)
			{
				//Display the community
				cout << state.ID() << " (" << state.SiteConditions() << ") " << com.ID() << " - " << com.Name() << " =>Poss: " << poss << endl;
				//Flag, that a community was found for the current site state
				comFound=true;
			}
    }
		//If no community is found, give a message
		if (!comFound) cout << "No community found at  " << state.ID() << state.SiteConditions() << endl;
	}

	//Example 4: Find most suitable community at a given site state
	cout << endl << "Example 4: Find most suitable community at a given site state" << endl;
	//Cycle through all site states
	for(SiteState::Map::const_iterator sIt = states.begin(); sIt != states.end(); ++sIt)
	{
		//Site state reference
		SiteState& state = *sIt->second;
		//Highest possibility of any community at the current site state
		double maxPoss=0;
		//Community that achieved maxPoss
		Community* bestCom=0;
		for(Community::Map::const_iterator cIt = natForestCommunities.begin(); cIt != natForestCommunities.end(); ++cIt)
		{
			//Reference to community
			Community& com = *cIt->second;
			//Get the possibility of the current community at the current site state		
			double poss = com.Possibility(state.SiteConditions());
			//If the possibility of the current community is higher than all communities before...
			if (poss>maxPoss)
			{
				//Set the current possibility(com,state) as the highest
				maxPoss=poss;
				//Remember the current community as the most suitable at that state
				bestCom=&com;
			}
		}
		//If no best community was found (maxPoss==0)
		if (!bestCom) 
			//Give a message about that
			cout << "No community found at  " << state.ID() << state.SiteConditions() << endl;
		else 
			//Otherwise display the most suitable community
			cout << state.ID() << " (" << state.SiteConditions() << ") " << bestCom->ID() << " - " << bestCom->Name() << " =>Poss: " << maxPoss << endl;
	}
	//Wait for input, to hold the results at the display if the program was started from the IDE
	cout << endl << "Press [ENTER] to finish...";
	cin.get();

	//Tiding up the memory
	delete &natForestCommunities;
	for(SiteState::Map::const_iterator it = states.begin(); it != states.end(); ++it)
		delete it->second;
	delete &states;
	for(Community::Map::const_iterator it = communities.begin(); it != communities.end(); ++it)
		delete it->second;
	delete &communities;
	for(Species::Map::const_iterator it = species.begin(); it != species.end(); ++it)
		delete it->second;
	delete &species;
	return 0;
}
#endif


