
include($$PWD/../lib/OpenIndy-Core/include.pri)


# openIndyDefaultPlugin
win32 {
CONFIG(release, debug|release) {
    openIndyDefaultPlugin.path = $$PWD/../install/release
    openIndyDefaultPlugin.files += $$PWD/../bin/release/p_defaultPlugin1.dll
} else {
    openIndyDefaultPlugin.path = $$PWD/../install/debug
    openIndyDefaultPlugin.files += $$PWD/../bin/debug/p_defaultPlugind1.dll
}
INSTALLS += openIndyDefaultPlugin
}

linux {
CONFIG(release, debug|release) {
    openIndyDefaultPlugin.path = $$PWD/../install/release
    openIndyDefaultPlugin.files += $$PWD/../bin/release/libp_defaultPlugin.so
} else {
    openIndyDefaultPlugin.path = $$PWD/../install/debug
    openIndyDefaultPlugin.files += $$PWD/../bin/debug/libp_defaultPlugin.so
}
INSTALLS += openIndyDefaultPlugin

}
