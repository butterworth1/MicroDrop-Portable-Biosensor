#Read a single trial CSV file 
#Requires name of file as a string 

def importCSV (filename):

    import csv
    import matplotlib.pyplot as plt

    linecounter = 0;
    X=[]
    Y1=[]
    Y2=[]
    Y=[]

    with open(filename) as csvfile:
        readCSV = csv.reader(csvfile)
        for row in readCSV:
            linecounter = linecounter+1
            if linecounter >= 5:
                X.append(float(row[0]))
                Y1=(float(row[1]))
                Y2=(float(row[2]))
                Y.append(Y2-Y1)
        plt.plot(X, Y)
        #plt.show()
        image_name = filename[0:9] + '.png'
        print(image_name)
        plt.savefig(image_name, bbox_inches='tight') #keeps the whitespace minimal
    return
