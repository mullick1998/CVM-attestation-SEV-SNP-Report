#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <sys/stat.h>

int main() {
  // Create a timestamp
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  char timestamp[100];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", std::localtime(&in_time_t));
  std::string folderName = "amdsev_" + std::string(timestamp);

  // Create the folder
  int status = mkdir(folderName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (status == -1) {
    std::cerr << "Error: Failed to create the folder " << folderName << std::endl;
    return 1;
  }

  // Change the working directory to the newly created folder
  chdir(folderName.c_str());

  // Execute the 'git clone' command
  std::string cmd1 = "git clone https://github.com/AMDESE/sev-tool.git";
  system(cmd1.c_str());

  // Change the working directory to the cloned repository
  std::string repoName = "sev-tool";
  chdir(repoName.c_str());

  // Execute the 'autoreconf' command
  std::string cmd2 = "autoreconf -vif && ./configure && make";
  system(cmd2.c_str());

  // Change the working directory to /src
  std::string src = "src";
  chdir(src.c_str());

  // Install tpm2-tools
  std::string cmd3 = "sudo apt install tpm2-tools";
  system(cmd3.c_str());

  // Obtain VCEK certificate
  std::string cmd4 = "curl -H Metadata:true http://169.254.169.254/metadata/THIM/amd/certification> vcek";
  system(cmd4.c_str());

  // Generate vcek.pem
  std::string cmd5 = "cat ./vcek | jq -r '.vcekCert , .certificateChain' > ./vcek.pem";
  system(cmd5.c_str());

  // Generate snp_report
  std::string cmd6 = "sudo tpm2_nvread -C o 0x01400001 > ./snp_report.bin";
  system(cmd6.c_str());

  // Generate guest_report
  std::string cmd7 = "dd skip=32 bs=1 count=1184 if=./snp_report.bin of=./guest_report.bin";
  system(cmd7.c_str());

  // Execute sudo ./sevtool –-ofolder . –-validate_guest_report
  std::string cmd8 = "sudo ./sevtool --ofolder . --validate_guest_report";
  FILE* pipe = popen(cmd8.c_str(), "r");

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
