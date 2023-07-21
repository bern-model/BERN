import sys
from setuptools import setup, Extension
import glob

sources = [f'BERNpp/{s}.cpp' for s in 'SiteVector,Community,species,DataAccess'.split(',')] + ['BERNpp/bern.i']
print('\n'.join(sources))

def version():
    with open('BERNpp/bern.i') as f:
        for line in f:
            if line.strip().startswith('__version__'):
                return line.split('==')[-1].strip()
    raise RuntimeError('No version info found in BERNpp/bern.i')

bern = Extension(
    '_bern',
    sources=sources,
    swig_opts=['-c++', '-doxygen', '-py3']
)

setup(
    name='bern',
    version='0.1.5',
    author="Philipp Kraft",
    description="""BERN habitat model""",
    ext_modules=[bern],
    py_modules=["bern"],
    package_dir={'': 'BERNpp', 'tools': 'pytools'}
)
