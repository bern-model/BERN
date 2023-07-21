#include <iostream>
#include <memory>
#include <chrono>

#include "BERNpp/SiteVector.h"
#include "BERNpp/Site.h"
#include "BERNpp/Species.h"
#include "BERNpp/Community.h"
#include "BERNpp/DataAccess.h"

void calculate_optima(BERN::Database& db) {
    auto t_start = std::chrono::high_resolution_clock::now();
    db.calculate_optima();
    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dt = t_end - t_start;
    std::cout << dt.count() * 0.001 << "sec to calculate all optima\n";

}

void print_species(BERN::Database& db) {
    for(int id: db.species_ids()) {
        auto & spec = db.species(id);
        std::cout << spec.id << " " << spec.name << ": \n    " << spec.pess.min << "\n    " << spec.pess.max << "\n";
    }
}
void print_communities(BERN::Database& db) {
    for (int id: db.community_ids()) {
        auto& com = db.community(id);
        try {
            std::cout << com.optimum().value << "\t" << com.id << "\t" << com.name  << "\n";
        } catch (BERN::NoSpeciesError& e) {
            std::cerr << com.id << "\t" << com.name << "\t" << e.what() << "\n";
        }
        std::cout.flush();
    }

}
void print_community_details(BERN::Community& com) {
    std::cout << com.id << " " << com.name << "\n";
    for (auto& spec: com.species) {
        std::cout << "\t" << spec->id << " " << spec->name << "\n";
        std::cout << "\t\tmin:" << spec->pess.min << "\n";
        std::cout << "\t\tmax:" << spec->pess.max << "\n";

    }
    std::cout << "->min: " << com.envelope().min << "\n";
    std::cout << "->max: " << com.envelope().max << "\n";
    std::cout.flush();
    BERN::Possibility opt = com.optimum();
    std::cout << "->opt: " << opt.value << "\n";
    std::cout << "->com(opt)" << com.possibility(opt.site) << "\n";

}

int main() {

    try {

        BERN::load_variables("BERNdata/site_type.tsv");
        BERN::Database db;
        size_t species_count = db.load_species("BERNdata/plant-species.tsv");
        std::cout << species_count << " species loaded\n";
        size_t community_count = db.load_communities("BERNdata/communities.tsv");
        std::cout << community_count << " communities loaded\n";

        size_t link_count = db.link_communities(
                "BERNdata/link_plantspecies_to_community.tsv"
        );
        std::cout << link_count << " links between communities and species\n";
        print_community_details(db.community(2755));
        calculate_optima(db);
        print_communities(db);
        std::cout.flush();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}
