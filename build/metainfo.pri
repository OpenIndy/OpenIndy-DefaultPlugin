# create plugin metainfo file
METAINFO=$$cat($$PWD/../metaInfo.json.in, blob)

# TODO: change linux-g++ to GCC, change / fix also OiMetaData::getCompiler()
linux-g++* : COMPILER=minGW
win32-g++* : COMPILER=minGW
win32-msvc2013 : COMPILER="MVSC 64bit"
METAINFO=$$replace(METAINFO, 'COMPILER', $$COMPILER)

linux : OS=linux
win32 : OS=windows
METAINFO=$$replace(METAINFO, 'OS', $$OS)

write_file($$PWD/../metaInfo.json, METAINFO)
