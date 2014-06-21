//
//  main.cpp
//  svmify
//
//  Created by mac on 30.03.14.
//  Copyright (c) 2014 riovcharenko. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

using namespace cv;

const int clusters = 20;
const int steps = 9;

void usage()
{
    std::cout << "usage:\tsvmify <method> <inFolderLearn> <inFolderTest> [<outFilePrec> <outFileRec> <outFileFScore>]" << std::endl;
}

void printResults(float matrix[steps][clusters], const char* header, const char* resultParameter, const char* valParameter, float param[steps])
{
    std::cout << header << " results " << resultParameter << std::endl;
    
    std::cout << std::setw(5) << valParameter << ":";
    for(int i = 0; i < steps; ++i)
        std::cout << std::setw(6) << param[i];
    std::cout << std::endl;
    for(int i = 0; i < clusters; ++i)
    {
        std::cout << std::setw(5) << (i+1)*50 << ":";
        for(int j = 0; j < steps; ++j)
            std::cout << std::setw(6) << std::setprecision(3) << matrix[j][i]*100;
        std::cout << std::endl;
    }
}

int main(int argc, const char * argv[])
{
    if(argc < 4)
    {
        usage();
        return 0;
    }
    int method = argv[1][0] - '0';
    auto inFolderLearn = argv[2];
    auto inFolderTest = argv[3];
    bool outputToFile = (argc == 7);
    CvSVM svm;
    float precision[steps][clusters];
    float recall[steps][clusters];
    float fscore[steps][clusters];
    float param[steps];
    int totalSteps = steps*clusters;
    for(int step = 1; step <= steps; ++step)
    {
        param[step-1] = 0.1*step;
        for(int k = 50; k <= 1000; k+=50)
        {
            Mat dataFile;
            char filename[256];

            sprintf(filename, "%s/hystogram%s-%d.xml", inFolderLearn, (method == 1)?"SIFT":"SURF", k);
            FileStorage fs(filename, cv::FileStorage::READ);
            FileNode fn = fs.getFirstTopLevelNode();
            fn >> dataFile;
            
            Mat histLearn;
            dataFile.colRange(1, dataFile.cols).convertTo(histLearn, DataType<float>::type);
            Mat histLabelsLearn;
            dataFile.col(0).convertTo(histLabelsLearn, DataType<float>::type);
            dataFile.release();
            
            Mat varID, sampleID;
            CvSVMParams params = CvSVMParams();
            params.svm_type = CvSVM::NU_SVC;
            params.kernel_type = CvSVM::POLY;
            params.degree = 3;
            params.gamma = 1;
            params.coef0 = 1;
            params.nu = param[step-1];
            
            int kfold = 10;
            svm.train_auto(histLearn, histLabelsLearn, varID, sampleID, params, kfold);
        
            sprintf(filename, "%s/hystogram%s-%d.xml", inFolderTest, (method == 1)?"SIFT":"SURF", k);
            FileStorage fs2(filename, cv::FileStorage::READ);
            FileNode fn2 = fs2.getFirstTopLevelNode();
            fn2 >> dataFile;
            
            Mat histTest;
            dataFile.colRange(1, dataFile.cols).convertTo(histTest, DataType<float>::type);
            Mat histLabelsTest;
            dataFile.col(0).convertTo(histLabelsTest, DataType<float>::type);
            dataFile.release();
            
            Mat svmResults;
            svm.predict(histTest, svmResults);
            float truePositive = 0;
            float falsePositive = 0;
            float trueNegative = 0;
            float falseNegative = 0;
            int totalTest = svmResults.rows;
            for(int i = 0; i < totalTest; ++i)
            {
                if(svmResults.at<float>(0,i) == 1 && histLabelsTest.at<float>(0,i) == 1) truePositive++;
                if(svmResults.at<float>(0,i) == 1 && histLabelsTest.at<float>(0,i) == 2) falsePositive++;
                if(svmResults.at<float>(0,i) == 2 && histLabelsTest.at<float>(0,i) == 1) falseNegative++;
                if(svmResults.at<float>(0,i) == 2 && histLabelsTest.at<float>(0,i) == 2) trueNegative++;
            }
            
            svm.clear();
            int clusterstep = k/50 - 1;
            precision[step-1][clusterstep] = truePositive / (truePositive + falsePositive);
            recall[step-1][clusterstep] = truePositive / (truePositive + falseNegative);
            fscore[step-1][clusterstep] = 2 * precision[step-1][clusterstep] * recall[step-1][clusterstep] /
                                            (precision[step-1][clusterstep] + recall[step-1][clusterstep]);
            float done = float((step-1)*clusters + k/50-1)/totalSteps;
            std::cout << "Done " << done*100 << "%" << std::endl;
        }
    }
    std::cout << "Done 100%" << std::endl;

    if(outputToFile)
    {
        auto outFilePrec = argv[4];
        freopen(outFilePrec,"w",stdout);
    }
    printResults(precision, ((method == 1) ? "SIFT" : "SURF"), "Precision", "NU", param);
    
    if(outputToFile)
    {
        auto outFileRec = argv[5];
        freopen(outFileRec,"w",stdout);
    }
    printResults(recall, ((method == 1) ? "SIFT" : "SURF"), "Recall", "NU", param);
    
    if(outputToFile)
    {
        auto outFileFscore = argv[6];
        freopen(outFileFscore,"w",stdout);
    }
    printResults(fscore, ((method == 1) ? "SIFT" : "SURF"), "FScore", "NU", param);
    return 0;
}

