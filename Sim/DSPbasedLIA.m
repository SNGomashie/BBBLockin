% Robert Skillington Project DSP based Lock-in Amplifier 2013
 % LIA algorithm using Square OR sine waveform for Reference signal
  clc; clear all; close all;%
  %%
   tic
   fs = 80000;                   % sampling frequency (samples/second) >4000
   Ar = 1;                       % Reference Amplitude
   Ai = 1.8;                     % Input Amplitude
   n = [0:round(0.1*fs)-1]'/fs;  % Time axis
   fo = 2000;                    % Frequency of sinusoid
   %% Experement signal
   VsigCos = Ai*cos((n*2*pi*fo)-(pi/10)) ;%   % V signal Sine waveform equation to add the phase difference through the experiment
   %% Cosine Reference signals
   % Square Reference signals
   VrefCos = Ar*cos(n*2*pi*fo); % Ar*square(n*2*pi*fo);
    % V reference Sine waveform equation   VrefSin = Ar*cos((n*2*pi*fo)-(pi/2));% Ar*square((n*2*pi*fo)+(pi/2));  % V reference Cosine waveform equation      %% Vs and Vc (VsigSinandNoise x V reference Sine) and (VsigSinandNoise x V reference Cosine)   Vs = VsigCos.*VrefSin; % Vs is the multiplication of noisy input signal and Sine reference Vc = VsigCos.*VrefCos; % Vs is the multiplication of noisy input signal and Cosine reference   %% Filter for Vs and Vc to form I and Q (X and Y)   Ivs = mean(Vs); % mean of  Qvc = mean(Vc); % mean of      %% Finds the magnitude of signal   Ao = 2*sqrt((Ivs^2)+(Qvc^2));   % Ao = (pi/4)*2*sqrt((Ivs^2)+(Qvc^2));% for square wave %LIA output Amplitude    fprintf('Ai: %1.10f \n',Ai) fprintf('Ao: %1.10f \n',Ao)   %% Finds the Phase of signal   Phase = atan2(Ivs,Qvc); % atan(Qvc./Ivs);   % Phase difference from signal and reference   fprintf('Phase: %1.5g radians \n',Phase); 
