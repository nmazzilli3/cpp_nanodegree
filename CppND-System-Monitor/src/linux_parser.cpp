#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
    return kernel;
  }
  stream.close();
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  //https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
  string line;
  float total_used_memory = 0.0; 
  float non_cache_memory = 0.0;  
  float cache_memory = 0.0; 
  float MemTotal = 0.0; 
  float MemFree = 0.0;
  float Buffers = 0.0;
  float Cached = 0.0; 
  float SReclaimable = 0.0;
  float Shmem = 0.0;
  string name; 
  float memory; 
  string size; 
  float kBtoGb = 1024.0*1024.0;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) 
  {
      while (std::getline(filestream, line)) 
      {
        std::istringstream linestream(line);
        while (linestream >> name >> memory >> size) 
        {
          if (name == "MemTotal:") 
          {
            MemTotal = memory; 
            MemTotal = MemTotal/kBtoGb;
          }
          else if(name == "MemFree:")
          {
            MemFree = memory; 
            MemFree = MemFree/kBtoGb;
          }
          else if(name == "Buffers:")
          {
            Buffers = memory; 
            Buffers = Buffers/kBtoGb;
          }
          else if(name == "Cached:")
          {
            Cached = memory;
            Cached = Cached/kBtoGb;
          }
          else if(name == "SReclaimable:")
          {
            SReclaimable = memory;
            SReclaimable = SReclaimable/kBtoGb;
          }
          else if(name == "Shmem:")
          {
            Shmem = memory; 
            Shmem = Shmem/kBtoGb;
          }
          else
          {
            //do nothing
          }
          
        }
      }

  }
  total_used_memory = MemTotal-MemFree; 
  cache_memory = Cached + SReclaimable - Shmem; 
  non_cache_memory = total_used_memory - (Buffers+cache_memory);
  float percentage = (cache_memory+non_cache_memory+Buffers)/MemTotal;

  return percentage; 
  
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  string line;
  long up_time = 0;
  long idle_time = 0;
  std::string::size_type sz; 
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
    return up_time;
  }
    
  filestream.close();
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) 
{ 
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system jiffies is just jargin time measured in USER_HZ
long LinuxParser::ActiveJiffies() 
{ 
  //user,nice,system,irq,softirq,steal,guest are all active
  long activetime = 0; 
  /* 
  Values nice for debugging
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
  */
  long userjif = std::stol(LinuxParser::CpuUtilization()[kUser_]);
  long nicejif = std::stol(LinuxParser::CpuUtilization()[kNice_]);
  long systemjif = std::stol(LinuxParser::CpuUtilization()[kSystem_]);
  long irqjif = std::stol(LinuxParser::CpuUtilization()[kIRQ_]);
  long softirqjif = std::stol(LinuxParser::CpuUtilization()[kSoftIRQ_]);
  long stealjif = std::stol(LinuxParser::CpuUtilization()[kSteal_]);
  long guestjf = std::stol(LinuxParser::CpuUtilization()[kGuest_]);
  long guestnicejf = std::stol(LinuxParser::CpuUtilization()[kGuestNice_]);
  activetime = userjif+nicejif+irqjif+softirqjif+stealjif+guestjf+guestnicejf;
  return activetime; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  //idle is idle and iowait
  long idletime = 0; 
  idletime = std::stol(LinuxParser::CpuUtilization()[kIdle_])+std::stol(LinuxParser::CpuUtilization()[kIOwait_]);
  return idletime; 
}

// TODO: Read and return CPU utilization
/*
Explanation of data 
The very first  "cpu" line aggregates the  numbers in all  of the other "cpuN"
lines.  These numbers identify the amount of time the CPU has spent performing
different kinds of work.  Time units are in USER_HZ (typically hundredths of a
second).  The meanings of the columns are as follows, from left to right:

- user: normal processes executing in user mode
- nice: niced processes executing in user mode
- system: processes executing in kernel mode
- idle: twiddling thumbs
- iowait: In a word, iowait stands for waiting for I/O to complete. But there
  are several problems:
  1. Cpu will not wait for I/O to complete, iowait is the time that a task is
     waiting for I/O to complete. When cpu goes into idle state for
     outstanding task io, another task will be scheduled on this CPU.
  2. In a multi-core CPU, the task waiting for I/O to complete is not running
     on any CPU, so the iowait of each CPU is difficult to calculate.
  3. The value of iowait field in /proc/stat will decrease in certain
     conditions.
  So, the iowait is not reliable by reading from /proc/stat.
- irq: servicing interrupts
- softirq: servicing softirqs
- steal: involuntary wait
- guest: running a normal guest
- guest_nice: running a niced guest



enum in header

*/
vector<string> LinuxParser::CpuUtilization() 
{ 
  string line;
  long user = 0; 
  long nice = 0;  
  long system = 0; 
  long idle = 0; 
  long iowait = 0; 
  long irq = 0; 
  long softirq = 0; 
  long steal = 0; 
  long guest = 0; 
  long guest_nice = 0;
  string name; 
  std::vector<long> cpu; 
  std::vector<long> cpu1; 
  std::vector<long> cpu2; 
  std::vector<long> cpu3; 
  std::vector<long> cpu0; 
  std::vector<string> aggregated; 

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) 
  {
      while (std::getline(filestream, line)) 
      {
        std::istringstream linestream(line);
        while (linestream >> name >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) 
        {
          if (name == "cpu") 
          {
            cpu.push_back(user); 
            cpu.push_back(nice); 
            cpu.push_back(system); 
            cpu.push_back(idle); 
            cpu.push_back(iowait); 
            cpu.push_back(irq); 
            cpu.push_back(softirq); 
            cpu.push_back(steal); 
            cpu.push_back(guest); 
            cpu.push_back(guest_nice); 
          }
          else if(name == "cpu0")
          {
            cpu0.push_back(user); 
            cpu0.push_back(nice); 
            cpu0.push_back(system); 
            cpu0.push_back(idle); 
            cpu0.push_back(iowait); 
            cpu0.push_back(irq); 
            cpu0.push_back(softirq); 
            cpu0.push_back(steal); 
            cpu0.push_back(guest);    
            cpu0.push_back(guest_nice);          
          }
          else if(name == "cpu1")
          {
            cpu1.push_back(user); 
            cpu1.push_back(nice); 
            cpu1.push_back(system); 
            cpu1.push_back(idle); 
            cpu1.push_back(iowait); 
            cpu1.push_back(irq); 
            cpu1.push_back(softirq); 
            cpu1.push_back(steal); 
            cpu1.push_back(guest); 
            cpu1.push_back(guest_nice); 
          }
          else if(name == "cpu2")
          {
            cpu2.push_back(user); 
            cpu2.push_back(nice); 
            cpu2.push_back(system); 
            cpu2.push_back(idle); 
            cpu2.push_back(iowait); 
            cpu2.push_back(irq); 
            cpu2.push_back(softirq); 
            cpu2.push_back(steal); 
            cpu2.push_back(guest); 
            cpu2.push_back(guest_nice); 
          }
          else if(name == "cpu3")
          {
            cpu3.push_back(user); 
            cpu3.push_back(nice); 
            cpu3.push_back(system); 
            cpu3.push_back(idle); 
            cpu3.push_back(iowait); 
            cpu3.push_back(irq); 
            cpu3.push_back(softirq); 
            cpu3.push_back(steal); 
            cpu3.push_back(guest); 
            cpu3.push_back(guest_nice); 
          }
          else
          {
            //do nothing 
          }
        }
      }
      

  }
  /*
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};

  */
  user = cpu[kUser_]+cpu0[kUser_]+cpu1[kUser_]+cpu2[kUser_]+cpu3[kUser_];
  nice = cpu[kNice_]+cpu0[kNice_]+cpu1[kNice_]+cpu2[kNice_]+cpu3[kNice_];
  system = cpu[kSystem_]+cpu0[kSystem_]+cpu1[kSystem_]+cpu2[kSystem_]+cpu3[kSystem_];
  idle = cpu[kIdle_]+cpu0[kIdle_]+cpu1[kIdle_]+cpu2[kIdle_]+cpu3[kIdle_];
  iowait = cpu[kIOwait_]+cpu0[kIOwait_]+cpu1[kIOwait_]+cpu2[kIOwait_]+cpu3[kIOwait_];
  irq = cpu[kIRQ_]+cpu0[kIRQ_]+cpu1[kIRQ_]+cpu2[kIRQ_]+cpu3[kIRQ_];
  softirq = cpu[kSoftIRQ_]+cpu0[kSoftIRQ_]+cpu1[kSoftIRQ_]+cpu2[kSoftIRQ_]+cpu3[kSoftIRQ_];
  steal = cpu[kSteal_]+cpu0[kSteal_]+cpu1[kSteal_]+cpu2[kSteal_]+cpu3[kSteal_];
  guest = cpu[kGuest_]+cpu0[kGuest_]+cpu1[kGuest_]+cpu2[kGuest_]+cpu3[kGuest_];
  guest_nice = cpu[kGuestNice_]+cpu0[kGuestNice_]+cpu1[kGuestNice_]+cpu2[kGuestNice_]+cpu3[kGuestNice_];

  aggregated.push_back(std::to_string(user)); 
  aggregated.push_back(std::to_string(nice)); 
  aggregated.push_back(std::to_string(system)); 
  aggregated.push_back(std::to_string(idle)); 
  aggregated.push_back(std::to_string(iowait)); 
  aggregated.push_back(std::to_string(irq)); 
  aggregated.push_back(std::to_string(softirq)); 
  aggregated.push_back(std::to_string(steal)); 
  aggregated.push_back(std::to_string(guest)); 
  aggregated.push_back(std::to_string(guest_nice)); 

  cpu0.clear();
  cpu1.clear();
  cpu2.clear();
  cpu3.clear();


  filestream.close();
  return aggregated;         

}

