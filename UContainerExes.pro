QT       += core gui sql

TARGET = PathBasket
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
        UContainerExes.cpp \
    UAboutProgram.cpp \
    ULineEditWithButton/ULineEditWithButtons.cpp \
    UAddUrlDialog.cpp \
    USettings.cpp \
    sbglass.cpp \
    UPasswordWidget.cpp \
    USettingsPassword.cpp

HEADERS  += UContainerExes.h \
    UQueryPatterns.h \
    UAboutProgram.h \
    USupport.h \
    UDatabaseManagement.h \
    ULineEditWithButton/ULineEditWithClearButton.h \
    ULineEditWithButton/ULineEditWithButtons.h \
    UAddUrlDialog.h \
    USettings.h \
    sbglass.h \
    UPasswordWidget.h \
    USettingsPassword.h

FORMS    += UContainerExes.ui \
    UAboutProgram.ui \
    UAddUrlDialog.ui \
    USettings.ui \
    UPasswordWidget.ui \
    USettingsPassword.ui

OTHER_FILES += \
    app.rc

RC_FILE = app.rc

RESOURCES += \
    resource.qrc
