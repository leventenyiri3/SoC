from PIL import Image, ImageChops
import sys, getopt
import numpy as np

if len(sys.argv) != 3:
    print("Usage: python test.py image1.png image2.png")
    sys.exit(1)

img1 = Image.open(sys.argv[1])
img2 = Image.open(sys.argv[2])

diff = ImageChops.difference(img1, img2)

diff_array = np.array(diff)

different_pixels = np.count_nonzero(diff_array.any(axis=2))

print(f"Differing pixels: {different_pixels}")
total_pixels = diff_array.shape[0] * diff_array.shape[1]
print(f"Total pixels: {total_pixels}")
print(f"Percentage different: {different_pixels / total_pixels * 100:.2f}%")
