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
	int idx;

	Cache (int nbEnds, int nbVids) {
		this->latencies = vector<int>(nbEnds, -1);
		this->videos = vector<bool>(nbVids, false);
	}

	Cache(){}

	const double getNbViews () const;

	bool operator<(const Cache & obj) const {
        return this->getNbViews() < obj.getNbViews(); // keep the same order
    }
};


int nbVids, nbEnds, nbRequests, nbCaches, cacheSize;
vector<Cache> caches;
vector<Endpoint> endPts;
vector<int> videos;


const double Cache::getNbViews () const {
		double views = 0;

		for (int endIdx=0 ; endIdx<nbEnds ; endIdx++) {
			int latency = this->latencies[endIdx];

			Endpoint & endPt = endPts[endIdx];
			
			if (latency != -1) { // Si joignable
				for (int vidIdx=0 ; vidIdx<nbVids ; vidIdx++) {
					int reqs = endPt.requests[vidIdx];

					if (reqs > 0) {
						views += reqs * endPt.latency - reqs * this->latencies[endIdx];
					}
				}
			}
		}

		return views;
	}


class VidsOpt {
public:
	int vidIdx;
	int requests;
	int opti;

	VidsOpt(){}
	
	VidsOpt(int vidIdx, int requests, int opti) {
		this->vidIdx = vidIdx;
		this->requests = requests;
		this->opti = opti;
	}

	const double weight () const {
		double vidSize = videos[vidIdx];
		// double req = requests;
		// return vidSize / req;
		return vidSize / this->opti;
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
	int _i = 0;
	for (Cache & c : caches)
		c.idx = _i++;

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

	vector<Cache> sol;
	sort(caches.begin(), caches.end());
	int _i = 0;
	for (Cache & cache : caches) {
		cout << _i << endl;
		// cout << caches.size() << endl;
		// Cache cache = caches[0];
		// Possible vids
		vector<bool> possible (nbVids, false);
		vector<int> requests (nbVids, 0);
		vector<int> optis (nbVids, 0);
		for (int endIdx=0 ; endIdx<nbEnds ; endIdx++) {
			int latency = cache.latencies[endIdx];

			Endpoint & endPt = endPts[endIdx];
			
			if (latency != -1) { // Si joignable
				for (int vidIdx=0 ; vidIdx<nbVids ; vidIdx++) {
					int reqs = endPt.requests[vidIdx];

					if (reqs > 0) {
						possible[vidIdx] = true;
						requests[vidIdx] += reqs;
						optis[vidIdx] += reqs * endPt.latency - reqs * cache.latencies[endIdx];
					}
				}
			}
		}

		// Create vector
		vector<VidsOpt> vids_opts;
		for (int i=0 ; i<nbVids ; i++) {
			if (possible[i]) {
				vids_opts.push_back(VidsOpt(i, requests[i], optis[i]));
			}
		}

		// order
		sort(vids_opts.begin(), vids_opts.end());

		// select
		int size = 0;
		// for (int idx : vids) {
		for (VidsOpt & vo : vids_opts) {
			int idx = vo.vidIdx;
			if (size + videos[idx] <= cacheSize) {
				cache.videos[idx] = true;
				size += videos[idx];

				// Maj des views
				for (int endIdx=0 ; endIdx<nbEnds ; endIdx++) {
					int latency = cache.latencies[endIdx];
					
					if (latency != -1) { // Si joignable
						Endpoint & endPt = endPts[endIdx];
						endPt.requests[idx] = 0;
					}
				}
			}
		}

		sol.push_back(cache);
		// caches.erase(caches.begin());
	}

	// ------------------------------------------------------
	// Output

	stringstream filename;
	filename << "res/" << name << ".txt";
	ofstream out (filename.str());
	out << nbCaches << endl;

	for (int i=0 ; i<nbCaches ; i++) {
		Cache & cache = sol[i];
		out << cache.idx;

		for (int vidIdx=0 ; vidIdx<nbVids ; vidIdx++) {
			if (cache.videos[vidIdx]) {
				out << " " << vidIdx;
			}
		}

		out << endl;
	}

	return 0;
}
