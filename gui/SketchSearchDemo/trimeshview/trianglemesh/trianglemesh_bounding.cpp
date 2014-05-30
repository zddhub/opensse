/*************************************************************************
 * Copyright (c) 2014 Zhang Dongdong
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/

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


