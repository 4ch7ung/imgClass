#imgClass
##Summary

This project is a set of tools that allows one to build and perform experiments with binary image classification using Bag-of-Features approach with OpenCV. The specialty of used method is that feature extraction performed as follows:

1. The image is converted in HSV color space.
2. The Value component of the image is used to detect features.
3. The Hue component of the image is used to compute descriptors.

##Requirements

* cmake >= v2.8
* OpenCV >= v2.4.8
* bash

##Build

Build project using **bash** as follows
```Bash
git clone https://github.com/4ch7ung/imgClass.git
cd imgClass
cmake .
make
```

After succesful build four new components will be added to **imgClass/bin**
* extractor
* cluterize
* classify
* svmify
