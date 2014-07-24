//
//  main.cpp
//  clusterize
//
//  Created by riovcharenko on 30.03.14.
//  Copyright (c) 2014 riovcharenko. All rights reserved.
//

#include <iostream>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace cv;

inline void usage()
{
    std::cout << "usage:\tclusterize <method> <in_folder> <out_folder>" << std::endl;
    std::cout << "\tmethod - method of extracted features: 1 - SIFT, 2 - SURF" << std::endl;
    std::cout << "\tin_folder - path to input folder" << std::endl;
    std::cout << "\tout_folder - path to output folder" << std::endl;
}

int main(int argc, const char * argv[])
{
    if(argc < 4)
    {
        usage();
        return 0;
    }
    char method = argv[1][0];
    auto inFolder = argv[2];
    auto outFolder = argv[3];
    std::vector< Mat > labelVectorS;
    std::vector< Mat > clusterVectorS;
    
    Mat dataFileS;
    char filename[256];
    sprintf(filename, "%s/descriptors%s.xml", inFolder, (method == '1') ? "SIFT" : "SURF");
    FileStorage fs(filename, cv::FileStorage::READ);
    FileNode fn = fs.getFirstTopLevelNode();
    fn >> dataFileS;
    Mat descriptorsS = dataFileS.colRange(1, dataFileS.cols);
    dataFileS.release();
    std::cout << descriptorsS.rows << ((method == '1') ? " SIFT" : " SURF") << " descriptors read" << std::endl;
    
    std::vector< Mat > resultVectorS;
    for(int k = 200; k <= 400; k+=50)
    {
        Mat clusters, labels;
        TermCriteria termcrit;
        if(method == '1') termcrit = TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0);
        else termcrit = TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.001);
        kmeans(descriptorsS, k, labels, termcrit, 3, KMEANS_PP_CENTERS, clusters);
        labelVectorS.push_back(labels);
        clusterVectorS.push_back(clusters);
        std::cout << "Clustering " << ((method=='1')?"SIFT":"SURF") << " descriptors ended for k = " << k << std::endl;
    }

    Mat labelsS;
    hconcat(labelVectorS, labelsS);

    CvMat save = labelsS;
    if(labelsS.dims >= 2)
    {
        char filename[256];
        sprintf(filename, "%s/clusters200-400%s.xml", outFolder, (method == '1')?"SIFT":"SURF");
        cvSave(filename, &save);
        std::cout << "Cluster data (" << ((method == '1')?"SIFT":"SURF") << ") saved." << std::endl;
    }
    labelsS.release();
    
    int classcount[5];
    for(int i = 200, k = 0; i <= 400; i+=50) classcount[k++] = i;
    for(int i = 0; i < clusterVectorS.size(); ++i)
    {
        char filename[256];
        sprintf(filename, "%s/clusterCenters%s-%d.xml", outFolder,(method=='1')?"SIFT":"SURF",classcount[i]);
        CvMat save = clusterVectorS[i];
        if(clusterVectorS[i].dims >= 2)
        {
            cvSave(filename, &save);
            std::cout << ((method=='1')?"SIFT":"SURF") << "-based cluster centers saved for " << classcount[i] << " clusters" << std::endl;
        }
        
    }
    return 0;
}

