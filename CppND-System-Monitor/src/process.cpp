#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "system.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() 
{ 
    return pid_; 
}
    /*
**********************************************************
Algoithm
// Guest time is already accounted in usertime
usertime = usertime - guest;                     # As you see here, it subtracts guest from user time
nicetime = nicetime - guestnice;                 # and guest_nice from nice time
// Fields existing on kernels >= 2.6
// (and RHEL's patched kernel 2.4...)
idlealltime = idletime + ioWait;                 # ioWait is added in the idleTime
systemalltime = systemtime + irq + softIrq;
virtalltime = guest + guestnice;
totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
NonIdle = user + nice + system + irq + softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald

    */
// TODO: Return this process's CPU utilization
float Process::CpuUtilization() 
{ 
    /*
    std::vector<string> cpu_aggregated = LinuxParser::CpuUtilization(); 
    long userjif = std::stol(cpu_aggregated[LinuxParser::kUser_]);
    long nicejif = std::stol(cpu_aggregated[LinuxParser::kNice_]);
    long systemjif = std::stol(cpu_aggregated[LinuxParser::kSystem_]);
    long irqjif = std::stol(cpu_aggregated[LinuxParser::kIRQ_]);
    long softirqjif = std::stol(cpu_aggregated[LinuxParser::kSoftIRQ_]);
    long stealjif = std::stol(cpu_aggregated[LinuxParser::kSteal_]);
    long guestjf = std::stol(cpu_aggregated[LinuxParser::kGuest_]);
    long guestnicejf = std::stol(cpu_aggregated[LinuxParser::kGuestNice_]);
    long idletime = std::stol(cpu_aggregated[LinuxParser::kIdle_])+std::stol(cpu_aggregated[LinuxParser::kIOwait_]);
    */ 
    static long nonidle; 
    nonidle = LinuxParser::ActiveJiffies() ;
    static long idle; 
    idle = LinuxParser::IdleJiffies();
    static long total; 
    total = LinuxParser::Jiffies(); 

    static long PrevIdle; 
    static long PrevNonIdle; 
    static long PrevTotal; 

    long totald = total-PrevTotal;
    long idled = idle-PrevIdle;
    float num = float(totald-idled);
    float denom = float(totald);


    cpu_ = num/denom;

    PrevIdle = idle;
    PrevTotal = total; 
    PrevNonIdle = nonidle; 

    return cpu_; 
}

// TODO: Return the command that generated this process
string Process::Command() 
{ 
    cmd_ = LinuxParser::Command(pid_) ;
    return cmd_;
}

// TODO: Return this process's memory utilization
string Process::Ram() 
{ 
    ram_ = LinuxParser::Ram(pid_);
    return ram_; 
}

// TODO: Return the user (name) that generated this process
string Process::User() 
{ 
    user_ = LinuxParser::User(pid_);
    return user_; 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
    uptime_ = LinuxParser::UpTime(pid_);
    return uptime_; 
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const 
{ 
    return a.cpu_< cpu_; 
}