# RF-WISE
RF-Wise is the first work to obtain fine-grained CSI-like sensing samples purely from RFID signals to advance RFID sensing. It is a software solution atop standard RFID without using any extra device, compatible to EPC Gen2 within ISM band, requires one tag for sensing and is generic for various applications.

If our codes facilitate your research on RFID sensing, please cite **C. Zhao, Z. Li, H. Ding, G. Wang, W. Xi, and J. Zhao, "RF-Wise: Pushing the Limit of RFID-based Sensing", in Proc. of IEEE INFOCOM, 2022.** 

## Hardware
RF-Wise is developed using:
1) USRP X310;
2) SBX Daughterboard;
3) Intel Converged Network Adapter X520-DA1;
4) 10 Gigabit Ethernet Cable & 10G SFP+ Transceiver;
5) Laird S9028PCR Antenna (x2);
6) Commodity RFID tags.

## Software
The back-end of RF-Wise runs on the EPC Gen2 protocol using:
1) GNU Radio 3.7;
2) UHD 3.15;
3) Ubuntu 18.04.

## Guide
The following guide assumes that the environment has been configured, especially the GNU Radio.
1) Download our codes and connect the devices;
2) Set your PC's IP adress as 192.168.40.xx, open a ternimal and input `uhd_find_device`
