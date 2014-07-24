//
//  main.cpp
//  classify
//
//  Created by riovcharenko on 07.03.14.
//  Copyright (c) 2014 riovcharenko. All rights reserved.
//

#include <iostream>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;

Mat extractFeatures(const Mat& image, const char* method)
{
    Mat img = image.clone();
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
    cv::cvtColor(img, tmpImg, CV_BGR2HLS);
    split(tmpImg,planes);
    hueImg = planes[0];
    grayImg = planes[2];
    tmpImg.release();
    
    Ptr<FeatureDetector> detector = FeatureDetector::create(method);
    Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create(method);
    
    std::vector<KeyPoint> keypoints;
    detector->detect(grayImg, keypoints);
    Mat descriptors, descriptors_gs, descriptors_hue;
    extractor->compute(hueImg, keypoints, descriptors_hue);
    extractor->compute(grayImg, keypoints, descriptors_gs);
    hconcat(descriptors, descriptors_gs, descriptors_hue)
    
    return descriptors;
}

std::vector< Mat > extractHystogram(const Mat& image, CvKNearest* classifiers, size_t size, int* classcount, const char* method, int k)
{
    Mat desc = extractFeatures(image, method);
    std::vector< Mat > result;
    result.resize(size);
    for(int i = 0; i < size; ++i)
    {
        Mat hysto = Mat(1, classcount[i]+1, DataType<int>::type, Scalar_<int>(0));
        hysto.at<int>(0,0) = k;
        for(int j = 0; j < desc.rows; ++j)
        {
            int response = classifiers[i].find_nearest(desc.row(j), 10);
            hysto.at<int>(0,int(response)+1)++;
        }
        std::cout << hysto.cols << "-size " << method << " hystogram extracted" << std::endl;
        result[i] = hysto.clone();
    }
    return result;
}

void mergeHysto(std::vector< Mat > &a, std::vector< Mat > &b)
{
    for(int i = 0; i < a.size(); ++i)
    {
        Mat X = a[i].clone();
        vconcat(X, b[i], a[i]);
    }
}

inline void usage()
{
    std::cout << "usage: classify <method> <in_folder> <out_folder> <image_path> [<image_path> ...]" << std::endl;
    std::cout << "\tmethod - type of extractor to be used: 1 - SIFT, 2 - SURF" << std::endl;
    std::cout << "\tin_folder - path to input folder" << std::endl;
    std::cout << "\tout_folder - path to output folder" << std::endl;
    std::cout << "\timage_path - full path to the input image" << std::endl;
}

int main(int argc, const char * argv[])
{
    if(argc < 3)
    {
        usage();
        return 0;
    }
    int method = argv[1][0] - '0';
    auto inFolder = argv[2];
    auto outFolder = argv[3];
    
    std::vector< Mat > labelsS;
    Mat descriptorsS;
    
    int numImages = argc - 4;
    std::cout << "Processing " << numImages << " images" << std::endl;

    Mat dataFileS;
    char filename[256];
    sprintf(filename, "%s/descriptors%s.xml", inFolder, (method == 1)?"SIFT":"SURF");
    FileStorage fs(filename, FileStorage::READ);
    FileNode fn = fs.getFirstTopLevelNode();
    fn >> dataFileS;
    descriptorsS = dataFileS.colRange(1, dataFileS.cols);
    Mat descriptorClassesSIFT = dataFileS.col(0);
    dataFileS.release();
    std::cout << descriptorsS.rows << ((method == 1)?" SIFT":" SURF") << " descriptors read" << std::endl;

    Mat clastersS;
    char filename2[256];
    sprintf(filename2, "%s/clusters200-400%s.xml", inFolder, (method==1)?"SIFT":"SURF");
    FileStorage fsc(filename2, FileStorage::READ);
    FileNode fnc = fsc.getFirstTopLevelNode();
    fnc >> clastersS;
    std::cout << ((method == 1)?"SIFT":"SURF") << " cluster data read" << std::endl;
    for(int i = 0; i < clastersS.cols; ++i)
        labelsS.push_back(clastersS.col(i));
    
    size_t lssize = labelsS.size();
    CvKNearest* classifiersS = new CvKNearest[lssize];
    
    for(int i = 0; i < labelsS.size(); ++i)
    {
        classifiersS[i].train(descriptorsS, labelsS[i]);
    }
    std::cout << ((method == 1)?"SIFT":"SURF") << " kNearest classifiers trained" << std::endl;

    int classcount[5];
    for(int i = 200, k = 0; i <= 400; i+=50) classcount[k++] = i;
    std::vector< Mat > hystogramsS;
    hystogramsS.resize(lssize);
    for(int i = 1; i <= numImages; ++i)
    {
        std::vector< Mat > tmpS;
        tmpS.resize(lssize);
        auto currentImage = argv[i+3];
        int k = (i<=numImages/2)?1:2;
        std::cout << currentImage << " class " << k << std::endl;
        Mat img = imread(currentImage);
        tmpS = extractHystogram(img, classifiersS, lssize, classcount, (method==1)?"SIFT":"SURF", k);
        if( hystogramsS[0].empty() )
            hystogramsS = tmpS;
        else mergeHysto(hystogramsS, tmpS);
    }
    std::cout << hystogramsS[0] << std::endl;
    for(int i = 0; i < hystogramsS.size(); ++i)
    {
        char filename[256];
        sprintf(filename, "%s/hystogram%s-%d.xml", outFolder, (method==1)?"SIFT":"SURF", classcount[i]);
        CvMat save = hystogramsS[i];
        if(hystogramsS[i].dims >= 2)
        {
            cvSave(filename, &save);
            std::cout << ((method==1)?"SIFT":"SURF") << "-based hystograms saved for " << classcount[i] << " clusters" << std::endl;
        }
    }

    return 0;
}

