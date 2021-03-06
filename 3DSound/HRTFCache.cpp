#include "stdafx.h"
#include "HRTFCache.h"
#include "dirent.h"
#include <stdio.h>

HRTFCache::HRTFCache()
{
	InitializeCache();
}

void HRTFCache::InitializeCache()
{
	cout << "Initializing HRTF cache.";
	lEar.resize(19);
	rEar.resize(19);
	for (int i = 0; i < lEar.size(); i++)
		LoadElevation(lEar[i], -90 + 10 * i, Left);
	cout << ".";
	for (int i = 0; i < rEar.size(); i++)
		LoadElevation(rEar[i], -90 + 10 * i, Right);
	cout << ".done!" << endl;
}

void HRTFCache::LoadElevation(vector<Nsound::Buffer>& buf, int elevation, Channel channel)
{
	// Since there is no HRTF data for elevation angles smaller than -40�, we clamp those angles to use the -40� data
	int elev = elevation < -40 ? -40 : elevation;

	// Look up the right directory
	string dirname = "HRTF/";
		   dirname += channel == Left ? "L/" : "R/";
		   dirname += to_string(elev) + "/";

	DIR *dir;
	struct dirent *ent;
	dir = opendir (dirname.c_str());
	if (dir != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
			string filename = ent->d_name;
			if (filename.substr(filename.find_last_of(".") + 1) == "wav")
			{
				filename = dirname + filename;
				buf.push_back(Nsound::Buffer(filename));
			}
		}
	}

	//cout << "HRTF " << (channel == 0 ? "(CHL_L)" : "(CHL_R)") << " elev " << elevation << " initialized with " << buf.size() << " impulse response(s) " << endl;
}

Nsound::Buffer HRTFCache::GetHRTF(vec3 lisPos, vec3 lisDir, vec3 srcPos, Channel channel)
{
	// Direction from listener to source
	vec3 dir = Normalize(srcPos - lisPos);
	lisDir = Normalize(lisDir);

	// Calculate elevation angle
	vec3 dProj = Project(dir, vec3(0, 0, 1));
	float elevation = Angle(dir, dProj) * sign(dir.z);

	// Calculate listener "right" direction (assume up is (0, 0, 1))
	vec3 r = Cross(lisDir, vec3(0, 0, 1));
	float pan;
	if (PointSide(r, lisPos - srcPos))
		pan = Angle(lisDir, dProj);
	else
		pan = 360 - Angle(lisDir, dProj);

	// Look up right/left channel
	if ((!invertChannels && channel == Right) || (invertChannels && channel == Left))
	{
		// Calculate elevation/pan index
		size_t elevIndex = round((elevation + 90.f) / 180.f * rEar.size());
		// size_t panIndex  = round(pan / 360.f * rEar[elevIndex].size());

		/*
		// Pan wraparound
		if (panIndex >= rEar[elevIndex].size())
			panIndex = 0;

		return rEar[elevIndex][panIndex];
		*/

		// Pan lerp
		float d = pan / 360.f * rEar[elevIndex].size();
		size_t panIndexLO = floor(d);
		size_t panIndexHI = ceil (d);
		d -= panIndexLO;

		// Wraparound
		if (panIndexLO >= rEar[elevIndex].size()) panIndexLO = 0;
		if (panIndexHI >= rEar[elevIndex].size()) panIndexHI = 0;

		// Interpolate between two buffers
		auto lo = rEar[elevIndex][panIndexLO];
		auto hi = rEar[elevIndex][panIndexHI];
		return lo * d + hi * (1 - d);
	}
	else
	{
		// Calculate elevation/pan index
		size_t elevIndex = round((elevation + 90.f) / 180.f * rEar.size());
		// size_t panIndex  = round(pan / 360.f * rEar[elevIndex].size());

		/*
		// Pan wraparound
		if (panIndex >= lEar[elevIndex].size())
			panIndex = 0;

		return lEar[elevIndex][panIndex];
		*/

		// Pan lerp
		float d = pan / 360.f * lEar[elevIndex].size();
		size_t panIndexLO = floor(d);
		size_t panIndexHI = ceil (d);
		d -= panIndexLO;

		// Wraparound
		if (panIndexLO >= lEar[elevIndex].size()) panIndexLO = 0;
		if (panIndexHI >= lEar[elevIndex].size()) panIndexHI = 0;

		// Interpolate between two buffers
		auto lo = lEar[elevIndex][panIndexLO];
		auto hi = lEar[elevIndex][panIndexHI];
		return lo * d + hi * (1 - d);
	}
}

void HRTFCache::ls()
{
	// Load HRTF files
	DIR *dir;
	struct dirent *ent;

	string dirname = "HRTF/R/";

	/* Open directory stream */
	dir = opendir (dirname.c_str());
	if (dir != NULL)
	{
		/* Print all files and directories within the directory */
		while ((ent = readdir (dir)) != NULL)
		{
			switch (ent->d_type)
			{
				case DT_REG:
				printf ("%s\n", ent->d_name);
				break;

				case DT_DIR:
				{
					printf ("%s/\n", ent->d_name);
					string dirname2 = dirname + ent->d_name;

					DIR *dir2;
					struct dirent *ent2;
					dir2 = opendir (dirname2.c_str());
					if (dir2 != NULL)
					{
						/* Print all files and directories within the directory */
						while ((ent2 = readdir (dir2)) != NULL)
						{
							string newName = ent2->d_name;
							int e = newName.find_first_of('e');
							if (e != string::npos)
							{
								newName = newName.substr(e+1, string::npos);
								
							}
							int a = newName.find_first_of('a');
							if (a != string::npos)
							{
								newName.erase(a, 1);
								
							}

							string oldName = dirname2 + "/" + ent2->d_name;
								   newName = dirname2 + "/" + newName;

							//cout << oldName << endl;
							//cout << newName << endl;
							
							int result;
							result = rename( oldName.c_str() , newName.c_str() );
							/*if ( result == 0 )
								puts ( "File successfully renamed" );
							else
								perror( "Error renaming file" );*/
							
						}
					}

					closedir (dir2);
				}
				break;

				case DT_LNK:
				printf ("%s@\n", ent->d_name);
				break;

				default:
				printf ("%s*\n", ent->d_name);
			}
		}

	closedir (dir);

	}
	else
	{
		/* Could not open directory */
		printf ("Cannot open directory %s\n", dirname);
		exit (EXIT_FAILURE);
	}
}

HRTFCache::~HRTFCache()
{
}
