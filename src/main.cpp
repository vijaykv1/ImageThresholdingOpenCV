#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Timer.h"

////////////////////////////////////////////////////////////////////////////////////
// compute threshold image using the OpenCV function - only for reference
////////////////////////////////////////////////////////////////////////////////////
void threshold_cv(const cv::Mat &input, cv::Mat &output, uchar threshold)
{
    cv::threshold(input, output, 128, 255, cv::THRESH_BINARY);
}

////////////////////////////////////////////////////////////////////////////////////
// compute threshold image by looping over the elements
////////////////////////////////////////////////////////////////////////////////////
void threshold_loop(const cv::Mat &input, cv::Mat &output, uchar threshold)
{
    int rows = input.rows;
    int cols = input.cols;
    
    // cv::Mat for result
    output.create(rows, cols, CV_8U);
    
    // checking for continuity for the image is not necessary here as the rastering is value based and not pointer based.
    for (int i = 0; i < rows ; ++i )
    {
        for (int j = 0; j < cols ; ++j)
        {
            if (input.at<uchar>(i,j) < threshold)
            {
                output.at<uchar>(i,j) = 0 ; // image pixel pushed to black zone
            }
            else
            {
                output.at<uchar>(i,j) = 255; // image pixel pushed to white zone
            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////////////
// compute threshold image by looping over the elements (pointer with index access)
////////////////////////////////////////////////////////////////////////////////////
void threshold_loop_ptr(const cv::Mat &input, cv::Mat &output, uchar threshold)
{
    int rows = input.rows;
    int cols = input.cols;
    
    // cv::Mat for result
    output.create(rows, cols, CV_8U);
    
    if (input.isContinuous())
    {
        // continuous ? .. Then only proceed ... else send back a failure code as we expect a continously populated matrix.
        cols = rows * cols;
        rows = 1;
    } // defaults set here before rastor start ! 

    for (int r = 0 ; r< rows ; ++r )
    {
        // row wise pointer setting 
        const uchar *pRow = input.ptr<uchar>(r); // typecasting performed ...
        uchar *pRowOut = output.ptr<uchar>(r);
        for (int c = 0 ; c< cols; ++c )
        {
            
            if (pRow[c] < threshold) 
            {
                pRowOut[c] = 0;
            }
            else 
            {
                pRowOut[c] = 255;
            }

        }
    }
    
}

////////////////////////////////////////////////////////////////////////////////////
// compute threshold image by looping over the elements (pointer arithmetic access)
////////////////////////////////////////////////////////////////////////////////////
void threshold_loop_ptr2(const cv::Mat &input, cv::Mat &output, uchar threshold)
{
    int rows = input.rows;
    int cols = input.cols;
    
    output.create(rows, cols, CV_8U);
    
    if (input.isContinuous()){
        cols = rows * cols;
        rows = 1;
    } // push defaults to statefix methodology ...

    for (int r = 0; r<rows ; ++r)
    {
        const uchar *pRow = input.ptr<uchar>(r);
        uchar *pRowOut = output.ptr<uchar>(r);
        for(int c = 0; c < cols; ++c )
        {
            if (*pRow < threshold)
            {
                *pRowOut = 0;
            }
            else
            {
                *pRowOut = 255;
            }
            ++pRow ; // pointer itself is incremented ... Dirty Hack , not recommended .. but fastest rastor.
            ++pRowOut;
        }
    } 

}

int main(int argc, char *argv[])
{
    //read image
    cv::Mat img = cv::imread("../data/lena.tiff");
    //display image
    cv::imshow("Image", img);
    
    //convert to grayscale
    cv::Mat imgGray;
    cv::cvtColor(img, imgGray, CV_BGR2GRAY);
    //display grayscale
    cv::imshow("Grayscale", imgGray);

    //declare output variables
    cv::Mat output,output2,output3,output4;

    // begin processing ///////////////////////////////////////////////////////////

    INIT_TIMER
    
    START_TIMER
    threshold_cv(imgGray, output, 50);
    STOP_TIMER("threshold_cv")
    cv::imshow("threshold_cv_50", output);

    
    START_TIMER
    threshold_loop(imgGray, output2, 200);
    STOP_TIMER("threshold_loop")
    cv::imshow("threshold_loop_200", output2);

    
    START_TIMER
    threshold_loop_ptr(imgGray, output3, 100);
    STOP_TIMER("threshold_loop_ptr")
    cv::imshow("threshold_loop_ptr_100", output3);
    
    
    START_TIMER
    threshold_loop_ptr2(imgGray, output4, 120);
    STOP_TIMER("threshold_loop_ptr2")
    cv::imshow("threshold_loop_ptr2_120", output4);
    
    // end processing /////////////////////////////////////////////////////////////
    
    //wait for key pressed
    cv::waitKey();
  
    return 0;
}
