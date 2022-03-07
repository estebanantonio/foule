#include "InOutClass.h"

InOutClass::InOutClass (std::string in_filename,std::string out_filename, World* world) : OutputFilename(out_filename),InputFilename(in_filename),IOworld(world)
{
    ofstream file(OutputFilename);
    file << "Iteration \t Time \t AgentID \t pos_x \t pos_y \t DownPos_x \t DownPos_y " << std::endl; // write the header of the output file
    file << "TimeStep 0 = " << IOworld->getTimeStep() << std::endl; 
}

void InOutClass::WriteIteration(int iteration) const
{
    ofstream file(OutputFilename,std::ios_base::app); // Argument "std::ios_base::app" mean that we are appening text in the file.
    for (std::shared_ptr<Agent> a:IOworld->getCrowd())
    {
        file << iteration << '\t' <<IOworld->getClock() << '\t' <<a->ToString() << endl;
    }
     
}