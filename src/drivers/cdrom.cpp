#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class CDROM {
public:
  CDROM(string device) {
    this->device = device;
  }

  void open() {
    ifstream fs(device);
    if (!fs.is_open()) {
      throw runtime_error("Error Opening the CDROM Device");
    }
  }

  void close() {
    fs.close();
  }

  void read(vector<byte>& buffer, int offset, int size) {
    fs.seekg(offset, ios::beg);
    fs.read(buffer.data(), size);
  }

  void write(vector<byte>& buffer, int offset, int size) {
    fs.seekp(offset, ios::beg);
    fs.write(buffer.data(), size);
  }


  bool isCD(string device) {
    return device.find("CD-ROM") != string::npos;
  }

  bool isDVD(string device) {
    return device.find("DVD") != string::npos;
  }

private:
  string device;
  fstream fs;
};

int main() {
  CDROM cdrom("/dev/sr0");
  cdrom.open();

  if (cdrom.isCD()) {
    cout << "CD-ROM Detected" << endl;
  } else if (cdrom.isDVD()) {
    cout << "DVD Detected" << endl;
  } else {
    cout << "Unknown Device" << endl;
  }

  cdrom.close();
  return 0;
}
