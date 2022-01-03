## Motivation

For RFID-based sensing, great efforts have been made to increase the sensing sample’s diversity by using frequency hopping or tag array, which however are not effective due to the inherently large latency between two sample collections. Some recent works propose to utilize an extra device to transmit additional wide-band signals to improve the sensing fidelity. The cost (from the high-end extra device) and overhead (to synchronize this extra signal with the RFID signal) are still large. **We thus wonder naturally whether we can improve RFID sensing without such extra costs and overhead?** 

To the best of our knowledge, RF-WISE is the first research to bring a positive answer!

## Observations

RF-Wise is designed based on the following observations:

1) The RFID signal contains a continuous wave to power tag’s backscattering, which is configured with a constant amplitude value usually. We observe that the tag’s backscattering is not sensitive to the waveform format of the continuous wave — even it is designed to be another type of sequences with enough energy, tags may still be activated and functioned normally. This observation inspires us to “customize” the continuous wave by employing the frequency multiplexing, so
that we can collect multiple sensing samples over frequencies concurrently from each query (no frequency hopping). 
2) Moreover, improved by using more allowable bandwidth in RFID, e.g., 26 MHz in U.S., the sensing dimension can be increased up further, which thus fundamentally breaks the limit in current RFID sensing.

## Overview

RF-Wise contains three main components, including 1) _Multi-carrier Frequency Spreading_, 2) _Harnessing Hardware-constrained Wider-band_ and 3) _Sensing Feature Extraction_.

1) Multi-carrier Frequency Spreading. Multiplexing frequencies by customizing continuous wave to obtain multi-dimensional sensing samples from different frequencies.
2) Harnessing more allowable bandwidth to enrich the sensing samples further, and enhance the timing resolution of sensing as well, after a set of the hardware related issues are addressed.
3) Extracting representative and effective features from the sensing samples obtained by RF-Wise, to be used by various applications.



## Implementation
1) Hardware. RF-Wise is developed using one USRP X310 with one SBX-40 daughterboard. Two directional antennas (Laird S9028PCR with the gain of 8 dBi) are installed to transmit and receive signals to and from the Alien 9640 RFID tag, respectively. USRP is connected to a desktop of an Intel Core i9-9900K CPU, 32 GB RAM, Intel Converged Network Adapter X520-DA1 and 10 Gigabit Ethernet Cable for a high bit-rate sensing data collection.
2) Software development. The back-end of RF-Wise runs on the EPC Gen2 protocol using GNU Radio 3.7 and UHD 3.15 on Ubuntu 18.04.


[Link](url) and ![Image](src)
```

For more details see [Basic writing and formatting syntax](https://docs.github.com/en/github/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax).

