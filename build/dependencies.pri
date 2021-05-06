
include($$PWD/../lib/OpenIndy-Core/include.pri)


# openIndyDefaultPlugin
win32 {
CONFIG(release, debug|release) {
    dlltarget.path = $$PWD/../install/release
} else {
    dlltarget.path = $$PWD/../install/debug
}
INSTALLS += dlltarget
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
