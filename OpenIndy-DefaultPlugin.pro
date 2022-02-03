CONFIG += c++11

TEMPLATE = subdirs

SUBDIRS = \
    core \
    plugin \
    test

# project locations
plugin.subdir = $$PWD/build
core.subdir = $$PWD/lib/OpenIndy-Core
test.subdir = $$PWD/test

# project dependencies
plugin.depends = core
test.depends = plugin

