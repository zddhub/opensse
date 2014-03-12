
#include "trianglemesh.h"
#include "bsphere.h"
using namespace std;



//use the Miniball code


// Compute bounding sphere of the vertices.
void TriangleMesh::need_bsphere()
{
        if (vertices.empty() || bsphere.valid)
                return;

        dprintf("Computing bounding sphere... ");

        Miniball<3,float> mb;
        mb.check_in(vertices.begin(), vertices.end());
        mb.build();
        bsphere.center = mb.center();
        bsphere.r = sqrt(mb.squared_radius());
        bsphere.valid = true;

        dprintf("Done.\n  center = (%g, %g, %g), radius = %g\n",
                bsphere.center[0], bsphere.center[1],
                bsphere.center[2], bsphere.r);
}


