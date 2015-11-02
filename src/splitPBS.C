/*######################################################
  # splitPBS.C                                         #
  # Version: 0.1                                       #
  #                                                    #
  # Create PBS submit files for specified macro        #
  #                                                    #
  # Notes:                                             #
  # Macro arguments should start with -a               #
  #                                                    #
  # Initial directory should contain ./macros/         #
  #                                  ./logs/           #
  #                                  ./output/         #
  #                                                    #
  #                                                    #
  # Machine requirements can be specified by using     #
  # an optional final pararmeter.                      #
  # (default is any machine)                           #
  #                                                    #
  # APR: 11/07/12                                      #
  # andrew.paul.robinson@manchester.ac.uk              #
  ######################################################*/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

// Simple string replace function
bool str_replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


int main(int argc, char *argv[])
{

  //------------------------------------------------------------------------
  // Check arguments...
  // argv[1] = macro to run
  // argv[2] = file with macro arguments in
  // argv[3] = file with PBS header in
  // argv[4] = Initial Directory
  // argv[5] = output filename
  // argv[6] = total time of simulation
  // argv[7] = number of splits
  // argv[8] = Number of angles (optional)
  // argv[9] = Requirements (optional)

  if (argc < 9 || argc > 11){
      cout << "Usage: " << endl;
      cout << argv[0] << " macro_filename arguments_filename pbs_arguments_filename InitialDir output_dir output_filename total_sim_time number_of_splits number_of_angles(optional) machine_requirements_filename(optional)" << endl;
      cout << endl;
      cout << "eg:" << endl;
      cout << argv[0] << " TOMO.mac args.txt pbs.txt /home/me/SPECT/ TOMO_3MBq scan1 600 30 (30) (reqs.txt)" << endl;
      exit(1);
    }
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Read arguments
  string macro = argv[1];

  // Read macro arguments into string - assume the arguments are one per line
  string macro_args;
  string line;

  ifstream argumentsFile(argv[2]);
  if(!argumentsFile){
    cout << "Error opening arguments file " << argv[2] << endl;
    exit(1);
  }

  while(!argumentsFile.eof()){
    getline(argumentsFile,line);
    macro_args.append(line);
    macro_args.append(" ");
  }

  // Read condor arguments into string
  string condor_args;

  ifstream condorFile(argv[3]);
  if(!condorFile){
    cout << "Error opening condor file " << argv[3] << endl;
    exit(1);
  }
  stringstream buffer;
  buffer << condorFile.rdbuf();
  condor_args = buffer.str();

  // Copy the condor_args so we can start with the same one each time
  string condor_args_orig = condor_args;

  string InitialDir = argv[4];
  string OutputDir = argv[5];
  string OutputFile = argv[6];
  int totalTime = atoi(argv[7]);
  int nSplits = atoi(argv[8]);

  // Set number of angles to match splits unless specified
  int nAngles = nSplits;

  // If number of angles is specified....
  if (argc > 9){
    nAngles = atoi(argv[9]);
    if(nAngles > nSplits){
      cout << "nAngles (" << nAngles << ") is GREATER than nSplits (" << nSplits << ") - this is not possible!" << endl;
      exit(1);
    }
    if(totalTime % nAngles != 0){
      cout << "Total time is NOT a multiple of nAngles" << endl;
      exit(1);
    }
    if(nSplits % nAngles != 0){
      cout << "nAngles is NOT a multiple of nSplits" << endl;
      exit(1);
    }
  }
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Requirements (if specified)
  // If requirements are specified...
  if (argc == 11){
    cout << "Requirements not implemented in this version - exiting..." << endl;
    exit(1);
  }
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Check that the number of splits is a multiple of the total time
  if(totalTime % nSplits != 0){
    cout << "Total time is NOT a multiple of nSplits" << endl;
    exit(1);
  }

  float tStep = (float)totalTime / (float)nSplits;
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Open split output file
  string split_filename;
  split_filename = OutputFile;
  split_filename.append(".split");
  ofstream splitFile(split_filename.c_str());
  if(!splitFile){
    cout << "Error opening split file " << split_filename << endl;
    exit(1);
  }

  splitFile << "Number of files: " << nSplits << endl;
  splitFile << endl;
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Open submit file
  string sub_filename;
  sub_filename = OutputFile;
  sub_filename.append(".submit");
  ofstream subFile(sub_filename.c_str());
  if(!subFile){
    cout << "Error opening submission file " << sub_filename << endl;
    exit(1);
  }
  //------------------------------------------------------------------------


  //------------------------------------------------------------------------
  // Calculate the time / angle
  float timePerAngle = (float)totalTime / (float)nAngles;
  //------------------------------------------------------------------------

  float currentAngle;
  float omega;
  int angleIndex = 1;

  // Loop through nSplits...
  for(int i = 1; i <= nSplits; i++){

    // Reset the condor_arguments
    condor_args = condor_args_orig;

    //------------------------------------------------------------------------
    // Calculate what angle to use for the split (assumes that the heads only go 180deg during the scan)
    // Default:
    if(nAngles == nSplits){
      omega = 180.0 / (float)totalTime;
    }
    // nAngles < nSplits (greater than already excluded)
    else{

      if( (float)((i-1)*tStep) < ((float)angleIndex * timePerAngle) ){
  	// Still on current angle
  	// If we are on the first angle (always 0)
  	if(angleIndex == 1){
  	  currentAngle = 0;
  	  omega = 0;
  	}
  	else{
  	  currentAngle = (float)(angleIndex-1) * 180.0 / (float)nAngles;
  	  omega = currentAngle / (float)((i-1)*tStep);
  	}
      }
      else{
  	// Move to next angle
  	angleIndex++;
  	currentAngle = (float)(angleIndex-1) * 180.0 / (float)nAngles;
  	omega = currentAngle / (float)((i-1)*tStep);
      }  
    }

    // We want to move clockwise so omega needs to be -ve !!!
    omega = -1.0 * omega;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Open the pbs file for this split
    string submit_filename;
    submit_filename = OutputFile;
    submit_filename.append("-");
    char eye[3];
    sprintf(eye,"%d",i);
    submit_filename.append(eye);
    submit_filename.append(".pbs");
    ofstream pbsFile(submit_filename.c_str());
    if(!pbsFile){
      cout << "Error opening pbs file " << submit_filename << endl;
      exit(1);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Write Header to pbs file
    pbsFile << "#!/bin/bash" << endl;
    pbsFile << "#############################################################" << endl;
    pbsFile << "# PBS submit file" << endl;
    pbsFile << "#" << endl;
    pbsFile << "# macro: " << macro << endl;
    pbsFile << "# Arguments: " << argv[2] << endl;
    pbsFile << "# Total time: " << totalTime << endl;
    pbsFile << "# nSplits: " << nSplits << endl;
    pbsFile << "# nAngles: " << nAngles << endl;
    pbsFile << "#" << endl;
    pbsFile << "# Created: ";
    time_t rawtime;
    time(&rawtime);
    pbsFile << ctime(&rawtime);
    pbsFile << "#############################################################" << endl;
    pbsFile << endl;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // String replace the variables in pbs header file    
    // JOBNAME:
    string JOBNAME = OutputFile + "-" + eye ;
    cout << "JOBNAME = " << JOBNAME << endl;
    str_replace(condor_args,"JOBNAME",JOBNAME);

    // OUTFILE:
    string OUTFILE = InitialDir + "/logs/" + JOBNAME +  ".out";
    cout << "OUTFILE = " << OUTFILE << endl;
    str_replace(condor_args,"OUTFILE",OUTFILE);

    // ERRORFILE:
    string ERRORFILE = InitialDir + "/logs/" + JOBNAME + ".err"; 
    cout << "ERRORFILE = " << ERRORFILE << endl;
    str_replace(condor_args,"ERRORFILE",ERRORFILE);

    // WORKINGDIR:
    string WORKINGDIR = InitialDir + "/macros/";
    str_replace(condor_args,"WORKINGDIR",WORKINGDIR);

    // MACRO_ARGUMENTS:
    stringstream stepBuffer;
    stepBuffer.precision(10);
    //stepBuffer << "\"";
    stepBuffer << macro_args;
    stepBuffer << " -a seed auto -a time_slice " << tStep << " -a time_start 0 -a time_stop " << totalTime;
    stepBuffer << " -a t0 "<< (float)((i-1)*tStep) << " -a t1 " << (float)(i*tStep) << " -a output_file " << OutputFile << "-" << i;
    stepBuffer << " -a output_dir " << OutputDir << " -a omega " << omega << " " << macro;
    //stepBuffer << "\"";
    string step_args = stepBuffer.str();

    str_replace(condor_args,"MACRO_ARGUMENTS",step_args);
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Write split pbs file
    pbsFile << condor_args << endl;

    // Close the pbs file for this split
    pbsFile.close();
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Update Split file
    splitFile << "Root filename: ./output/" << OutputDir << "/ROOT/" << OutputFile << "-" << i << endl;
    splitFile << "interfile filename: ./output/" << OutputDir << "/InterFile/"  << OutputFile << "-" << i << endl;
    splitFile << "Timeslice is: " << tStep << " s" << endl;
    splitFile << "Start time is: 0.0 s " << endl;
    splitFile << "Stop time is: " << totalTime << " s" << endl;
    splitFile << "Virtual starttime: " << ((i-1)*tStep) << " s" << endl; 
    splitFile << "Virtual stoptime: " << (i*tStep) << " s" << endl;
    splitFile << endl;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Update submission file
    subFile << "qsub " << submit_filename << endl;
    //------------------------------------------------------------------------

  } // Close loop through splits

  //------------------------------------------------------------------------
  // Close splitFile
  splitFile << "Original Root filename: ./output/" << OutputDir << "/" << OutputFile << "-total" << endl;
  splitFile.close();
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  // Close submission file
  subFile.close();
  //------------------------------------------------------------------------


}

