//
//  main.cpp
//  DiscreteEventSimulator
//
//  Created by Arunima Mitra on 2/22/23.
//  Copyright © 2023 Arunima. All rights reserved.
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <list>


using namespace std;

enum transitionNextState {TRANS_TO_READY, TRANS_TO_BLOCK, TRANS_TO_RUN, TRANS_TO_PREEMPT};

class Process{
public:
    int pid, ofsInd;
    int arrivalTime,executionTime,cpuBurst,IOBurst, remainingWork, blockedTotal, timeOfTransition;
    int waiting, staticPriority, processPrio;
    int cpuBurstRemaining;
    string currentState;
    
    int getPID(){ return pid; }
    int getOFS(){ return ofsInd; }
    int getArrivalTime(){ return arrivalTime; }
    int getExecutionTime(){ return executionTime; }
    int getRemainingWorkTime(){ return remainingWork; }
    int getCPUBurst(){ return cpuBurst; }
    int getIOBurst(){ return IOBurst; }
    int getProcessPriority(){ return processPrio; }
    int getStaticPriority(){ return staticPriority; }
    const int getWaitingTime(){ return waiting; }
    int getBlocked(){return blockedTotal;}
    int getTimeOfTransition(){ return timeOfTransition;}
    int getCPUBurstRemaining() { return cpuBurstRemaining; }
    
    void setTimeOfTransition( int t){ timeOfTransition = t;}
    void setRemainingWorkTime(int s){ remainingWork = s;} // updating exec time left
    void setStaticPriority(int s){ staticPriority = s;}
    void setProcessPriority(int p){processPrio=p;}
    void setWaitingTime(int w){ waiting = w;}
    void setBlocked(int b){blockedTotal=b;}
    void setCPUBurstRemaining(int t){ cpuBurstRemaining=t;}
};


class Event{
public:
    int pid, processStartTime,ofsInd, prev, processEndTime;
    int eventTimestamp;
    Process * process;
    string oldState, newState,extra;
    transitionNextState transitionTo;
    void setValues (int processID, string oldS, string newS, string transitionInfo, Process * proc, int previousInfo, int ofsI, int time, transitionNextState trans){
        pid=processID;
        newState = newS;
        oldState = oldS;
        extra = transitionInfo;
        process = proc;
        prev=previousInfo;
        ofsInd=ofsI;
        eventTimestamp = time;
        transitionTo = trans;
    }
    
    int getPID() {return pid;}
    
    void printEvent(){
//        if(oldState.compare("CREATED")==0)
//            cout<<processStartTime<<"\t"<<pid<<"\t"<<prev<<"\t"<<oldState<<"->"<<newState<<"\t"<<extra<<endl;
//        else cout<<timestamp<<"\t"<<pid<<"\t"<<prev<<"\t"<<oldState<<"->"<<newState<<"\t"<<extra<<endl;
//
        
        cout<<eventTimestamp<<"\t"<<pid<<"\t"<<oldState<<"->"<<newState<<"\t"<<extra<<endl;
    }
};


int maxprio=4;

class Scheduler
{

    public:
    deque<Process*> readyQueue;
        virtual void addProcess(Process* process)=0;
        virtual Process* getNextProcess()=0;
//        virtual void rmProcess()=0;
   virtual void print()=0;
};



class FCFS:public Scheduler{
public:
    void addProcess( Process* process){
        //cout<<"adding process "<<process->pid<<endl;
        process->currentState="READY";
        readyQueue.push_back(process);
        Process* value;
        value=readyQueue.front();
        
        //cout<<"at front of queue, we now have pid "<<value->pid<<endl;
    }
    
    //0x00007ffeefbff0b8
 Process * getNextProcess()
    {
        if(readyQueue.empty()) return NULL;
        
        Process *proc;
        proc=readyQueue.front();
        readyQueue.erase(readyQueue.begin());
        return proc;
    }
    
