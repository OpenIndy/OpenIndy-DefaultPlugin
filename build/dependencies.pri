
include($$PWD/../lib/OpenIndy-Core/include.pri)


# openIndyDefaultPlugin
CONFIG(release, debug|release) {
    openIndyDefaultPlugin.path = $$PWD/../install/release
    openIndyDefaultPlugin.files += $$PWD/../bin/release/p_defaultPlugind1.dll
} else {
    openIndyDefaultPlugin.path = $$PWD/../install/debug
    openIndyDefaultPlugin.files += $$PWD/../bin/debug/p_defaultPlugind1.dll
}
INSTALLS += openIndyDefaultPlugin