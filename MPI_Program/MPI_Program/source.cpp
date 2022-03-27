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
#include <mpi.h>
// The std:thread library is implemented on top of pthreads in an window environment supporting pthreads.
#include <thread>

using namespace cv;
using namespace std;

void displayResult(Mat image) {
    //Define names of the windows
    String window_name = "Image Blurred with various technique";

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

    // Image 3 Bluring
    Mat img3_blurred;
    GaussianBlur(ori, img3_blurred, Size(9, 9), 0);

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

void threadSubtask1(Mat ori, Mat image, Size sz1) {
    // Image Ori 
    Mat img_ori = ori;
    Mat topleft(image, Rect(0, 0, sz1.width, sz1.height));
    img_ori.copyTo(topleft);
}

void threadSubtask2(Mat ori, Mat image, Size sz1) {
    // Image 1 Bluring
    Mat img1_blurred;
    GaussianBlur(ori, img1_blurred, Size(3, 3), 0);
    Mat topright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
    img1_blurred.copyTo(topright);
}

void threadSubtask3(Mat ori, Mat image, Size sz1) {
    // Image 2 Bluring
    Mat img2_blurred;
    GaussianBlur(ori, img2_blurred, Size(7, 7), 0);
    //medianBlur(ori, img2_blurred, 5);
    Mat bottomleft(image, Rect(0, sz1.height, sz1.width, sz1.height));
    img2_blurred.copyTo(bottomleft);
}

void threadSubtask4(Mat ori, Mat image, Size sz1) {
    // Image 3 Bluring
    Mat img3_blurred;
    GaussianBlur(ori, img3_blurred, Size(9, 9), 0);
    //blur(ori, img3_blurred, Size(3, 3), Point(3, 3), BORDER_DEFAULT);
    Mat botoomright(image, Rect(sz1.width, sz1.height, sz1.width, sz1.height));
    img3_blurred.copyTo(botoomright);
}

Mat threadProgram(Mat ori) {
    // Get image size
    Size sz1 = ori.size();
    Mat image((sz1.height + sz1.height), (sz1.width + sz1.width), CV_8UC3);

    thread th1(threadSubtask1, ori, image, sz1);
    thread th2(threadSubtask2, ori, image, sz1);
    thread th3(threadSubtask3, ori, image, sz1);
    thread th4(threadSubtask4, ori, image, sz1);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    return image;
}

void evaluateProgram(Mat ori, double mpi_t) {
    // Sequential Program
    double start_time1 = omp_get_wtime();
    sequentialProgram(ori);
    double end_time1 = omp_get_wtime();
    // Parallel Program - STD:Thread
    double start_time3 = omp_get_wtime();
    threadProgram(ori);
    double end_time3 = omp_get_wtime();

    system("CLS");
    int anynumber;
    // Header
    printf("====================================================\n");
    printf("|   Image Bluring Execution Time Evaluation        |\n");
    printf("====================================================\n");
    printf("| Sequential program (1 process)| %f seconds |\n", end_time1 - start_time1);
    printf("| MPI        program (4 process)| %f seconds |\n", mpi_t);
    printf("| STD:thread program (4 thread) | %f seconds |\n", end_time3 - start_time3);
    printf("====================================================\n");
    printf("Enter key to exit...");
    cin >> anynumber;
    printf("3...");
    this_thread::sleep_for(chrono::milliseconds(600));
    printf("2...");
    this_thread::sleep_for(chrono::milliseconds(800));
    printf("Back to menu...");
    this_thread::sleep_for(chrono::milliseconds(1000));
}

int main(int argc, char* argv[])
{
    string path = "Resources/Image/CTscan.png";

    // Read the image file
    Mat ori = imread(path);
    Mat result;

    // Check for failure
    if (ori.empty())
    {
        cout << "Could not open or find the image." << endl;
        return -1;
    }

    // Get image size
    Size sz1 = ori.size();
    Mat image((sz1.height + sz1.height), (sz1.width + sz1.width), CV_8UC3);

    char name[MPI_MAX_PROCESSOR_NAME];
    int len = 0;
    int i;
    int ierr = 0;
    int process_id = 0;
    int num_processs = 0;
    double begin = 0.0, end = 0.0;

    // Initiatlise MPI Process
    MPI_Init(&argc, &argv);

    // Process Details 
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processs);

    //Synchronize all processes and get the begin time
    MPI_Barrier(MPI_COMM_WORLD);
    begin = MPI_Wtime();

    if (process_id == 0) {
        // Image Ori 
        Mat img_ori = ori;
        Mat topleft(image, Rect(0, 0, sz1.width, sz1.height));
        img_ori.copyTo(topleft);
    }
    else if (process_id == 1) {
        // Image 1 Bluring
        Mat img1_blurred;
        GaussianBlur(ori, img1_blurred, Size(3, 3), 0);
        Mat topright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
        img1_blurred.copyTo(topright);
    }
    else if (process_id == 2) {
        // Image 2 Bluring
        Mat img2_blurred;
        GaussianBlur(ori, img2_blurred, Size(7, 7), 0);
        //medianBlur(ori, img2_blurred, 5);
        Mat bottomleft(image, Rect(0, sz1.height, sz1.width, sz1.height));
        img2_blurred.copyTo(bottomleft);
    }
    else {
        // Image 3 Bluring
        Mat img3_blurred;
        GaussianBlur(ori, img3_blurred, Size(9, 9), 0);
        //blur(ori, img3_blurred, Size(3, 3), Point(3, 3), BORDER_DEFAULT);
        Mat botoomright(image, Rect(sz1.width, sz1.height, sz1.width, sz1.height));
        img3_blurred.copyTo(botoomright);
    }

    //Synchronize all processes and get the end time
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    //Caculate execution time
    if (process_id == 0)
    {
        printf("np=%2d;    Time=%fs;\n", num_processs, end - begin);
        string path = "Resources/Image/CTscan.png";

        // Decide which image to process
        int img_choice;
        system("CLS");
        printf("=============================================\n");
        printf("|       Select The Image For Process        |\n");
        printf("=============================================\n");
        printf("| 1     | Lung CT Scan Image                |\n");
        printf("| 2     | Brainok Ultra Sound Image         |\n");
        printf("| 3     | Baby Ultra Sound Image            |\n");
        printf("| Else. | Baby Ultra Sound Image 2          |\n");
        printf("=============================================\n");

        cout << "I wish to...(Input your choice)" << endl;
        cin >> img_choice;

        if (img_choice == 1) {
            path = "Resources/Image/CTscan.png";
        }
        else if (img_choice == 2) {
            path = "Resources/Image/brainok.png";
        }
        else if (img_choice == 3) {
            path = "Resources/Image/ultrasoundforbaby.png";
        }
        else {
            path = "Resources/Image/ultrasoundbaby2.png";
        }

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
                choice = NULL;
                exitFlag = false;
            }
            else if (choice == 2) {
                result = threadProgram(ori);
                displayResult(result);
                choice = NULL;
                exitFlag = false;
            }
            else if (choice == 3) {
                evaluateProgram(ori, (end - begin));
                choice = NULL;
                exitFlag = false;
            }
            else {
                exitFlag = true;
                cout << "Preparing to exit..." << endl;
                this_thread::sleep_for(chrono::milliseconds(2000));
                break;
            }
        }
    }

    /* Stop this process */
    ierr = MPI_Finalize();

    return 0;
}