    void rmProcess(){
        readyQueue.pop_front();
        cout<<readyQueue.size();
        return;
    }
    
    
    void print(){
        //cout<<readyQueue.size();
        deque<Process*> readyQueue2=readyQueue;
        cout<<readyQueue2.front()<<" hello "<<readyQueue2.front()->getPID();
        cout<<endl;
    }
};


class LCFS:public Scheduler{
public:
    void addProcess( Process* process){
        process->currentState="READY";
        readyQueue.push_back(process);
    }
    
 Process * getNextProcess()
    {
        Process* proc;
        if(readyQueue.empty()) return NULL;
        
        proc=readyQueue.back();
        readyQueue.erase(readyQueue.end());
        return proc;
    }
    
    void rmProcess(){
        readyQueue.pop_front();
        cout<<readyQueue.size();
        return;
    }
    
    
    void print(){
        deque<Process*> readyQueue2=readyQueue;
        cout<<readyQueue2.front()<<" hello "<<readyQueue2.front()->getPID();
        cout<<endl;
    }
};

class SRTF:public Scheduler{
public:
    void addProcess( Process* process){
        int j=0;
        bool check=false;
        for(deque<Process *>::iterator i = readyQueue.begin() ; i!= readyQueue.end(); i++)
        {
            j++;
            Process *p = *i;
            if(p->getRemainingWorkTime()> process->getRemainingWorkTime())
            {
                check=true;
                break;
            }
        }
        process->currentState="READY";
        if(!check) readyQueue.push_back(process);
        else
        {
            deque<Process *>::iterator it = readyQueue.begin();
            advance(it,j-1);
            readyQueue.insert(it,process);
        }
    }
    
    Process * getNextProcess()
    {
        if(readyQueue.empty()) return NULL;
        Process *proc;
        proc=readyQueue.front();
        readyQueue.erase(readyQueue.begin());
        return proc;
        
    }
};



class RR:public Scheduler{
public:
    void addProcess( Process* process){
        //cout<<"adding process "<<process->pid<<endl;
        process->currentState="READY";
        readyQueue.push_back(process);
        Process* value;
        value=readyQueue.front();
    }
 Process * getNextProcess()
    {
       if(readyQueue.empty()) return NULL;
       Process *proc;
       proc=readyQueue.front();
       readyQueue.erase(readyQueue.begin());
       return proc;
    }
    
    void rmProcess(){
        readyQueue.pop_front();
        cout<<readyQueue.size();
        return;
    }
    
};

class PriorityScheduler:public Scheduler{
    vector<list<Process*> > *activeQ, *expiredQ, q1,q2;
    public:
    void initialize()
    {
        for(int i=0; i<maxprio; i++)
        {
            q1.push_back(list<Process *>());
            q2.push_back(list<Process *>());
        }
        activeQ = &q1;
        expiredQ = &q2;
        
    }
    void addProcess(Process * process)
    {
        if(activeQ==NULL) initialize();
        if(process->currentState.compare("PREEMPT")==0)
        {
            process->setProcessPriority(process->getProcessPriority()-1);
            cout<<"Entering preempt condition "<<process->getProcessPriority()<<endl;
            
        }
        process->currentState="READY";
        if(process->getProcessPriority()<0){
            process->setProcessPriority(process->getStaticPriority()-1);
            expiredQ->at(process->getProcessPriority()).push_back(process);
        }
        else{
//          cout << "size of active queue" <<active_queue->size()<<endl;
//          cout<<"process' priority = "<<process->getProcessPriority()<<endl;
            activeQ->at(process->getProcessPriority()).push_back(process);
        }
    }
    Process * getNextProcess()
    {
        bool anyActiveProc=false;
        for(int i=0;i<activeQ->size();i++){
            if(activeQ->at(i).size()!=0){
                anyActiveProc=true;
                break;
            }
        }
        if(!anyActiveProc){
            vector<list<Process*> > *temp = activeQ;
            activeQ = expiredQ;
            expiredQ = temp;
        }
        int ind = -1;
        for(int i=activeQ->size()-1;i>=0;i--){
            if(activeQ->at(i).empty()==false){
                ind=i;
                break;
            }
        }

        if(ind==-1) return NULL;
        Process *proc;
        proc = activeQ->at(ind).front();
        activeQ->at(ind).pop_front();
        return proc;
    }
    
