//Demchenko Yaroslav K-23 "Process model imitation"

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <cstdlib>
#include <string>

using namespace std;

void clear(char* s, size_t n) {
	for (size_t i = 0; i < n; i++) {
		s[i] = '\0';
	}
}

char* get(char* s) {
	const size_t n = 30;
	size_t count = 0;
	char* res = (char*) malloc(n * sizeof(char));
	clear(res, n);

	size_t i = 0;
	while (s[i] != '\0' && s[i] != ' ') {
		res[i] = s[i++];	
		count++;
	}
	if (s[i] == '\0') {
		clear(s, count);
		return res;
	}
	for (i = 0; i < strlen(s) - count - 1; i++){
		s[i] = s[i + count + 1];
	}
	clear(s + strlen(s) - count - 1, count + 1);

	return res;
}

template <size_t N>
void comp(bitset<N>& accum) {
	accum = ~accum;
	bool up = false;

	if (accum[0] == 0) {
		accum[0] = 1;
		return;
	}

	up = true;
	
	size_t i = 0;
	while (up && i < N) {
		if (accum[i] == 1) {
			accum[i++] = 0;
			continue;
		}
		accum[i] = 1;
		up = false;		
	}
}

template<size_t Nbit>
class Processor {
private:
	string ins = "";
	bitset<Nbit> accum;
	string comm = "";
	string op = "";
	bitset<Nbit> opBits;
	bitset<Nbit> r1;
	bitset<Nbit> r2;
	size_t tc = 0;
	size_t pc = 0;
	size_t ps = 0;

	void printStatus();
	bool isRegister();
	bool isBitSet();
	void toBitSet(bitset<Nbit>& bs, char sys = ' ');
	void getOpInBits();
	void mov();
	void inv();
	void add();
	void sub();
	void mult();
	void div();
	void or();
	void and();
	string group(string str);
public:
	Processor();
	void exec(char* s);
};

int main(){
	const size_t size = 30;
	const size_t n = 255;
	const size_t Nbit = 14;
	char* s = (char*) malloc(size * sizeof(char));
	clear(s, size);

	Processor<Nbit> proc = Processor<Nbit>();
	ifstream f("../Lab2aos/commands.txt");
	while (f.getline(s, n)) {
		proc.exec(s);		
	}

	f.close();
    return 0;
}

template<size_t Nbit>
void 
Processor<Nbit>::printStatus() {
	cout << "Command: " << ins << endl;
	cout << "Ins: " << comm << " | ";
	if (isRegister()) {
		cout << "R" << op[1];
	}
	else {
		cout << group(opBits.to_string());
	}
	cout << endl;
	cout << "R1: " << group(r1.to_string()) << endl;
	cout << "R2: " << group(r2.to_string()) << endl;
	cout << "Accum: " << group(accum.to_string()) << endl;
	cout << "PC: " << pc << endl;
	cout << "TC: " << tc << endl;
	cout << "PS: " << ps << "\n\n\n";

	system("pause");
	cout << "\n\n";
}

template<size_t Nbit>
bool 
Processor<Nbit>::isRegister() {
	return op[0] == 'r' || op[0] == 'R';
}

template<size_t Nbit>
bool 
Processor<Nbit>::isBitSet() {
	for (size_t i = 0; i < op.length(); i++) {
		if (op[i] != '0' && op[i] != '1') {
			return false;
		}
	}
	return true;
}

template<size_t Nbit>
void 
Processor<Nbit>::toBitSet(bitset<Nbit>& bs, char sys) {
	opBits.reset();
	if (sys == 'b') {
		for (size_t i = 0; i < op.length(); i++) {
			bs[i] = op[op.length() - 1 - i] == '1' ? 1 : 0;
		}
		return;
	}

	int x = stoi(op);
	bool neg = false;
	if (x < 0) {
		neg = true;
		x *= -1;
	}
	bs = *(new bitset<Nbit>(x));
	if (neg) {
		comp(bs);
	}
}

template<size_t Nbit>
void 
Processor<Nbit>::getOpInBits() {
	if (isRegister()) {
		opBits = op[1] == '1' ? r1 : r2;
		return;
	}
	else if (isBitSet()) {
		toBitSet(opBits, 'b');
		return;
	}
	else {
		toBitSet(opBits);
	}
}

template<size_t Nbit>
string 
Processor<Nbit>::group(string str) {
	string res = "";
	size_t rest = Nbit % 4;
	size_t wh = (Nbit - rest) / 4;
	size_t i = 0;
	while (i < rest) {
		res += str[i++];
	}
	res += " ";
	for (size_t j = 0; j < wh; j++) {
		for (size_t k = j * 4 + rest; k < (j + 1) * 4 + rest; k++) {
			res += str[k];
		}
		res += " ";
	}
	return res;
}

template<size_t Nbit>
Processor<Nbit>::Processor() {
	for (size_t i = 0; i < Nbit; i++) {
		r1[i] = rand() % 2;
		r2[i] = rand() % 2;
	}
}

template<size_t Nbit>
void 
Processor<Nbit>::exec(char* s) {
	pc++;
	tc = 1;

	ins = s;
	comm = get(s);
	op = get(s);

	getOpInBits();
	printStatus();

	tc++;
	if (comm == "mov") {
		mov();
	}
	else if (comm == "inv") {
		inv();
	}
	ps = accum[Nbit - 1];
	printStatus();
}

template<size_t Nbit>
void
Processor<Nbit>::mov() {
	accum = opBits;
}

template<size_t Nbit>
void
Processor<Nbit>::inv() {
	size_t i;
	if (opBits[0] == 0) {
		i = 0;
		while (i < Nbit) {
			accum[i] = accum[i] == 0 ? 1 : 0;
			i += 2;
		}
	}
	else {
		i = 1;
		while (i < Nbit) {
			accum[i] = accum[i] == 0 ? 1 : 0;
			i += 2;
		}
	}
}

template<size_t Nbit>
void
Processor<Nbit>::add() {
	size_t up = 0;
	size_t bitSum;
	for (size_t i = Nbit - 1; i >= 0; i--) {
		bitSum = opBits[i] + accum[i] + up;
		accum[i] = bitSum % 2;
		up = bitSum > 1 ? 1 : 0;
	}
}

template<size_t Nbit>
void
Processor<Nbit>::sub() {
	opBits = opBits[Nbit - 1] == 1 ? comp(opBits) : opBits;
	add();
}