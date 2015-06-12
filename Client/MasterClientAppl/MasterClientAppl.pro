APP_NAME = MasterClientAppl

CONFIG += qt warn_on cascades10
LIBS+= -lbb


include($$quote($$_PRO_FILE_PWD_)/../../Common/ClientServerShareLib/static.pri)

include(config.pri)
