#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <cstring>
using namespace std;
struct Process
{
    string name;
    int arrival_time;
    int service_time;
    int remaining_time;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    double normalized_turnaround_time;
    int priority;
    int initial_priority;
    Process(string n = "", int a = 0, int s = 0, int p = 0)
        : name(n), arrival_time(a), service_time(s), remaining_time(s),
          start_time(-1), completion_time(0), turnaround_time(0),
          waiting_time(0), normalized_turnaround_time(0.0),
          priority(p), initial_priority(p) {}
    bool operator<(const Process &other) const
    {
        if (priority == other.priority)
            return arrival_time > other.arrival_time; 
        return priority < other.priority;             
    }
};
void parse_input();
void FCFS(vector<Process> &processes, int max_time, bool trace);
void RoundRobin(vector<Process> &processes, int max_time, bool trace, int quantum);
void SPN(vector<Process> &processes, int max_time, bool trace);
void SRT(vector<Process> &processes, int max_time, bool trace);
void HRRN(vector<Process> &processes, int max_time, bool trace);
void FB1(vector<Process> &processes, int max_time, bool trace);
void FB2i(vector<Process> &processes, int max_time, bool trace);
void fcfs_print_process_trace(const vector<Process> &processes, int max_time, bool trace, const string &algorithm_name);
void spn_print_process_trace(const vector<Process> &processes, int max_time, queue<pair<string, int>> &ready_queue_events);
void hrrn_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline);
void roundrobin_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline, const vector<vector<bool>> &is_waiting, string name, string last_running_name);
void fb1_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline);
void fb2i_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline);
void print_stats(const vector<Process> &processes, string algorithm);
void Aging(vector<Process> &processes, int max_time, bool trace, int originalQuantum, vector<string> &timeline);
void aging_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline);
void srt_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline, const vector<vector<bool>> &is_waiting, string name);
int main()
{
    parse_input();

    return 0;
}
void parse_input()
{
    string line;
    string mode;
    int max_time, num_processes;
    getline(cin, line);
    bool trace = (line == "trace");
    getline(cin, line);
    stringstream ss(line);
    vector<pair<int, int>> algorithms; 
    string segment;
    while (getline(ss, segment, ','))
    {
        stringstream alg_and_qntm(segment);
        string algo_str, qntm_str = "0"; 
        int algo, qntm = 0;
        if (getline(alg_and_qntm, algo_str, '-'))
        {
            algo = stoi(algo_str);
            if (getline(alg_and_qntm, qntm_str))
            {
                qntm = stoi(qntm_str);
            }
            else
            {
                qntm = 0;
            }
        }

        algorithms.push_back({algo, qntm});
    }
    getline(cin, line);
    max_time = stoi(line);
    getline(cin, line);
    num_processes = stoi(line);
    vector<Process> processes;
    for (int i = 0; i < num_processes; i++)
    {
        string line;
        cin >> ws;
        getline(cin, line);
        stringstream ss(line);
        char name;
        int arrival_time, service_time;
        ss >> name;
        ss.ignore(1);
        ss >> arrival_time;
        ss.ignore(1);
        ss >> service_time;
        processes.push_back(Process(string(1, name), arrival_time, service_time));
    }
    vector<string> timeline(max_time, " "); 
    for (const auto &alg : algorithms)
    {
        int algo = alg.first;
        int qntm = alg.second;
        switch (algo)
        {
        case 1: 
            FCFS(processes, max_time, trace);
            break;
        case 2: 
            RoundRobin(processes, max_time, trace, qntm);
            break;
        case 3: 
            SPN(processes, max_time, trace);
            break;
        case 4: 
            SRT(processes, max_time, trace);
            break;
        case 5: 
            HRRN(processes, max_time, trace);
            break;
        case 6: 
            FB1(processes, max_time, trace);
            break;
        case 7: 
            FB2i(processes, max_time, trace);
            break;
        case 8: 
            Aging(processes, max_time, trace, qntm, timeline);
            break;
        default:
            cerr << "Unknown algorithm: " << algo << endl;
        }
    }
}
void HRRN(vector<Process> &process_list, int max_duration, bool trace)
{
    vector<tuple<string, double, int>> ready_queue;       
    vector<string> execution_timeline(max_duration, "-"); 
    unordered_map<string, int> process_name_to_index;     
    for (int i = 0; i < process_list.size(); i++)
    {
        process_name_to_index[process_list[i].name] = i;
    }

    int total_completed = 0;    
    int next_process_index = 0; 

    for (int current_time = 0; current_time < max_duration && total_completed < process_list.size(); current_time++)
    {
        while (next_process_index < process_list.size() && process_list[next_process_index].arrival_time <= current_time)
        {
            ready_queue.emplace_back(process_list[next_process_index].name, 1.0, 0); 
            next_process_index++;
        }
        for (auto &process_entry : ready_queue)
        {
            string process_name = get<0>(process_entry);
            int process_index = process_name_to_index[process_name];
            int wait_time = current_time - process_list[process_index].arrival_time;
            int service_time = process_list[process_index].service_time;
            get<1>(process_entry) = (wait_time + service_time) / static_cast<double>(service_time); 
        }
        sort(ready_queue.begin(), ready_queue.end(), [](const auto &a, const auto &b)
             {
                 if (get<1>(a) != get<1>(b))
                     return get<1>(a) > get<1>(b);
                 return get<0>(a) < get<0>(b);     
             });

        if (!ready_queue.empty())
        {
            string selected_process_name = get<0>(ready_queue[0]);
            int selected_process_index = process_name_to_index[selected_process_name];
            if (process_list[selected_process_index].start_time == -1)
            {
                process_list[selected_process_index].start_time = current_time;
            }
            while (current_time < max_duration &&
                   get<2>(ready_queue[0]) < process_list[selected_process_index].service_time)
            {
                execution_timeline[current_time] = selected_process_name;
                get<2>(ready_queue[0])++;
                current_time++;
            }
            current_time--;
            process_list[selected_process_index].completion_time = current_time + 1;
            process_list[selected_process_index].turnaround_time = process_list[selected_process_index].completion_time - process_list[selected_process_index].arrival_time;
            process_list[selected_process_index].waiting_time = process_list[selected_process_index].turnaround_time - process_list[selected_process_index].service_time;
            process_list[selected_process_index].normalized_turnaround_time =
                static_cast<double>(process_list[selected_process_index].turnaround_time) / process_list[selected_process_index].service_time;

            total_completed++;
            ready_queue.erase(ready_queue.begin()); 
        }
        else
        {
            execution_timeline[current_time] = "-";
        }
    }

    if (trace)
    {
        hrrn_print_process_trace(process_list, max_duration, execution_timeline);
    }
    else
    {
        print_stats(process_list, "HRRN");
    }
}

