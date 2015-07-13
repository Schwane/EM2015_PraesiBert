APP_NAME = ListenerClientAppl

CONFIG += qt warn_on cascades10
LIBS += -lbb -lpps -lscreen -lEGL -lGLESv1_CM -lfreetype -lpng -lbbdevice -lbbsystem -lbbmultimedia -lbbutility
		+= -$$_PRO_FILE_PWD_/../../Common/OpenCV/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
 		
INCLUDEPATH += $$_PRO_FILE_PWD_/../../Common/OpenCV/include

include($$quote($$_PRO_FILE_PWD_)/../../Common/ClientServerShareLib/static.pri)

include(config.pri)
