TEMPLATE = subdirs

SUBDIRS = oiexchangeascii \
    function

INSTALLS = 

QT = core

QMAKE_EXTRA_TARGETS += run-test
win32-msvc* {
run-test.commands = \
    if not exist reports mkdir reports & if not exist reports exit 1 $$escape_expand(\n\t)\
    cd $$shell_quote($$OUT_PWD/function) && $(MAKE) run-test & \
    cd $$shell_quote($$OUT_PWD/oiexchangeascii) && $(MAKE) run-test
} else:win32-g++ {
run-test.commands = \
    [ -e "reports" ] || mkdir reports ; \
    $(MAKE) -C $$shell_quote($$OUT_PWD/function) run-test & \
    $(MAKE) -C $$shell_quote($$OUT_PWD/oiexchangeascii) run-test
} else:linux {
run-test.commands = \
    [ -e "reports" ] || mkdir reports ; \
    $(MAKE) -C oiexchangeascii run-test ; \
    $(MAKE) -C function run-test
}
