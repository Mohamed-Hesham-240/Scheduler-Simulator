#pragma once
#include <bits/stdc++.h>
using namespace std;
class TimeSlot
{
public:
    int id;
    static double avg;
    double start;
    double ending;
    TimeSlot(int id, double start, double ending);
};
class Process {
public:
    int id = -1;
    double burst;
    double arrival_time;
    int priority;
    Process(int priority, double arrival_time, double burst);
    Process(int id, int priority, double arrival_time, double burst);
};
struct Arrivalcomparator {
    bool operator()(Process const& p1, Process const& p2);
};
struct Shortestcomparator {
    bool operator()(Process const& p1, Process const& p2);
};
struct Prioritycomparator {
    bool operator()(Process const& p1, Process const& p2);
};
priority_queue<Process, vector<Process>, Arrivalcomparator> vectorToQueue(vector<Process> totalProcesses);
vector<TimeSlot> FCFS(vector<Process> totalProcesses);
vector<TimeSlot> SJF(vector<Process> totalProcesses);
vector<TimeSlot> SJFPrem(vector<Process> totalProcesses);
vector<TimeSlot> Priority(vector<Process> totalProcesses);
vector<TimeSlot> PriorityPrem(vector<Process> totalProcesses);
vector<TimeSlot> RoundRobin(vector<Process> totalProcesses, double CPUDuration);
void calculateAvg(vector<TimeSlot>& v, vector<Process>& p);
