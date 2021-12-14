clc,clear all,close all

B = 25;                                                % bandwidth(MHz)
N = 150;                                               % Number of subcarriers (IFFT points)
alpha = 6;                                             % Scaling factor
win = [0.5, ones(1,N-2),0.5];                          % Window
TS_f = sign(rand(1,N)*2-1);                            % Training sequence (for channel measurement) in the frequency domain, BPSK
TS_f([1,N/2+1,end]) = 0;                               % Dorp DC, set GI
TS_t = round(ifft(ifftshift(TS_f)), 4)*alpha.*win;     % Training sequence in the time domain
% figure,plot(abs(TS_t));

ofdm_cw = [];
for i = 1:length(TS_t)
    ofdm_cw = [ofdm_cw, 'gr_complex(', num2str(real(TS_t(i))), ', ', num2str(imag(TS_t(i))), '), '];
end
ofdm_cw(end-1:end) = [];                               % Paste ofdm_cw to #global_vars.h#
