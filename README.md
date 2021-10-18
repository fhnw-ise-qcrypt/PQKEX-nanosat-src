# PQKEX-nanosat-src

This repository contains the source code and the exchanged messages for the _"First demonstration of a post-quantum key-exchange with a nanosatellite"_ experiment.

### Ground Station (amd64)

The full source code for the ground station is publicly available here: [fhnw-ise-qcrypt/kyber-avr32](https://github.com/fhnw-ise-qcrypt/kyber-avr32/tree/gosh-commands-gnd).

### Satellite (avr32)

The source code for the satellite's operating system is subject to an NDA. The `src-sat` directory contains only the code written by us to implement the kyber algorithm on the satellite.

### Directories

- `keys` experiment messages and keys
- `src-gnd` ground station source code (ubuntu amd64) 
	+  `ref` 
- `src-sat` satellite source code (avr32)
	+ `lib` kyber source
	+ `ref` mission implementation


