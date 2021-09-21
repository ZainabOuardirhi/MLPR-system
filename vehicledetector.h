#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include <QObject>

#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

struct plate
{
     std::vector<int> classID;
     std::vector<cv::Rect> box;
     std::vector<int> id;
     std::vector<float> conf;
};

struct Image{
    cv::Mat image;
    float conf;
};

class VehicleDetector : public QObject
{
    Q_OBJECT
public:
    explicit VehicleDetector(QObject *parent = nullptr);

    void initNetwork();

    plate detect(cv::Mat& frame);

    // Remove the bounding boxes with low confidence using non-maxima suppression
    plate postprocess(cv::Mat& frame, const std::vector<cv::Mat>& out);

    // Draw the predicted bounding box
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);

    // Get the names of the output layers
    std::vector<cv::String> getOutputsNames(const cv::dnn::Net& net);
signals:

public slots:


private:

    float confThreshold; // Confidence threshold
    float nmsThreshold;  // Non-maximum suppression threshold
    int inpWidth;  // Width of network's input image
    int inpHeight; // Height of network's input image
    std::vector<std::string> classes;
    cv::dnn::Net net;

};

#endif // VEHICLEDETECTOR_H
