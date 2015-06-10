APP_NAME = MasterClientAppl

CONFIG += qt warn_on cascades10
LIBS+= -lbb

include($$quote($$_PRO_FILE_PWD_)/../ClientLib/static.pri)
include($$quote($$_PRO_FILE_PWD_)/../../Common/NetworkLib/static.pri)

include(config.pri)
