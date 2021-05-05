/*
 * Copyright (C) 2020  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Include the single-file, header-only middleware libcluon to create high-performance microservices
#include "cluon-complete.hpp"
#include "cluon-complete.cpp"

// Include the OpenDLV Standard Message Set that contains messages that are usually exchanged for automotive or robotic applications 
#include "opendlv-standard-message-set.hpp"

// Include the GUI and image processing header files from OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{1};
    // Parse the command line parameters as we require the user to specify some mandatory information on startup.
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("cid")) ||
       (0 == commandlineArguments.count("name")) ||
       (0 == commandlineArguments.count("width")) ||
       (0 == commandlineArguments.count("height")) ) {
        std::cerr << argv[0] << " attaches to a shared memory area containing an ARGB image." << std::endl;
    std::cerr << "Usage:   " << argv[0] << " --cid=<OD4 session> --name=<name of shared memory area> [--verbose]" << std::endl;
    std::cerr << "         --cid:    CID of the OD4Session to send and receive messages" << std::endl;
    std::cerr << "         --name:   name of the shared memory area to attach" << std::endl;
    std::cerr << "         --width:  width of the frame" << std::endl;
    std::cerr << "         --height: height of the frame" << std::endl;
    std::cerr << "Example: " << argv[0] << " --cid=253 --name=img --width=640 --height=480 --verbose" << std::endl;
}
else {
        // Extract the values from the command line parameters
    const std::string NAME{commandlineArguments["name"]};
    const uint32_t WIDTH{static_cast<uint32_t>(std::stoi(commandlineArguments["width"]))};
    const uint32_t HEIGHT{static_cast<uint32_t>(std::stoi(commandlineArguments["height"]))};
    const bool VERBOSE{commandlineArguments.count("verbose") != 0};

        // Attach to the shared memory.
    std::unique_ptr<cluon::SharedMemory> sharedMemory{new cluon::SharedMemory{NAME}};
    if (sharedMemory && sharedMemory->valid()) {
        std::clog << argv[0] << ": Attached to shared memory '" << sharedMemory->name() << " (" << sharedMemory->size() << " bytes)." << std::endl;

            // Interface to a running OpenDaVINCI session where network messages are exchanged.
            // The instance od4 allows you to send and receive messages.
        cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"]))};

        opendlv::proxy::GroundSteeringRequest gsr;
        std::mutex gsrMutex;
        auto onGroundSteeringRequest = [&gsr, &gsrMutex](cluon::data::Envelope &&env){
                // The envelope data structure provide further details, such as sampleTimePoint as shown in this test case:
                // https://github.com/chrberger/libcluon/blob/master/libcluon/testsuites/TestEnvelopeConverter.cpp#L31-L40
            std::lock_guard<std::mutex> lck(gsrMutex);
            gsr = cluon::extractMessage<opendlv::proxy::GroundSteeringRequest>(std::move(env));
                //std::cout << "lambda: groundSteering = " << gsr.groundSteering() << std::endl;
        };

        od4.dataTrigger(opendlv::proxy::GroundSteeringRequest::ID(), onGroundSteeringRequest);

// HSV values
        int minHueBlue = 102;
        int maxHueBlue = 150;
        int minSatBlue = 88;
        int maxSatBlue = 165;
        int minValueBlue = 43;
        int maxValueBlue = 222;

        int minHueYellow = 0;
        int maxHueYellow = 46;
        int minSatYellow  = 108;
        int maxSatYellow = 221;
        int minValueYellow = 104;
        int maxValueYellow = 255;

        int frameCounter = 0;

// left car direction is negative (counter clockwise), default value
        int carDirection = -1;
        int makeNegative = -1;
        int frameSampleSize = 5;

        // value may need to be changed depending on threshold and our performance
        int identifiedShape = 150;

        // flag to check if blue cones have been detected
        int blueConeExists = 0;
        int blueConeCenter = 0;
        int yellowConeCenter = 0;
        double increment = 0.0025;
        double steeringWheelAngle = 0.0;
        double steeringMax = 0.290888;
        double steeringMin = -0.290888;

        cv::Mat leftContourImage;


    // Endless loop; end the program by pressing Ctrl-C.
        while (od4.isRunning()) {

            frameCounter++;
        // OpenCV data structure to hold an image.
            cv::Mat img;

        // Wait for a notification of a new frame.
            sharedMemory->wait();

        // Lock the shared memory.
            sharedMemory->lock();
            {
            // Copy the pixels from the shared memory into our own data structure.
                cv::Mat wrapped(HEIGHT, WIDTH, CV_8UC4, sharedMemory->data());
                img = wrapped.clone();
            }
        // TODO: Here, you can add some code to check the sampleTimePoint when the current frame was captured.                            

        // Get current sample time
        // this is a data type that holds 2 in
        std::pair<bool, cluon::data::TimeStamp> sTime = sharedMemory->getTimeStamp(); // Saving current time in sTime var

       // Convert TimeStamp obj into microseconds
        int64_t sMicro = cluon::time::toMicroseconds(sTime.second);

        char buffer[25];
        std::sprintf(buffer, "ts: %ld; ", sMicro); 
        //std::cout << buffer;

        sharedMemory->unlock();

        // TODO: Do something with the frame.
        // Example: Draw a red rectangle and display image.
        
        cv::rectangle(img, cv::Rect(80, 235, 125, 100), cv::Scalar(0,0,255));
        cv::rectangle(img, cv::Rect(200, 245, 230, 115), cv::Scalar(255,255,255));

        // Cutting a region of interest
        // centered cv::Rect(200, 245, 230, 115)

        cv::Rect regionOfInterestCentre = cv::Rect(200, 245, 230, 115);
        cv::Rect regionOfInterestLeft = cv::Rect(80, 235, 125, 100);
        cv::Mat imageWithRegionCentre = img(regionOfInterestCentre);
        cv::Mat imageWithRegionLeft = img(regionOfInterestLeft);
        cv::Mat hsvLeftImg;
        cv::Mat hsvCenterImg;
        cv::Mat detectLeftImg;
        cv::Mat detectCenterImg;
        

            // loop runs until frame counter is greater than the sample size of 5

        if (frameCounter < frameSampleSize)
        {

        // Operation to find blue cones in HSV image
// maybe good for blue cv::Rect(125, 245, 230, 115)
            cv::cvtColor(imageWithRegionLeft, hsvLeftImg, cv::COLOR_BGR2HSV);
            cv::inRange(hsvLeftImg, cv::Scalar(minHueBlue, minSatBlue, minValueBlue), cv::Scalar(maxHueBlue, maxSatBlue, maxValueBlue), detectLeftImg);

    //Applying Gaussian blur to detectBlueImg
            cv::GaussianBlur(detectLeftImg, detectLeftImg, cv::Size(5, 5), 0);

    //Applying dilate and erode to detectBlueImg to remove holes from foreground
            cv::dilate(detectLeftImg, detectLeftImg, 0);
            cv::erode(detectLeftImg, detectLeftImg, 0);

    //Applying erode and dilate to detectBlueImg to remove small objects from foreground
            cv::erode(detectLeftImg, detectLeftImg, 0);
            cv::dilate(detectLeftImg, detectLeftImg, 0);

    // The below will find the contours of the cones in detectBlueImg and store them in a vector
            std::vector<std::vector<cv::Point> > leftContours;
            std::vector<cv::Vec4i> leftHierarchy;
            cv::findContours(detectLeftImg, leftContours, leftHierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // The below will draw the cone contours onto detectBlueImg (copied code from opencv doc)

            leftContourImage = cv::Mat::zeros(detectLeftImg.rows, detectLeftImg.cols, CV_8UC3);

            for (unsigned int i = 0; i < leftContours.size(); i++){

                if (cv::contourArea(leftContours[i]) > identifiedShape )
                {
                    cv::Scalar colour( 255, 255, 0);
                    cv::drawContours(leftContourImage, leftContours, i, colour, -1, 8, leftHierarchy );
                    blueConeExists = 1;

                    // if no blue cones are detected that means the car direction is counter clockwise, which means the steering angle needs to be inverted and car direction is made negative
                    if (blueConeExists == 1) 
                    {
                        carDirection = 1;
                    }
                }
            }
            std::cout << "frame counter" << frameCounter;
        }

        if (VERBOSE) {
            cv::imshow("Video", img);
            //cv::imshow(sharedMemory->name().c_str(), yellowContourImage);
            cv::waitKey(1);
        }


        if (frameCounter >= frameSampleSize) {
            cv::cvtColor(imageWithRegionCentre, hsvCenterImg, cv::COLOR_BGR2HSV);
            cv::inRange(hsvCenterImg, cv::Scalar(minHueBlue, minSatBlue, minValueBlue), cv::Scalar(maxHueBlue, maxSatBlue, maxValueBlue), detectCenterImg);

    //Applying Gaussian blur to detectCenterImg
            cv::GaussianBlur(detectCenterImg, detectCenterImg, cv::Size(5, 5), 0);

    //Applying dilate and erode to detectCenterImg to remove holes from foreground
            cv::dilate(detectCenterImg, detectCenterImg, 0);
            cv::erode(detectCenterImg, detectCenterImg, 0);

    //Applying erode and dilate to detectBlueImg to remove small objects from foreground
            cv::erode(detectCenterImg, detectCenterImg, 0);
            cv::dilate(detectCenterImg, detectCenterImg, 0);

    // The below will find the contours of the cones in detectBlueImg and store them in a vector
            std::vector<std::vector<cv::Point> > blueContours;
            std::vector<cv::Vec4i> blueHierarchy;
            cv::findContours(detectCenterImg, blueContours, blueHierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            cv::Mat blueContourImage = cv::Mat::zeros(detectCenterImg.rows, detectCenterImg.cols, CV_8UC3);

            for (unsigned int i = 0; i < blueContours.size(); i++){

                if (cv::contourArea(blueContours[i]) > identifiedShape )
                {
                    cv::Scalar colour( 255, 255, 0);
                    cv::drawContours(blueContourImage, blueContours, i, colour, -1, 8, blueHierarchy );
                    blueConeCenter = 1;

                    if (steeringWheelAngle > steeringMin && steeringWheelAngle < steeringMax)
                    {

                    // if no blue cones are detected that means the car direction is counter clockwise, which means the steering angle needs to be inverted and car direction is made negative
                        if (blueConeCenter == 1 && carDirection == 1 ) 
                        {
                           steeringWheelAngle = (steeringWheelAngle + increment) * carDirection * makeNegative;
                           std::cout << "line 253 " << steeringWheelAngle << std::endl;

                       } else if (blueConeCenter == 1 && carDirection == -1) {
                           steeringWheelAngle = (steeringWheelAngle + increment) * carDirection;
                           std::cout << "line 257 " << steeringWheelAngle << std::endl;
                       }

                   }

               }
           }

       //  if (blueConeCenter != 1) {

           cv::cvtColor(imageWithRegionCentre, hsvCenterImg, cv::COLOR_BGR2HSV);
           cv::inRange(hsvCenterImg, cv::Scalar(minHueYellow, minSatYellow, minValueYellow), cv::Scalar(maxHueYellow, maxSatYellow, maxValueYellow), detectCenterImg);

    //Applying Gaussian blur to detectCenterImg
           cv::GaussianBlur(detectCenterImg, detectCenterImg, cv::Size(5, 5), 0);

    //Applying dilate and erode to detectCenterImg to remove holes from foreground
           cv::dilate(detectCenterImg, detectCenterImg, 0);
           cv::erode(detectCenterImg, detectCenterImg, 0);

    //Applying erode and dilate to detectBlueImg to remove small objects from foreground
           cv::erode(detectCenterImg, detectCenterImg, 0);
           cv::dilate(detectCenterImg, detectCenterImg, 0);

    // The below will find the contours of the cones in detectBlueImg and store them in a vector
           std::vector<std::vector<cv::Point> > yellowContours;
           std::vector<cv::Vec4i> yellowHierarchy;
           cv::findContours(detectCenterImg, yellowContours, yellowHierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

           cv::Mat yellowContourImage = cv::Mat::zeros(detectCenterImg.rows, detectCenterImg.cols, CV_8UC3);

           for (unsigned int i = 0; i < yellowContours.size(); i++){

            if (cv::contourArea(yellowContours[i]) > identifiedShape)
            {
                cv::Scalar colour( 255, 255, 0);
                cv::drawContours(yellowContourImage, yellowContours, i, colour, -1, 8, yellowHierarchy);
                yellowConeCenter = 1;


                if (steeringWheelAngle > steeringMin && steeringWheelAngle < steeringMax)
                {

                    // if no blue cones are detected that means the car direction is counter clockwise, which means the steering angle needs to be inverted and car direction is made negative
                    if (yellowConeCenter == 1 && carDirection == 1) 
                    {
                       steeringWheelAngle = (steeringWheelAngle + increment) * carDirection;
                       std::cout << "line 307 " << steeringWheelAngle << std::endl;

                   } else if (yellowConeCenter == 1 && carDirection == -1) {
                    steeringWheelAngle = (steeringWheelAngle + increment) * carDirection * makeNegative;
                    std::cout << "line 312 " << steeringWheelAngle << std::endl;
                }

            }

        }
    }   
//}
    if (yellowConeCenter == 0 && blueConeCenter == 0)
    {
        steeringWheelAngle = 0.0;
        std::cout << "line 303 " << steeringWheelAngle << std::endl;
    }

}

                // Add current UTC time
                // Ref: https://stackoverflow.com/questions/38686405/convert-time-t-from-localtime-zone-to-utc   
                cluon::data::TimeStamp time = cluon::time::now(); // Saves current time to var
                int sec = time.seconds(); // Saves current time as int to sec var
                std::time_t lt = sec; // Initialize time_t using sec var
                char buf[30]; // Buffer to hold time
                auto utc_field = *std::gmtime(&lt); // Converts local time_t to UTC tm, auto deduces type

                // Formats buffer results into string
                std::strftime(buf, sizeof(buf), "Now: %FT%TZ; ", &utc_field); 

                // Prints current time in terminal
                //std::cout << buf; 

                // Concatonating three items into one complete string
                std::string name = "Group 16"; // name
                std::string total = std::string(buf).append(buffer); // Concatonates ms and UTC
                std::string complete = total.append(std::string(name)); // Concats all info

                // Displays information on video
                cv::putText(img, //target image
                    complete, 
                    cv::Point(25, 50), 
                    cv::FONT_HERSHEY_DUPLEX,
                    0.5,
                    CV_RGB(0,250,154));

                {
                    std::lock_guard<std::mutex> lck(gsrMutex);
                    // std::cout << "group_16;" << " sampleTimeStamp in microseconds: " << sMicro << " steeringWheelAngle: " << steeringWheelAngle << "Car direction: " << carDirection << "Frame Counter: " << frameCounter << std::endl;
                    std::cout << "our angle: " << steeringWheelAngle << " real angle: " << gsr.groundSteering() << std::endl;
                }

                // Display image on your screen.
                if (VERBOSE) {
                    cv::imshow(sharedMemory->name().c_str(), leftContourImage);
            //cv::imshow(sharedMemory->name().c_str(), yellowContourImage);
                    cv::waitKey(1);
                }

            }
        }
        retCode = 0;
    }
    return retCode;
}
