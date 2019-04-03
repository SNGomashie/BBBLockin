import numpy as np
import matplotlib.pyplot as plt

y = np.genfromtxt('Sim/data.txt', delimiter='\n', unpack=True, dtype=int)
print(y)
plt.plot(range(1023), y, label='Loaded from file!')

plt.plot(range(1023), Y)
plt.show()
