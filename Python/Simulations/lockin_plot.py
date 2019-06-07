import numpy as np
import matplotlib.pyplot as plt

%matplotlib qt5

snr = np.int16(np.genfromtxt('C:/Users/senag/Documents/Project_SRON/Python/Simulations/snr.out', delimiter='\n', unpack=True, dtype=float))

plt.plot(snr, 'b-')
plt.grid()
