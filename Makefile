
compile:
	pio run

check:
	pio check

upload:
	pio run -t upload

ide:
	rm .ccls
	pio init --ide emacs
