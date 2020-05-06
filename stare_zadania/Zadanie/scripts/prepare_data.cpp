#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>

using namespace std;

int main(int argc, char*argv[]){
	if(argc < 2){
		cout << "usage: prepare_data <file>" << endl;
		return 1;
	}

	bool calc_mean = false;
	if(argc == 3 && strcmp(argv[2],"mean") == 0){
		calc_mean = true;
	}

	ifstream file(argv[1]);
	vector<int> x;
	vector<double> y_flow1;
	vector<double> y_flow2;
	vector<double> y_flow3;
	vector<double> y_flow4;
	vector<double> y_flow5;

	int x_value = 0;
	double y_value = 0.0;
	while(!file.eof()){
		file >> x_value;
		x.push_back(x_value);

		file >> y_value;
		y_flow1.push_back(y_value);

		file >> x_value;
		file >> y_value;
		y_flow2.push_back(y_value);

		file >> x_value;
		file >> y_value;
		y_flow3.push_back(y_value);

		file >> x_value;
		file >> y_value;
		y_flow4.push_back(y_value);

		file >> x_value;
		file >> y_value;
		y_flow5.push_back(y_value);
	}
	file.close();
	x.pop_back();
	y_flow1.pop_back();
	y_flow2.pop_back();
	y_flow3.pop_back();
	y_flow4.pop_back();
	y_flow5.pop_back();

	ofstream output(argv[1]);
	if(calc_mean){
		output << "#X Y" << endl;
		for(int i=0; i<x.size(); i++){
			double y = (y_flow1[i]+y_flow2[i]+y_flow3[i]+y_flow4[i]+y_flow5[i])/5.0;
			output << x[i] << " " << y << endl;
		}
	} else {
		output << "#X Y1 Y2 Y3 Y4 Y5" << endl;
		for(int i=0; i<x.size(); i++){
			output << x[i] << " " << y_flow1[i] << " " << y_flow2[i] << " " << y_flow3[i] << " " << y_flow4[i] << " " << y_flow5[i] << endl;
		}
	}
	output.close();

	return 0;
}