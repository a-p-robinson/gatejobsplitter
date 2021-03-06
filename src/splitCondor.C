/*######################################################
  # splitCondor.C                                      #
  # Version: 2.1                                       #
  #                                                    #
  # Create condor submit file for specified macro      #
  #                                                    #
  # Notes:                                             #
  # nSplits determines the time step so ensure         #   
  # that this matches the time used in the scan.       #
  #                                                    #
  # Macro arguments and condor arguments should be     #
  # in a text file one per line.                       #
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
  # It is now possible to adjust the number of angles  #
  # to make this indepenedent of the number fo splits. #
  # This requires a scanner macro with omega as a      #
  # parameter.                                         #
  #                                                    #
  # EJP / APR: 16/05/17                                #
  # andrew.robinson@npl.co.uk                          #
  ######################################################*/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <iomanip>

#include <time.h>
#include <stdlib.h>



using namespace std;

int main(int argc, char *argv[])
{

  // Added the option to generate peusdo random seeds (quick and dirty)
  srand(time(NULL));
  
  // Check arguments...
  // argv[1] = Version of GATE to use 
  // argv[2] = macro to run
  // argv[3] = file with macro arguments in
  // argv[4] = file with condor header in
  // argv[5] = Initial Directory
  // argv[6] = Output Location (RAID, home directory, local disk)
  // argv[7] = output directory
  // argv[8] = output filename
  // argv[9] = total time of simulation
  // argv[10] = number of splits
  // argv[11] = Number of angles (optional)
  // argv[12] = Requirements (optional)

  if (argc < 11 || argc > 13){
      cout << "Usage: " << endl;
      cout << argv[0] << " Gate_version macro_filename arguments_filename condor_arguments_filename InitialDir output_location output_dir output_filename total_sim_time number_of_splits number_of_angles(optional) machine_requirements_filename(optional)" << endl;
      cout << endl;
      cout << "eg:" << endl;
      cout << argv[0] << "7.0 TOMO.mac args.txt condor.txt $PWD output /home/me/SPECT/ TOMO_3MBq scan1 600 30 (30) (reqs.txt)" << endl;
      exit(1);
    }

  // Check GATE version
  double GATE_version;
  if (atof(argv[1]) == 6.1){
    GATE_version = atof(argv[1]);
  }
  else if (atof(argv[1]) == 6.2){
    GATE_version = atof(argv[1]);
  }
  else if (atof(argv[1]) == 7.0){
    GATE_version = atof(argv[1]);
  }
  else if (atof(argv[1]) == 7.1){
    GATE_version = atof(argv[1]);
  }
  else if (atof(argv[1]) == 7.2){
    GATE_version = atof(argv[1]);
  }
  else{
    cout << "GATE version is not set correctly. Options are 6.1, 6.2, 7.0, 7.1, 7.2" << endl;
    exit(1);
  }

  // Read arguments
  string macro = argv[2];

  // Read macro arguments into string
  // Assume the arguments are one per line
  string macro_args;
  string line;

  ifstream argumentsFile(argv[3]);
  if(!argumentsFile){
    cout << "Error opening arguments file " << argv[3] << endl;
    exit(1);
  }

  while(!argumentsFile.eof()){
    getline(argumentsFile,line);
    macro_args.append(line);
    //macro_args.append(" ");
  }

  // Read condor arguments into string
  string condor_args;

  ifstream condorFile(argv[4]);
  if(!condorFile){
    cout << "Error opening condor file " << argv[4] << endl;
    exit(1);
  }
  stringstream buffer;
  buffer << condorFile.rdbuf();
  condor_args = buffer.str();

  string InitialDir = argv[5];

  string OutputLoc = argv[6];

  string OutputDir = argv[7];

  string OutputFile = argv[8];
  int totalTime = atoi(argv[9]);
  int nSplits = atoi(argv[10]);

  // Set number of angles to match splits unless specified
  int nAngles = nSplits;

  // If number of angles is specified....
  if (argc > 11){
    nAngles = atoi(argv[11]);
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

  int nReqs;
  string requirements;
  ifstream reqFile;
  // If requirements are specified...
  if (argc == 13){
    // Read requirements
    reqFile.open(argv[12]);
    if(!reqFile){
      cout << "Error opening requirements file " << argv[12] << endl;
      exit(1);
    }

    // First line is the number of requirements
    getline(reqFile,line);
    nReqs = atoi(line.c_str());

    if (nReqs > 1 && nReqs < nSplits){
      cout << "Error: number of requirements (" << nReqs << ") > 1 AND < nSplits (" << nSplits << ")" << endl;
      exit(1);
    }
    else if (nReqs == 1){
      getline(reqFile,requirements);
    }
  }
  // else use defaults (any machine in pool)
  else{
    nReqs = 1;
    requirements = "Arch == \"INTEL\" || Arch == \"x86_64\"";
  }

  float tStep = (float)totalTime / (float)nSplits;

  // Open submit output file
  string submit_filename;
  submit_filename = OutputFile;
  submit_filename.append(".submit");
  ofstream submitFile(submit_filename.c_str());
  if(!submitFile){
    cout << "Error opening submit file " << submit_filename << endl;
    exit(1);
  }

  submitFile << "#############################################################" << endl;
  submitFile << "# Condor submit file" << endl;
  submitFile << "#" << endl;
  submitFile << "# macro: " << macro << endl;
  submitFile << "# Arguments: " << argv[2] << endl;
  submitFile << "# Total time: " << totalTime << endl;
  submitFile << "# nSplits: " << nSplits << endl;
  submitFile << "# nAngles: " << nAngles << endl;
  submitFile << "#" << endl;
  submitFile << "# Created: ";
  time_t rawtime;
  time(&rawtime);
  submitFile << ctime(&rawtime);
  submitFile << "#############################################################" << endl;


  submitFile << endl;
  submitFile << condor_args;
  submitFile << "Initialdir =" << InitialDir << "/macros/" << endl;
  submitFile << endl;


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

  // Calculate the time / angle
  float timePerAngle = (float)totalTime / (float)nAngles;

  float currentAngle;
  float omega;
  int angleIndex = 1;

  // Loop through nSplits...
  // Is there a reason this is set to start at 1?
  for(int i = 0; i < nSplits; i++){

    // Calculate what angle to use for the split (assumes that the heads only go 180deg during the scan)
    // Default:
    if(nAngles == nSplits){
      omega = 180.0 / (float)totalTime;
    }
    // nAngles < nSplits (greater than already excluded)
    // I.e. more than one split per angle
    // Don't need to change angle every split
    else{
      // If we are still on current angle
      if( (float)(i*tStep) < ((float)angleIndex * timePerAngle) ){
	// If we are on the first angle (always 0)
	if(angleIndex == 1){
	  currentAngle = 0;
	  omega = 0;
	}
	// Else we are not on the first angle
	// Need to set omega to a non-zero value
	else{
	  currentAngle = (float)(angleIndex-1) * 180.0 / (float)nAngles;
	  omega = currentAngle / (float)(i*tStep);
	}
      }
      // Else move to next angle
      else{
	angleIndex++;
	currentAngle = (float)(angleIndex-1) * 180.0 / (float)nAngles;
	omega = currentAngle / (float)(i*tStep);
      }  
      //cout << "[" << i << "] " << ((i-1)*tStep) << " " << currentAngle << " " << omega << endl; 
    }

    // Set the requirments for this split
    if(nReqs != 1){
      // We have specified individual requirements
      // Read next line from requirements file
      getline(reqFile,requirements);
    }

    // We want to move clockwise so omega needs to be -ve !!!
    omega = -1.0 * omega;
    // If the camera was set to rotate Counter Clockwise, leave omega +ve !!!
    // This should be a very rare occurence

    // Submit file
    stringstream stepBuffer;
    stepBuffer.precision(10);
    stepBuffer << "\"";

    // Adding option to manually specify seed
    // This is to fix a problem with GATE 7.2 where the auto seed does not work reliably
    // (initially hard code the option)
    stringstream seed_options;
    int seed = 2;
    if (seed == 0){
      // GATE auto seed
      seed_options << "auto";
    }
    if (seed == 1){
      // Sequential seeds
      seed_options << i;
    }
    if (seed == 2){
      // Fixed Random Seeds
      int r = rand();
      seed_options << r;
    }
    
    // GATE 6.2 and higher
    if (GATE_version != 6.1){
      stepBuffer << "-a ";
      stepBuffer << macro_args;
      stepBuffer << "[seed," << seed_options.str() << "][time_slice," << tStep << "][time_start,0][time_stop," << totalTime << "]";
      stepBuffer << "[t0,"<< (float)(i*tStep) << "][t1," << (float)((i+1)*tStep) << "][output_file," << OutputFile << "-" << i << "]";
      stepBuffer << "[output_dir," << OutputDir << "][omega," << omega << "]"  << " " << macro;
    }
    else if (GATE_version == 6.1){
    // GATE 6.1
      stepBuffer << macro_args;
      stepBuffer << " -a seed " << seed_options.str() << " -a time_slice " << tStep << " -a time_start 0 -a time_stop " << totalTime;
      stepBuffer << " -a t0 "<< (float)(i*tStep) << " -a t1 " << (float)((i+1)*tStep) << " -a output_file " << OutputFile << "-" << i;
      stepBuffer << " -a output_dir " << OutputDir << " -a omega " << omega << " " << macro;
    }

    stepBuffer << "\"";
    string step_args = stepBuffer.str();

    submitFile << "# Split " << i << ": " << (i*tStep) << " - " <<  ((i+1)*tStep) << "s" << endl;
    submitFile << "Requirements = " << requirements << endl;
    submitFile << "Arguments = " << step_args << endl;
    submitFile << "Output    = " << InitialDir << "/" << OutputLoc << "/" << OutputDir << "/Logs/" << OutputFile << "-" << i << ".out" <<endl; 
    submitFile << "Error     = " << InitialDir << "/" << OutputLoc << "/" << OutputDir << "/Logs/" << OutputFile << "-" << i << ".err" <<endl; 
    submitFile << "Log       = " << InitialDir << "/" << OutputLoc << "/" << OutputDir << "/Logs/" << OutputFile << "-" << i << ".log" <<endl; 
    submitFile << "Queue" << endl;
    submitFile << endl;

    // Split file
    splitFile << "Root filename: ./" << OutputLoc << "/" << OutputDir << "/ROOT/" << OutputFile << "-" << i << endl;
    splitFile << "interfile filename: ./" << OutputLoc << "/" << OutputDir << "/InterFile/"  << OutputFile << "-" << i << endl;
    splitFile << "Timeslice is: " << tStep << " s" << endl;
    splitFile << "Start time is: 0.0 s " << endl;
    splitFile << "Stop time is: " << totalTime << " s" << endl;
    splitFile << "Virtual starttime: " << (i*tStep) << " s" << endl; 
    splitFile << "Virtual stoptime: " << ((i+1)*tStep) << " s" << endl;
    splitFile << endl;

  } 

  splitFile << "Original Root filename: ./" << OutputLoc << "/" << OutputDir << "/" << OutputFile << "-total" << endl;


  // -------------------------------------------------------------------------------------
  // APR: Don't really need to generate these shell scripts - they seem a bit redundant

  // // Write bash shell script to source appropriate version of GATE
  // // and submit job to cluster
  // // Open shell script file
  // string submit_script_fname;
  // submit_script_fname = "Submit-";
  // submit_script_fname.append(OutputFile);
  // submit_script_fname.append(".sh");

  // ofstream submitScript(submit_script_fname.c_str());
  // if(!submitScript){
  //   cout << "Error opening submit file " << submit_script_fname << endl;
  //   exit(1);
  // }

  // submitScript << "#!/bin/bash" << endl;
  // submitScript << endl;
  // submitScript << "#######################################" << endl;
  // submitScript << fixed << setprecision(1);
  // submitScript << "# Script to source GATE version " << GATE_version << endl;
  // submitScript << "# and submit job to condor" << endl;
  // submitScript << "# Created: ";
  // submitScript << ctime(&rawtime);
  // submitScript << "#######################################" << endl;
  // submitScript << endl;

  // // The location of the shell script depends on the version of GATE
  // // This is hardcoded, as everything should end up like
  // // nucpc121
  // if (GATE_version == 6.1){
  //   submitScript << "# Source Gate v6.1" << endl;
  //   submitScript << "echo \"Will source GATE v6.1 from /opt/gate_v6.1/bin/gateConf.sh\"" << endl;
  //   submitScript << "source /opt/gate_v6.1/bin/gateConf.sh" << endl;
  //   submitScript << "# Submit jobs to condor" << endl;
  //   submitScript << "echo \"Will submit " << submit_filename << " to condor\"" << endl;
  //   submitScript << "condor_submit " << submit_filename << endl;
  // }
  // else if (GATE_version == 6.2){
  //   submitScript << "# Source Gate v6.2" << endl;
  //   submitScript << "echo \"Will source GATE v6.2 from /opt/gate_v6.2-install/bin/gateConf.sh\"" << endl;
  //   submitScript << "source /opt/gate_v6.2-install/bin/gateConf.sh" << endl;
  //   submitScript << "# Submit jobs to condor" << endl;
  //   submitScript << "echo \"Will submit " << submit_filename << " to condor\"" << endl;
  //   submitScript << "condor_submit " << submit_filename << endl;
  // }
  // else if (GATE_version == 7.0){
  //   submitScript << "# Source Gate v7.0" << endl;
  //   submitScript << "echo \"Will source GATE v7.0 from /opt/gate_v7.0-install/bin/gateConf.sh\"" << endl;
  //   submitScript << "source /opt/gate_v7.0-install/bin/gateConf.sh" << endl;
  //   submitScript << "# Submit jobs to condor" << endl;
  //   submitScript << "echo \"Will submit " << submit_filename << " to condor\"" << endl;
  //   submitScript << "condor_submit " << submit_filename << endl;
  // }
  // else if (GATE_version == 7.1){
  //   submitScript << "# Source Gate v7.1" << endl;
  //   submitScript << "echo \"Will source GATE v7.1 from /opt/gate_v7.1-install/bin/gateConf.sh\"" << endl;
  //   submitScript << "source /opt/gate_v7.1-install/bin/gateConf.sh" << endl;
  //   submitScript << "# Submit jobs to condor" << endl;
  //   submitScript << "echo \"Will submit " << submit_filename << " to condor\"" << endl;
  //   submitScript << "condor_submit " << submit_filename << endl;
  // }

  // // Write bash shell script to source appropriate version of GATE
  // // and merge root files
  // // Open shell script file
  // string merge_script_fname;
  // merge_script_fname = "Merge-";
  // merge_script_fname.append(OutputFile);
  // merge_script_fname.append(".sh");

  // ofstream mergeScript(merge_script_fname.c_str());
  // if(!mergeScript){
  //   cout << "Error opening merge file " << merge_script_fname << endl;
  //   exit(1);
  // }

  // mergeScript << "#!/bin/bash" << endl;
  // mergeScript << endl;
  // mergeScript << "#######################################" << endl;
  // mergeScript << fixed << setprecision(1);
  // mergeScript << "# Script to source GATE version " << GATE_version << endl;
  // mergeScript << "# and merge output files" << endl;
  // mergeScript << "# Created: ";
  // mergeScript << ctime(&rawtime);
  // mergeScript << "#######################################" << endl;
  // mergeScript << endl;

  // // The location of the shell script depends on the version of GATE
  // // This is hardcoded, as everything should end up like
  // // nucpc121
  // if (GATE_version == 6.1){
  //   mergeScript << "# Source Gate v6.1" << endl;
  //   mergeScript << "echo \"Will source GATE v6.1 from /opt/gate_v6.1/bin/gateConf.sh\"" << endl;
  //   mergeScript << "source /opt/gate_v6.1/bin/gateConf.sh" << endl;
  //   mergeScript << "# Merge root files" << endl;
  //   mergeScript << "echo \"Will merge " << split_filename << " output files\"" << endl;
  //   mergeScript << "gjm " << split_filename << endl;
  // }
  // else if (GATE_version == 6.2){
  //   mergeScript << "# Source Gate v6.2" << endl;
  //   mergeScript << "echo \"Will source GATE v6.2 from /opt/gate_v6.2-install/bin/gateConf.sh\"" << endl;
  //   mergeScript << "source /opt/gate_v6.2-install/bin/gateConf.sh" << endl;
  //   mergeScript << "# Merge root files" << endl;
  //   mergeScript << "echo \"Will merge " << split_filename << " output files\"" << endl;
  //   mergeScript << "gjm " << split_filename << endl;
  // }
  // else if (GATE_version == 7.0){
  //   mergeScript << "# Source Gate v7.0" << endl;
  //   mergeScript << "echo \"Will source GATE v7.0 from /opt/gate_v7.0-install/bin/gateConf.sh\"" << endl;
  //   mergeScript << "source /opt/gate_v7.0-install/bin/gateConf.sh" << endl;
  //   mergeScript << "# Merge root files" << endl;
  //   mergeScript << "echo \"Will merge " << split_filename << " output files\"" << endl;
  //   mergeScript << "gjm " << split_filename << endl;
  // }
  // else if (GATE_version == 7.1){
  //   mergeScript << "# Source Gate v7.1" << endl;
  //   mergeScript << "echo \"Will source GATE v7.1 from /opt/gate_v7.1-install/bin/gateConf.sh\"" << endl;
  //   mergeScript << "source /opt/gate_v7.1-install/bin/gateConf.sh" << endl;
  //   mergeScript << "# Merge root files" << endl;
  //   mergeScript << "echo \"Will merge " << split_filename << " output files\"" << endl;
  //   mergeScript << "gjm " << split_filename << endl;
  // }
  //
  // submitScript.close();
  // mergeScript.close();
  // -------------------------------------------------------------------------------------

  // Close files
  submitFile.close();
  splitFile.close();

}

