#pragma once
#include <bits/stdc++.h>
#include "scheduler.h"
using namespace std;

TimeSlot::TimeSlot(int id, double start, double ending) {
    this->id = id;
    this->ending = ending;
    this->start = start;
}

Process::Process(int priority, double arrival_time, double burst) {
    this->priority = priority;
    this->burst = burst;
    this->arrival_time = arrival_time;

}

Process::Process(int id, int priority, double arrival_time, double burst) {
    this->id = id;
    this->priority = priority;
    this->burst = burst;
    this->arrival_time = arrival_time;

}

bool Arrivalcomparator::operator()(Process const& p1, Process const& p2) {

    return p1.arrival_time > p2.arrival_time;
}


bool Shortestcomparator::operator()(Process const& p1, Process const& p2) {

    return p1.burst > p2.burst;
}

bool Prioritycomparator::operator()(Process const& p1, Process const& p2) {

    return p1.priority > p2.priority;
}

priority_queue<Process, vector<Process>, Arrivalcomparator> vectorToQueue(vector<Process> totalProcesses) {

    priority_queue<Process, vector<Process>, Arrivalcomparator>pq;

    for (int i = 0; i < totalProcesses.size(); i++) {
        pq.push(totalProcesses[i]);
    }
    return pq;
}

struct compareBurst {
    bool operator() (Process const& a, Process const& b) const {
        return a.burst > b.burst;
    }
} myobj1;
struct compareArrival {
    bool operator() (Process const& a, Process const& b) const {
        return a.arrival_time < b.arrival_time;
    }
} myobj2;
struct comparePriority {
    bool operator() (Process const& a, Process const& b) const {
        return a.priority > b.priority;
    }
} myobj3;
vector<TimeSlot> FCFS(vector<Process> totalProcesses) {

    priority_queue<Process, vector<Process>, Arrivalcomparator> pq = vectorToQueue(totalProcesses);

    vector<TimeSlot> TimeSlotsVector;
    for (int i = 0; i < totalProcesses.size(); i++) {
        if ((i > 0) && (pq.top().arrival_time <= TimeSlotsVector[i - 1].ending)) {
            TimeSlot t = TimeSlot(pq.top().id, TimeSlotsVector[i - 1].ending, TimeSlotsVector[i - 1].ending + pq.top().burst);
            TimeSlotsVector.push_back(t);
        }
        else {
            TimeSlot t = TimeSlot(pq.top().id, pq.top().arrival_time, pq.top().arrival_time + pq.top().burst);
            TimeSlotsVector.push_back(t);
        }
        pq.pop();

    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}

vector<TimeSlot> SJF(vector<Process> totalProcesses) {

    priority_queue<Process, vector<Process>, Arrivalcomparator> pq = vectorToQueue(totalProcesses);
    priority_queue<Process, vector<Process>, Shortestcomparator>sq;
    vector<TimeSlot> TimeSlotsVector;
    double quantum = pq.top().arrival_time;
    for (int i = 0; i < totalProcesses.size(); i++) {
        if ((pq.top().arrival_time > quantum) && (sq.empty())) {
            quantum = pq.top().arrival_time;
        }
        while (pq.top().arrival_time <= quantum && !pq.empty()) {
            sq.push(pq.top());
            pq.pop();
        }
        if (i == 0 || sq.top().arrival_time > TimeSlotsVector[i - 1].ending) {
            TimeSlot t = TimeSlot(sq.top().id, sq.top().arrival_time, sq.top().arrival_time + sq.top().burst);
            TimeSlotsVector.push_back(t);

        }
        else {
            TimeSlot t = TimeSlot(sq.top().id, TimeSlotsVector[i - 1].ending, TimeSlotsVector[i - 1].ending + sq.top().burst);
            TimeSlotsVector.push_back(t);

        }
        quantum += sq.top().burst;
        sq.pop();


    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}
vector<TimeSlot> SJFPrem(vector<Process> totalProcesses) {

    sort(totalProcesses.begin(), totalProcesses.end(), myobj2);
    priority_queue<Process, vector<Process>, compareBurst> pqProcesses;
    vector<TimeSlot> TimeSlotsVector;

    size_t i = 0;
    while (i < totalProcesses.size() || !pqProcesses.empty()) {

        double currentArrival{};
        if (TimeSlotsVector.size() == 0)
            currentArrival = totalProcesses.at(i).arrival_time;
        else
            currentArrival = TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending;


        for (; i < totalProcesses.size(); i++) {
            if (currentArrival >= totalProcesses.at(i).arrival_time)
                pqProcesses.push(totalProcesses.at(i));
            else
                break;
        }

        double currentBurst;
        if (i < totalProcesses.size())
            currentBurst = totalProcesses.at(i).arrival_time - currentArrival;
        else
            currentBurst = DBL_MAX;

        if (!pqProcesses.empty()) {
            Process p = pqProcesses.top();
            pqProcesses.pop();
            if (p.burst - currentBurst > 0 && TimeSlotsVector.size() > 0
                && TimeSlotsVector.at(TimeSlotsVector.size() - 1).id == p.id) {

                TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending = currentArrival + currentBurst;
                p.burst = (p.burst - currentBurst);
                pqProcesses.push(p);
            }
            else if (p.burst - currentBurst > 0) {
                TimeSlot ts1(p.id, currentArrival, currentArrival + currentBurst);
                TimeSlotsVector.push_back(ts1);
                p.burst = (p.burst - currentBurst);
                pqProcesses.push(p);
            }
            else if (TimeSlotsVector.size() > 0
                && TimeSlotsVector.at(TimeSlotsVector.size() - 1).id == p.id) {
                TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending = currentArrival + p.burst;
            }
            else {
                TimeSlot ts1(p.id, currentArrival, currentArrival + p.burst);
                TimeSlotsVector.push_back(ts1);
            }
        }
        else {
            TimeSlotsVector.push_back(TimeSlot(-1, currentArrival, currentArrival + currentBurst));
        }
    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}

vector<TimeSlot> Priority(vector<Process> totalProcesses) {

    priority_queue<Process, vector<Process>, Arrivalcomparator> pq = vectorToQueue(totalProcesses);
    priority_queue<Process, vector<Process>, Prioritycomparator>Q;
    vector<TimeSlot> TimeSlotsVector;
    double quantum = pq.top().arrival_time;
    for (int i = 0; i < totalProcesses.size(); i++) {
        if ((pq.top().arrival_time > quantum) && (Q.empty())) {
            quantum = pq.top().arrival_time;
        }
        while (pq.top().arrival_time <= quantum && !pq.empty()) {
            Q.push(pq.top());
            pq.pop();
        }
        if (i == 0 || Q.top().arrival_time > TimeSlotsVector[i - 1].ending) {
            TimeSlot t = TimeSlot(Q.top().id, Q.top().arrival_time, Q.top().arrival_time + Q.top().burst);
            TimeSlotsVector.push_back(t);

        }
        else {
            TimeSlot t = TimeSlot(Q.top().id, TimeSlotsVector[i - 1].ending, TimeSlotsVector[i - 1].ending + Q.top().burst);
            TimeSlotsVector.push_back(t);

        }
        quantum += Q.top().burst;
        Q.pop();


    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}
vector <TimeSlot> PriorityPrem(vector <Process> totalProcesses) {
    sort(totalProcesses.begin(), totalProcesses.end(), myobj2);
    priority_queue<Process, vector<Process>, comparePriority> pqProcesses;
    vector<TimeSlot> TimeSlotsVector;

    size_t i = 0;
    while (i < totalProcesses.size() || !pqProcesses.empty()) {

        double currentArrival{};
        if (TimeSlotsVector.size() == 0)
            currentArrival = totalProcesses.at(i).arrival_time;
        else
            currentArrival = TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending;


        for (; i < totalProcesses.size(); i++) {
            if (currentArrival >= totalProcesses.at(i).arrival_time)
                pqProcesses.push(totalProcesses.at(i));
            else
                break;
        }

        double currentBurst;
        if (i < totalProcesses.size())
            currentBurst = totalProcesses.at(i).arrival_time - currentArrival;
        else
            currentBurst = DBL_MAX;

        if (!pqProcesses.empty()) {
            Process p = pqProcesses.top();
            pqProcesses.pop();
            if (p.burst - currentBurst > 0 && TimeSlotsVector.size() > 0
                && TimeSlotsVector.at(TimeSlotsVector.size() - 1).id == p.id) {

                TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending = currentArrival + currentBurst;
                p.burst = (p.burst - currentBurst);
                pqProcesses.push(p);
            }
            else if (p.burst - currentBurst > 0) {
                TimeSlot ts1(p.id, currentArrival, currentArrival + currentBurst);
                TimeSlotsVector.push_back(ts1);
                p.burst = (p.burst - currentBurst);
                pqProcesses.push(p);
            }
            else if (TimeSlotsVector.size() > 0
                && TimeSlotsVector.at(TimeSlotsVector.size() - 1).id == p.id) {
                TimeSlotsVector.at(TimeSlotsVector.size() - 1).ending = currentArrival + p.burst;
            }
            else {
                TimeSlot ts1(p.id, currentArrival, currentArrival + p.burst);
                TimeSlotsVector.push_back(ts1);
            }
        }
        else {
            TimeSlotsVector.push_back(TimeSlot(-1, currentArrival, currentArrival + currentBurst));
        }
    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}

vector<TimeSlot> RoundRobin(vector<Process> totalProcesses, double CPUDuration){

    priority_queue<Process, vector<Process>,Arrivalcomparator>pq = vectorToQueue(totalProcesses);
    vector<TimeSlot> TimeSlotsVector;
    double quantum = 0;
    while(!pq.empty()){
            if(pq.top().arrival_time<=quantum){
                //we have process have burst more than quantum of cpu
                //this process will continue execution later
                if(pq.top().burst>CPUDuration){
                    TimeSlot t=TimeSlot(pq.top().id,quantum,quantum+CPUDuration);
                    TimeSlotsVector.push_back(t);
                    Process temp=Process(pq.top().id,pq.top().priority,quantum+CPUDuration,pq.top().burst-CPUDuration);
                    pq.pop();
                    pq.push(temp);
                    quantum+=CPUDuration;
                }
                // we have process have burst less than or equal quantum of cpu
                // this process will finish execution here
                else{
                    TimeSlot t=TimeSlot(pq.top().id,quantum,quantum+pq.top().burst);
                    TimeSlotsVector.push_back(t);
                    quantum+=pq.top().burst;
                    pq.pop();
                }

            }
            else{
                quantum=pq.top().arrival_time;
            }

    }
    calculateAvg(TimeSlotsVector, totalProcesses);
    return TimeSlotsVector;
}
double TimeSlot::avg;
void calculateAvg(vector<TimeSlot>& v, vector<Process>& p)
{
    double res = 0;
    vector<bool> taken(p.size(), 0);
    for (int i = v.size()-1; i >= 0; i--)
    {
        int curr = v[i].id;
        if (!taken[curr])
        {
            res += v[i].ending - p[curr].arrival_time - p[curr].burst;
            taken[curr] = 1;
        }
    }
    //timeslot::timeslot::avg;
    TimeSlot::avg = res / p.size();
}
