import numpy as np


def calc_coef(Fc, LH, PR, NP, p):
    RP = -np.cos(np.pi / (NP * 2) + (p - 1) * np.pi / NP)
    IP = -np.sin(np.pi / (NP * 2) + (p - 1) * np.pi / NP)


A = np.zeros(22)
B = np.zeros(22)
TA = np.zeros(22)
TB = np.zeros(22)

A[2] = 1
B[2] = 1

Fc = 0.5
LH = 0
PR = 0
NP = 4

for p = 1 in range(NP / 2):
