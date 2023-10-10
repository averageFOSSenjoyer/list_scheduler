# List Scheduler
---
This is a list scheduler implemented with C++ using slack heuristics.

### Dependencies
```
gcc >= 13.1
boost/libboost-all-dev >= 1.74
graphviz (if you want to see dependency graph)
```

#### Instruction for g++13 on ubuntu
```
apt update && apt install software-properties-common
add-apt-repository ppa:ubuntu-toolchain-r/test
apt install g++-13

(Do this before cmake)
export CXX=/usr/bin/g++-13
```


### Build

```
git clone https://github.com/averageFOSSenjoyer/list_scheduler.git
cd list_scheduler
mkdir build && cd build
cmake ..
cmake --build .
```

### Usage
```
[frank@homeDesktop build]$ ./scheduler --help
Options:
  -g [ --graph ] arg       Graph file - Topological dependency for scheduling
  -t [ --timing ] arg      Timing file - Latency of operators
  -c [ --constraints ] arg Constraints file - Number of operators
  -v [ --verbose ]         Verbose output
  -h [ --help ]            Prints the help menu
Usage:
   ./scheduler -g <graph file> -t <timing file> -c <constraints file>
```

### Result
Five files will be produced: `asap.txt` `alap.txt` `slack.txt` `list_scheduling.txt` `graph.dot`