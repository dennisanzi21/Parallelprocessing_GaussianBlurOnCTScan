//#include <opencv2/imgcodecs/imgcodecs.hpp>
//#include <opencv2/highgui.hpp>	
//#include <opencv2/imgproc.hpp>
//#include <opencv2/opencv.hpp>
//#include <opencv2/core.hpp>
//#include <iostream>
//#include <stdlib.h>
//#include <stdio.h>
//#include <iomanip>
//#include <cstdlib>
//#include <string>
//#include <chrono>
//#include <thread>
//#include <ctime>
//
//#include <thread>
//#include <mpi.h>
//#include <omp.h>
//
//using namespace std;
//using namespace cv;
//
//const int MAXBYTES = 8 * 1024 * 1024;
//uchar buffer[MAXBYTES];
//double kernel_size = 15;
//double num_of_thread = 1;
//string input_path = "Resources/Image/ultrasoundbaby2.png";
//string result_path = "Resources/Image/Result.png";
//string window_title = "Image Processing Result (Left: Original, Right: Processed)";
//
//void displayResult(Mat image) {
//    //Define names of the windows
//    String window_name = window_title;
//
//    // Create windows with above names
//    namedWindow(window_name);
//
//    // Show our images inside the created windows.
//    imshow(window_name, image);
//
//    waitKey(0); // Wait for any keystroke in the window
//    destroyAllWindows(); // Destroy all opened windows
//}
//
//Mat sequentialProgram(Mat ori) {
//    // Image Ori 
//    Mat img_ori = ori;
//    imwrite("process_1_result.png", img_ori);
//
//    // Image 1 Bluring
//    Mat img1_blurred;
//    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
//    imwrite("process_2_result.png", img1_blurred);
//
//    // Get image size and combine into new image
//    Size sz1 = img_ori.size();
//    Size sz2 = img1_blurred.size();
//    Mat image((sz1.height), (sz1.width + sz2.width), CV_8UC3);
//    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
//    img_ori.copyTo(mostleft);
//    Mat mostright(image, Rect(sz1.width, 0, sz2.width, sz2.height));
//    img1_blurred.copyTo(mostright);
//
//    return image;
//}
//
//void combinedthreadtask(Mat ori, Mat image, Size sz1) {
//    // Image Ori 
//    Mat img_ori = ori;
//    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
//    img_ori.copyTo(mostleft);
//    // Image 1 Bluring
//    Mat img1_blurred;
//    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
//    Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
//    img1_blurred.copyTo(mostright);
//}
//
//void threadSubtask1(Mat ori, Mat image, Size sz1) {
//    // Image Ori 
//    Mat img_ori = ori;
//    Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height)); // x, y, width, height
//    img_ori.copyTo(mostleft);
//}
//
//void threadSubtask2(Mat ori, Mat image, Size sz1) {
//    // Image 1 Bluring
//    Mat img1_blurred;
//    GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
//    Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
//    img1_blurred.copyTo(mostright);
//}
//
//Mat threadProgram(Mat ori) {
//    // Get image size
//    Size sz1 = ori.size();
//    Mat image(sz1.height, (sz1.width + sz1.width), CV_8UC3);
//
//    if (num_of_thread == 1) {
//        thread th1(combinedthreadtask, ori, image, sz1);
//        th1.join();
//    }
//    else if (num_of_thread == 2) {
//        thread th1(threadSubtask1, ori, image, sz1);
//        thread th2(threadSubtask2, ori, image, sz1);
//        th1.join();
//        th2.join();
//    }
//    else {
//        thread th1(threadSubtask1, ori, image, sz1);
//        thread th2(threadSubtask2, ori, image, sz1);
//        th1.join();
//        th2.join();
//    }
//
//    return image;
//}
//
//void evaluateProgram(Mat ori, double mpi_t, double num_processs) {
//    // Sequential Program
//    double start_time1 = omp_get_wtime();
//    sequentialProgram(ori);
//    double end_time1 = omp_get_wtime();
//
//    // Calculation
//    double seq_t = end_time1 - start_time1;
//    double mpi_s = (end_time1 - start_time1) / mpi_t;
//    double mpi_e = (mpi_s / 4) * 100;
//    int anynumber;
//
//    system("CLS");
//    system("cls");
//    // Program Evaluation
//    printf("\n");
//    printf("==========================================================================================================\n");
//    printf("|                                    Image Bluring Program Evaluation                                    |\n");
//    printf("==========================================================================================================\n");
//    printf("| Program               | Execution Cost |     Execution Time |     Speed Up | Efficiency(4 core machine)|\n");
//    printf("|--------------------------------------------------------------------------------------------------------|\n");
//    printf("| Sequential program    | 1 process      | %*f seconds |            - |                         - |\n", 10, seq_t);
//    printf("| MPI        program    | %*.0f process      | %*f seconds | %*f | %*.2f percent |\n", 1, num_processs, 10, mpi_t, 12, mpi_s, 17, mpi_e);
//    printf("==========================================================================================================\n");
//    printf("\n");
//
//    printf("Enter key to exit...");
//    cin >> anynumber;
//    printf("3...");
//    this_thread::sleep_for(chrono::milliseconds(600));
//    printf("2...");
//    this_thread::sleep_for(chrono::milliseconds(800));
//    printf("Back to menu...");
//    this_thread::sleep_for(chrono::milliseconds(1000));
//}
//
//void matsnd(const Mat m, int dest) {
//    int rows = m.rows;
//    int cols = m.cols;
//    int type = m.type();
//    int channels = m.channels();
//    memcpy(&buffer[0 * sizeof(int)], (uchar*)&rows, sizeof(int));
//    memcpy(&buffer[1 * sizeof(int)], (uchar*)&cols, sizeof(int));
//    memcpy(&buffer[2 * sizeof(int)], (uchar*)&type, sizeof(int));
//
//    // See note at end of answer about "bytes" variable below!!!
//    int bytespersample = 1; // change if using shorts or floats
//    int bytes = m.rows * m.cols * channels * bytespersample;
//    cout << "matsnd: rows=" << rows << endl;
//    cout << "matsnd: cols=" << cols << endl;
//    cout << "matsnd: type=" << type << endl;
//    cout << "matsnd: channels=" << channels << endl;
//    cout << "matsnd: bytes=" << bytes << endl;
//
//    memcpy(&buffer[3 * sizeof(int)], m.data, bytes);
//    MPI_Send(&buffer, bytes + 3 * sizeof(int), MPI_UNSIGNED_CHAR, dest, 0, MPI_COMM_WORLD);
//}
//
//Mat matrcv(int src) {
//    MPI_Status status;
//    int count, rows, cols, type, channels;
//
//    MPI_Recv(&buffer, sizeof(buffer), MPI_UNSIGNED_CHAR, src, 0, MPI_COMM_WORLD, &status);
//    MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);
//    memcpy((uchar*)&rows, &buffer[0 * sizeof(int)], sizeof(int));
//    memcpy((uchar*)&cols, &buffer[1 * sizeof(int)], sizeof(int));
//    memcpy((uchar*)&type, &buffer[2 * sizeof(int)], sizeof(int));
//
//    cout << "matrcv: Count=" << count << endl;
//    cout << "matrcv: rows=" << rows << endl;
//    cout << "matrcv: cols=" << cols << endl;
//    cout << "matrcv: type=" << type << endl;
//
//    // Make the mat
//    Mat received = Mat(rows, cols, type, (uchar*)&buffer[3 * sizeof(int)]);
//    return received;
//}
//
//int main(int argc, char* argv[])
//{
//    // Read the image file
//    Mat ori = imread(input_path);
//    Mat result;
//
//    // Check for failure
//    if (ori.empty())
//    {
//        cout << "Could not open or find the image." << endl;
//        return -1;
//    }
//
//    // Get image size
//    Size sz1 = ori.size();
//    Mat image((sz1.height), (sz1.width + sz1.width), CV_8UC3);
//
//    // Initialise MPI
//    MPI_Init(&argc, &argv);
//
//    // Process Details 
//    int process_id;
//    int num_processs = 0;
//    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
//    MPI_Comm_size(MPI_COMM_WORLD, &num_processs);
//
//    // Synchronize all processes and get the begin time
//    double begin = 0.0, end = 0.0;
//    MPI_Barrier(MPI_COMM_WORLD);
//    begin = MPI_Wtime();
//
//    if (num_processs == 1) {
//        // Assign tasks to processes
//        if (process_id == 0) {
//            // Image Ori 
//            Mat img_ori = ori;
//            imwrite("process_1_result.png", img_ori);
//            // Image Bluring
//            Mat img1_blurred;
//            GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
//            imwrite("process_2_result.png", img1_blurred);
//        }
//    }
//    else {
//        // Assign tasks to processes
//        if (process_id == 0) {
//            // Image Ori 
//            Mat img_ori = ori;
//            imwrite("process_1_result.png", img_ori);
//        }
//        else if (process_id == 1) {
//            // Image Bluring
//            Mat img1_blurred;
//            GaussianBlur(ori, img1_blurred, Size(kernel_size, kernel_size), 0);
//            imwrite("process_2_result.png", img1_blurred);
//        }
//        else {
//            // Do nothing as the distribution of task maximum 2 process only
//        }
//    }
//
//    //Synchronize all processes and get the end time
//    MPI_Barrier(MPI_COMM_WORLD);
//    end = MPI_Wtime();
//
//    /* Stop this process */
//    int terminateMPI = MPI_Finalize();
//
//    //Mat process_1_result = imread("process_1_result.png", IMREAD_COLOR);
//    //Mat process_2_result = imread("process_2_result.png", IMREAD_COLOR);
//    //Mat mostleft(image, Rect(0, 0, sz1.width, sz1.height));
//    //process_1_result.copyTo(mostleft);
//    //Mat mostright(image, Rect(sz1.width, 0, sz1.width, sz1.height));
//    //process_2_result.copyTo(mostright);
//    //imwrite("result.png", image);
//
//    //Caculate execution time
//    if (process_id == 0)
//    {
//        string path = "Resources/Image/CTscan.png";
//
//        // Decide which image to process
//        int img_choice;
//        system("CLS");
//        printf("=============================================\n");
//        printf("|       Select The Image For Process        |\n");
//        printf("=============================================\n");
//        printf("| 1     | Lung CT Scan Image                |\n");
//        printf("| 2     | Brainok Ultra Sound Image         |\n");
//        printf("| 3     | Baby Ultra Sound Image            |\n");
//        printf("| Else. | Baby Ultra Sound Image 2          |\n");
//        printf("=============================================\n");
//
//        cout << "I wish to...(Input your choice)" << endl;
//        cin >> img_choice;
//
//        if (img_choice == 1) {
//            path = "Resources/Image/CTscan.png";
//        }
//        else if (img_choice == 2) {
//            path = "Resources/Image/brainok.png";
//        }
//        else if (img_choice == 3) {
//            path = "Resources/Image/ultrasoundforbaby.png";
//        }
//        else {
//            path = "Resources/Image/ultrasoundbaby2.png";
//        }
//
//        // Read the image file
//        Mat ori = imread(path);
//        Mat result;
//
//        // Check for failure
//        if (ori.empty())
//        {
//            cout << "Could not open or find the image." << endl;
//            return -1;
//        }
//
//        int choice;
//        bool exitFlag = false;
//        while (exitFlag != true) {
//            system("CLS");
//            printf("=============================================\n");
//            printf("|       Image Bluring Processing Menu       |\n");
//            printf("=============================================\n");
//            printf("| 1     | Sequencial image bluring program  |\n");
//            printf("| 2     | MPI        image bluring program  |\n");
//            printf("| 3     | Evaluate both program             |\n");
//            printf("| Else. | Exit the program                  |\n");
//            printf("=============================================\n");
//
//            cout << "I wish to...(Input your choice)" << endl;
//            cin >> choice;
//
//            if (choice == 1) {
//                result = sequentialProgram(ori);
//                displayResult(result);
//                choice = NULL;
//                exitFlag = false;
//            }
//            else if (choice == 2) {
//                Mat result = imread("result.png", IMREAD_COLOR);
//                displayResult(result);
//                choice = NULL;
//                exitFlag = false;
//            }
//            else if (choice == 3) {
//                evaluateProgram(ori, (end - begin), num_processs);
//                choice = NULL;
//                exitFlag = false;
//            }
//            else {
//                exitFlag = true;
//                cout << "Preparing to exit..." << endl;
//                this_thread::sleep_for(chrono::milliseconds(2000));
//                break;
//            }
//        }
//    }
//}