import shutil
import os
import sys

def copy_textures(source_dir, destination_dir):
    source_path = os.path.join(source_dir, "textures")
    destination_path = os.path.join(destination_dir, "textures")
    
    if not os.path.exists(source_path):
        print(f"Source directory does not exist: {source_path}")
        return
    
    os.makedirs(destination_path, exist_ok=True)
    
    for file_name in os.listdir(source_path):
        src_file = os.path.join(source_path, file_name)
        dest_file = os.path.join(destination_path, file_name)
        
        if os.path.isfile(src_file):
            shutil.copy(src_file, dest_file)
            print(f"Copied: {src_file} -> {dest_file}")
    
    print("All textures copied successfully!")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python move_textures.py <source_dir> <destination_dir>")
        sys.exit(1)
    
    print(sys.argv[1])
    print(sys.argv[2])
    copy_textures(sys.argv[1], sys.argv[2])
