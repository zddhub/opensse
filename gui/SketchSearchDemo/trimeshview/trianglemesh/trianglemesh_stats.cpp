
#include "trianglemesh.h"
#include <algorithm>
#include <numeric>
using namespace std;

// A characteristic "feature size" for the mesh.  Computed as an approximation
// to the median edge length
float TriangleMesh::feature_size()
{
        need_faces();
        if (faces.empty())
                return 0.0f;

        int nf = faces.size();
        int nsamp = min(nf / 2, 333);

        vector<float> samples;
        samples.reserve(nsamp * 3);

        for (int i = 0; i < nsamp; i++) {
                // Quick 'n dirty portable random number generator
                static unsigned randq = 0;
                randq = unsigned(1664525) * randq + unsigned(1013904223);

                int ind = randq % nf;
                const point &p0 = vertices[faces[ind][0]];
                const point &p1 = vertices[faces[ind][1]];
                const point &p2 = vertices[faces[ind][2]];
                samples.push_back(dist2(p0,p1));
                samples.push_back(dist2(p1,p2));
                samples.push_back(dist2(p2,p0));
        }
        nth_element(samples.begin(),
                    samples.begin() + samples.size()/2,
                    samples.end());
        return sqrt(samples[samples.size()/2]);
}
