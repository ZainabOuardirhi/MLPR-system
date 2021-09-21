#include "charactersdetector.h"

using namespace cv;
using namespace dnn;
using namespace std;


charactersDetector::charactersDetector(QObject *parent) : QObject(parent)
{
    // Initialize the parameters
    confThreshold = 0.3; // Confidence threshold
    nmsThreshold = 0.3;  // Non-maximum suppression threshold
    inpWidth = 96;  // Width of network's input image
    inpHeight = 96; // Height of network's input image
}

void charactersDetector::initNetwork()
{
    string classesFile = "/home/hp/Mascir_LPR/data1/PL.names";
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) classes.push_back(line);

    // Give the configuration and weight files for the model
    String modelConfiguration = "/home/hp/Mascir_LPR/data1/PL.cfg";
    String modelWeights = "/home/hp/Mascir_LPR/data1/PL_45000.weights";

    // Load the network
    net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
}

Doub charactersDetector::detect(Mat& frame)
{
    Mat blob,frame1;
    vector<Rect> New_boxes;
    Doub result;
    cv::resize(frame,frame1,cv::Size(416,416));
    // Create a 4D blob from a frame.
    blobFromImage(frame1, blob, 1/255.0, cvSize(inpWidth, inpHeight), Scalar(0,0,0), true, false);

    //Sets the input to the network
    net.setInput(blob);

    // Runs the forward pass to get output of the output layers
    vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));

    // Remove the bounding boxes with low confidence
    result = postprocess(frame, outs);

    // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    string label = "";//format("Inference time for a frame : %.2f ms", t);
    //string out= format("size = %d",outs.size());
    putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255));

    //cout<<result.classID.size()<<endl;

    return result;
}

Doub charactersDetector::postprocess(Mat& frame, const vector<Mat>& outs)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
//                if (left>0 && width>0 && left+width<frame.cols && top+height<frame.rows){
                    boxes.push_back(Rect(left, top, width, height));
//                }
            }
        }
    }

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    vector<int> idx_indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
   // Mat img;
    int k = 0;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
        k++;
        idx_indices.push_back(idx);
        //cout<<indices[i]<<endl;
    }
    Doub results = {classIds,boxes,indices};
    //cout<<indices.size()<<endl;

    return results;
}

void charactersDetector::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    //Draw a rectangle displaying the bounding box
    //rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId];// + ":" + label;
        //cout<<label<<endl;//show in console
    }
    //cout<<classId<<endl;

////////////////////////////////////////////////////////////////////////////////////////
    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    //rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    //putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.2, Scalar(0,0,0),1);
    if (!frame.empty()) {
        //cout<<classId<<endl;
    }
}

vector<String> charactersDetector::getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}
