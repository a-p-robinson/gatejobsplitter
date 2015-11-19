/*######################################################
  # splitCondor.C                                      #
  # Version: 0.2                                       #
  #                                                    #
  # Create condor submit file for specified macro      #
  #                                                    #
  # Notes:                                             #
  # nSplits determines the time step so ensure         #   
  # that this matches the time used in the scan.       #
  #                                                    #
  # Macro arguments and condor arguments should be     #
  # in a text file one per line.                       #
  #                                                    #
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
  # APR: 26/10/11                                      #
  # andrew.paul.robinson@manchester.ac.uk              #
  ######################################################*/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{

  // Check arguments...
  // argv[1] = macro to run
  // argv[2] = file with macro arguments in
  // argv[3] = file with condor header in
  // argv[4] = Initial Directory
  // argv[5] = output filename
  // argv[6] = total time of simulation
  // argv[7] = number of splits
  // argv[8] = Number of angles (optional)
  // argv[9] = Requirements (optional)
  // argv[10] = Use old (Gate 6.1) format (optional)

  if (argc < 9 || argc > 12){
      cout << "Usage: " << endl;
      cout << argv[0] << " macro_filename arguments_filename condor_arguments_filename InitialDir output_dir output_filename total_sim_time number_of_splits number_of_angles(optional) machine_requirements_filename(optional) Gate6.1_format=0(optional)" << endl;
      cout << endl;
      cout << "eg:" << endl;
      cout << argv[0] << " TOMO.mac args.txt condor.txt /home/me/SPECT/ TOMO_3MBq scan1 600 30 (30) (reqs.txt)" << endl;
      exit(1);
    }

  // Read arguments
  string macro = argv[1];

  // Read macro arguments into string
  // Assume the arguments are one per line
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
    //macro_args.append(" ");
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

  int nReqs;
  string requirements;
  ifstream reqFile;
  // If requirements are specified...
  if (argc == 11){
    // Read requirements
    reqFile.open(argv[10]);
    if(!reqFile){
      cout << "Error opening requirements file " << argv[10] << endl;
      exit(1);
    }
    //getline(reqFile,requirements);

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

  // Have we asked for GATE6.1 version?
  int oldGATE = 0;
  if (argc == 12){
    int oldGATE = 1;
  }

  // Check that the number of splits is a multiple of the total time
  // if(totalTime % nSplits != 0){
  //   cout << "Total time is NOT a multiple of nSplits" << endl;
  //   exit(1);
  // }

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
  for(int i = 1; i <= nSplits; i++){

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

    // Submit file
    stringstream stepBuffer;
    stepBuffer.precision(10);
    stepBuffer << "\"";

    // GATE 6.2 and >
    if (oldGATE == 0){
      stepBuffer << "-a ";
      stepBuffer << macro_args;
      stepBuffer << "[seed,auto][time_slice," << tStep << "][time_start,0][time_stop," << totalTime << "]";
      stepBuffer << "[t0,"<< (float)((i-1)*tStep) << "][t1," << (float)(i*tStep) << "][output_file," << OutputFile << "-" << i << "]";
      stepBuffer << "[output_dir," << OutputDir << "][omega," << omega << "]"  << " " << macro;
    }
    else if (oldGATE == 1){
    // GATE 6.1
      stepBuffer << macro_args;
      stepBuffer << " -a seed auto -a time_slice " << tStep << " -a time_start 0 -a time_stop " << totalTime;
      stepBuffer << " -a t0 "<< (float)((i-1)*tStep) << " -a t1 " << (float)(i*tStep) << " -a output_file " << OutputFile << "-" << i;
      stepBuffer << " -a output_dir " << OutputDir << " -a omega " << omega << " " << macro;
    }

    stepBuffer << "\"";
    string step_args = stepBuffer.str();

    submitFile << "# Split " << i << ": " << ((i-1)*tStep) << " - " <<  (i*tStep) << "s" << endl;
    submitFile << "Requirements = " << requirements << endl;
    submitFile << "Arguments = " << step_args << endl;
    submitFile << "Output    = " << InitialDir << "/logs/" << OutputFile << "-" << i << ".out" <<endl; 
    submitFile << "Error     = " << InitialDir << "/logs/" << OutputFile << "-" << i << ".err" <<endl; 
    submitFile << "Log       = " << InitialDir << "/logs/" << OutputFile << "-" << i << ".log" <<endl; 
    submitFile << "Queue" << endl;
    submitFile << endl;

    // Split file
    splitFile << "Root filename: ./output/" << OutputDir << "/ROOT/" << OutputFile << "-" << i << endl;
    splitFile << "interfile filename: ./output/" << OutputDir << "/InterFile/"  << OutputFile << "-" << i << endl;
    splitFile << "Timeslice is: " << tStep << " s" << endl;
    splitFile << "Start time is: 0.0 s " << endl;
    splitFile << "Stop time is: " << totalTime << " s" << endl;
    splitFile << "Virtual starttime: " << ((i-1)*tStep) << " s" << endl; 
    splitFile << "Virtual stoptime: " << (i*tStep) << " s" << endl;
    splitFile << endl;

  } 

  splitFile << "Original Root filename: ./output/" << OutputDir << "/" << OutputFile << "-total" << endl;

}