    void rmProcess(){
        readyQueue.pop_front();
        cout<<readyQueue.size();
        return;
    }
    
    
    void print(){
        //cout<<readyQueue.size();
        deque<Process*> readyQueue2=readyQueue;
        while(!readyQueue2.empty()) {
            cout<<readyQueue2.front()<<" hello "<<readyQueue2.front()->getPID()<<" priority="<<readyQueue2.front()->getProcessPriority();
            cout<<endl;
            readyQueue2.pop_front();
        }
    }
};


class PreemptivePriority:public Scheduler{
    vector<list<Process*> > *activeQ, *expiredQ, q1,q2;
    public:
    void initialize()
    {
        for(int i=0; i<maxprio; i++)
        {
            q1.push_back(list<Process *>());
            q2.push_back(list<Process *>());
        }
        activeQ = &q1;
        expiredQ = &q2;
        
    }
    void addProcess(Process * process)
    {
        if(activeQ==NULL) initialize();
        if(process->currentState.compare("PREEMPT")==0)
        {
            process->setProcessPriority(process->getProcessPriority()-1);
            //cout<<"Entering prempt condition "<<process->getProcessPriority()<<endl;
            
        }
        process->currentState="READY";
        if(process->getProcessPriority()<0){
            process->setProcessPriority(process->getStaticPriority()-1);
            expiredQ->at(process->getProcessPriority()).push_back(process);
        }
        else{
//          cout << "size of active queue" <<active_queue->size()<<endl;
//          cout<<"process' priority = "<<process->getProcessPriority()<<endl;
            activeQ->at(process->getProcessPriority()).push_back(process);
        }
    }
    Process * getNextProcess()
    {
        bool anyActiveProc=false;
        for(int i=0;i<activeQ->size();i++){
            if(activeQ->at(i).size()!=0){
                anyActiveProc=true;
                break;
            }
        }
        if(!anyActiveProc){
            vector<list<Process*> > *temp = activeQ;
            activeQ = expiredQ;
            expiredQ = temp;
        }
        int ind = -1;
        for(int i=activeQ->size()-1;i>=0;i--){
            if(activeQ->at(i).empty()==false){
                ind=i;
                break;
            }
        }

        if(ind==-1) return NULL;
        Process *proc;
        proc = activeQ->at(ind).front();
        activeQ->at(ind).pop_front();
        return proc;
    }
    
    void rmProcess(){
        readyQueue.pop_front();
        cout<<readyQueue.size();
        return;
    }
    
    
    void print(){
        //cout<<readyQueue.size();
        deque<Process*> readyQueue2=readyQueue;
        while(!readyQueue2.empty()) {
            cout<<readyQueue2.front()<<" hello "<<readyQueue2.front()->getPID()<<" priority="<<readyQueue2.front()->getProcessPriority();
            cout<<endl;
            readyQueue2.pop_front();
        }
    }
};





// keeping a track of time in blocked state here - time atleast one process was in blocked state
int timeInBlocked=0;
int timestampOfBlocked=-1;
// whenever a proc goes into blocked, update timeInBlocked+=ioWait and timestamp=current+ioWait
// do above calculation only if the block times are not overlapping!!!

struct LessThanPriority
{
  bool operator()( const Process& lhs,  const Process& rhs) const
  {
      return lhs.processPrio < rhs.processPrio;
  }
};

struct FirstCome
{
  bool operator()(const Event& lhs, const Event& rhs)
  {
      return lhs.processStartTime > rhs.processStartTime || (lhs.processStartTime==rhs.processStartTime && lhs.ofsInd>rhs.ofsInd);
      
  }
};

