#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <ctime>

using namespace std;
using namespace cv;

vector<vector<int>> getColorList() {
    vector<vector<int>> colorList;

    // 紅色
    vector<int> red_lower = { 0, 43, 46 };
    vector<int> red_upper = { 10, 255, 255 };
    colorList.push_back(red_lower);
    colorList.push_back(red_upper);

    // 黃色
    vector<int> yellow_lower = { 15, 43, 46 };
    vector<int> yellow_upper = { 35, 255, 255 };
    colorList.push_back(yellow_lower);
    colorList.push_back(yellow_upper);

    // 綠色
    vector<int> green_lower = { 35, 43, 46 };
    vector<int> green_upper = { 89, 255, 255 };
    colorList.push_back(green_lower);
    colorList.push_back(green_upper);

    // 藍色
    vector<int> blue_lower = { 90, 43, 46 };
    vector<int> blue_upper = { 124, 255, 255 };
    colorList.push_back(blue_lower);
    colorList.push_back(blue_upper);

    return colorList;
}

string get_color(Mat frame) {
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    int maxsum = 15000;
    int yellow_maxsum = 29000;
    string color;
    Mat background(170, 960, CV_8UC1, Scalar(0));
    background.colRange(450, 650) = Scalar(255);
    //cvtColor(background, background, COLOR_GRAY2BGR);
    auto colorList = getColorList();
    hsv.copyTo(hsv, background);
    frame.copyTo(frame, background);

    for (size_t i = 0; i < colorList.size(); ++i) {
        auto lower = Scalar(colorList[i][0], colorList[i][1], colorList[i][2]);
        auto upper = Scalar(colorList[i][3], colorList[i][4], colorList[i][5]);

        Mat mask, binary;
        inRange(hsv, lower, upper, mask);
        threshold(mask, binary, 127, 255, THRESH_BINARY);
        dilate(binary, binary, Mat(), Point(-1, -1), 2);

        Mat gs_frame;
        GaussianBlur(frame, gs_frame, Size(5, 5), 0);
        cvtColor(gs_frame, hsv, COLOR_BGR2HSV);
        Mat erode_hsv;
        erode(hsv, erode_hsv, Mat(), Point(-1, -1), 10);

        vector<vector<Point>> contours;
        findContours(binary.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        int sum = 0;
        for (size_t j = 0; j < contours.size(); ++j) {
            sum += contourArea(contours[j]);
        }

        if (sum > maxsum && i != 1) {
            color = to_string(i);
        }
        else if (i == 1) {
            if (sum > yellow_maxsum) {
                color = to_string(i);
            }
            else if (color.empty()) {
                color = "";
            }
        }
    }

    imshow("Contours", hsv);
    return color;
}

int main() {
    vector<string> target_color = { "red", "yellow", "green", "blue" };
    int red_count = 0, yellow_count = 0, green_count = 0, blue_count = 0;
    vector<string> color_temp(5);
    vector<pair<int, string>> total_color;
    string image_path = "c:\\Users\\User\\Desktop\\V_20231103_153743_ES5.mp4";
    VideoCapture cap(image_path);
    Mat frame;

    while (waitKey(1) < 0) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        rotate(frame, frame, ROTATE_90_COUNTERCLOCKWISE);
        //flip(frame, frame, 1);
        resize(frame, frame, Size(960, 540));
        frame = frame(Range(200, 370), Range::all());

        string color = get_color(frame);
        for (int i = 0; i < color_temp.size() - 1; ++i) {
            color_temp[i] = color_temp[i + 1];
        }
        color_temp[4] = color;

        if (color_temp[1].empty() && color_temp[2].empty() && color_temp[3].empty() && color_temp[4].empty()) {
            string color_result = color_temp[0];
            if (!color_result.empty()) {
                if (color_result == "1") {
                    ++red_count;
                }
                if (color_result == "2") {
                    ++yellow_count;
                }
                if (color_result == "3") {
                    ++green_count;
                }
                if (color_result == "4") {
                    ++blue_count;
                }
                int total_count = red_count + yellow_count + green_count + blue_count;
                total_color.emplace_back(total_count, color_result);
            }
        }

        cout << "red: " << red_count << endl;
        cout << "yellow: " << yellow_count << endl;
        cout << "green: " << green_count << endl;
        cout << "blue: " << blue_count << endl;
        cout << "-------------------------" << endl;

        imshow("Ball Detection", frame);
        waitKey(2);
    }

    return 0;
}