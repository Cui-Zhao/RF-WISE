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
2) Set your PC's IP adress as 192.168.40.xx, open a ternimal and input `uhd_find_device` to ensure the connection;
3) Using gen_ofdm_cw.m to generate the customized CW, and paste it into "RF-WISE/gr-rf-wise/include/rfid/global_vars.h";
4) Open a ternimal in "RF-WISE/gr-rf-wise/build/" and input `cd ../;sudo rm -rf build;mkdir build;cd build;cmake ../;sudo make install;sudo ldconfig`;
5) Open a ternimal in "RF-WISE/gr-rf-wise/apps/" and input `sudo GR_SCHEDULER=STS nice -n -20 python ./reader.py`;
6) In "RF-WISE/gr-rf-wise/misc/code/" you can observe the transmitted and received baseband signals using plot_signal.m, while the raw data are stored in "RF-WISE/gr-rf-wise/misc/data/";
7) The sensing feature extraction is similar as that of Wi-Fi CSI measurement, which you can handle on your own, as well as the RFID signal preprocessing, e.g., segmenting ON/OFF feilds of tag's EPC responses.

## Notes
Some common issues may appear when implement RF-Wise, including but not limited to:
1) You may need to update the FPGA images for USRP X310, https://files.ettus.com/manual/page_images.html;
2) You may need to input `sudo sysctl -w net.core.wmem_max=xxxx` before step 5 in Guide, wherein the "xxxx" will be reported in the ternimal after step 5 in Guide ~lol~;
3) Issues about the environment configuration can be found at https://kb.ettus.com/Building_and_Installing_the_USRP_Open-Source_Toolchain_(UHD_and_GNU_Radio)_on_Linux;
4) Long-running operation may rise unknown fault, power off the USRP and wait for a few seconds may fix it;
5) To be continued...
