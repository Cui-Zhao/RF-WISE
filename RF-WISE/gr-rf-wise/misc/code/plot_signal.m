clc,clear all,close all

fi_1 = fopen('../data/trans','rb');
x_inter_1 = fread(fi_1, 'float32');
x_1 = x_inter_1(1:2:end) + 1i*x_inter_1(2:2:end);
figure,plot(abs(x_1));

fi_2 = fopen('../data/source','rb');
x_inter_2 = fread(fi_2, 'float32');
x_2 = x_inter_2(1:2:end) + 1i*x_inter_2(2:2:end);
figure,plot(abs(x_2));

