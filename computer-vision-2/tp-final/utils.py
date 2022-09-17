import os
import cv2


def load_image(path, filename, grayscale=False):
    flags = {"flags": cv2.IMREAD_GRAYSCALE} if grayscale else {}
    img = cv2.imread(os.path.join(path, filename), **flags)
    if grayscale:
        return img
    return cv2.cvtColor(img, cv2.COLOR_BGR2RGB)


def get_filename_generator():
    counter = 1
    while True:
        yield str(counter).zfill(5) + ".jpg"
        counter += 1
