
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

struct Task {
    int id;
    int length;
    int arrival_time;
};

// Scheduler 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class Scheduler {
protected:
    string name;
public:
// Scheduler 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    Scheduler(const string& name): name(name) {}
    virtual int run(vector<Task>& tasks) = 0;
    virtual ~Scheduler() {}
};

// FCFS 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class FCFS : public Scheduler {
public:
// FCFS 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    FCFS() : Scheduler("FCFS") {}
// run 함수: FCFS 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });
        int time = 0, total_turnaround = 0;
        for (auto& task : tasks) {
            if (time < task.arrival_time) time = task.arrival_time;
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += task.length;
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
            total_turnaround += time - task.arrival_time;
        }
        return total_turnaround / tasks.size();
    }
};

// SJF 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class SJF : public Scheduler {
public:
// SJF 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    SJF() : Scheduler("SJF") {}
// run 함수: SJF 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });

        auto cmp = [](const Task& a, const Task& b) {
            return a.length > b.length;
        };
        priority_queue<Task, vector<Task>, decltype(cmp)> pq(cmp);

        int idx = 0, time = 0, total_turnaround = 0;
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
        while (idx < tasks.size() || !pq.empty()) {
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                pq.push(tasks[idx++]);
            }
// 준비 큐가 비어 있는 경우, 다음 태스크의 도착 시간까지 시간 점프를 수행합니다.
            if (pq.empty()) {
                time = tasks[idx].arrival_time;
                continue;
            }
            Task cur = pq.top(); pq.pop();
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += cur.length;
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
            total_turnaround += (time - cur.arrival_time);
        }
        return total_turnaround / tasks.size();
    }
};

// RR 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class RR : public Scheduler {
private:
    int time_quantum;
public:
// RR 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    RR(int quantum): Scheduler("RR"), time_quantum(quantum) {}

// run 함수: RR 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
        queue<Task> q;
        int time = 0, idx = 0, total_turnaround = 0;
// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });

// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
        while (!q.empty() || idx < tasks.size()) {
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                q.push(tasks[idx++]);
            }

// 준비 큐가 비어 있는 경우, 다음 태스크의 도착 시간까지 시간 점프를 수행합니다.
            if (q.empty()) {
                time = tasks[idx].arrival_time;
                continue;
            }

            Task cur = q.front(); q.pop();
            int run_time = min(time_quantum, cur.length);
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += run_time;
            cur.length -= run_time;

// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                q.push(tasks[idx++]);
            }

// 큐 또는 우선순위 큐에 태스크를 추가합니다.
            if (cur.length > 0) q.push(cur);
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
            else total_turnaround += (time - cur.arrival_time);
        }

        return total_turnaround / tasks.size();
    }
};

// FeedBack 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class FeedBack : public Scheduler {
private:
    int base_quantum;
    static const int LEVEL = 4;
public:
// FeedBack 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    FeedBack(int quantum): Scheduler("FeedBack"), base_quantum(quantum) {}

// run 함수: FeedBack 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
        vector<queue<Task>> queues(LEVEL);
        int time = 0, idx = 0, total_turnaround = 0;
// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });

        while (true) {
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                queues[0].push(tasks[idx++]);
            }

            int level = 0;
            while (level < LEVEL && queues[level].empty()) level++;

            if (level == LEVEL) {
                if (idx == tasks.size()) break;
                time = tasks[idx].arrival_time;
                continue;
            }

            Task cur = queues[level].front(); queues[level].pop();
            int tq = base_quantum * (1 << level);
            int run_time = min(tq, cur.length);
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += run_time;
            cur.length -= run_time;

// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                queues[0].push(tasks[idx++]);
            }

            if (cur.length > 0) {
                int next_level = min(level + 1, LEVEL - 1);
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                queues[next_level].push(cur);
            } else {
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
                total_turnaround += (time - cur.arrival_time);
            }
        }

        return total_turnaround / tasks.size();
    }
};

// Lottery 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class Lottery : public Scheduler {
public:
// Lottery 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    Lottery(): Scheduler("Lottery") {}

// run 함수: Lottery 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
        default_random_engine gen(42);
        int time = 0, idx = 0, total_turnaround = 0;
        vector<Task> ready;

// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });

// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
        while (idx < tasks.size() || !ready.empty()) {
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
                ready.push_back(tasks[idx++]);
            }

// 준비 큐가 비어 있는 경우, 다음 태스크의 도착 시간까지 시간 점프를 수행합니다.
            if (ready.empty()) {
                time = tasks[idx].arrival_time;
                continue;
            }

            vector<int> tickets;
            for (int i = 0; i < ready.size(); ++i) {
                tickets.insert(tickets.end(), ready[i].length, i);
            }

            uniform_int_distribution<int> dist(0, tickets.size() - 1);
            int selected = tickets[dist(gen)];
            Task cur = ready[selected];
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += cur.length;
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
            total_turnaround += (time - cur.arrival_time);
            ready.erase(ready.begin() + selected);
        }

        return total_turnaround / tasks.size();
    }
};

// Stride 클래스: 스케줄링 알고리즘 구현 클래스입니다.
class Stride : public Scheduler {
private:
    struct StrideTask {
        Task task;
        int stride, pass;
        bool operator<(const StrideTask& other) const {
            return pass > other.pass;
        }
    };
public:
// Stride 생성자: Scheduler의 부모 생성자를 호출하고 name을 설정합니다.
    Stride(): Scheduler("Stride") {}

// run 함수: Stride 알고리즘에 따라 태스크들을 실행하고 평균 반환시간을 계산합니다.
    int run(vector<Task>& tasks) override {
        const int BIG_STRIDE = 10000;
        int time = 0, idx = 0, total_turnaround = 0;
        priority_queue<StrideTask> pq;

// 태스크들을 도착 시간 순으로 정렬합니다.
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.arrival_time < b.arrival_time;
        });

// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
        while (idx < tasks.size() || !pq.empty()) {
// 아직 도착하지 않은 태스크가 있거나 준비 큐에 태스크가 남아있는 동안 반복합니다.
            while (idx < tasks.size() && tasks[idx].arrival_time <= time) {
                int ticket = max(1, tasks[idx].length);
// 큐 또는 우선순위 큐에 태스크를 추가합니다.
                pq.push({tasks[idx], BIG_STRIDE / ticket, 0});
                idx++;
            }

// 준비 큐가 비어 있는 경우, 다음 태스크의 도착 시간까지 시간 점프를 수행합니다.
            if (pq.empty()) {
                time = tasks[idx].arrival_time;
                continue;
            }

            StrideTask cur = pq.top(); pq.pop();
// 현재 시간을 태스크의 실행 시간만큼 증가시킵니다.
            time += cur.task.length;
// 반환 시간 = 종료 시간 - 도착 시간. 총 반환 시간에 누적합니다.
            total_turnaround += (time - cur.task.arrival_time);
            cur.pass += cur.stride;
        }

        return total_turnaround / tasks.size();
    }
};
