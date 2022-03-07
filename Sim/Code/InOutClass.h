#ifndef InOutClass_H
#define InOutClass_H

#include <set>
#include <memory>
#include <sstream>
#include <fstream>

#include "vector2D.h"
#include "Agent.h"
#include "World.h"

class World;

class InOutClass
{
    std::string OutputFilename;
    std::string InputFilename;
    World* IOworld;

public:
    // Output
    InOutClass (std::string in_filename,std::string out_filename, World* world); // creat the output file
    void WriteIteration(int iteration) const; // Append the current state of the crowd in the Output file
    void AppendInFile(std::string text) const; // Append "text" in the output file.

    // Input
    float ReadFloat( int line) const;// Read the corresponding type at line "line" of the file named after "filename".
    int ReadInt(int line) const;
    Vector2D Read2DVector(int line) const;
};

#endif //InOutClass_H