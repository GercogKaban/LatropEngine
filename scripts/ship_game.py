import subprocess
import sys
import os

currentdir = os.getcwd()

def run_cmake_build():
    try:
        # Run cmake to configure the build
        print("Running cmake configuration...")
        subprocess.check_call(["cmake", "-DCMAKE_BUILD_TYPE=Release", "."])
        
        # Run cmake to build the project
        print("Running cmake build...")
        subprocess.check_call(["cmake", "--build", ".", "--config", "Release"])
        
    except subprocess.CalledProcessError as e:
        print(f"An error occurred during the cmake process: {e}")
        sys.exit(1)

def run_python_script():
    try:
        # Call the other Python script
        print("Running move textures")
        subprocess.check_call(["python", "move_textures.py", 
                       f"{currentdir}/LatropGraphics", 
                       f"{currentdir}/textures/Release"])
        
    except subprocess.CalledProcessError as e:
        print(f"An error occurred when calling the Python script: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Step 1: Run cmake build in release mode
    run_cmake_build()

    # Step 2: Run the other Python script
    run_python_script()