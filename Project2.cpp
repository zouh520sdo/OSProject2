#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

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
		cout << line << endl;
		vector<string> output;
		split(line, "/ ", output);
		char id = output[0][0];
		int mem_time = stoi(output[1]);
		for (int i = 2; i < output.size(); i+=2) {
			cout << output[i] << endl;
			cout << output[i + 1] << endl;

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
	for (int i = 0; i < frames.size(); i++) {
		if ((i + 1) % framesPerLine == 0) {
			cout << endl;
		}
		cout << frames[i];
	}
}

void NonContiguous(vector<char> frames, vector<Process> inputProcs) {
	vector<Process> arrivalProcs = inputProcs;
	vector<Process> runningProcs;
	int t = 0;

	vector<int> portions = findFreePortions(frames, 10);
	for (int i = 0; i < portions.size(); i+=2) {
		cout << "portion " << portions[i] << " " << portions[i + 1] << endl;
	}

	while (arrivalProcs.empty() && runningProcs.empty()) {
		int next_t = INT_MAX;

		if (!arrivalProcs.empty()) {
			next_t = min(next_t, arrivalProcs[0].arr_time - t);
		}

		if (!runningProcs.empty()) {
			next_t = min(next_t, runningProcs[0].run_time);
		}

		findFreePortions(frames, 10);

		t += next_t;
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
	cout << procs.size() << endl;

	for (int i = 0; i < procs.size(); i++) {
		cout << procs[i].ID << " " << procs[i].mem << " " << procs[i].arr_time << " " << procs[i].run_time << endl;
	}

	vector<char> frames;
	for (int i = 0; i < 40; i++) {
		frames.push_back('.');
	}
	for (int i = 10; i < 11; i++) {
		frames[i] = 'A';
	}
	frames[25] = 'A';
	frames[39] = 'B';
	NonContiguous(frames, procs);

	printFrames(frames, framesPerLine);

	return 0;
}