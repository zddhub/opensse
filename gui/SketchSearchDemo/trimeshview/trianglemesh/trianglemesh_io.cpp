#include "trianglemesh.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <cstdarg>
#include "strutil.h"
using namespace std;


#define dprintf TriangleMesh::dprintf
#define eprintf TriangleMesh::eprintf

#define GET_LINE() do { if (!fgets(buf, 1024, f)) return false; } while (0)
#define COND_READ(cond, where, len) do { if ((cond) && !fread((void *)&(where), (len), 1, f)) return false; } while (0)
#define FPRINTF(...) do { if (fprintf(__VA_ARGS__) < 0) return false; } while (0)
#define FWRITE(ptr, size, nmemb, stream) do { if (fwrite((ptr), (size), (nmemb), (stream)) != (nmemb)) return false; } while (0)
#define LINE_IS(text) begins_with(buf, text)

#define BIGNUM 1.0e10

//提前声明
static bool read_obj(FILE *f, TriangleMesh *mesh);

static void check_ind_range(TriangleMesh *mesh);
static void skip_comments(FILE *f);
static void tess(const vector<point> &verts, const vector<int> &thisface,
                 vector<TriangleMesh::Face> &tris);


// Read a TriMesh from a file.  Defined to use a helper function to make
// subclassing easier.
TriangleMesh *TriangleMesh::read(const char *filename)
{
    TriangleMesh *mesh = new TriangleMesh();

    if(read_helper(filename, mesh))
    {
        return mesh;
    }

    delete mesh;
    return NULL;
}

// Actually read a mesh.  Tries to figure out type of file from first
// few bytes.  Filename can be "-" for stdin.
// STL doesn't have a magic number, nor any other way of recognizing it.
// Recognize file.stl and stl:- constructions.
bool TriangleMesh::read_helper(const char *filename, TriangleMesh *mesh)
{
    if(!filename || *filename == '\0')
        return false;

    FILE *f = NULL;
    bool ok = false;
    int c;

    if (strcmp(filename, "-") == 0) {
        f = stdin;
        filename = "standard input";
    } else if (begins_with(filename, "stl:-")) {
        f = stdin;
        filename = "standard input";
    } else {
        f = fopen(filename, "rb");
        if (!f) {
                eprintf("Error opening [%s] for reading: %s.\n", filename,
                        strerror(errno));
                return false;
        }
    }
    dprintf("Reading %s... ", filename);

    c = fgetc(f);
    if (c == EOF) {
        eprintf("Can't read header.\n");
        goto out;
    }

    //读Obj文件
    if (c == '#' || c == 'v' || c == 'u' || c == 'f' || c == 'g' || c == 's' || c == 'o')
    {
        ungetc(c, f);
        ok = read_obj(f, mesh);
    }


out:
    if (f)
            fclose(f);
    if (!ok || mesh->vertices.empty()) {
            eprintf("Error reading file [%s].\n", filename);
            return false;
    }

    dprintf("Done.\n");
    check_ind_range(mesh);
    return true;
}

// Read an obj file
static bool read_obj(FILE *f, TriangleMesh *mesh)
{
    vector<int> thisface;
    while (1) {
        skip_comments(f);
        if (feof(f))
            return true;
        char buf[1024];
        GET_LINE();
        if (LINE_IS("v ") || LINE_IS("v\t")) {
            float x, y, z;
            if (sscanf(buf+1, "%f %f %f", &x, &y, &z) != 3) {
                return false;
            }
            mesh->vertices.push_back(point(x,y,z));
        } else if (LINE_IS("vn ") || LINE_IS("vn\t")) {
            float x, y, z;
            if (sscanf(buf+2, "%f %f %f", &x, &y, &z) != 3) {
                    return false;
            }
            mesh->normals.push_back(vec(x,y,z));
        } else if (LINE_IS("f ") || LINE_IS("f\t") ||
                   LINE_IS("t ") || LINE_IS("t\t")) {
            thisface.clear();
            char *c = buf;
            while (1) {
                while (*c && *c != '\n' && !isspace(*c))
                        c++;
                while (*c && isspace(*c))
                        c++;
                int thisf;
                if (sscanf(c, " %d", &thisf) != 1)
                        break;
                if (thisf < 0)
                        thisf += mesh->vertices.size();
                else
                        thisf--;
                thisface.push_back(thisf);
            }
            tess(mesh->vertices, thisface, mesh->faces);
        }
    }

    // XXX - FIXME
    // Right now, handling of normals is fragile: we assume that
    // if we have the same number of normals as vertices,
    // the file just uses per-vertex normals.  Otherwise, we can't
    // handle it.
    if (mesh->vertices.size() != mesh->normals.size())
            mesh->normals.clear();

    return true;
}