struct LastCome
{
  bool operator()(const Event& lhs, const Event& rhs)
  {
      return lhs.processStartTime > rhs.processStartTime || (lhs.processStartTime==rhs.processStartTime && lhs.ofsInd>rhs.ofsInd);
      
  }
};

struct LessThanWaiting
{
  bool operator()(const Process& lhs, const Process& rhs) const
  {
    return lhs.waiting < rhs.waiting;
  }
};

std::vector<int>randomValues;
std::vector<Process> createdProcesses;
std::map<int, std::string> mapper;
double summary[6];

void mappingOutput(int timestamp, Process * proc){
    int procArrival = proc->getArrivalTime();
    int procFinishingTime = timestamp;
    int turnaround = procFinishingTime-procArrival;
    int blockedTime = proc->blockedTotal;
    int timeInReadyQueue = turnaround-blockedTime-proc->getExecutionTime();
    int pid = proc->getPID();
    string str;
    
    if(pid/10!=0) str="00";
    if(pid/100!=0) str="0";
    else str="000";
    
    
    string value = str+to_string(pid)+":\t"+to_string(procArrival)+"\t"+to_string(proc->getExecutionTime())+"\t"+to_string(proc->getCPUBurst())+"\t"+to_string(proc->getIOBurst())+"\t"+to_string(proc->getProcessPriority())+"\t|\t"+to_string(procFinishingTime)+"\t"+to_string(turnaround)+"\t"+to_string(blockedTime)+"\t"+to_string(timeInReadyQueue);
    
    
    
    summary[0] = timestamp;
    summary[1]+=proc->getExecutionTime();
    summary[2]+=proc->getBlocked();
    summary[3]+=turnaround;
    summary[4]+=turnaround-blockedTime-proc->getExecutionTime();
    
    int key = pid;
    mapper[key] = value;
}

void printOutput(){
    for(int i=0;i<mapper.size();i++){
        auto value = mapper.find(i);
        cout<<value->second<<endl;
    }
    
    summary[1]=(summary[1]*100)/summary[0];
    summary[2] =(timeInBlocked*100)/summary[0];
    summary[3]=summary[3]/mapper.size();
    summary[4] = summary[4]/mapper.size();
    summary[5] =(100*mapper.size())/summary[0];
    cout<<"SUM:\t"<<summary[0];
     cout<<setprecision(2)<<fixed<<"\t"<<summary[1]<<"\t";
     cout<<setprecision(2)<<summary[2]<<"\t"<<setprecision(2)<<summary[3]<<"\t"<<setprecision(2)<<fixed<<summary[4]<<"\t"<<setprecision(3)<<summary[5]<<endl;
    
}


int ofs=0;
int quantum=10; //ofs<40000 (size of rfile provided)
ifstream file;


priority_queue<Event, vector<Event>, FirstCome> eventQueueFCFS;
priority_queue<Event, vector<Event>, LastCome> eventQueueLCFS;


void createRandomArray(char* fileName){
    ifstream rfile;
    rfile.open(fileName);
    
    if (rfile.is_open()==false) {
        cout << "The gates of hell will open if I continue to parse with a non-existent file <%s>. Hence I decide to exit this program at this point avoiding insufferable pain\n" << fileName;
        exit(1);
    }
    int size;
    while(rfile >> size)
        randomValues.push_back(size);
    randomValues.erase(randomValues.begin());
    //cout<<randomValues.size()<<endl;
    quantum = randomValues.size(); // default value of quantum
}

int myrandom(int burst) { return 1 + (randomValues.at(ofs) % burst); }




