#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;
using namespace dnn;

int main()
{
    VideoCapture cap("C:/Users/Vlada/Desktop/DNN/x64/Release/samp.MOV");
    std::string model = "C:/Users/Vlada/Desktop/DNN/x64/Release/yolov3-tiny.weights";  //findFile(parser.get<String>("model"));
    std::string config = "C:/Users/Vlada/Desktop/DNN/x64/Release/yolov3-tiny.cfg"; //findFile(parser.get<String>("config"));

    Net network = readNet(model, config,"Darknet");
    network.setPreferableBackend(DNN_BACKEND_DEFAULT);
    network.setPreferableTarget(DNN_TARGET_OPENCL);

    for (;;)
    {
        if (!cap.isOpened()) {
            cout << "Video Capture Fail" << endl;
            break;
        }
        Mat img;
        cap >> img;

        static Mat blobFromImg;
        bool swapRB = true;
        blobFromImage(img, blobFromImg, 1, Size(416, 416), Scalar(), swapRB, false);
        cout << blobFromImg.size() << endl; 
        
        float scale = 1.0 / 255.0;
        Scalar mean = 0;
        network.setInput(blobFromImg, "", scale, mean);

        Mat outMat;
        network.forward(outMat);
            // rows represent number of detected object (proposed region)
            int rowsNoOfDetection = outMat.rows;

            // The coluns looks like this, The first is region center x, center y, width
            // height, The class 1 - N is the column entries, which gives you a number, 
            // where the biggist one corrsponding to most probable class. 
            // [x ; y ; w; h; class 1 ; class 2 ; class 3 ;  ; ;....]
            //  
            int colsCoordinatesPlusClassScore = outMat.cols;
            // Loop over number of detected object. 
            for (int j = 0; j < rowsNoOfDetection; ++j)
            {
                // for each row, the score is from element 5 up
                // to number of classes index (5 - N columns)
                Mat scores = outMat.row(j).colRange(5, colsCoordinatesPlusClassScore);

                Point PositionOfMax;
                double confidence;

                // This function find indexes of min and max confidence and related index of element. 
                // The actual index is match to the concrete class of the object.
                // First parameter is Mat which is row [5fth - END] scores,
                // Second parameter will gives you min value of the scores. NOT needed 
                // confidence gives you a max value of the scores. This is needed, 
                // Third parameter is index of minimal element in scores
                // the last is position of the maximum value.. This is the class!!
                minMaxLoc(scores, 0, &confidence, 0, &PositionOfMax);
            
                if (confidence > 0.0001)
                {
// thease four lines are
// [x ; y ; w; h;
                    int centerX = (int)(outMat.at<float>(j, 0) * img.cols); 
                    int centerY = (int)(outMat.at<float>(j, 1) * img.rows); 
                    int width =   (int)(outMat.at<float>(j, 2) * img.cols+20); 
                   int height =   (int)(outMat.at<float>(j, 3) * img.rows+100); 

                    int left = centerX - width / 2;
                    int top = centerY - height / 2;


                    stringstream ss;
                    ss << PositionOfMax.x;
                    string clas = ss.str();
                    int color = PositionOfMax.x * 10;
                    putText(img, clas, Point(left, top), 1, 2, Scalar(color, 255, 255), 2, false);
                    stringstream ss2;
                    ss << confidence;
                    string conf = ss.str();

                    rectangle(img, Rect(left, top, width, height), Scalar(color, 0, 0), 2, 8, 0);
                }
            }
        
        namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
        imshow("Display window", img);
        waitKey(25);
    }
    return 0;
}