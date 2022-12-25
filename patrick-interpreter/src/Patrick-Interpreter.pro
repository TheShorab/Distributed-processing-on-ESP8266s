QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    interpreter/core/core.h \
    interpreter/core/error.h \
    interpreter/core/expression-solver.h \
    interpreter/core/function.h \
    interpreter/core/if-else.h \
    interpreter/core/llcore.h \
    interpreter/core/loop-state.h \
    interpreter/core/loop.h \
    interpreter/core/pcb-manager.h \
    interpreter/core/pcb.h \
    interpreter/core/scope.h \
    interpreter/core/variable.h \
    interpreter/front/front-end.h \
    interpreter/interpreter.h \
    tests.h