class EventManager{
public:
    list<Event> eventQueue;
    void addEvent(Event e){
        
        int i=0,j=0;
        for(list<Event>::iterator k = eventQueue.begin() ; k!= eventQueue.end(); k++){
            i++;
            Event e1 = *k;
            if(e1.eventTimestamp>e.eventTimestamp) {j=1;break;}
        }
        if(j==0) eventQueue.push_back(e);
        else
        {
            list<Event>::iterator l = eventQueue.begin();
            advance(l,i-1);
            eventQueue.insert(l,e);
        }
        
    }
    Event* getEvent(){
        return &eventQueue.front();
    }
    void removeEvent(){
        eventQueue.pop_front();
    }
    int getNextEventTime(){
        return eventQueue.front().eventTimestamp;
    }
    
    void removeEventsPreemption(Process * p){
        int ind=0;
        bool check=false;
        list<Event>::iterator i=eventQueue.begin();
        while(i!=eventQueue.end()){
            
            ind++;
            Event e = *i;
            if(e.process==p){
                check=true;
                break;
            }
            i++;
        }

        if(check)
        {
            list<Event>::iterator it = eventQueue.begin();
            advance(it,ind-1);
            eventQueue.erase(it);
        }
    }
    
};

EventManager initialize(char* fileName, int maxprio, string sch){
    file.open(fileName);
    string lineNow;
     
     if (file.is_open()==false) {
         cout << "The gates of hell will open if I continue to parse with a non-existent file <%s>. Hence I decide to exit this program at this point avoiding insufferable pain\n" << fileName;
         exit(1);
     }
    
    int arrival,execution,cb,ib;
    
    while(file >> arrival >> execution >> cb >> ib){
        
        int processPrio = myrandom(maxprio);
        ofs++;
        
        Process p;
        p.pid=createdProcesses.size();
        p.arrivalTime = arrival;
        p.executionTime = execution;
        p.remainingWork = execution;
        p.cpuBurst=cb;
        p.IOBurst=ib;
        p.ofsInd=ofs;
        p.processPrio=processPrio-1;
        p.blockedTotal=0;
        p.cpuBurstRemaining=0;
        p.setTimeOfTransition(0);
        p.currentState = "CREATE";
        p.staticPriority=processPrio;
        
//        Event e;
//        e.setValues(p.pid, "CREATED", "READY", "",p,0,ofs, arrival, TRANS_TO_READY);
//        if(sch.compare("FCFS")==0) eventQueueFCFS.push(e);
//        else eventQueueLCFS.push(e);
        //readyQueue.push(p);
        createdProcesses.push_back(p);
    }
    
    EventManager evm;
    cout<<"createdProcess.size() = "<<createdProcesses.size()<<endl;
    
    for(int i=0; i<createdProcesses.size();i++)
    {
        
        Event event;
        event.setValues(createdProcesses[i].pid, "CREATED", "READY", "", &createdProcesses[i], 0, ofs, createdProcesses[i].arrivalTime, TRANS_TO_READY);
        evm.addEvent(event);
        //event.printEvent();
    }
    
    return evm;
}

Scheduler * scheduler;

bool prio=false, preprio=false;

