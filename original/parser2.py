import sys

# Let's get the average and our grades:
FILES = ["pack.txt"]
i=0
sprite=""


# Get the Data
for file in FILES:
    infile = open(file,"rU")
    while infile:
        line = infile.readline()
        sprite = line.split(".")[0]
        #sprite="glImage* "+sprite+";"
        #sprite=sprite+","
        sprite=sprite+"="+"&tex_1["+str(i)+"];"
        print (sprite)        
        infile.readline()
        infile.readline()
        infile.readline()
        infile.readline() 
        infile.readline() 
        infile.readline()
        i=i+1;