void FB1(vector<Process> &process_list, int max_time, bool trace)
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> ready_queue;
    unordered_map<int, int> remaining_time_map;
    vector<string> timeline(max_time, "-");
    int current_process_index = 0;
    if (process_list[0].arrival_time == 0)
    {
        ready_queue.push(make_pair(0, current_process_index));
        remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
        current_process_index++;
    }

    for (int current_time = 0; current_time < max_time; current_time++)
    {
        if (!ready_queue.empty())
        {
            int current_priority_level = ready_queue.top().first;
            int process_id = ready_queue.top().second;
            int process_arrival_time = process_list[process_id].arrival_time;
            int process_service_time = process_list[process_id].service_time;
            ready_queue.pop();
            while (current_process_index < process_list.size() && process_list[current_process_index].arrival_time == current_time + 1)
            {
                ready_queue.push(make_pair(0, current_process_index));
                remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
                current_process_index++;
            }
            remaining_time_map[process_id]--;
            timeline[current_time] = process_list[process_id].name; 
            if (remaining_time_map[process_id] == 0)
            {
                process_list[process_id].completion_time = current_time + 1;
                process_list[process_id].turnaround_time = process_list[process_id].completion_time - process_arrival_time;
                process_list[process_id].waiting_time = process_list[process_id].turnaround_time - process_service_time;
                process_list[process_id].normalized_turnaround_time = static_cast<double>(process_list[process_id].turnaround_time) / process_service_time;
            }
            else
            {
                if (!ready_queue.empty())
                {
                    ready_queue.push(make_pair(current_priority_level + 1, process_id));
                }
                else
                {
                    ready_queue.push(make_pair(current_priority_level, process_id));
                }
            }
        }
        else
        {
 
            timeline[current_time] = "-";
        }
        while (current_process_index < process_list.size() &&
               process_list[current_process_index].arrival_time == current_time + 1)
        {
            ready_queue.push(make_pair(0, current_process_index));
            remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
            current_process_index++;
        }
    }
    if (trace)
    {
        fb1_print_process_trace(process_list, max_time, timeline);
    }
    else
    {
        print_stats(process_list, "FB-1");
    }
}
void FB2i(vector<Process> &process_list, int max_time, bool trace)
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> ready_queue;
    unordered_map<int, int> remaining_time_map;
    vector<string> timeline(max_time, "-");
    int current_process_index = 0;
    if (process_list[0].arrival_time == 0)
    {
        ready_queue.push(make_pair(0, current_process_index));
        remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
        current_process_index++;
    }
    for (int current_time = 0; current_time < max_time; current_time++)
    {
        if (!ready_queue.empty())
        {
            int current_priority_level = ready_queue.top().first;
            int process_id = ready_queue.top().second;
            int process_arrival_time = process_list[process_id].arrival_time;
            int process_service_time = process_list[process_id].service_time;
            ready_queue.pop();
            while (current_process_index < process_list.size() && process_list[current_process_index].arrival_time <= current_time + 1)
            {
                ready_queue.push(make_pair(0, current_process_index));
                remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
                current_process_index++;
            }
            int current_quantum = pow(2, current_priority_level);
            int temp_time = current_time;
            while (current_quantum > 0 && remaining_time_map[process_id] > 0 && temp_time < max_time)
            {
                current_quantum--;
                remaining_time_map[process_id]--;
                timeline[temp_time++] = process_list[process_id].name; 
            }
            if (remaining_time_map[process_id] == 0)
            {
                process_list[process_id].completion_time = temp_time;
                process_list[process_id].turnaround_time = process_list[process_id].completion_time - process_arrival_time;
                process_list[process_id].waiting_time = process_list[process_id].turnaround_time - process_service_time;
                process_list[process_id].normalized_turnaround_time = static_cast<double>(process_list[process_id].turnaround_time) / process_service_time;
            }
            else
            {
                if (!ready_queue.empty())
                {
                    ready_queue.push(make_pair(current_priority_level + 1, process_id));
                }
                else
                {
                    ready_queue.push(make_pair(current_priority_level, process_id));
                }
            }
            current_time = temp_time - 1;
        }
        else
        {
            timeline[current_time] = "-";
        }
        while (current_process_index < process_list.size() && process_list[current_process_index].arrival_time <= current_time + 1)
        {
            ready_queue.push(make_pair(0, current_process_index));
            remaining_time_map[current_process_index] = process_list[current_process_index].service_time;
            current_process_index++;
        }
    }
    if (trace)
    {
        fb2i_print_process_trace(process_list, max_time, timeline);
    }
    else
    {
        print_stats(process_list, "FB-2i");
    }
}
void hrrn_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline)
{
    cout << "HRRN  ";
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
    for (const auto &p : processes)
    {
        cout << p.name << setw(6) << " |";
        for (int t = 0; t < max_time; t++)
        {
            if (t < p.arrival_time)
            {
                cout << " |"; 
            }
            else if (timeline[t] == p.name)
            {
                cout << "*|"; 
            }
            else if (t >= p.arrival_time && t < p.completion_time && timeline[t] != p.name)
            {
                cout << ".|"; 
            }
            else if (t >= p.completion_time)
            {
                cout << " |"; 
            }
            else
            {
                cout << " |"; 
            }
        }
        cout << " " << endl;
    }

    cout << "------------------------------------------------\n"
         << endl;
}

