#!/usr/bin/env python

from setuptools import find_packages
from common.setuptools_support import setup

setup(
    name="pygraph",
    scripts=[],

    # install SWIG modules {module_name: (source_path, target_path)
    swig_modules={
        'pygraphSWIG': ('pygraph', 'pygraph'),
    },

    # # metadata for upload to PyPI
    description="A Python wrapper to https://github.com/bjoern-andres/graph/tree/master/include/andres/graph.",
    long_description="",
    keywords="",
    url="",
)
