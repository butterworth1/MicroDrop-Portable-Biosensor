#Takes name of file as a string and uploads to Dropbox with a new name of format "DVP_YYYY_MM_DD_HHhMMm.csv"
#Deletes ouploaded file
#Note: This assumes all files are in working directory (ie. dropbox_uploader.sh, 


def Upload_and_Delete (filename):

    import subprocess
    import datetime 
  
    #create upload name
    uploadname = datetime.datetime.now().strftime('DPV_%Y-%m-%d_%Hh%Mm.csv')

    #create function call as a terminal command
    functioncall = "./dropbox_uploader.sh upload "+str(filename)+ " " +str(uploadname)

    #execute function call
    subprocess.call(functioncall, shell=True)

    #delete ouploaded file
    os.remove(filename)

    return