#include "ofApp.h"
#include "ofxCv.h"
#include "ofxXmlSettings.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup()
{
    ofEnableSetupScreen();
    //enableSetupScreen();

    ofxXmlSettings settings;
    settings.loadFile("lasor.xml");

    int camera_width = settings.getValue("lasor:camera_width", 640);
    int camera_height = settings.getValue("lasor:camera_height", 480);
    int camera_fps = settings.getValue("lasor:camera_fps", 60);

    int display_width = settings.getValue("lasor:display_width", 1920);
    int display_height = settings.getValue("lasor:display_height", 1080);
    int display_x = settings.getValue("lasor:display_x", 0);
    int display_y = settings.getValue("lasor:display_y", 0);

    draw_raw = settings.getValue("lasor:draw_raw", true);
    threshold_value = settings.getValue("lasor:threshold", 128);

    points[0].x = settings.getValue("lasor:point_x0", 0);
    points[0].y = settings.getValue("lasor:point_y0", 0);

    points[1].x = settings.getValue("lasor:point_x1", cam.width);
    points[1].y = settings.getValue("lasor:point_y1", 0);

    points[2].x = settings.getValue("lasor:point_x2", cam.width);
    points[2].y = settings.getValue("lasor:point_y2", cam.height);

    points[3].x = settings.getValue("lasor:point_x3", 0);
    points[3].y = settings.getValue("lasor:point_y3", cam.height);

    ofSetWindowShape(display_width, display_height);
    ofSetWindowPosition(display_x, display_y);

    cam.setDesiredFrameRate(camera_fps);
    cam.initGrabber(camera_width, camera_height, true);
    cam.videoSettings();

    thresh.allocate(camera_width, camera_height, OF_IMAGE_GRAYSCALE);
    warped.allocate((display_width * camera_height / display_height) & ~1 | 2, camera_height, OF_IMAGE_GRAYSCALE);

    ofSetBackgroundAuto(false);
    ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::update()
{
    cam.update();
    if (cam.isFrameNew())
    {
        convertColor(cam, thresh, CV_RGB2GRAY);

        threshold(thresh, threshold_value);

        Point2f screen_points[4] = {
            Point2f(0, 0),
            Point2f(warped.width, 0),
            Point2f(warped.width, warped.height),
            Point2f(0, warped.height),
        };

        auto transform = ofxCv::getPerspectiveTransform(points, screen_points);

        auto warpedmat = toCv(warped);
        warpPerspective(toCv(thresh), warpedmat, transform, warpedmat.size());

        static ofxCv::SimpleBlobDetector::Params params;
        params.blobColor = 255;
        params.minArea = 5;
        params.maxArea = 300;
        params.filterByConvexity = false;
        params.filterByCircularity = false;
        params.thresholdStep = 64;

        ofxCv::SimpleBlobDetector blob(params);

        std::vector<cv::KeyPoint> points;
        blob.detect(warpedmat, points);

        if (laser_detected = (points.size() > 0))
        {
            laser.x = ofMap(points[0].pt.x, 0, warped.width, 0, ofGetWidth());
            laser.y = ofMap(points[0].pt.y, 0, warped.height, 0, ofGetHeight());
        }

        thresh.update();
        warped.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if (laser_detected)
    {
        ofSetColor(0, 0, 255);
        ofCircle(laser, 5.0f);
    }


    if (draw_raw)
    {
        ofSetColor(255);
        cam.draw(0, 0, cam.width, cam.height);
        thresh.draw(0, cam.height, thresh.width, thresh.height);
        warped.draw(cam.width, 0);
    }

    char fps[16];
    sprintf(fps, "FPS: %d", (int)ofGetFrameRate());

    ofSetColor(0);
    ofRect(0, 0, 100, 20);

    ofSetColor(255);
    ofDrawBitmapString(fps, 2, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    int num = key - '0';
    if (num >= 1 && num <= 4)
    {
        points[num - 1].x = ofMap(mouseX, 0, cam.width, 0, cam.width);
        points[num - 1].y = ofMap(mouseY, 0, cam.height, 0, cam.height);
    }

    if (key == 'w')
        draw_raw = !draw_raw;

    if (key == 't')
    {
        threshold_value = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
    }
    //
    
    if (key == 'f')
    {
        ofToggleFullscreen();
    }

    if (key == 's')
    {
        ofxXmlSettings settings;

        settings.setValue("lasor:camera_width", cam.width);
        settings.setValue("lasor:camera_height", cam.height);

        settings.setValue("lasor:display_width", ofGetWindowWidth());
        settings.setValue("lasor:display_height", ofGetWindowHeight());
        settings.setValue("lasor:display_x", ofGetWindowPositionX());
        settings.setValue("lasor:display_y", ofGetWindowPositionY());

        settings.setValue("lasor:draw_raw", draw_raw);
        settings.setValue("lasor:threshold", threshold_value);

        settings.setValue("lasor:point_x0", points[0].x);
        settings.setValue("lasor:point_y0", points[0].y);

        settings.setValue("lasor:point_x1", points[1].x);
        settings.setValue("lasor:point_y1", points[1].y);

        settings.setValue("lasor:point_x2", points[2].x);
        settings.setValue("lasor:point_y2", points[2].y);

        settings.setValue("lasor:point_x3", points[3].x);
        settings.setValue("lasor:point_y3", points[3].y);

        settings.saveFile("lasor.xml");
    }

    if (key == 'c')
        ofBackground(0);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
