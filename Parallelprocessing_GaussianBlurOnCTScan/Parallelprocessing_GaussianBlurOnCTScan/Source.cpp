#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui.hpp>	
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <omp.h>

//----Multithreading includes----
// The thread library is implemented on top of pthreads in an environment supporting pthreads.
#include <thread>

using namespace cv;
using namespace std;

void displayResult(Mat image) {
    //Define names of the windows
    String window_name = "Lung CT Scan Blurred with various technique";

    // Create windows with above names
    namedWindow(window_name);

    // Show our images inside the created windows.
    imshow(window_name, image);

    waitKey(0); // Wait for any keystroke in the window
    destroyAllWindows(); // Destroy all opened windows
}

Mat sequentialProgram(Mat ori) {
    // Image Ori 
    Mat img_ori = ori;

    // Image 1 Bluring
    Mat img1_blurred;
    GaussianBlur(ori, img1_blurred, Size(3, 3), 0);

    // Image 2 Bluring
    Mat img2_blurred;
    GaussianBlur(ori, img2_blurred, Size(7, 7), 0);
    //medianBlur(ori, img2_blurred, 5);

    // Image 3 Bluring
    Mat img3_blurred;
    GaussianBlur(ori, img3_blurred, Size(9, 9), 0);
    //blur(ori, img3_blurred, Size(3, 3), Point(3, 3), BORDER_DEFAULT);

    // Get image size and combine into new image
    Size sz1 = img_ori.size();
    Size sz2 = img1_blurred.size();
    Size sz3 = img2_blurred.size();
    Size sz4 = img3_blurred.size();
    Mat image((sz1.height + sz3.height), (sz1.width + sz2.width), CV_8UC3);
    Mat topleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
    img_ori.copyTo(topleft);
    Mat topright(image, Rect(sz1.width, 0, sz2.width, sz2.height));
    img1_blurred.copyTo(topright);
    Mat bottomleft(image, Rect(0, sz1.height, sz3.width, sz3.height));
    img2_blurred.copyTo(bottomleft);
    Mat botoomright(image, Rect(sz1.width, sz1.height, sz4.width, sz4.height));
    img3_blurred.copyTo(botoomright);

    return image;
}

void pthreadSubtask1(Mat ori, Mat image, Size sz1) {
    // Image Ori 
    Mat img_ori = ori;
    Mat topleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
    img_ori.copyTo(topleft);
}

void pthreadSubtask2(Mat ori, Mat image, Size sz1) {
    // Image 1 Bluring
    Mat img1_blurred;
    GaussianBlur(ori, img1_blurred, Size(3, 3), 0);
    Mat topright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
    img1_blurred.copyTo(topright);
}

void pthreadSubtask3(Mat ori, Mat image, Size sz1) {
    // Image 2 Bluring
    Mat img2_blurred;
    GaussianBlur(ori, img2_blurred, Size(7, 7), 0);
    //medianBlur(ori, img2_blurred, 5);
    Mat bottomleft(image, Rect(0, sz1.height, sz1.width, sz1.height));
    img2_blurred.copyTo(bottomleft);
}

void pthreadSubtask4(Mat ori, Mat image, Size sz1) {
    // Image 3 Bluring
    Mat img3_blurred;
    GaussianBlur(ori, img3_blurred, Size(9, 9), 0);
    //blur(ori, img3_blurred, Size(3, 3), Point(3, 3), BORDER_DEFAULT);
    Mat botoomright(image, Rect(sz1.width, sz1.height, sz1.width, sz1.height));
    img3_blurred.copyTo(botoomright);
}

Mat pthreadProgram(Mat ori) {
    // Get image size
    Size sz1 = ori.size();
    Mat image((sz1.height + sz1.height), (sz1.width + sz1.width), CV_8UC3);

    thread th1(pthreadSubtask1, ori, image, sz1);
    thread th2(pthreadSubtask2, ori, image, sz1);
    thread th3(pthreadSubtask3, ori, image, sz1);
    thread th4(pthreadSubtask4, ori, image, sz1);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    return image;
}

void evaluateProgram(Mat ori) {
    // Sequential Program
    double start_time1 = omp_get_wtime();
    sequentialProgram(ori);
    double end_time1 = omp_get_wtime();
    // Parallel Program - Pthread
    double start_time4 = omp_get_wtime();
    pthreadProgram(ori);
    double end_time4 = omp_get_wtime();

    int anynumber;
    system("CLS");
    // Header
    printf("===============================================\n");
    printf("|   Image Bluring Execution Time Evaluation   |\n");
    printf("===============================================\n");
    printf("| Sequential program       | %f seconds |\n", end_time1 - start_time1);
    printf("| Pthread    program       | %f seconds |\n", end_time4 - start_time4);
    printf("===============================================\n");
    printf("Enter any number to back to menu...");
    try {
        cin >> anynumber;
        printf("3...");
        this_thread::sleep_for(chrono::milliseconds(600));
        printf("2...");
        this_thread::sleep_for(chrono::milliseconds(800));
        printf("Back to menu...");
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    catch (Exception) {
        printf("3...");
        this_thread::sleep_for(chrono::milliseconds(600));
        printf("2...");
        this_thread::sleep_for(chrono::milliseconds(800));
        printf("Back to menu...");
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void parallelMenu(Mat ori) {
    system("CLS");

    Mat result;
    int parallel_choice;
    printf("==============================================\n");
    printf("|       Parallel Image Processing Menu       |\n");
    printf("==============================================\n");
    printf("| 1     | OpenMP  implementation             |\n");
    printf("| 2     | Cuda    implementation             |\n");
    printf("| 3     | OpenCL  implementation             |\n");
    printf("| 4     | Pthread implementation             |\n");
    printf("| Else. | Back to main Menu                  |\n");
    printf("==============================================\n");

    cout << "I wish to...(Input your choice)" << endl;
    cin >> parallel_choice;

    if (parallel_choice == 1) {
        // OpenMP
    }
    else if (parallel_choice == 2) {
        // Cuda
    }
    else if (parallel_choice == 3) {
        // OpenCl
    }
    else if (parallel_choice == 4) {
        // Pthread
        result = pthreadProgram(ori);
        displayResult(result);
    }
    else {
        // Do nothing
    }
}

int main(int argc, char** argv)
{
    string path = "Resources/Image/image.JPG";

    // Read the image file
    Mat ori = imread(path);
    Mat result;

    // Check for failure
    if (ori.empty())
    {
        cout << "Could not open or find the image." << endl;
        return -1;
    }

    int choice;
    bool exitFlag = false;
    while (exitFlag != true) {
        system("CLS");
        printf("=============================================\n");
        printf("|       Image Bluring Processing Menu       |\n");
        printf("=============================================\n");
        printf("| 1     | Sequencial image bluring program  |\n");
        printf("| 2     | Parallel image bluring program    |\n");
        printf("| 3     | Evaluate both program             |\n");
        printf("| Else. | Exit the program                  |\n");
        printf("=============================================\n");

        cout << "I wish to...(Input your choice)" << endl;
        cin >> choice;

        if (choice == 1) {
            result = sequentialProgram(ori);
            displayResult(result);
        }
        else if (choice == 2) {
            parallelMenu(ori);
        }
        else if (choice == 3) {
            evaluateProgram(ori);
        }
        else {
            exitFlag = true;
            cout << "Preparing to exit..." << endl;
            this_thread::sleep_for(chrono::milliseconds(2000));
            break;
        }
    }

    return 0;
}