#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sys/stat.h>

int main() {
// time(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", std::localtime(&in_time_t));
//  std::string folderName = "amdsev_" + std::string(timestamp);

  // Create the folder
  // int status = mkdir(folderName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  // if (status == -1) {
  //  std::cerr << "Error: Failed to create the folder " << folderName << std::endl;
  //  return 1;
//  }

  // Change the working directory to the newly created folder
  // chdir(folderName.c_str());

  // copy the prerequisite file to this filder
  std::string cmd1 = "chmod +x check.sh .";
  system(cmd1.c_str());

  // Execute the shell file
  std::string cmd2 = "./check.sh";
  FILE* pipe = popen(cmd2.c_str(), "r");

  // Read the output of the command and confirm it was printed
  char buffer[128];
  std::string output = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      output += buffer;
  }
  pclose(pipe);
  
  std::cout << "The output of the command is: " << output << std::endl;
  return 0;
}