// Check whether the indices in the file mistakenly go
// from 1..N instead of 0..N-1
static void check_ind_range(TriangleMesh *mesh)
{
    if (mesh->faces.empty())
        return;
    int min_ind = mesh->faces[0][0];
    int max_ind = mesh->faces[0][0];
    for (size_t i = 0; i < mesh->faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            min_ind = min(min_ind, mesh->faces[i][j]);
            max_ind = max(max_ind, mesh->faces[i][j]);
        }
    }

    int nv = mesh->vertices.size();

    // All good
    if (min_ind == 0 && max_ind == nv-1)
        return;

    // Simple fix: offset everything
    if (max_ind - min_ind == nv-1) {
        dprintf("Found indices ranging from %d through %d\n",
                         min_ind, max_ind);
        dprintf("Remapping to %d through %d\n", 0, nv-1);
        for (size_t i = 0; i < mesh->faces.size(); i++)
                for (int j = 0; j < 3; j++)
                        mesh->faces[i][j] -= min_ind;
        return;
    }

    // Else can't do anything...
}


// Skip comments in an ASCII file (lines beginning with #)
static void skip_comments(FILE *f)
{
    int c;
    bool in_comment = false;
    while (1) {
            c = fgetc(f);
            if (c == EOF)
                    return;
            if (in_comment) {
                    if (c == '\n')
                            in_comment = false;
            } else if (c == '#') {
                    in_comment = true;
            } else if (!isspace(c)) {
                    break;
            }
    }
    ungetc(c, f);
}


// Tesselate an arbitrary n-gon.  Appends triangles to "tris".
static void tess(const vector<point> &verts, const vector<int> &thisface,
                 vector<TriangleMesh::Face> &tris)
{
    if (thisface.size() < 3)
        return;
    if (thisface.size() == 3) {
        tris.push_back(TriangleMesh::Face(thisface[0],
                                         thisface[1],
                                         thisface[2]));
        return;
    }
    if (thisface.size() == 4) {
        // Triangulate in the direction that
        // gives the shorter diagonal
        const point &p0 = verts[thisface[0]], &p1 = verts[thisface[1]];
        const point &p2 = verts[thisface[2]], &p3 = verts[thisface[3]];
        float d02 = dist2(p0, p2);
        float d13 = dist2(p1, p3);
        int i = (d02 < d13) ? 0 : 1;
        tris.push_back(TriangleMesh::Face(thisface[i],
                                     thisface[(i+1)%4],
                                     thisface[(i+2)%4]));
        tris.push_back(TriangleMesh::Face(thisface[i],
                                     thisface[(i+2)%4],
                                     thisface[(i+3)%4]));
        return;
    }

        // 5-gon or higher - just tesselate arbitrarily...
    for (size_t i = 2; i < thisface.size(); i++)
        tris.push_back(TriangleMesh::Face(thisface[0],
                                     thisface[i-1],
                                     thisface[i]));
}





// Debugging printout, controllable by a "verbose"ness parameter, and
// hookable for GUIs
#undef dprintf

int TriangleMesh::verbose = 1;

void TriangleMesh::set_verbose(int verbose_)
{
    verbose = verbose_;
}

void (*TriangleMesh::dprintf_hook)(const char *) = NULL;

void TriangleMesh::set_dprintf_hook(void (*hook)(const char *))
{
    dprintf_hook = hook;
}

void TriangleMesh::dprintf(const char *format, ...)
{
    if (!verbose)
        return;

    va_list ap;
    va_start(ap, format);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    if (dprintf_hook) {
        dprintf_hook(buf);
    } else {
        fprintf(stderr, "%s", buf);
        fflush(stderr);
    }
}


// Same as above, but fatal-error printout
#undef eprintf

void (*TriangleMesh::eprintf_hook)(const char *) = NULL;

void TriangleMesh::set_eprintf_hook(void (*hook)(const char *))
{
    eprintf_hook = hook;
}

void TriangleMesh::eprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    if (eprintf_hook) {
        eprintf_hook(buf);
    } else {
        fprintf(stderr, "%s", buf);
        fflush(stderr);
    }
}
