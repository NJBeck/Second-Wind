// TODO: actually do assertions rather than printing and inspecting
#include <iostream>
#include "SDL2/SDL.h"
#include <vector>

#include "utility.h"
#include "PositionHandler.h"

using namespace std;

void PositionTest() {
	struct TestEntity {
		unsigned long ID;
		double xPos;
		double yPos;
	};
	vector<TestEntity> vec{ {1, .2, .3}, { 2,.3,.4 }, { 0,.1,.4 }, { 4,0.0,.7 }};
	PositionHandler ph;
	for (auto& ent : vec) {
		ph.add(ent.ID, { ent.xPos,ent.yPos });
	}
	for (auto& entry : ph.Index) {
		cout << "ID: " << entry.first << " Indices: " << entry.second.xIndex << " " << entry.second.yIndex << endl;
	}
	cout << "xPositions ";
	for (auto& xpos : ph.xPositions) {
		cout << xpos.Pos << " ";
	}
	cout << endl;

	cout << "yPositions ";
	for (auto& ypos : ph.yPositions) {
		cout << ypos.Pos << " ";
	}
	cout << endl;
}

int main(int argc, char* args[]) {
	PositionTest();
	cin.get();
	return 0;
}