void fb1_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline)
{
    cout << "FB-1  ";
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;

    for (const auto &process : processes)
    {
        cout << process.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++)
        {
            if (t < process.arrival_time)
            {
                cout << " |"; 
            }
            else if (timeline[t] == process.name)
            {
                cout << "*|"; 
            }
            else if (t >= process.arrival_time && t < process.completion_time)
            {
                cout << ".|"; 
            }
            else
            {
                cout << " |"; 
            }
        }
        cout << " " << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}
void fb2i_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline)
{
    cout << "FB-2i ";
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;

    for (const auto &process : processes)
    {
        cout << process.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++)
        {
            if (t < process.arrival_time)
            {
                cout << " |"; 
            }
            else if (timeline[t] == process.name)
            {
                cout << "*|"; 
            }
            else if (t >= process.arrival_time && t < process.completion_time)
            {
                cout << ".|"; 
            }
            else
            {
                cout << " |";
            }
        }
        cout << " " << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}
void Aging(vector<Process> &processes, int max_time, bool trace, int originalQuantum, vector<string> &timeline)
{
    vector<tuple<int, int, int>> ready_queue;
    int current_process = -1;
    int completed_processes = 0;
    for (auto &process : processes)
    {
        process.remaining_time = process.service_time;
        process.start_time = -1;
        process.completion_time = 0;
        process.turnaround_time = 0;
        process.waiting_time = 0;
        process.normalized_turnaround_time = 0.0;
    }

    for (int time = 0; time < max_time && completed_processes < processes.size(); time++)
    {
        for (size_t i = 0; i < processes.size(); i++)
        {
            if (processes[i].arrival_time == time)
            {
                ready_queue.emplace_back(processes[i].priority, i, 0);
            }
        }
        for (auto &entry : ready_queue)
        {
            if (get<1>(entry) != current_process)
            {
                get<0>(entry)++; 
                get<2>(entry)++; 
            }
        }
        sort(ready_queue.begin(), ready_queue.end(), [](const auto &a, const auto &b)
             {
                 if (get<0>(a) != get<0>(b))
                     return get<0>(a) > get<0>(b); 
                 return get<1>(a) < get<1>(b);     
             });
        if (!ready_queue.empty())
        {
            current_process = get<1>(ready_queue[0]); 
            Process &p = processes[current_process];
            if (p.start_time == -1)
            {
                p.start_time = time;
            }
            int quantum_remaining = originalQuantum;
            while (quantum_remaining > 0 && p.remaining_time > 0 && time < max_time)
            {
                timeline[time] = p.name;
                p.remaining_time--;
                quantum_remaining--;
                time++;
            }
            time--;
            if (p.remaining_time == 0)
            {
                p.completion_time = time + 1;
                p.turnaround_time = p.completion_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.service_time;
                p.normalized_turnaround_time = static_cast<double>(p.turnaround_time) / p.service_time;
                completed_processes++;
                ready_queue.erase(ready_queue.begin()); 
            }
            else
            {
                get<0>(ready_queue[0]) = p.initial_priority;
            }
        }
        else
        {
            
            timeline[time] = "-";
        }
    }

    if (trace)
    {
        aging_print_process_trace(processes, max_time, timeline);
    }
    else
    {
        print_stats(processes, "AGING");
    }
}

