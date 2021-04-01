# 2021-group-16

# READ ME
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

