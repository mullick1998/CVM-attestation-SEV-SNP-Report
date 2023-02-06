#!/bin/bash
if [ ! "$(ls -A | grep ^amdsev)" ]; then
  timestamp=$(date +%F_%T)
  echo "Creating a directory named amdsev_$timestamp"
  mkdir "amdsev_$timestamp"
  echo "Getting inside amdsev_$timestamp"
  cd  "amdsev_$timestamp"

  # Check if sev-tool directory exists
  if [ ! -d "sev-tool" ]; then
    git clone https://github.com/AMDESE/sev-tool.git
    cd sev-tool
    autoreconf -vif && ./configure && make
    cd src
  else
    echo "Directory sev-tool already exists."
    cd sev-tool
    autoreconf -vif && ./configure && make
    cd src
  fi

  # Check if tpm2-tools is installed
  if ! dpkg -s tpm2-tools &> /dev/null; then
    sudo apt-get update
    sudo apt-get install tpm2-tools
  else
    echo "tpm2-tools is already installed."
  fi

  # Check if jq is installed
  if ! dpkg -s jq &> /dev/null; then
    sudo apt-get update
    sudo apt-get install jq
  else
    echo "jq is already installed."
  fi
    
    echo "Downloading vcek certificate..."
    curl -H Metadata:true http://169.254.169.254/metadata/THIM/amd/certification> vcek
    echo "Generating vcek.pem"
    cat ./vcek | jq -r '.vcekCert , .certificateChain' > ./vcek.pem
    echo "Generating snp_report.bin"
    sudo tpm2_nvread -C o 0x01400001 > ./snp_report.bin
    echo "Generating guest_report.bin"
    dd skip=32 bs=1 count=1184 if=./snp_report.bin of=./guest_report.bin
    echo "READY TO ATTEST"
    sudo ./sevtool --ofolder . --validate_guest_report

else
  echo "Directory with name starting with 'amdsev' already exists"
  echo "Getting inside the directories to check –-validate_guest_report"
  cd "$(ls -d amdsev* | head -1)"
  cd sev-tool/src
  echo "READY TO ATTEST"
  sudo ./sevtool --ofolder . --validate_guest_report
fi