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

%module bern

%include "std_string.i"
%include "std_vector.i"
// enable exception support
%include "exception.i"
%exception {
    try {
        $action
    } catch (const std::out_of_range& e) {
        SWIG_exception(SWIG_IndexError, e.what());
    } catch (const std::exception& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }

}
%{

#include "SiteVector.h"
#include "Species.h"
#include "Community.h"
 // #include "Site.h"
#include "DataAccess.h"

%}
namespace std {
    %template(IntVector) std::vector<int>;
    %template(DoubleVector) std::vector<double>;
    %template(SiteValueVector) std::vector<BERN::SiteValue>;
    %template(SpeciesVector) std::vector<const BERN::Species*>;
    %template(CommunityVector) std::vector<const BERN::Community*>;
    %template(SiteVectorVector) std::vector<BERN::SiteVector>;
};
// Add typemap(in) iterable to SiteVector
%include "SiteVector.h"

%extend BERN::SiteValue {
    std::string __repr__() const {
        return $self->Name;
    }
};

%extend BERN::SiteType {
        std::string __repr__() const {
            return $self->str();
        }
}

%extend BERN::SiteVector {
    std::string __repr__() const {
        return $self->str();
    }
    %pythoncode{

    def __getattr__(self, item):
        t = self.type()
        try:
            pos = t.find(item)
            return self[pos]
        except IndexError:
            raise AttributeError(f'{item} is not a dimension of the site vector')

    def __setattr__(self, item, value):
        t = self.type()
        try:
            pos = t.find(item)
            self[pos] = value
        except IndexError:
            super().__setattr__(item, value)


    def __dir__(self):
        return ['type', 'center', 'min', 'max', 'to_dict'] + [sv.Name for sv in self.type()]

    def to_dict(self):
        return dict(zip([sv.Name for sv in self.type()], self))
    }

}

%extend BERN::SiteRange {
        std::string __repr__() const {
            return "min:" + $self->min.str() + "\nmax:" + $self->max.str();
        }
};

%extend BERN::Possibility {
    std::string __repr__() const {
        return $self->str();
    }
};

%include "Species.h"
%extend BERN::Species {
    std::string __repr__() const {
        return "Species(" + std::to_string($self->id) + ":\"" + $self->name + "\")";
    }
}

%rename (_possibility_matrix) BERN::possibility_matrix;
%include "Community.h"
%extend BERN::Community {
    std::string __repr__() const {
        return "Community(" + std::to_string($self->id) + ":\"" + $self->name + "\")";
    }
}

%include "DataAccess.h"

%extend BERN::Database {
    %pythoncode {
        def communities(self):
            """Returns an iterator through all loaded communities"""
            return (self.community(c_id) for c_id in self.community_ids())
    }
};
%pythoncode {
def possiblity_matrix(communities, sites):
    import numpy as np
    dv = _possibility_matrix(communities, sites)
    return np.array(dv).reshape(len(sites), len(communities))

__version__ = '0.1.5'
}