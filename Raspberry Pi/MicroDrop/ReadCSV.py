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
    conc =0
    with open(filename) as csvfile:
        readCSV = csv.reader(csvfile)
        for row in readCSV:
            linecounter = linecounter+1
            if linecounter > 5:
                X.append(float(row[0]))
                Y1=(float(row[1]))
                Y2=(float(row[2]))
                Y.append((Y2-Y1)*1650/4095)
        plt.plot(X, Y)
        plt.xlabel('Applied Potential [mV]')
        plt.ylabel('Measured Current [uA]')
        plt.title('DPV Curve of Sensor')
        max_current = max(Y)
        print(max_current)
	 #plt.show()
        image_name = filename[0:9] + '.png'
        plt.savefig(image_name, bbox_inches='tight') #keeps the whitespace minimal
        #conc = ((max_current+158)/43)
        #conc = ((max_current+152.7)/37.3)
        conc = ((max_current+3.556)/26.15)
        print(conc)
    return conc
