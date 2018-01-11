import os

rename_dir = r'F:\\libs\\FFmpeg\\ffmpeg-3.4.1\\'
old_postfix = r".obj"
new_postfix = r".o"

def modify_postfix(dir, oldftype, newftype):
    all_file_list = os.listdir(dir)
    for file_name in all_file_list:
        currentdir = os.path.join(dir, file_name)
        if os.path.isdir(currentdir):
           continue
        fname = os.path.splitext(file_name)[0]
        ftype = os.path.splitext(file_name)[1]
        if ftype == oldftype:
            newname = os.path.join(dir, fname + newftype)
            os.rename(currentdir, newname)

if __name__ == '__main__':
    modify_postfix(rename_dir, old_postfix, new_postfix)
    for root, dirs, files in os.walk(rename_dir):
        for dir in dirs:
            file_path = os.path.join(root, dir)
            modify_postfix(file_path, old_postfix, new_postfix)
    print("DONE")
