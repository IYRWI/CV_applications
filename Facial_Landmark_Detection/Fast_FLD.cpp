#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include "renderFace.hpp"


using namespace dlib;
using namespace std;


//To speed-up, we will process frames at a specific height of 480 and run face detection
//& facial landmark detection on frame after an interval. We are defining these global
//parameters here.

#define RESIZE_HEIGHT 480
#define SKIP_FRAMES 2
#define OPENCV_FACE_RENDER

//Now we will create a VideoCapture object and read first frame.

int main()
{
    try
    {
        //Create an imshow window
        string winName("Fast Facial Landmark Detector");
        cv::namedWindow(winName, cv::WINDOW_NORMAL);

        //Create a VideoCapture object
        cv::VideoCapture cap(0);
        //Check if OpenCV is able to read feed from camera
        if(!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

        //Just a place holder. Actual value calculated after 100 frames.
        double fps = 30.0;

        //Get first frame and allocate memory.
        cv::Mat im;
        cap >> im;

        //We will use a fixed height image as input to face detector
        cv::Mat imSmall, imDisplay;
        float height = im.rows;
        //calculate resize scale
        float RESIZE_SCALE = height/RESIZE_HEIGHT;
        cv::Size size = im.size();

        //Load face detection and pose estimation models
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor predictor;
        deserialize("./common/shape_predictor_68_face_landmarks.dat") >> predictor;

        //initiate the tickCounter
        double t = (double)cv::getTickCount();
        int count = 0;

        //Now, we will read frames from camera feed till we get a stop signal. Frame will be
        //resized to fixed height. We already calculated RESIZE_SCALE using first captured
        //frame. Since we are using OpenCV to read frames from camera, we will convert it into
        //Dlib’s image object format.

        std::vector<rectangle> faces;
        //Grab and process frames until the main window is closed by the user.
        while(1)
        {
            if(count == 0)
                t = cv::getTickCount();

            //Grab a frame
            cap >> im;
            //create imSmall by resizing image by resize scale
            cv::resize(im, imSmall, cv::Size(), 1.0/RESIZE_SCALE, 1.0/RESIZE_SCALE);
            //Change to dlib's image format. No memory is copied
            cv_image<bgr_pixel> cimgSmall(imSmall);
            cv_image<bgr_pixel> cimg(im);

            //Process frames at an interval of SKIP_FRAMES.
            //This value should be set depending on your system hardware
            //and camera fps.
            //To reduce computations, this value should be increased
            if(count % SKIP_FRAMES == 0)
            {
                //Detect faces
                faces = detector(cimgSmall);
            }

            //Find facial landmarks for each face.
            std::vector<full_object_detection> shapes;
            //Iterate over faces
            for(unsigned long i=0; i<faces.size(); i++)
            {
                //Since we ran face detection on a resized image,
                //we will scale up coordinates of face rectangle
                rectangle r(
                            (long)(faces[i].left() * RESIZE_SCALE),
                            (long)(faces[i].top() * RESIZE_SCALE),
                            (long)(faces[i].right() * RESIZE_SCALE),
                            (long)(faces[i].bottom() * RESIZE_SCALE)
                            );
                //Find face landmarks by providing rectangle for each face
                full_object_detection shape = predictor(cimg, r);
                shapes.push_back(shape);
                //Draw facial landmarks
                renderFace(im, shape);
            }

            //Put fps at which we are processinf camera feed on frame
            cv::putText(im, cv::format("fps %.2f", fps), cv::Point(50, size.height - 50),
                        cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(0, 0, 255), 3);
            //Display it all on the screen
            cv::imshow(winName, im);
            //Wait for keypress
            char key = cv::waitKey(1);
            if(key == 27) //ESC
            {
                //If ESC is pressed,exit.
                return EXIT_SUCCESS;
            }

            //We can calculate function execution time using OpenCV’s functions getTickCount and
            //getTickFrequency. Dividing number of clock-cycles, spent in executing a function, by
            //frequency of clock-cycles gives the function execution time.

            //increment frame counter
            count++;
            //calculate fps after each 100 frams are processed
            if(count == 100)
            {
                t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
                fps = 100.0/t;
                count = 0;
            }

            //Close all OpenCV windows when the program exits. Catch blocks catch and print, if
            //there are any errors.
            cap.release();
            cv::destroyAllWindows();
        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() <<endl;
    }
}


