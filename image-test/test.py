from PIL import Image, ImageChops
import sys, getopt
import numpy as np

# Expecting two image paths as arguments
if len(sys.argv) != 3:
    print("Usage: python test.py image1.png image2.png")
    sys.exit(1)


# Load the two images
img1 = Image.open(sys.argv[1])
img2 = Image.open(sys.argv[2])


# Compute the difference
diff = ImageChops.difference(img1, img2)

# Show the difference or save it
# diff.show()
# Convert difference image to numpy array
diff_array = np.array(diff)

# Count non-zero pixels (i.e., pixels that differ)
# If any of the R, G, or B values differ, that pixel counts as different
different_pixels = np.count_nonzero(diff_array.any(axis=2))

print(f"Differing pixels: {different_pixels}")
total_pixels = diff_array.shape[0] * diff_array.shape[1]
print(f"Total pixels: {total_pixels}")
print(f"Percentage different: {different_pixels / total_pixels * 100:.2f}%")
