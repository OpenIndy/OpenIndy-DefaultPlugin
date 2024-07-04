CONFIG += c++11

TEMPLATE = subdirs

SUBDIRS = \
    core \
    plugin \
    test

# project locations
plugin.subdir = build
core.subdir = lib/OpenIndy-Core
test.subdir = test

# project dependencies
plugin.depends = core
test.depends = plugin

QMAKE_EXTRA_TARGETS += run-test
run-test.commands = \
    cd test && $(MAKE) run-test
