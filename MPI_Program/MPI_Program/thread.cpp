#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui.hpp>	
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

#include <thread>
#include <mpi.h>
#include <omp.h>

using namespace std;
using namespace cv;

const int MAXBYTES = 8 * 1024 * 1024;
uchar buffer[MAXBYTES];
double kernel_size = 15;
double num_of_thread = 3;
string result_path = "Resources/Image/Result.jpg";
string window_title = "Image Processing Result (Left: Original, Right: Processed)";

void displayResult(Mat image) {
    //Define names of the windows
    String window_name = window_title;

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
    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);

    // Get image size and combine into new image
    Size sz1 = img_ori.size();
    Mat image((sz1.height), (sz1.width + sz1.width), CV_8UC3);

    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
    img_ori.copyTo(mostleft);
    Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
    img1_blurred.copyTo(mostright);

    return image;
}

Mat combinedthreadtask(Mat ori, Mat image, Size sz1) {
    // Image Ori 
    Mat img_ori = ori;

    // Image 1 Bluring
    Mat img1_blurred;
    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);

    // Get image size and combine into new image
    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
    img_ori.copyTo(mostleft);
    Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
    img1_blurred.copyTo(mostright);

    return image;
}

void threadSubtask1(Mat ori, Mat image, Size sz1) {
    // Image Ori 
    Mat img_ori = ori;
    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
    img_ori.copyTo(mostleft);
}

void threadSubtask2(Mat ori, Mat image, Size sz1) {
    // Image 1 Bluring
    Mat img1_blurred;
    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
    Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
    img1_blurred.copyTo(mostright);
}

Mat threadProgram(Mat ori) {
    // Get image size
    Size sz1 = ori.size();
    Mat image(sz1.height, (sz1.width + sz1.width), CV_8UC3);

    if (num_of_thread == 1) {
        thread th1(combinedthreadtask, ori, image, sz1);
        th1.join();
    }
    else {
        thread th1(threadSubtask1, ori, image, sz1);
        thread th2(threadSubtask2, ori, image, sz1);
        th1.join();
        th2.join();
    }

    return image;
}

void evaluateProgram(Mat ori) {
    // Sequential Program
    double start_time1 = omp_get_wtime();
    sequentialProgram(ori);
    double end_time1 = omp_get_wtime();
    // Parallel Program - STD:Thread
    double start_time3 = omp_get_wtime();
    threadProgram(ori);
    double end_time3 = omp_get_wtime();

    // Calculation
    double seq_t = end_time1 - start_time1;
    double thr_t = end_time3 - start_time3;
    double thr_s = (end_time1 - start_time1) / thr_t;
    double thr_e = (thr_s / 4) * 100;
    int anynumber;

    system("CLS");
    system("cls");
    // Program Evaluation
    printf("\n");
    printf("==========================================================================================================\n");
    printf("|                                    Image Bluring Program Evaluation                                    |\n");
    printf("==========================================================================================================\n");
    printf("| Program               | Execution Cost |     Execution Time |     Speed Up | Efficiency(4 core machine)|\n");
    printf("|--------------------------------------------------------------------------------------------------------|\n");
    printf("| Sequential program    | 1 process      | %*f seconds |            - |                         - |\n", 10, seq_t);
    printf("| STD:thread program    | %*.0f thread       | %*f seconds | %*f | %*.2f percent |\n", 1, num_of_thread, 10, thr_t, 12, thr_s, 17, thr_e);
    printf("==========================================================================================================\n");
    printf("\n");

    printf("Enter key to exit...");
    cin >> anynumber;
    printf("3...");
    this_thread::sleep_for(chrono::milliseconds(600));
    printf("2...");
    this_thread::sleep_for(chrono::milliseconds(800));
    printf("Back to menu...");
    this_thread::sleep_for(chrono::milliseconds(1000));
}

void matsnd(const Mat m, int dest) {
    int rows = m.rows;
    int cols = m.cols;
    int type = m.type();
    int channels = m.channels();
    memcpy(&buffer[0 * sizeof(int)], (uchar*)&rows, sizeof(int));
    memcpy(&buffer[1 * sizeof(int)], (uchar*)&cols, sizeof(int));
    memcpy(&buffer[2 * sizeof(int)], (uchar*)&type, sizeof(int));

    // See note at end of answer about "bytes" variable below!!!
    int bytespersample = 1; // change if using shorts or floats
    int bytes = m.rows * m.cols * channels * bytespersample;
    cout << "matsnd: rows=" << rows << endl;
    cout << "matsnd: cols=" << cols << endl;
    cout << "matsnd: type=" << type << endl;
    cout << "matsnd: channels=" << channels << endl;
    cout << "matsnd: bytes=" << bytes << endl;

    memcpy(&buffer[3 * sizeof(int)], m.data, bytes);
    MPI_Send(&buffer, bytes + 3 * sizeof(int), MPI_UNSIGNED_CHAR, dest, 0, MPI_COMM_WORLD);
}

Mat matrcv(int src) {
    MPI_Status status;
    int count, rows, cols, type, channels;

    MPI_Recv(&buffer, sizeof(buffer), MPI_UNSIGNED_CHAR, src, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);
    memcpy((uchar*)&rows, &buffer[0 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&cols, &buffer[1 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&type, &buffer[2 * sizeof(int)], sizeof(int));

    cout << "matrcv: Count=" << count << endl;
    cout << "matrcv: rows=" << rows << endl;
    cout << "matrcv: cols=" << cols << endl;
    cout << "matrcv: type=" << type << endl;

    // Make the mat
    Mat received = Mat(rows, cols, type, (uchar*)&buffer[3 * sizeof(int)]);
    return received;
}

int main(int argc, char* argv[])
{
    int img_choice;
    string path;
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

    // MASTER - read Mat from disk and send to master
    Mat ori = imread(path, IMREAD_COLOR);

    // Check for failure
    if (ori.empty())
    {
        cout << "Could not open or find the image." << endl;
        return -1;
    }
    else {
        // Get image size
        Size sz1 = ori.size();
        // Declaring result image
        Mat result((sz1.height), (sz1.width + sz1.width), CV_8UC3);

        int choice;
        bool exitFlag = false;
        while (exitFlag != true) {
//////            system("CLS");
            printf("=============================================\n");
            printf("|       Image Bluring Processing Menu       |\n");
            printf("=============================================\n");
            printf("| 1     | Sequencial  image bluring program |\n");
            printf("| 2     | Std::thread image bluring program |\n");
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
                evaluateProgram(ori);
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
}