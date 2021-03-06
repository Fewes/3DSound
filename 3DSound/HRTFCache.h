#pragma once
#include <Nsound/NsoundAll.h>
#include <vector>
#include <SFML/System.hpp>
#include "Utils.h"

using namespace std;
using namespace sf;

class HRTFCache
{
public:
	HRTFCache();
	~HRTFCache();

	void ls();

	Nsound::Buffer		GetHRTF(vec3 lisPos, vec3 lisDir, vec3 srcPos, Channel channel);
	
private:
	bool invertChannels = false;
	void InitializeCache();
	void LoadElevation(vector<Nsound::Buffer>& buf, int elevation, Channel channel);

	vector<vector<Nsound::Buffer>> rEar;
	vector<vector<Nsound::Buffer>> lEar;
};

