TEMPLATE = subdirs

SUBDIRS = oiexchangeascii \
    function

INSTALLS = 

QT = core

QMAKE_EXTRA_TARGETS += run-test
run-test.commands = \
    if not exist "reports" (mkdir reports & echo create reports) else (echo reports exists) & \
    cd $$shell_path($$OUT_PWD/oiexchangeascii) && $(MAKE) run-test & \
    cd $$shell_path($$OUT_PWD/function) && $(MAKE) run-test

