all: install start

install:
	cd PRU0 && $(MAKE)
	cd PRU1 && $(MAKE)
	cd PRU0 && $(MAKE) upload
	cd PRU1 && $(MAKE) upload

stop:
	@cd PRU0 && $(MAKE) stop || echo '- Cannot stop PRU0'
	@cd PRU1 && $(MAKE) stop ||	echo '- Cannot stop PRU1'

start:
	@cd PRU0 && $(MAKE) start || echo '- Cannot start PRU0'
	@cd PRU1 && $(MAKE) start || echo '- Cannot start PRU1'
