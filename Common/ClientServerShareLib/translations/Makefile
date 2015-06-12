QMAKE_TARGET  = ClientServerShareLib
LUPDATE       = $(QNX_HOST)/usr/bin/lupdate
LRELEASE      = $(QNX_HOST)/usr/bin/lrelease

update: ../$(QMAKE_TARGET).pro FORCE
	$(LUPDATE) $(QMAKE_TARGET).pro

release: ../$(QMAKE_TARGET).pro $(QMAKE_TARGET).ts
	@rm -rf qm
	mkdir -p qm
	$(LRELEASE) $(QMAKE_TARGET).pro
	mv *.qm qm/
	
	cd qm && rcc -project -o ../ClientServerShareLib_translations.qrc
	mv ClientServerShareLib_translations.qrc qm/

FORCE:
