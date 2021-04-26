# 2021-group-16

[![project status](https://git.chalmers.se/courses/dit638/students/2021-group-16/badges/master/pipeline.svg)](https://git.chalmers.se/courses/dit638/students/2021-group-16/badges/master)

# READ ME

## Purpose
We will create a microservice that will determine the steering angle of the vehicle, to the best of our ability mimic the behavior of the car in the provided videos.

## Software Requirement Specification (SRS)
1. The system shall open a debug window when the command line parameter --verbose is provided.
2. The system shall not display any windows if --verbose is omitted from the command line parameters.
3. The system shall not crash when no GUI is available.
4. The system shall be integrated to work with opendlv-vehicle-view and h264decoder.
5. The system shall receive image information via the shared memory area.
6. The system shall use colour segmentation in conjunction with object tracking to identify yellow, blue and red objects.

## Non-functional Requirements
1. The steering angle must be within +/- 50% of the original steering angle in more than 30% of all video frames.
2. The steering angle must be +/-0.05 when the original steering angle is 0.
3. The algorithm will perform faster than 150 milliseconds per frame.

## Constraints
1. Needs to run on ARM, Intel/AMD x86_64 platforms
2. Written in C++
3. Application not allowed to read video files from a file or work on separate image files
4. The following must be printed in the console per frame:
`group_XY;sampleTimeStamp in microseconds;steeringWheelAngle`

## How to set up the project
1. Set up your Linux development environment by downloading all the necessary tools using the following commands:

`sudo apt-get update`

`sudo apt-get upgrade`

`sudo apt-get install build-essential cmake git`

To install docker, go to the following:
https://docs.docker.com/engine/install/ubuntu/

Add your user to the group docker to run Docker without superuser privileges using the following command:

`sudo usermod -aG docker $USER `

To install docker, go to the following:
https://docs.docker.com/compose/install/

2. To check that you have all the required tools, use the following commands:

`g++ --version`

`make --version`

`cmake --version`

`git --version`

`docker --version`

`docker-compose --version`

3. Create a clean folder to house the repository
4. To generate a public SSH key, go to the following SSH key documentation:
    1. To generate an SSH key: (sh/README.html) 
    2. To copy public key (https://docs.gitlab.com/ee/ssh/README.html#add-an-ssh-key-to-your-gitlab-account)
    

5. Click on "Clone" and copy the "Clone with SSH" URL
6. Navigate to the clean folder on your system and type the following to clone the repo: git clone <your clone with SSH repo URL>

7. Build the project using the following commands: 

`mkdir build`

`cd build`

`cmake ..`

`make`

8. Build the project using Docker. Open a new terminal and navigate to the folder containing all the source files. Use the following command to run the build:

`docker build -t <tag> -f Dockerfile .`

9. If successful, create a Docker container based on the Docker image

`docker run --rm <tag> `

### Tools
* G++ 
* Git 
* CMake 
* Make 
* Docker 
* Docker-compose 

## Way of Working

### New features
If there is a new feature, an issue for the feature will be created. A new branch will then be created either from the up-to-date master or from the branch that the new feature depends on. When pushing code to the feature branch, the commit message will reference the associated issue. The feature will then be worked on in this new branch until it is ready to merge to master. When a merge request is made, a code review will be performed by a member of the team who did not work on the feature. Once the code has been reviewed and approved, the new feature can then be merged into master.

### Unexpected behaviour
If there is unexpected behaviour caused by a feature, an issue describing the bug should be made and the team members who worked on the feature will be tagged. 

## Commit Style Guide
* All git commit messages will follow the style outlined here: https://chris.beams.io/posts/git-commit/
* Use present tense in the title of commit (ex: “Fix bug #79”) and then in the body of the commit use past tense to describe exactly what was done
* At the start of the commit description, write the names of each of the members who contributed to the commit.
* If the code contributes to the resolution of an issue, the issue will be referenced at the end of the commit.


