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
#include "debug.h"
#include "accelerationstruct.h"

      #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
ObjectLoader::ObjectLoader()
{
}


ObjectLoader::~ObjectLoader() {
}


bool ObjectLoader::loadOBJ(const std::string& filename, AccelerationStruct *tl) {
    std::cout << "Loading monkey.." << std::endl;
    std::vector<std::string> file;
    std::string line;
    file.clear();
    std::ifstream infile (filename.c_str(), std::ios_base::in);
    while (getline(infile, line, '\n'))  {
        file.push_back (line);
    }
    DEBUG("Read " << file.size() << " lines.");

    int vertexcount = 0;
    int facecount = 0;

    // regular expressions for normals, vertices, and faces
    std::string normalRE = "vn (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+)\\s*";
    std::string vertexRE = "v (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+) (-?[0-9]+\\.[0-9]+)\\s*";
    std::string faceRE = "f (\\d+)\\/(\\d*)\\/(\\d*) (\\d+)\\/(\\d*)\\/(\\d*) (\\d+)\\/(\\d*)\\/(\\d*)";

    std::vector<Vector3D> vertices;
    std::vector<Vector3D> normals;
    boost::regex re;
    boost::regex facere;
    boost::regex normalre;
//     determine scene boundaries while loading;
    fliess boundings[6] = {UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH};
    try  {
        // Set up the regular expression for case-insensitivity
        re.assign(vertexRE);
        facere.assign(faceRE);
        normalre.assign(normalRE);
    } catch (boost::regex_error& e) {
        std::cout << vertexRE << " is not a valid regular expression: \""
        << e.what() << "\"" <<std:: endl;
    }

    int currentLine;currentLine = 0;
    for (std::vector<std::string>::iterator it = file.begin(); it!=file.end(); ++it) {
        DEBUG(currentLine++);
        std::string& line = *it;
        if ( !line.empty()) {
            if ( ( line[0] == 'v' ) &&  ( line[1] == 'n' )) {
                DEBUG("Reading normal");
                ++vertexcount;
                  boost::cmatch matches;
                if (boost::regex_match(line.c_str(), matches, normalre)) {
                    const std::string m1(matches[1].first, matches[1].second);
                    const std::string m2(matches[2].first, matches[2].second);
                    const std::string m3(matches[3].first, matches[3].second);
                    normals.push_back(Vector3D(atof(m1.c_str()),atof(m2.c_str()),atof(m3.c_str())));
                } else {
                    std::cout << "The regexp \"" << normalre << "\" does not match \"" << line << "\"" << std::endl;
                }
           } else if ( line[0] == 'v' ) {
                DEBUG("Reading vertex");
                ++vertexcount;
                  boost::cmatch matches;
                if (boost::regex_match(line.c_str(), matches, re)) {
                    const std::string m1(matches[1].first, matches[1].second);
                    const std::string m2(matches[2].first, matches[2].second);
                    const std::string m3(matches[3].first, matches[3].second);
                    Vector3D newV(atof(m1.c_str()),atof(m2.c_str()),atof(m3.c_str()));
                    vertices.push_back(newV);
                    
                    unsigned int twoI;
                    for (unsigned int i = 0; i < 3; ++i) {
                      twoI = 2*i;
                      if (newV[i] < boundings[twoI])
                        boundings[twoI] = newV[i];
                      if (newV[i] > boundings[twoI+1])
                        boundings[twoI+1] = newV[i];
                    }
                } else {
                    std::cout << "The regexp \"" << re << "\" does not match \"" << line << "\"" << std::endl;
                }
            } else if ( line[0] == 'f' ) {
                DEBUG("Reading face");
                boost::cmatch matches;

                if (boost::regex_match(line.c_str(), matches, facere)) {

                    bool boundsOk = true;
                    int vidx[3];
                    int nidx[3];
                    int vertexCount = vertices.size();
                    int normalCount = normals.size();
                    for ( unsigned int i = 0; (i < 3) && boundsOk; ++i) {
                      std::string vmatch(matches[i*3 +1 ].first, matches[i*3 +1 ].second);
                      std::string nmatch(matches[i*3 +3 ].first, matches[i*3 +3 ].second);
                      vidx[i] = atoi(vmatch.c_str()) - 1;
                      nidx[i] = atoi(nmatch.c_str()) - 1;
                      boundsOk = (vidx[i] >= 0 ) && (vidx[i] < vertexCount) && (nidx[i] >= 0 ) && (nidx[i] < normalCount);
                      if (!boundsOk) {
                        DEBUG("i:" << i << "(" << vidx[i] << ", " << nidx[i]);
                      }
                    }
                    if (boundsOk) {
                      tl->addTriangle(Triangle(vertices[vidx[0]], vertices[vidx[1]], vertices[vidx[2]],
                                               normals[nidx[0]], normals[nidx[1]], normals[nidx[2]] ));
                      DEBUG("addTriangle(Triangle(vertices[" << vidx[0] << "] , vertices[" <<   vidx[1] << "], vertices[" << vidx[2] << "], normals[" << nidx[0] << "], normals[" << nidx[1] << "], normals[" << nidx[2] << "] ));") ;
                      ++facecount;                    
                    } else {
                      WARN("face specified invalid vertex or normal");
                    }
                } else
                  WARN("Face definition doesnt match syntax ("<< faceRE << ")");

            }
            else if ( line[0] == '#' ) 
              DEBUG("skipping commentary");;

        } 
    }
    std::cout << "done loading monkey(" << vertices.size() << ": vertices, " << facecount << " faces, " << normals.size() << " normals)" << std::endl;
    std::cout << "Scene boundaries x(min:" << boundings[0]  << ",max:" << boundings[1]  << ") y(min:" << boundings[2]  << ",max:" << boundings[3]  << ") z(min:" << boundings[4]  << ",max:" << boundings[5]  << ")" << std::endl;
    tl->setBounds(boundings);
    return true;
}

