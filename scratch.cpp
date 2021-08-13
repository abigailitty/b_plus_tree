#include <iostream>
#include <vector>

using namespace std;

int main()
{
	vector<int> first;
	vector<int> second;
	int i = 5;
	for(int j = 0; j < i/2; j++)
		first.push_back(j);
	for(int j = i/2; j < i; j++)
		second.push_back(j);

	for(unsigned int i = 0; i < first.size(); i++)
		cout << first[i] << ", ";
	cout << endl;
	
	for(unsigned int i = 0; i < second.size(); i++)
		cout << second[i] << ", ";
	cout << endl;	
}