void aging_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline)
{
    cout << "Aging ";
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
    for (const auto &p : processes)
    {
        cout << p.name << setw(6) << " |";
        for (int t = 0; t < max_time; t++)
        {
            if (t < p.arrival_time)
            {
                cout << " |"; 
            }
            else if (timeline[t] == p.name)
            {
                cout << "*|";
            }
            else if (timeline[t] == "-" || timeline[t] != p.name)
            {
                if (timeline[t] != "-" && t >= p.arrival_time)
                {
                    cout << ".|";
                }
                else
                {
                    cout << " |"; 
                }
            }
        }
        cout <<" " << endl;
    }
    cout << "------------------------------------------------\n" << endl;
}

void FCFS(vector<Process> &processes, int max_time, bool trace)
{
    int time = 0;

    for (auto &process : processes)
    {
        if (time < process.arrival_time)
        {
            time = process.arrival_time;
        }
        process.start_time = time;
        process.completion_time = time + process.service_time;
        process.turnaround_time = process.completion_time - process.arrival_time;
        process.waiting_time = process.turnaround_time - process.service_time;
        process.normalized_turnaround_time = static_cast<double>(process.turnaround_time) / process.service_time;
        time = process.completion_time;
    }

    if (trace)
    {
        fcfs_print_process_trace(processes, max_time, trace, "FCFS");
    }
    else
    {
        print_stats(processes, "FCFS");
    }
}

