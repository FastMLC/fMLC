# MLCV (Windows version)


## Dependencies : 

- [Boost 1.60.0](http://www.boost.org/users/history/version_1_60_0.html)

- [Eigen 3.2](http://eigen.tuxfamily.org/dox-3.2/)

- [LargeVis](https://github.com/lferry007/LargeVis) 

- [DIVE](https://github.com/NLeSC/DiVE)


## Installation:

The folders LargeVis and [DIVE](https://github.com/NLeSC/DiVE) and the file BioScience.x64.dll should be put in the same folder where the application file MfcCluster.exe is.	
The folder LargeVis should contain LargeVis.exe as compiled from [LargeVis](https://github.com/lferry007/LargeVis). 


## The main windows:

-Input file path: The path of the fasta file of sequences to be clustered. In this fasta file, a sequence is represented by two lines. The title line starting with character ">" containing multiple information fields separated by the pipe character "|". The first information field is the index of the sequence starting at 1. The second line contains the sequence. Another input file with the same name of the fasta file in the format .title describing the information fields can be given optionally. The two input files Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas and Yeast_CBS_GB_ITS_1seqfor1strain_Species.title in the Working folder can be seen as given examples.

-Reference field: When the input file is given, the drop down list on second position will display all possible fields found between the pipe characters. If the .title file is given, then the drop down list will display the information given in this file, otherwise, it displays the information given in the first sequence of the fasta file.
Output file path: The path to save the clustering result, or to save the prediction of the optimal threshold to cluster the given dataset.

-Algorithms: The algorithm used to cluster. There are three clustering algorithms to be selected for clustering: MLC (multilevel clustering, Vu et al. 2014), CCBC (connected components based cluster, Bolten et al. 2001) and GC (the greedy clustering, Edgar 2010) using multi threads or single threads.

-Thresholds: For CCBC and GC, this is a threshold to cluster the dataset. Its value is in between 0 and 1. For MLC, this is a list of increasing thresholds between 0 and 1. The final threshold of the list is the actual threshold that we want to cluster the dataset with. For example, the thresholds to cluster a dataset can be 0.95;0.98. MLC will first cluster the dataset with the threshold of 0.95, and then the obtained groups will be clustered with the threshold of 0.98.

-Cluster: Cluster the given dataset with the selected algorithm and thresholds. 

-Computing Fmeasure checkbox: Compute Fmeasure  immediately after clustering.

-Compute Fmeasure: Compute Fmeasure based on the clustering result.

-Group #: The group number obtained after clustering.

-Fmeasure: The Fmeasure (Paccanaro et al. 2006) obtained by comparing the clustering result with the classification of the given dataset based on the selected field.

-The bottom grid: The grid at the bottom of the window displays the obtained clusters after clustering.

-Save result in input file: The titles of the sequences in the input file will be extended with the centrality indexes of the groups that the sequences belong to at each level.

-Save result in output file: The clustering result are saved in the output file in tab delimited format.

-Save sparse SM: Save a sparse similarity matrix based on the clustering result.

-Save full SM: Save a complete similarity matrix.

-From: The lower boundary threshold for the prediction. 

-To: The upper boundary threshold for the prediction.

-Step: The incremental step of the thresholds in the prediction.

-Predict OPT: To find an optimal threshold between the lower and upper boundaries that produces the best Fmeasure for clustering.

-Show final groups: Display only the grouping at the final level of MLC.

-Visualize: Visualize the dataset using DiVE. For this action, a sparse/full similarity matrix is required.

-More options: Open an option windows to modify parameters used in clustering, saving similarity matrix and visualizing.

The options windows:

-Minimum overlap: This parameter is used to recompute the similarity score between two sequences if the overlap obtained by BLAST when aligning them is shorter than this value (see Vu et al. 2014).

-Minimum sequence number for MLC: The minimum sequence number for that MLC can be applied.

-Minimum similarity: The minimum similarity score to be saved for a sparse or full similarity matrix. 

-K-neighbor number: This K-neighbor number parameter set up for LargeVis. It’s default value is 150. The remaining parameters of LargeVis are set as default.

-Visualization dimension: 2D or 3D.




