APP_NAME = ClientServerShareLib

CONFIG += qt warn_on

# uncomment for building static library
CONFIG += staticlib

# uncomment for generating separate debug info
# CONFIG += separate_debug_info

LIBS += -L$$_PRO_FILE_PWD_/../../Common/OpenCV/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lcamapi -lcamerafeatures

INCLUDEPATH += $$_PRO_FILE_PWD_/../../Common/OpenCV/include

include(cascades10_library.pri)

include(config.pri)

# specify a list of .ts files for supported languages
# TRANSLATIONS += $${TARGET}_en_US.ts

# uncomment following line to include translations to binary
# RESOURCES += translations/qm/ClientServerShareLib_translations.qrc