#Following points demonstrate the experimental details of the project

1) First, the SNND.cpp has to be run on the dataset to create the algorithm components involving
KNN list, data_matrix(base dataset), similarity matrix, core and non-core points, clusters.

2)BISD_add.cpp is performed for adding points in batches. The input to this code is the base data set and an added dataset.
The added dataset set is a fragment of overall dataset. The base dataset is obtained by running the SNND.cpp in step 1. 
The incoming batches has to be done by creating sperate data files eg:file1, file2, file3 and so on.

3)BISD_del.cpp is performed for the deletion purpose. Prior to running BISD_del.cpp, SNND.cpp has to be run. Similar to BISD.cpp, points are deleted in batches eg:file1, file2, file3 and so on.

4)INSD_add.cpp is performed for adding points one at a time. Eg: If a batch of points in file1 is input to the code, then INSD.cpp processes the 
batch one point at a time.

5)INSD_del.cpp is performed for deleting points one at a time.

6)Convert all the input files file1,file2, KNN list, similarity matrix, data matrix(base dtaset) to binary file.

7)pre_proc_data.cpp converts a text file to binary file for faster I/O. The input file in binary format is fed to the program.

Video link for SNND code: https://www.youtube.com/watch?v=Xi7a_eBDXT8&feature=youtu.be
For any clarification please do contact:panthadeep.edu@gmail.com.
