clc
clear all
close all

nfft = 32



;  % amount of subcarriers  
p = 1; % energy
%------------------2M  dac_rate bit_1_high=50 samples 40kHz_BLF FM0 _code
%------------------4M  dac_rate bit_1_high=100 samples
%------------------5M  dac_rate bit_1_high=125 samples
%------------------8M  dac_rate bit_1_high=200 samples
%------------------10M dac_rate bit_1_high=250 samples
%------------------20M dac_rate bit_1_high=500 samples
DC = nfft/2+1;  % nfft is the even number  
KnwSeq = randi([0,1],nfft,1); 
n = find(KnwSeq==0); % BPSK
KnwSeq(n) = -1;
KnwSeq(DC) = 0; % abandon DC carrier

knwSeq_ifft = ifft(ifftshift(KnwSeq));
figure,plot(abs(knwSeq_ifft));
cw_ofdm = knwSeq_ifft* p;
cw_ofdm1 = knwSeq_ifft*0.5 +1;
cw_ofdm2 = knwSeq_ifft*0.25 + complex(sqrt(2)/2,(sqrt(2)/2));
hold on,plot(abs(cw_ofdm1));
hold on,plot(abs(cw_ofdm2));
x = fftshift(fft(cw_ofdm));
x1 = fftshift(fft(cw_ofdm1));
x2 = fftshift(fft(cw_ofdm2));

% save 'Data/cw_ofdm_for_10M.mat' cw_ofdm;


