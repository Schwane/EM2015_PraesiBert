APP_NAME = ServerAppl

CONFIG += qt warn_on cascades10
 LIBS += -lbb
      += -$$_PRO_FILE_PWD_/../Common/OpenCV/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
 		
INCLUDEPATH += $$_PRO_FILE_PWD_/../Common/OpenCV/include
 
include(config.pri)
include($$quote($$_PRO_FILE_PWD_)/../Common/ClientServerShareLib/static.pri)