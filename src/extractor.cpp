//
//  main.cpp
//  extractor
//
//  Created by riovcharenko on 04.02.14.
//  Copyright (c) 2014 riovcharenko. All rights reserved.
//
#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/legacy/legacy.hpp>

using namespace cv;

Mat extractFeatures(const char* filename, const char* method, int mark)
{
    Mat img = imread(filename);
    int height = img.size().height, width = img.size().width;
    // 800 x 600
    if(width > 800)
    {
        height = int(800.0/width*height);
        width = 800;
    }
    if(height > 600)
    {
        width = int(600.0/height*width);
        height = 600;
    }
    resize(img, img, cvSize(width, height));
    
    Mat grayImg;
    Mat hueImg;
    
    Mat tmpImg;
    std::vector<Mat> planes;
    cvtColor(img,tmpImg,CV_BGR2HLS);
    printf("HLS img size: %ld x %ld\n", tmpImg.size().width, tmpImg.size().height);
	split(tmpImg,planes);
    hueImg = planes[0];
    grayImg = planes[1];
    printf("HUE img size: %ld x %ld\n", hueImg.size().width, hueImg.size().height);
    printf("Gray img size: %ld x %ld\n", grayImg.size().width, grayImg.size().height);	
//    tmpImg.release();
    
    Ptr<FeatureDetector> detector = FeatureDetector::create(method);
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(method);
    
    std::vector<KeyPoint> keypoints;
    
    detector->detect(grayImg, keypoints);
    
    printf("Keypoints detected: %d\n", keypoints.size());
    Mat descriptors, descriptors_gs, descriptors_hue;
    extractor->compute(grayImg, keypoints, descriptors_gs);
    extractor->compute(hueImg, keypoints, descriptors_hue);
	printf("GS descriptors size: %ld x %ld\n", descriptors_gs.size().width, descriptors_gs.size().height);
	printf("HUE descriptors size: %ld x %ld\n", descriptors_hue.size().width, descriptors_hue.size().height);
    hconcat(descriptors_gs, descriptors_hue, descriptors);
    
    Mat I = Mat(descriptors.rows, 1, descriptors.type(), mark);
    hconcat(I, descriptors, descriptors);
    return descriptors;
}

inline void usage()
{
    std::cout << "usage:\textractor <method> <out_folder> <image_path> [<image_path> ...]" << std::endl;
    std::cout << "\tmethod - method to extract features: SIFT or SURF" << std::endl;
    std::cout << "\tout_folder - output folder" << std::endl;
    std::cout << "\timage_path - full path to the image to use" << std::endl << std::endl;
    std::cout << "\tIMPORTANT NOTICE: All input images are considered to be sorted," << std::endl;
    std::cout << "\tso that first half are examples of 1st class, the rest are 2nd class " << std::endl;
}

int main(int argc, const char * argv[])
{
    std::vector<Mat> descriptorsS;
    if(argc < 4)
    {
        usage();
        return 0;
    }
    auto method = argv[1];
    auto outFolder = argv[2];
    int numImages = argc - 3;
    std::cout << "Will produce " << method << " desrciptors." << std::endl;
    std::cout << "Processing " << numImages << " images..." << std::endl;
    for(int i = 1; i <= numImages; ++i)
    {
        int k = (i<=numImages/2)?1:2;
        std::cout << argv[i+2] << " class " << k << std::endl;
        descriptorsS.push_back(extractFeatures(argv[i+2], method, k));
    }
    Mat resultS;
    vconcat(descriptorsS, resultS);
    
    CvMat save = resultS;
    if(resultS.dims >= 2)
    {
        char filename[256];
        sprintf(filename, "%s/descriptors%s.xml",outFolder,method);
        cvSave(filename, &save);
        std::cout << method << " descriptors saved." << std::endl;
    }
    return 0;
}