void Simulation(EventManager* em, string sch) {
    cout<<sch<<"\t"<<endl;
    int currTime;
    bool CALL_SCHEDULER=false;
    
    //prio=true;
    list<Event> eventQueue=em->eventQueue;

    Process* CURRENT_RUNNING_PROCESS=NULL;
    //int ind=1, cpuTime=0;
    
    
    while(em->eventQueue.size()!=0) {
        //cout<<"iteration "<<ind<<endl;
        //ind++;
        Event* evt = em->getEvent();
        
        Process* proc = evt->process ; // this is the process the event works on
        
        currTime = evt->eventTimestamp;
        int transition = evt->transitionTo,timeOfTransition=currTime-proc->getTimeOfTransition();
        switch(transition) { // encodes where we come from and where we go
            case TRANS_TO_READY:
            {
                evt->printEvent();
                
                if(preprio){
                    if(proc->currentState.compare("BLOCK")==0)
                        proc->setProcessPriority(proc->getStaticPriority()-1);
                }
                
                if(preprio && CURRENT_RUNNING_PROCESS!=NULL){
                    //check here if process can be preempted or not
                    cout<<"PrioPreempt : ";
                    if(proc->currentState.compare("BLOCK")==0 || proc->currentState.compare("CREATE")==0){
                        cout<<"cond1 : yes";
                    int currentRunningPriority = CURRENT_RUNNING_PROCESS->getProcessPriority();
                    int readyProcessPriority = proc->getProcessPriority();
                    int evtTimestamp;
                        
                    for(list<Event>::iterator i=em->eventQueue.begin();i!=em->eventQueue.end();i++ ){
                        Event e2=*i;
                        if(e2.process==CURRENT_RUNNING_PROCESS) evtTimestamp = e2.eventTimestamp;
                    }
                        
                    if(currentRunningPriority<readyProcessPriority && currTime<evtTimestamp){ //preempt running process and delete events from event manager queue
                        cout<<"cond2 : YES ";
                        
                            em->removeEventsPreemption(CURRENT_RUNNING_PROCESS);
                        CURRENT_RUNNING_PROCESS->setCPUBurstRemaining(CURRENT_RUNNING_PROCESS->cpuBurstRemaining+evtTimestamp-currTime);
                        CURRENT_RUNNING_PROCESS->setRemainingWorkTime(CURRENT_RUNNING_PROCESS->getRemainingWorkTime()+evtTimestamp-currTime);
                            Event e;
                        
                            e.setValues(CURRENT_RUNNING_PROCESS->getPID(), "RUNNING", "READY", "", CURRENT_RUNNING_PROCESS, 0, ofs, currTime, TRANS_TO_PREEMPT);
                            em->addEvent(e);
                        }
                        
                    }
                    cout<<endl;
                }
                scheduler->addProcess(proc);
                //scheduler->print();
                CALL_SCHEDULER=true;
                break;
            }
            case TRANS_TO_PREEMPT: {
                Event e;
                proc->currentState="PREEMPT";
                e.setValues(proc->pid, "RUNNING", "READY", evt->extra, proc, currTime, ofs, currTime, TRANS_TO_READY);
                em->addEvent(e);
                CURRENT_RUNNING_PROCESS=NULL;
                CALL_SCHEDULER = true;
                break;}
            case TRANS_TO_RUN:{
//                if(prio){
//                proc->setProcessPriority(proc->getProcessPriority()-1);
//                    if(proc->getProcessPriority()==-1) proc->setProcessPriority(proc->getStaticPriority()-1);}
//
                proc->currentState="RUN";
                int remainingWork = proc->getRemainingWorkTime();
                int cpuBRem=proc->getCPUBurstRemaining();
                if(cpuBRem==0){ // we can assign new cpu time using rfile
                    cpuBRem = myrandom(proc->getCPUBurst());
                    proc->setRemainingWorkTime(cpuBRem);
                    ofs++;
                    //cout<<"ofs="<<ofs<<endl;
                }
                
                if(remainingWork < cpuBRem){
                    cpuBRem = remainingWork;
                }
                evt->extra = "\t cb="+to_string(cpuBRem)+"\trem="+to_string(remainingWork)+"\tprio="+to_string(proc->processPrio);

                evt->printEvent();
                Event e;
                if(quantum >= cpuBRem) {
                    proc->setRemainingWorkTime(remainingWork-cpuBRem);
                    proc->setCPUBurstRemaining(0);
                    e.setValues(proc->pid, "RUNNING", "BLOCK", "", proc, currTime, ofs, currTime+cpuBRem, TRANS_TO_BLOCK);
                    CURRENT_RUNNING_PROCESS = proc;
                }
                else{
                    proc->setRemainingWorkTime(remainingWork-quantum);
                    remainingWork = proc->getRemainingWorkTime();
                    cpuBRem=cpuBRem-quantum;
                    e.setValues(proc->pid, "RUNNING", "READY", "\t cb="+to_string(cpuBRem)+"\trem="+to_string(remainingWork)+"\tprio="+to_string(proc->processPrio), proc, currTime, ofs, currTime+quantum, TRANS_TO_PREEMPT);
                    proc->setCPUBurstRemaining(cpuBRem);
                    CURRENT_RUNNING_PROCESS = proc;
                }
                

                em->addEvent(e);
                CALL_SCHEDULER = false;
                break;
            }
            case TRANS_TO_BLOCK:{
                proc->currentState="BLOCK";
                CURRENT_RUNNING_PROCESS=NULL;
                CALL_SCHEDULER = true;
                if(proc->getRemainingWorkTime()<=0){
                    cout<<currTime<<"\t"<<proc->getPID()<<"\tDone"<<endl;
                    mappingOutput(currTime, proc);
                    break;
                }
                int ioWait = myrandom(proc->getIOBurst());
                evt->extra = "\t ib="+to_string(ioWait)+"\trem="+to_string(proc->remainingWork)+"\tprio="+to_string(proc->processPrio);
                proc->setBlocked(proc->getBlocked()+ioWait);
                evt->printEvent();
                ofs++;
                //cout<<"ofs="<<ofs<<endl;
                
                Event e;
                e.setValues(proc->pid, "BLOCK", "READY", "",
                            proc, currTime, ofs, currTime+ioWait, TRANS_TO_READY);
                em->addEvent(e);
                if(timestampOfBlocked==-1 || currTime>timestampOfBlocked){  // first process to go into blocked or currtime is more than last blocked process time
                    timeInBlocked+= ioWait;
                    timestampOfBlocked = currTime+ioWait;
                }
                else{
                    // check if overlapping blocks
                    if(currTime<=timestampOfBlocked){
                        if(currTime+ioWait>timestampOfBlocked){
                            timeInBlocked+=(currTime+ioWait-timestampOfBlocked); // add the delta time
                            timestampOfBlocked=currTime+ioWait;
                        }
                        // else no change in timestamp.
                    }
                }
                
                break;}
        }
        
//        if(!scheduler->readyQueue.empty())
//        scheduler->print();
//        else cout<<"null"<<endl;
        
        em->removeEvent();
        
        if(CALL_SCHEDULER) {
            if (em->getNextEventTime() == currTime ){
                //cout<<"same time";
                continue; //process next event from Event queue
            }
            CALL_SCHEDULER = false; // reset global flag
            if (CURRENT_RUNNING_PROCESS == NULL) {
                //cout<<"size of scheduler run queue is "<<scheduler->readyQueue.size();
                CURRENT_RUNNING_PROCESS = scheduler->getNextProcess();
                if (CURRENT_RUNNING_PROCESS == NULL || CURRENT_RUNNING_PROCESS==nullptr){
                    continue;
                }
                //cout<<"pid="<<CURRENT_RUNNING_PROCESS->getPID()<<endl;
                Event e;
                    e.setValues(CURRENT_RUNNING_PROCESS->getPID(), "READY", "RUNNING", "", CURRENT_RUNNING_PROCESS, ofs, ofs, currTime, TRANS_TO_RUN);
                    em->addEvent(e);
                
            }
        }
        
    }
    
    printOutput();
}

int main(int argc, const char * argv[]) {
    // insert code here...
    char* str= "/Users/asmitamitra/Desktop/Spring2023/OS/Lab2/lab2_assign/rfile";
    createRandomArray(str);
    char* str2="/Users/asmitamitra/Desktop/Spring2023/OS/Lab2/lab2_assign/input6";
    maxprio=5;
    preprio=true;
    string sch="PREPRIO";
    quantum=2;
    EventManager evm = initialize(str2,maxprio, sch);
    scheduler = new PreemptivePriority();
    
    

    Simulation(&evm, sch);
    
    //fcfs();
    //lcfs();
    
    return 0;
}

