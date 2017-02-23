#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;


class Endpoint {
public:
	int latency;
	vector<int> requests;

	Endpoint (int latency, int nbVids) {
		this->latency = latency;
		this->requests = vector<int> (nbVids, 0);
	}

	Endpoint(){}
};

class Cache {
public:
	vector<bool> videos;
	vector<int> latencies;

	Cache (int nbEnds, int nbVids) {
		this->latencies = vector<int>(nbEnds, -1);
		this->videos = vector<bool>(nbVids, false);
	}

	Cache(){}
};


int nbVids, nbEnds, nbRequests, nbCaches, cacheSize;
vector<Cache> caches;
vector<Endpoint> endPts;
vector<int> videos;


class VidsOpt {
public:
	int vidIdx;
	int requests;

	VidsOpt(){}
	
	VidsOpt(int vidIdx, int requests) {
		this->vidIdx = vidIdx;
		this->requests = requests;
	}

	const double weight () const {
		double vidSize = videos[vidIdx];
		double req = requests;
		return vidSize / req;
	}

	bool operator<(const VidsOpt & obj) const {
        return this->weight() < obj.weight(); // keep the same order
    }
};


int main () {
	// 5 videos, 2 endpoints, 4 request descriptions, 3 caches 100MB each.
	cin >> nbVids >> nbEnds >> nbRequests >> nbCaches >> cacheSize;

	// Init
	caches = vector<Cache> (nbCaches, Cache(nbEnds, nbVids));

	// Vids sizes
	for (int i=0 ; i<nbVids ; i++) {
		int val;
		cin >> val;
		videos.push_back(val);
	}

	
	// Endpoints
	for (int i=0 ; i<nbEnds ; i++) {
		int latency, localCaches;
		cin >> latency >> localCaches;

		endPts.push_back(Endpoint(latency, nbVids));

		for (int j=0 ; j<localCaches ; j++) {
			int idx, cacheLat;
			cin >> idx >> cacheLat;

			caches[idx].latencies[i] = cacheLat;
		}
	}

	// Vids
	for (int r=0 ; r<nbRequests ; r++) {
		int vidIdx, endIdx, nbReq;
		cin >> vidIdx >> endIdx >> nbReq;

		endPts[endIdx].requests[vidIdx] = nbReq;
	}

	string name;
	cin >> name;

	// ------------------------------------------------------

	for (int cacheIdx=0 ; cacheIdx<nbCaches ; cacheIdx++) {
		Cache & cache = caches[cacheIdx];

		// Possible vids
		vector<bool> possible (nbVids, false);
		vector<int> requests (nbVids, 0);
		for (int endIdx=0 ; endIdx<nbEnds ; endIdx++) {
			int latency = cache.latencies[endIdx];

			Endpoint & endPt = endPts[endIdx];
			
			if (latency != -1) { // Si joignable
				for (int vidIdx=0 ; vidIdx<nbVids ; vidIdx++) {
					int reqs = endPt.requests[vidIdx];

					if (reqs > 0) {
						possible[vidIdx] = true;
						requests[vidIdx] += reqs;
					}
				}
			}
		}

		// Create vector
		vector<int> vids;
		vector<VidsOpt> vids_opts;
		for (int i=0 ; i<nbVids ; i++) {
			if (possible[i]) {
				vids.push_back(i);
				vids_opts.push_back(VidsOpt(i, requests[i]));
			}
		}

		// order
		random_shuffle(vids.begin(), vids.end());
		sort(vids_opts.begin(), vids_opts.end());

		// select
		int size = 0;
		// for (int idx : vids) {
		for (VidsOpt & vo : vids_opts) {
			int idx = vo.vidIdx;
			if (size + videos[idx] <= cacheSize) {
				cache.videos[idx] = true;
				size += videos[idx];
			}
		}
	}

	// ------------------------------------------------------
	// Output

	stringstream filename;
	filename << "res/" << name << ".txt";
	ofstream out (filename.str());
	out << nbCaches << endl;

	for (int i=0 ; i<nbCaches ; i++) {
		Cache & cache = caches[i];
		out << i;

		for (int vidIdx=0 ; vidIdx<nbVids ; vidIdx++) {
			if (cache.videos[vidIdx]) {
				out << " " << vidIdx;
			}
		}

		out << endl;
	}

	return 0;
}