bool ObjectLoader::loadRA2(const std::string& filename, AccelerationStruct *tl) {
    FILE *fp;
    
    struct stat buf;
    stat(filename.c_str(), &buf);
    unsigned int trianglecount = buf.st_size / 36;
    
    ra2_chunk *currentchunk = new ra2_chunk[trianglecount];
    /* Open an existing binary file for reading.      */
    if (( fp = fopen ( filename.c_str(), "rb" ) ) == NULL ){
        printf ( "Cannot open file\n" );
        exit ( 1 );
    }

    fread ( currentchunk, trianglecount, sizeof(ra2_chunk), fp );
    std::cout << "Read " << trianglecount << " triangles";
    fclose ( fp );
    
    fliess boundings[6] = {UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH, UNENDLICH, -UNENDLICH};
    for ( unsigned int i = 0; i < trianglecount; ++i) {
      Vector3D p[] = {Vector3D(currentchunk[i][0]), Vector3D(currentchunk[i][1]), Vector3D(currentchunk[i][2])};
      for (unsigned char pi = 0; pi < 3; ++pi)
        for (unsigned char c = 0; c < 3; ++c) 
          if ( p[pi][c] < boundings[2*c] )
             boundings[2*c] = p[pi][c];
          else if ( p[pi][c] > boundings[2*c+1] )
             boundings[2*c+1] = p[pi][c];
      tl->addTriangle(Triangle(p[0], p[1], p[2]));
    }
    std::cout << "Scene boundaries x(min:" << boundings[0]  << ",max:" << boundings[1]  << ") y(min:" << boundings[2]  << ",max:" << boundings[3]  << ") z(min:" << boundings[4]  << ",max:" << boundings[5]  << ")" << std::endl;
    tl->setBounds(boundings);
    
    delete[] currentchunk;
    
    return 0;


}