void RoundRobin(vector<Process> &processes, int max_time, bool trace, int quantum)
{
    queue<pair<int, int>> ready_queue;
    vector<string> timeline(max_time, "-");
    vector<vector<bool>> is_waiting(processes.size(), vector<bool>(max_time, false));
    int process_index = 0;
    int current_quantum = quantum;
    string last_running_name;
    for (auto &process : processes)
    {
        process.remaining_time = process.service_time;
        process.start_time = -1;
        process.completion_time = 0;
        process.turnaround_time = 0;
        process.waiting_time = 0;
        process.normalized_turnaround_time = 0.0;
    }
    if (processes[process_index].arrival_time == 0)
    {
        ready_queue.push({process_index, processes[process_index].service_time});
        process_index++;
    }

    for (int time = 0; time < max_time; ++time)
    {
        if (!ready_queue.empty())
        {
            auto &front = ready_queue.front();
            int current_process_index = front.first;
            int &remaining_time = front.second;
            if (processes[current_process_index].start_time == -1)
            {
                processes[current_process_index].start_time = time; 
            }
            remaining_time--;
            current_quantum--;
            timeline[time] = processes[current_process_index].name;
            while (process_index < processes.size() && processes[process_index].arrival_time == time + 1)
            {
                ready_queue.push({process_index, processes[process_index].service_time});
                process_index++;
            }
            if (remaining_time == 0) 
            {
                char c;
                 c = 65 + current_process_index;
                processes[current_process_index].completion_time = time + 1;
                processes[current_process_index].turnaround_time = processes[current_process_index].completion_time - processes[current_process_index].arrival_time;
                processes[current_process_index].waiting_time = processes[current_process_index].turnaround_time - processes[current_process_index].service_time;
                processes[current_process_index].normalized_turnaround_time =
                    static_cast<double>(processes[current_process_index].turnaround_time) / processes[current_process_index].service_time;

                ready_queue.pop();         
                current_quantum = quantum; 
                if (time == max_time - 1)
                    last_running_name = last_running_name + c;
            }
            else if (current_quantum == 0) 
            {
                ready_queue.pop();                                         
                ready_queue.push({current_process_index, remaining_time}); 
                current_quantum = quantum;                                 
            }
        }
        queue<pair<int, int>> temp_queue = ready_queue;
        while (!temp_queue.empty())
        {
            int waiting_process_index = temp_queue.front().first;
            temp_queue.pop();
            if (timeline[time] != processes[waiting_process_index].name)
            {
                is_waiting[waiting_process_index][time] = true;
            }
        }
        while (process_index < processes.size() && processes[process_index].arrival_time == time + 1)
        {
            ready_queue.push({process_index, processes[process_index].service_time});
            process_index++;
        }
        if (ready_queue.empty())
        {
            timeline[time] = "-";
        }
    }
    string name = "RR-" + to_string(quantum) + "  ";
    string namestats = "RR-" + to_string(quantum);
    if (trace)
    {

        roundrobin_print_process_trace(processes, max_time, timeline, is_waiting, name, last_running_name);
    }
    else
    {
        print_stats(processes, namestats);
    }
}

void roundrobin_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline, const vector<vector<bool>> &is_waiting, string name, string last_running_name)
{
    cout << name;
    for (int t = 0; t <= max_time; t++) 
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
    for (size_t i = 0; i < processes.size(); ++i)
    {
        const auto &p = processes[i];
        cout << p.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++) 
        {
            if (t == max_time - 1 && processes[i].name == last_running_name)
            {
                cout << "*|";
            }
            if (t < p.arrival_time)
            {
                cout << " |"; 
            }
            else if (t < timeline.size() && timeline[t] == p.name) 
            {
                cout << "*|";
            }
            else if (t < is_waiting[i].size() && is_waiting[i][t]) 
            {
                cout << ".|"; 
            }
            else
            {
                if (processes[i].name != last_running_name)
                    cout << " |"; 
            }
        }
        cout << " " << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}
