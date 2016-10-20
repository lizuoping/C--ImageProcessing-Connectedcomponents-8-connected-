#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int **zeroFramedAry;
int *EQAry;
string *Property;
int neighborAry[8];
int numRows, numCols, minVal, maxVal, newMin, newMax=0, newLabel=0, numLabelUsed=0;

void initial(string fileName) {
	string line,str;
	int row = 1, col = 1;
	ifstream myfile(fileName);
	if (myfile.is_open()) {
		int order = 0;
		while(myfile>>str) {
			order++;
			if (order==1) numRows = stoi(str);
			else if(order==2){
				numCols = stoi(str);
				zeroFramedAry = new int*[numRows+2];
				for (int i = 0; i < numRows+2; i++)
					zeroFramedAry[i] = new int[numCols+2];
				for(int i=1; i<numRows+1; i++)
					for(int j=1; j<numCols+1; j++)
						zeroFramedAry[i][j]=0;
				EQAry = new int[numRows*numCols/4];
				for(int i=0; i<numRows*numCols/4; i++)
					EQAry[i]=i;
			} else if(order==3) minVal = stoi(str);
			else if(order==4) {maxVal = stoi(str); newMin=maxVal;}
			else {
				zeroFramedAry[row][col++] = stoi(str);
				if(col>numCols) {row++;	col=1;}
			}
		}
		myfile.close();
	} else {
		cout << "cannot open " << fileName << endl;
		exit(1);
	}
}

void loadNeighbors(int row, int col) {
	neighborAry[0]=zeroFramedAry[row-1][col-1];
	neighborAry[1]=zeroFramedAry[row-1][col];
	neighborAry[2]=zeroFramedAry[row-1][col+1];
	neighborAry[3]=zeroFramedAry[row][col-1];
	neighborAry[4]=zeroFramedAry[row][col+1];
	neighborAry[5]=zeroFramedAry[row+1][col-1];
	neighborAry[6]=zeroFramedAry[row+1][col];
	neighborAry[7]=zeroFramedAry[row+1][col+1];
}

void updateEQAry(int index, int val) {
	EQAry[index] = val;
}

void printEQAry(ofstream& myfile) {
	for(int i=1; i<=newLabel; i++)
		myfile<<i<<" ";
	myfile<<endl;
	for(int i=1; i<=newLabel; i++)
		if(i>9 && EQAry[i]<10)
			myfile<<" "<<EQAry[i]<<" ";
		else
			myfile<<EQAry[i]<<" ";
	myfile<<endl;
}

int findMinNotZero(int row, int col, int pass) {
	int min_num=0, not_zero[4], min_value=99;
	if(pass==1) {
		for(int i=0; i<4; i++)
			if(neighborAry[i]!=0)
				not_zero[min_num++]=neighborAry[i];
		for(int j=0; j<min_num; j++)
			if(not_zero[j]<min_value)
				min_value=not_zero[j];
	}
	if(pass==2) {
		for(int i=4; i<8; i++)
			if(neighborAry[i]!=0)
				not_zero[min_num++]=neighborAry[i];
		for(int j=0; j<min_num; j++)
			if(not_zero[j]<min_value)
				min_value=not_zero[j];
	}
	return min_value;
}

void ConnectCC_Pass1() {
	for(int i=1; i<numRows+1; i++) 
		for(int j=1; j<numCols+1; j++) 
			if(zeroFramedAry[i][j]>0) {
				loadNeighbors(i,j);
				if(neighborAry[0]==0 && neighborAry[1]==0 && neighborAry[2]==0 && neighborAry[3]==0)
					zeroFramedAry[i][j]=++newLabel;
				else
					zeroFramedAry[i][j]=findMinNotZero(i,j,1);
			}
}

void ConnectCC_Pass2() {
	for(int i=numRows; i>0; i--) 
		for(int j=numCols; j>0; j--) {
			loadNeighbors(i,j);
			if(zeroFramedAry[i][j]>0)
				if(neighborAry[4]!=0 || neighborAry[5]!=0 || neighborAry[6]!=0 || neighborAry[7]!=0)
					if(zeroFramedAry[i][j]!=neighborAry[4] || zeroFramedAry[i][j]!=neighborAry[5] || zeroFramedAry[i][j]!=neighborAry[6] || zeroFramedAry[i][j]!=neighborAry[7]) {
						int minLabel = findMinNotZero(i,j,2);
						updateEQAry(zeroFramedAry[i][j], minLabel);
						zeroFramedAry[i][j] = minLabel;
					}
		}
}

void manageEQAry() {
	for(int i=1; i<=newLabel; i++) 
		if(EQAry[i]==i)
			EQAry[i]=++numLabelUsed;
		else
			EQAry[i]=EQAry[EQAry[i]];
}

void ConnectCC_Pass3() {
	for(int i=1; i<numRows+1; i++) 
		for(int j=1; j<numCols+1; j++) 
			if(zeroFramedAry[i][j]>0)
				zeroFramedAry[i][j]=EQAry[zeroFramedAry[i][j]];
}

void computeProperty() {
	Property = new string[numLabelUsed+1];
	for(int m=1; m<=numLabelUsed; m++) {
		Property[m]=to_string(m)+" ";
		int sumPixel = 0;
		for (int i = 1; i < numRows+1; i++)
			for (int j = 1; j < numCols+1; j++)
				if (zeroFramedAry[i][j]==m)
					sumPixel++;
		Property[m]+=to_string(sumPixel)+" ";
		int minR=numRows, minC=numCols, maxR=0, maxC=0;
		for (int i = 1; i < numRows+1; i++)
			for (int j = 1; j < numCols+1; j++)
				if (zeroFramedAry[i][j]==m) {
					minR=(i<minR? i:minR);
					minC=(j<minC? j:minC);
					maxR=(i>maxR? i:maxR);
					maxC=(j>maxC? j:maxC);
				}
		Property[m]+=to_string(minR)+" "+to_string(minC)+" "+to_string(maxR)+" "+to_string(maxC);
	}
}

void connectedCC8(char **argv) {
	initial(argv[1]);
	ConnectCC_Pass1();
	ConnectCC_Pass2();
	manageEQAry();
	ConnectCC_Pass3();
	computeProperty();
	ofstream myfile(argv[2]);
	if (myfile.is_open()) {
		newMin = zeroFramedAry[1][1];
		for (int i = 1; i < numRows+1; i++) 
			for (int j = 1; j < numCols+1; j++){
				newMin=(zeroFramedAry[i][j]<newMin?zeroFramedAry[i][j]:newMin);
				newMax=(zeroFramedAry[i][j]>newMax?zeroFramedAry[i][j]:newMax);
			}
		myfile<<numRows<<" "<<numCols<<" "<<newMin<<" "<<newMax<<endl;
		for (int i = 1; i < numRows+1; i++) {
			for (int j = 1; j < numCols+1; j++) 
				if (zeroFramedAry[i][j] >0)
					myfile <<zeroFramedAry[i][j]<<" ";
				else
					myfile <<"0 ";
			myfile << endl;
		}
		myfile.close();
	} else {
		cout << "cannot write " << argv[2] << endl;
		exit(1);
	}
	myfile.open(argv[3]);
	if (myfile.is_open()) {
		for(int i=1; i<=numLabelUsed; i++)
			myfile<<Property[i]<<endl;
		myfile.close();
	} else {
		cout << "cannot write " << argv[3] << endl;
		exit(1);
	}
}

int main(int argc, char** argv) {
	connectedCC8(argv);
	cout << "All work done!" << endl;
	return 0;
}