float LinuxParser::CpuUtilization(int pid)
{
  return 0.0;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
 { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          
          return value;
        }
      }
    }
  } 
  filestream.close();
  return value;
  }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses()
 { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          
          return value;
        }
      }
    }
  } 
  filestream.close();
  return value;
  }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{ 
  string line; 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    return line;
  }
  filestream.close();
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{ 
  string line; 
  string name; 
  string size; 
  string units;
  string Vm_name = "VmSize:";
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> name >> size >> units) 
      {
        if (name == Vm_name) 
        {
          
          return size;
        }
      }
    }
  } 
  filestream.close();
  return size;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{ 
  //UID value in /proc/[pid]/status is Real,Effective, Saved set, and filesystem UIDS
  string line; 
  string value; 
  string key; 
  string uid_name = "Uid:";
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == uid_name) 
        {
          
          return value;
        }
      }
    }
  } 
  filestream.close();
  return value; 

}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{ 
  //kPasswordPath 
  string line; 
  std::ifstream filestream(kPasswordPath);
  string find_pid_name = "x" + LinuxParser::Uid(pid);
  if (filestream.is_open()) 
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    if(line.find(find_pid_name) != std::string::npos)
    {
      return line.substr(0,line.find(":"));
    }

  }
  filestream.close();
  return line; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{ 
  //starting element 21 process time since boot
  // divide by sysconf(_SC_CLK_TCK))
  long  starttime; 
  float time_return; 
  string line; 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) 
  {
    std::istringstream linestream(line);
    for(int idx =0; idx<21;idx++)
    {
      linestream >> starttime;
    }

  } 
  time_return = long(starttime/sysconf(_SC_CLK_TCK));
  filestream.close();
  return long(time_return);

}