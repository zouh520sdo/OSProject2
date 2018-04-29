#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

struct Process {
	char ID;
	int mem;
	int arr_time;
	int run_time;
};

void split(const string& s, char delim, vector<string>& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != string::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

void split(const string &s, const char* delim, vector<string> & v) {
	// to avoid modifying original string
	// first duplicate the original string and return a char pointer then free the memory
	char * dup = _strdup(s.c_str());
	char * next_token;
	char * token = strtok_s(dup, delim, &next_token);
	while (token != NULL) {
		v.push_back(string(token));
		// the call is treated as a subsequent calls to strtok:
		// the function continues from where it left in previous invocation
		token = strtok_s(NULL, delim, &next_token);
	}
	free(dup);
}

bool procCmpFunction(Process i, Process j) {
	return (i.arr_time < j.arr_time || (i.arr_time == j.arr_time && i.ID < j.ID));
}

bool procCmpRunFunction(Process i, Process j) {
	return (i.run_time < j.run_time || (i.run_time == j.run_time && i.ID < j.ID));
}

vector<Process> readInputFile(string inputFile) {
	ifstream inFile;
	vector<Process> outputPrc;
	inFile.open(inputFile);
	if (!inFile.is_open()) {
		cerr << "Unable to open file " << inputFile << endl;
		return outputPrc;
	}

	string line;
	while (getline(inFile, line)) {
		//line.replace(line.begin(), line.end(), "/", " ");
		vector<string> output;
		split(line, "/ ", output);
		char id = output[0][0];
		int mem_time = stoi(output[1]);
		for (int i = 2; i < output.size(); i+=2) {
			Process proc;
			proc.ID = id;
			proc.mem = mem_time;
			proc.arr_time = stoi(output[i]);
			proc.run_time = stoi(output[i + 1]);
			outputPrc.push_back(proc);
		}
	}
	inFile.close();
	sort(outputPrc.begin(), outputPrc.end(), procCmpFunction);
	return outputPrc;
}

vector<int> findFreePortions(vector<char> frames, int length) {
	vector<int> output;
	int start = -1;
	int end = -1;
	int i = 0;
	for (; i < frames.size(); i++) {
		if (start == -1 && frames[i] == '.') {
			start = i;
		}

		if (start != -1 && end == -1 && frames[i] != '.') {
			if (i - start >= length) {
				end = i - 1;
				output.push_back(start);
				output.push_back(end);
			}
			start = -1;
			end = -1;
		}
	}
	if (start != -1 && i-start >= length) {
		end = i - 1;
		output.push_back(start);
		output.push_back(end);
	}
	return output;
}

void printFrames(vector<char> frames, int framesPerLine) {
	for (int i = 0; i < framesPerLine; i++) {
		cout << '=';
	}
	cout << endl;
	for (int i = 0; i < frames.size(); i++) {
		if (i!=0 && i % framesPerLine == 0) {
			cout << endl;
		}
		cout << frames[i];
	}
	cout << endl;
	for (int i = 0; i < framesPerLine; i++) {
		cout << '=';
	}
	cout << endl;
}

void printPageTable(map<char, vector<int>> table) {
	cout << "PAGE TABLE [page,frame]:" << endl;
	for (map<char, vector<int>>::iterator it = table.begin(); it != table.end(); ++it) {
		cout << it->first << ":";
		vector<int> pages = it->second;
		int c = 0;
		for (int i = 0; i < pages.size(); i+=2) {
			if (c != 0 && c % 10 == 0) {
				cout << endl;
			}
			else {
				cout << " ";
			}
			cout << "[" << pages[i] << "," << pages[i+1] <<"]";
			c++;
		}
		cout << endl;
	}
}

bool isFramesEnough(vector<char>& frames, Process p) {
	int count = 0;
	for (int i = 0; i < frames.size(); i++) {
		if (frames[i] == '.') {
			count++;
		}
	}
	return count >= p.mem;
}

void removeProc(vector<char> &memo, Process p) {

	for (int i = 0; i < memo.size(); i++) {
		if (memo[i] == p.ID) {
			memo[i] = '.';
		}
	}
}

vector<int> noncontipush(std::vector<char> &memo, char a, int max) {
	int count = 0;
	vector<int> map;
	for (unsigned int i = 0; i<memo.size(); i++) {
		if (memo[i] == '.') {
			memo[i] = a;
			map.push_back(count);
			map.push_back(i);
			count++;
			if (count == max) break;
		}
	}
	return map;
}

void nextfit(vector<int> portions, vector<char> &memo, Process p) {
	for (int j = portions[portions.size()-2]; j < p.mem + portions[portions.size() - 2]; j++) {
		memo[j] = p.ID;
	}
}

void bestfit(vector<int> portions, vector<char> &memo, Process p) {
	vector<int> best_portions;
	best_portions.push_back(portions[0]);
	best_portions.push_back(portions[1]);
	for (int i = 0; i < portions.size(); i += 2) {
		if (portions[i + 1] - portions[i] + 1 < best_portions[1] - best_portions[0] + 1) {
			best_portions[1] = portions[i + 1];
			best_portions[0] = portions[i];
		}
	}
	for (int j = best_portions[0]; j < best_portions[0] + p.mem; j++) {
		memo[j] = p.ID;
	}
}

void worstfit(vector<int> portions, vector<char> &memo, Process p) {
	vector<int> best_portions;
	best_portions.push_back(portions[0]);
	best_portions.push_back(portions[1]);
	for (int i = 0; i < portions.size(); i += 2) {
		if (portions[i + 1] - portions[i] + 1 > best_portions[1] - best_portions[0] + 1) {
			best_portions[1] = portions[i + 1];
			best_portions[0] = portions[i];
		}
	}
	for (int j = best_portions[0]; j < best_portions[0] + p.mem; j++) {
		memo[j] = p.ID;
	}
}


int defrag(std::vector<char> &memo, vector<char>& moved) {
	int result = 0;
	int empty = 0;
	bool collect = true;
	int notempty = 0;
	std::vector<int> e;
	std::vector<int> note;

	for (unsigned int i = 0; i<memo.size(); i++) {
		if (memo[i] != '.' && collect == true) {
			e.push_back(empty);
			collect = false;
			empty = 0;
		}
		else if (memo[i] == '.') {
			collect = true;
			empty++;
		}
	}
	collect = true;
	for (unsigned int i = 0; i<memo.size(); i++) {
		if ((memo[i] == '.' && collect == true)) {
			note.push_back(notempty);
			collect = false;
			notempty = 0;
		}
		else if (memo[i] != '.' && i != memo.size() - 1) {
			collect = true;
			notempty++;
		}
		else if (memo[i] != '.' && i == memo.size() - 1) {
			collect = true;
			notempty++;
			note.push_back(notempty);
		}
	}
	for (unsigned int i = 0; i<note.size(); i++) {
		if (e[i] != 0) {
			result += note[i];
		}
	}

	std::vector<char> copy;
	char movePro = memo.front();
	bool startmove = false;
	moved.clear();
	for (unsigned int i = 0; i<memo.size(); i++) {
		if (memo[i] != '.') {
			copy.push_back(memo[i]);
		}

		if (!startmove && memo[i] == '.') {
			startmove = true;
		}

		if (startmove && movePro != memo[i] && memo[i] != '.') {
			moved.push_back(memo[i]);
			movePro = memo[i];
		}
	}
	while (copy.size() != memo.size()) {
		copy.push_back('.');
	}
	memo = copy;
	return result;
}

void NonContiguous(vector<char> frames, vector<Process> inputProcs, int framesPerLine) {
	vector<Process> arrivalProcs = inputProcs;
	vector<Process> runningProcs;
	map<char, vector<int>> pageTable;
	int t = 0;

	cout << "time " << t << "ms: Simulator started (Non-contiguous)" << endl;
	while (!arrivalProcs.empty() || !runningProcs.empty()) {
		int next_t = INT_MAX;

		if (!arrivalProcs.empty()) {
			next_t = min(next_t, arrivalProcs[0].arr_time - t);
		}

		if (!runningProcs.empty()) {
			next_t = min(next_t, runningProcs[0].run_time);
		}

		t += next_t;

		if (!runningProcs.empty()) {
			int lastToRmove = 0;
			for (int i = 0; i < runningProcs.size(); i++) {
				runningProcs[i].run_time -= next_t;
				if (runningProcs[i].run_time <= 0) {
					++lastToRmove;
					removeProc(frames, runningProcs[i]);
					pageTable.erase(runningProcs[i].ID);
					/* print remove msg */
					cout << "time " << t << "ms: Process " << runningProcs[i].ID << " removed:" << endl;
					printFrames(frames, framesPerLine);
					printPageTable(pageTable);
				}
			}
			if (lastToRmove != 0) {
				runningProcs.erase(runningProcs.begin(), runningProcs.begin() + lastToRmove);
			}
		}

		if (!arrivalProcs.empty()) {
			Process p = arrivalProcs.front();
			if (p.arr_time <= t) {
				arrivalProcs.erase(arrivalProcs.begin());
				cout << "time " << t << "ms: Process " << p.ID << " arrived (requires " << p.mem << " frames)" << endl;
				if (isFramesEnough(frames, p)) {
					runningProcs.push_back(p);
					sort(runningProcs.begin(), runningProcs.end(), procCmpRunFunction);
					vector<int> page = noncontipush(frames, p.ID, p.mem);
					pageTable[p.ID] = page;
					/* print arrival msg */
					cout << "time " << t << "ms: Placed process " << p.ID << ":" << endl;
					printFrames(frames, framesPerLine);
					printPageTable(pageTable);
				}
				else {
					/* print skip msg */
					cout << "time " << t << "ms: Cannot place process " << p.ID << " -- skipped!" << endl;
				}
			}
		}
	}
	cout << "time " << t << "ms: Simulator ended (Non-contiguous)" << endl;
}

void Contiguous(vector<char> frames, vector<Process> inputProcs, int framesPerLine, int t_movetime, void(fitfunc(vector<int> portions, vector<char> &memo, Process p))) {
	vector<Process> arrivalProcs = inputProcs;
	vector<Process> runningProcs;
	int t = 0;
	int recentIndex = 0;

	if (fitfunc == nextfit) {
		cout << "time " << t << "ms: Simulator started (Contiguous -- Next-Fit)" << endl;
	}
	else if (fitfunc == bestfit) {
		cout << "time " << t << "ms: Simulator started (Contiguous -- Best-Fit)" << endl;
	}
	else if (fitfunc == worstfit) {
		cout << "time " << t << "ms: Simulator started (Contiguous -- Worst-Fit)" << endl;
	}
	while (!arrivalProcs.empty() || !runningProcs.empty()) {
		int next_t = INT_MAX;

		if (!arrivalProcs.empty()) {
			next_t = min(next_t, arrivalProcs[0].arr_time - t);
		}

		if (!runningProcs.empty()) {
			next_t = min(next_t, runningProcs[0].run_time);
		}

		next_t = max(0, next_t);

		t += next_t;

		if (!runningProcs.empty()) {
			int lastToRmove = 0;
			for (int i = 0; i < runningProcs.size(); i++) {
				runningProcs[i].run_time -= next_t;
				if (runningProcs[i].run_time <= 0) {
					++lastToRmove;
					removeProc(frames, runningProcs[i]);
					/* print remove msg */
					cout << "time " << t << "ms: Process " << runningProcs[i].ID << " removed:" << endl;
					printFrames(frames, framesPerLine);
				}
			}
			if (lastToRmove != 0) {
				runningProcs.erase(runningProcs.begin(), runningProcs.begin() + lastToRmove);
			}
		}

		if (!arrivalProcs.empty()) {
			Process p = arrivalProcs.front();
			if (p.arr_time <= t) {
				arrivalProcs.erase(arrivalProcs.begin());
				cout << "time " << t << "ms: Process " << p.ID << " arrived (requires " << p.mem << " frames)" << endl;
				if (isFramesEnough(frames, p)) {
					vector<int> portions = findFreePortions(frames, p.mem);
					if (portions.empty()) {
						cout << "time " << t << "ms: Cannot place process " << p.ID << " -- starting defragmentation" << endl;
						vector<char> moved;
						int movedFrame = defrag(frames, moved);
						int dTime = movedFrame * t_movetime;
						t += dTime;
						for (int i = 0; i < arrivalProcs.size(); i++) {
							arrivalProcs[i].arr_time += dTime;
						}
						cout << "time " << t << "ms: Defragmentation complete (moved " << movedFrame << " frames:";
						for (int i = 0; i < moved.size(); i++) {
							cout << " " << moved[i];
							if (i != moved.size() - 1) {
								cout << ",";
							}
						}
						cout << ")" << endl;
						printFrames(frames, framesPerLine);
						portions = findFreePortions(frames, p.mem);
					}
					runningProcs.push_back(p);
					sort(runningProcs.begin(), runningProcs.end(), procCmpRunFunction);
					fitfunc(portions, frames, p);
					/* print arrival msg */
					cout << "time " << t << "ms: Placed process " << p.ID << ":" << endl;
					printFrames(frames, framesPerLine);
				}
				else {
					/* print skip msg */
					cout << "time " << t << "ms: Cannot place process " << p.ID << " -- skipped!" << endl;
				}
			}
		}
	}
	if (fitfunc == nextfit) {
		cout << "time " << t << "ms: Simulator ended (Contiguous -- Next-Fit)" << endl;
	}
	else if (fitfunc == bestfit) {
		cout << "time " << t << "ms: Simulator ended (Contiguous -- Best-Fit)" << endl;
	}
	else if (fitfunc == worstfit) {
		cout << "time " << t << "ms: Simulator ended (Contiguous -- Worst-Fit)" << endl;
	}
}


int main(int argc, char** argv) 
{
	string inFile = "p2-input01.txt";
	if (argc > 1) {
		inFile = argv[1];
	}

	int t_memmove = 1;
	int framesPerLine = 32;

	vector<Process> procs = readInputFile(inFile);

	vector<char> frames;
	for (int i = 0; i < 8 * framesPerLine; i++) {
		frames.push_back('.');
	}

	Contiguous(frames, procs, framesPerLine, t_memmove, nextfit);
	cout << endl;
	Contiguous(frames, procs, framesPerLine, t_memmove, bestfit);
	cout << endl;
	Contiguous(frames, procs, framesPerLine, t_memmove, worstfit);
	cout << endl;
	NonContiguous(frames, procs, framesPerLine);
	
	return 0;
}