[![logo](/images/logo.png)]()

# BERN habitat model
### Bioindication of Ecologic Regeneration within Natural conditions plant habitat model

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.10406722.svg)](https://doi.org/10.5281/zenodo.10406722)

A fuzzy logic based model for plant community habitat predictation. The model has been implemented in multiple languages, 
this version is written as a C++ library. A demo main function is provided and a [SWIG](www.swig.org) wrapper for Python.

## Code 

[![Data licence](https://mirrors.creativecommons.org/presskit/buttons/88x31/svg/by-sa.svg)](https://creativecommons.org/licenses/by-sa/4.0/legalcode)

The C++, SWIG and Python code in this repository is released under the GPL v3 (Gnu Public Licence) or its 
equivalent CC-BY-SA and is written by [Philipp Kraft](github.com/philippkraft) and can be freely modified 
and distributed, as long as the source code is distributed together with any binary release. This licence does
not include the data provided in the BERNdata repository.

## Database 

[![Data licence](https://mirrors.creativecommons.org/presskit/buttons/88x31/svg/by-nc-sa.svg)](https://creativecommons.org/licenses/by-nc/4.0/legalcode)

Angela Schlutow compiled the plant and community database in the folder BERNdata. The database is provided under a 
more restrictive licence, [CC-BY-NC-SA](https://creativecommons.org/licenses/by-nc/4.0/legalcode) and may not be used for commercial purposes. 
Research studies with the goal to publish the results in peer reviewed scientific publications are permitted. 
Contract research and commercial counselling services based on this database is understood as commercial 
use by the copyright holders. To obtain a commercial licence contact 
[IBE Ingeneurbüro Dr. Eckhof GmbH - bern@eckhof.de](mailto:bern@eckhof.de)

The full licence for the database can be found here: [BERNdata/DATA-LICENCE.md](/BERNdata/DATA-LICENCE.md) 

The BERN logo is protected and may not be used for derived works.

## Abstract

The BERN model (Bioindication for Ecosystem Regeneration towards Natural conditions) 
was developed to integrate ecological cause-effect relationships into studies on environmental 
status assessment and forecasting. Qualitative knowledge on the relationship between site types 
and vegetation communities is widely available, as can be seen from the extensive phytosociological 
publications. The aim of the BERN model is to make part of this knowledge available in numerical 
form for computer-based ecosystem modelling. The task of the BERN database is to document reference 
data for the occurrence of plant species in competitive equilibrium with each other and in equilibrium 
with the site factors. The BERN model combines the fundamental species niches that mainly form the 
competitively stable structure of a natural plant community in order to determine the realised multifactorial 
niche of this community. This is the decisive innovation compared to existing dynamic vegetation models. 
The methodology of creating the BERN database and the BERN model will be documented and possible applications 
are demonstrated with examples. One of the examples is the determination of the current regeneration potential 
of NATURA 2000 habitats.

## Installation

The BERN-library as well as the demo main function is build with CMAKE without any special needs for configuration. 
Installtion on Linux (needs a C++ compier as well as a CMAKE)

~~~~~~~~~~~~~~~.sh
cmake . build/
cd build
make
~~~~~~~~~~~~~~~~

Building the Python extension works on Linux with installed python-dev package and a C++ compiler

~~~~~~~~~~~~~.sh
python setup.py build_ext build_py bdist_wheel
pip install dist/*.whl
~~~~~~~~~~~~~

On Windows this works basically in the same manner if the MSVC-Compiler fitting to your Python version 
is installed and the commands above are issued in a Microsoft-SDK prompt.
