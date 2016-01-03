import sys

# Let's get the average and our grades:
FILES = ["pack.txt"]
i=0
sprite=""


# Get the Data
for file in FILES:
    infile = open(file,"r")
    while infile:
        line = infile.readline()
        sprite = line.split(".")[0]
        line = infile.readline()
        line = infile.readline()
        s = line.split()
        sys.stdout.write (s[1])
        sys.stdout.write (s[2])
        sys.stdout.write(",")
        line = infile.readline()
        s = line.split()
        sys.stdout.write (s[1])
        sys.stdout.write (s[2])
        sys.stdout.write(",")
        print (' //'+str(i)+'\t'+sprite)
        i=i+1;		
        line = infile.readline()
        line = infile.readline()
        line = infile.readline() 
        # try:
            # TOTAL += float( s[ n-1 ] )
        # except:
            # pass