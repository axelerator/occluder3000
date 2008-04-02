//
// C++ Implementation: objectloader
//
// Description:
//
//
// Author: Axel Tetzlaff <axel.tetzlaff@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "objectloader.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/regex.hpp>
#include "accelerationstructure.h"
#include "scene.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
ObjectLoader::ObjectLoader() {
}


ObjectLoader::~ObjectLoader() {
}


bool ObjectLoader::loadOBJ(const std::string& filename, Scene& scene ) {
    std::string currentMat = "default";

    List<std::string> file;
    std::string line;
    file.clear();
    std::ifstream infile (filename.c_str(), std::ios_base::in);
    while (getline(infile, line, '\n'))  {
        file.push_back (line);
    }

//     int vertexcount = 0;
    int facecount = 0;

    // regular expressions for normals, vertices, and faces
    std::string normalRE = "vn (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+)\\s*";
    std::string vertexRE = "v (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+)\\s*";
    std::string faceRE = "f (\\d+)\\/(\\d*)\\/(\\d*) (\\d+)\\/(\\d*)\\/(\\d*) (\\d+)\\/(\\d*)\\/(\\d*)";

//     std::vector<Vec3> vertices;
    List<Vec3> normals;
    boost::regex re;
    boost::regex facere;
    boost::regex normalre;
//     determine scene boundaries while loading;

    try  {
        // Set up the regular expression for case-insensitivity
        re.assign(vertexRE);
        facere.assign(faceRE);
        normalre.assign(normalRE);
    } catch (boost::regex_error& e) {
        std::cout << vertexRE << " is not a valid regular expression: \""
        << e.what() << "\"" <<std:: endl;
    }

    int currentLine;
    currentLine = 0;
    for (List<std::string>::iterator it = file.begin(); it!=file.end(); ++it) {
        std::string& line = *it;
        if ( !line.empty()) {
            if ( ( line[0] == 'v' ) &&  ( line[1] == 'n' )) {
                boost::cmatch matches;
                if (boost::regex_match(line.c_str(), matches, normalre)) {
                    const std::string m1(matches[1].first, matches[1].second);
                    const std::string m2(matches[2].first, matches[2].second);
                    const std::string m3(matches[3].first, matches[3].second);
                    normals.push_back(Vec3(atof(m1.c_str()),atof(m2.c_str()),atof(m3.c_str())));
                } else {
                    std::cout << "The regexp \"" << normalre << "\" does not match \"" << line << "\"" << std::endl;
                }
            } else if ( line[0] == 'v' ) {
                boost::cmatch matches;
                if (boost::regex_match(line.c_str(), matches, re)) {
                    const std::string m1(matches[1].first, matches[1].second);
                    const std::string m2(matches[2].first, matches[2].second);
                    const std::string m3(matches[3].first, matches[3].second);
                    const Vec3 newV(atof(m1.c_str()),atof(m2.c_str()),atof(m3.c_str()));
                    scene.addVertex(newV);
                } else {
                    std::cout << "The regexp \"" << re << "\" does not match \"" << line << "\"" << std::endl;
                }
            } else if ( line[0] == 'f' ) {
                boost::cmatch matches;

                if (boost::regex_match(line.c_str(), matches, facere)) {

                    bool boundsOk = true;
                    int vidx[3];
                    int nidx[3];
                    int vertexCount = scene.getVertexCount();
                    int normalCount = normals.size();
                    for ( unsigned int i = 0; (i < 3) && boundsOk; ++i) {
                        std::string vmatch(matches[i*3 +1 ].first, matches[i*3 +1 ].second);
                        std::string nmatch(matches[i*3 +3 ].first, matches[i*3 +3 ].second);
                        vidx[i] = atoi(vmatch.c_str()) - 1;
                        nidx[i] = atoi(nmatch.c_str()) - 1;
                        boundsOk = (vidx[i] >= 0 ) && (vidx[i] < vertexCount) && (nidx[i] >= 0 ) && (nidx[i] < normalCount);
                        if (!boundsOk) {
                            std::cerr << "i:" << i << "(" << vidx[i] << ", " << nidx[i] << std::endl;
                        }
                    }
                    if (boundsOk) {
//                         tl->addTriangle(Triangle(vidx[0], vidx[1], vidx[2],
//                                                  normals[nidx[0]], normals[nidx[1]], normals[nidx[2]] , scene.getMaterial(currentMat), *tl));
                        scene.addPrimitive(Primitive(vidx[0], vidx[1], vidx[2], scene, currentMat, scene.getPrimitiveCount()));
                        ++facecount;
                    }
                }
            } else if ( line[0] == 'u' ) {
                currentMat = line.substr(7);
            }
        }
    }
    std::cout << "done loading monkey(" << scene.getVertexCount() << ": vertices, " << facecount << " faces, "  << std::endl;
    return true;
}

bool ObjectLoader::loadRA2(const std::string& filename, Scene& scene ) {
    std::string currentMat = "default";
    FILE *fp;

    struct stat buf;
    stat(filename.c_str(), &buf);
    unsigned int trianglecount = buf.st_size / 36;

    ra2_chunk *currentchunk = new ra2_chunk[trianglecount];
    /* Open an existing binary file for reading.      */
    if (( fp = fopen ( filename.c_str(), "rb" ) ) == NULL ) {
        printf ( "Cannot open file\n" );
        exit ( 1 );
    }

    fread ( currentchunk, trianglecount, sizeof(ra2_chunk), fp );
    std::cout << "Read " << trianglecount << " triangles" << std::endl;
    fclose ( fp );
    unsigned int vertexCount = 0;
    for ( unsigned int i = 0; i < trianglecount; ++i) {
        for (int t3 = 0; t3 < 3; ++t3) {
            const Vec3 currentVertex(currentchunk[i]+ t3*3);
            scene.addVertex(currentVertex);
            ++vertexCount;
        }
//         tl->addTriangle(Triangle(vertexCount - 3, vertexCount - 2, vertexCount - 1 , scene.getMaterial(currentMat), *tl));
        scene.addPrimitive(Primitive(vertexCount - 3, vertexCount - 2, vertexCount - 1 , scene, currentMat, scene.getPrimitiveCount() ));
    }
    delete[] currentchunk;
    return 0;


}