void srt_print_process_trace(const vector<Process> &processes, int max_time, const vector<string> &timeline, const vector<vector<bool>> &is_waiting, string name)
{
    cout << name;
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
    for (size_t i = 0; i < processes.size(); ++i)
    {
        const auto &p = processes[i];
        cout << p.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++)
        {
            if (t < p.arrival_time)
            {
                cout << " |";
            }
            else if (timeline[t] == p.name)
            {
                cout << "*|"; 
            }
            else if (is_waiting[i][t])
            {
                cout << ".|";
            }
            else
            {
                cout << " |"; 
            }
        }
        cout << " " << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}

void SPN(vector<Process> &processes, int max_time, bool trace)
{
    int time = 0;
    queue<pair<string, int>> ready_queue_events;
    for (auto &process : processes)
    {
        process.remaining_time = process.service_time;
        process.start_time = -1;
    }
    while (true)
    {
        bool all_done = true;
        vector<Process *> ready_queue;
        for (auto &process : processes)
        {
            if (process.arrival_time <= time && process.remaining_time > 0)
            {
                ready_queue.push_back(&process);
                ready_queue_events.push({process.name, time});
            }
        }
        if (ready_queue.empty() && all_done)
        {
            break;
        }
        if (!ready_queue.empty())
        {
            sort(ready_queue.begin(), ready_queue.end(), [](Process *a, Process *b)
                 { return a->remaining_time < b->remaining_time; });
            Process *current = ready_queue.front();
            ready_queue.erase(ready_queue.begin()); 

            if (current->start_time == -1)
            {
                current->start_time = time;
            }
            time += current->remaining_time;
            current->completion_time = time;
            current->turnaround_time = current->completion_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->service_time;
            current->normalized_turnaround_time = static_cast<double>(current->turnaround_time) / current->service_time;
            current->remaining_time = 0;
        }
    }
    if (trace)
    {
        spn_print_process_trace(processes, max_time, ready_queue_events);
    }
    else
    {
        print_stats(processes, "SPN");
    }
}

