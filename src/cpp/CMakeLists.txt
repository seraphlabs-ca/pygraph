CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PYGRAPH)

# setup header
INCLUDE($ENV{CMAKE_COMMON_HEADER_FILE})

# add project directories
ADD_SUBDIRECTORY(pygraph)

# setup footer
INCLUDE(FooterConfig)
