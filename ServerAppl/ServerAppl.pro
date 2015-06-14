APP_NAME = ServerAppl

CONFIG += qt warn_on cascades10
 LIBS += -lbb
 
include(config.pri)
include($$quote($$_PRO_FILE_PWD_)/../Common/ClientServerShareLib/static.pri)