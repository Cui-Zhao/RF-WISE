## Motivation
RF-Wise is a general tool to benefit the RFID-based sensing application designs with the following advantages:
* A software-based design without using any extra device
* Compatible to EPC Gen2 within the ISM band
* Using only one tag for sensing

The applications include:
* Food and liquid sensing
* Contactless HCI
* Smart manufacturing
* ...

## Overview
RF-Wise contains three main components as shown in Figure 1.

<img src="pic/ov.jpg?raw=true" alt="ov.jpg" width="480">

Figure 1. The overview of the RF-Wise design.

1) Multi-carrier Frequency Spreading. Multiplexing frequencies by customizing the continuous wave to obtain multi-dimensional sensing samples from different frequencies.
2) Harnessing Hardware-constrained Wider-band. Harnessing more allowable bandwidth to enrich the sensing samples further, and enhance the timing resolution of sensing as well.
3) Sensing Feature Extraction. Extracting representative and effective features from the sensing samples obtained by RF-Wise, to be used by various applications.

## Observations
RF-Wise is designed based on the following observations:

1) The tag's backscattering is not sensitive to the waveform format of the continuous wave - even it is designed to be another type of sequences with enough energy, tags may still be activated and functioned normally, as shown in Figure 2. This observation inspires us to "customize" the continuous wave by employing the frequency multiplexing.
2) Moreover, improved by using more allowable bandwidth in RFID, e.g., 26 MHz in U.S., the sensing dimension can be increased up further, which thus fundamentally breaks the limit in the current RFID sensing.

<img src="pic/ob.jpg?raw=true" alt="ob.jpg" width="480">

Fig. 2. Tag's backscattering is not sensitive to the waveform format of continuous wave _s_.

## Sensing Example
We introduce two challenging settings intentionally to demonstrate the necessary to use the fine-grained sensing features in the liquid classification, as shown in Figure 3.

1) In the first setting, we start from the pure wine and add different volumes of water, e.g., 5 mL (2.5%), 10 mL (5%) and 15 mL (7.5%), and show the features obtained by RF-Wise. From Figure 3(a), we can see that with different volumes of water mixed into the wine, some sub-carriers exhibit distinct frequency responses, which provides an opportunity to distinguish them.

2) In the second setting, we open one bottle of fresh milk and collect its sensing features after one, two and four hours (in the environment with a temperature of 30∼35C). Figure 3(b) depicts that their features show differences across sub-carriers and we highlight the most evident ones by red circles. 

<img src="pic/f.jpg?raw=true" alt="f.jpg" width="480">

Fig. 3. Sensing features of RF-Wise for (a) the wine by adding different volumes of water and (b) the milk after it is opened for one to four hours.

## Publication
Cui Zhao, Zhenjiang Li, Han Ding, Ge Wang, Wei Xi, Jizhong Zhao, "RF-Wise: Pushing the Limit of RFID-based Sensing", in Proceedings of IEEE INFOCOM, 2022.
