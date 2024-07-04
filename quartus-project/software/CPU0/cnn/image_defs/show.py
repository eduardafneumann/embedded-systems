import numpy as np
import matplotlib.pyplot as plt
from glob import glob

for filename in glob("images/txt/img*_*.txt"):
    new_filename = filename.replace("images/txt/", "images/jpg/").replace(".txt", ".jpg")
    with open(filename) as f:
        text = f.read()
        data = np.array([list(map(float, line.split())) for line in text.split("\n") if line])
        plt.imshow(data, 'gray')
        plt.savefig(new_filename)
        plt.close()