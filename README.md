# Cluster_Analysis
A simple cluster analysis program based on the nearest neighbor method written in C

How to use:

1 argument: name of text file with data of clusters 
Format of text file:
1 row is number of objects in the file(count=N, N is number of objects) 
The following rows contain objects each object on a new line in format OBJID X Y, where OBJID is unique number of object and X, Y is position of cluster which is 0 <= X <= 1000, 0 <= Y <= 1000

2 argument: an optional argument that tells us how many clusters should be processed from the file
