#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofApp: public ofBaseApp
{
    ofVideoGrabber cam;
    ofImage thresh, warped;

    bool draw_raw;
    float threshold_value;
    cv::Point2f points[4];

    bool laser_detected;
    ofPoint laser;

public:

    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

};