void spn_print_process_trace(const vector<Process> &processes, int max_time, queue<pair<string, int>> &ready_queue_events)
{
    vector<pair<string, int>> ready_events;
    while (!ready_queue_events.empty())
    {
        ready_events.push_back(ready_queue_events.front());
        ready_queue_events.pop();
    }
    cout << "SPN   ";
    for (int t = 0; t <= max_time; t++)
    {
        cout << t % 10 << " ";
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;
    for (const auto &process : processes)
    {
        cout << process.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++)
        {
            bool is_in_ready_queue = false;
            for (const auto &event : ready_events)
            {
                if (event.first == process.name && event.second == t)
                {
                    is_in_ready_queue = true;
                    break;
                }
            }
            if (t < process.arrival_time)
            {
                cout << " |"; 
            }
            else if (t >= process.arrival_time && t < process.start_time)
            {
                cout << ".|"; 
            }
            else if (t >= process.start_time && t < process.completion_time)
            {
                cout << "*|";
            }
            else
            {
                cout << " |"; 
            }
        }
        cout << " " << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}
void fcfs_print_process_trace(const vector<Process> &processes, int max_time, bool trace, const string &algorithm_name)
{
    if (trace)
    {
        cout << algorithm_name << "  ";
        for (int t = 0; t <= max_time; t++)
        {
            cout << t % 10 << " ";
        }
        cout << endl;
        cout << "------------------------------------------------" << endl;
    }
    for (const auto &process : processes)
    {
        cout << process.name << setw(6) << " |"; 
        for (int t = 0; t < max_time; t++)
        {
            if (t < process.arrival_time)
            {
                cout << " |"; 
            }
            else if (t >= process.arrival_time && t < process.start_time)
            {
                cout << ".|"; 
            }
            else if (t >= process.start_time && t < process.completion_time)
            {
                cout << "*|"; 
            }
            else
            {
                cout << " |"; 
            }
        }
        cout << " " << endl;
    }
    if (trace)
    {
        cout << "------------------------------------------------\n"
             << endl;
    }
}
void SRT(vector<Process> &processes, int max_time, bool trace)
{
    vector<string> timeline(max_time + 1, "-");
    vector<vector<bool>> is_waiting(processes.size(), vector<bool>(max_time, false));
    int process_index = 0;
    for (auto &process : processes)
    {
        process.remaining_time = process.service_time;
        process.start_time = -1;
        process.completion_time = 0;
        process.turnaround_time = 0;
        process.waiting_time = 0;
        process.normalized_turnaround_time = 0.0;
    }

    for (int time = 0; time <= max_time; ++time)
    {
        while (process_index < processes.size() && processes[process_index].arrival_time == time)
        {
            process_index++;
        }
        int shortest_index = -1;
        int shortest_time = 500;
        for (size_t i = 0; i < processes.size(); ++i)
        {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0)
            {
                if (processes[i].remaining_time < shortest_time)
                {
                    shortest_time = processes[i].remaining_time;
                    shortest_index = i;
                }
            }
        }
        if (shortest_index != -1)
        {
            Process &current = processes[shortest_index];
            if (current.start_time == -1)
            {
                current.start_time = time; 
            }
            current.remaining_time--;
            timeline[time] = current.name;

            if (current.remaining_time == 0) 
            {
                current.completion_time = time + 1;
                current.turnaround_time = current.completion_time - current.arrival_time;
                current.waiting_time = current.turnaround_time - current.service_time;
                current.normalized_turnaround_time =
                    static_cast<double>(current.turnaround_time) / current.service_time;
            }
        }
        else
        {
            timeline[time] = "-";
        }
        for (size_t i = 0; i < processes.size(); ++i)
        {
            if (timeline[time] != processes[i].name && processes[i].remaining_time > 0 && processes[i].arrival_time <= time)
            {
                is_waiting[i][time] = true;
            }
        }
    }

    string name = "SRT   ";
    if (trace)
    {
        srt_print_process_trace(processes, max_time, timeline, is_waiting, name);
    }
    else
    {
        print_stats(processes, "SRT");
    }
}

void print_stats(const vector<Process> &processes, string algorithim)
{
    double total_turnaround = 0;
    double total_normalized_turnaround = 0;
    cout << algorithim << endl;
    cout << "Process    |";
    for (const auto &process : processes)
    {
        cout << "  " << process.name << "  |";
    }
    cout << endl;
    cout << "Arrival    |";
    for (const auto &process : processes)
    {
        cout << "  " << process.arrival_time << "  |";
    }
    cout << endl;
    cout << "Service    |";
    for (const auto &process : processes)
    {
        cout << "  " << process.service_time << "  |";
    }
    cout << " Mean|";
    cout << endl;
    cout << "Finish     |";
    for (const auto &process : processes)
    {

        if (process.completion_time > 9)
        {
            cout << " " << process.completion_time << "  |";
        }
        else
        {
            cout << "  " << process.completion_time << "  |";
        }
    }
    cout << "-----|";
    cout << endl;
    cout << "Turnaround |";
    for (const auto &process : processes)
    {
        if (process.turnaround_time > 9)
        {
            cout << " " << process.turnaround_time << "  |";
        }
        else
        {
            cout << "  " << process.turnaround_time << "  |";
        }
        total_turnaround += process.turnaround_time;
    }
    double avg_turnaround = total_turnaround / processes.size();
    cout << setw(5) << setfill(' ') << setprecision(2) << fixed << avg_turnaround << "|";
    cout << endl;
    cout << "NormTurn   |";
    for (const auto &process : processes)
    {
        cout << " " << fixed << setprecision(2) << process.normalized_turnaround_time << "|";
        total_normalized_turnaround += process.normalized_turnaround_time;
    }
    double avg_normalized_turnaround = total_normalized_turnaround / processes.size();
    cout << setw(5) << setfill(' ') << fixed << setprecision(2) << avg_normalized_turnaround << "|" << endl;
    cout << "\n";
}
