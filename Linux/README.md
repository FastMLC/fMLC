# fMLC (Linux version)


Dependencies :

    ## Dependencies : 

- [Boost 1.60.0](http://www.boost.org/users/history/version_1_60_0.html)

- [Eigen 3.2](http://eigen.tuxfamily.org/dox-3.2/)

- [LargeVis](https://github.com/lferry007/LargeVis) 

- [DIVE](https://github.com/NLeSC/DiVE)

- [NCBI BLAST](https://blast.ncbi.nlm.nih.gov/Blast.cgi?PAGE_TYPE=BlastDocs&DOC_TYPE=Download)

Installation:

The folders LargeVis and DIVE folders should be put in the same folder where the application file ./cluster is.

Parameters:

-input inputfilename: The fasta input file of DNA sequences. Examples can be found at https://github.com/FastMLC/MLC/tree/master/Working

-title titlefilename: The input file describing what are the properties in the titles of the sequences in the input file (optional).

-output outputfilename: The path of the output file to save the clustering result in tab delimited format.

-algo algorithmname(MLC/CCBC/GC/MLC_ST/CCBC_ST/GC_ST): the algorithm used to cluster the dataset. It can be chosen between MLC (MultiLevel Clustering, Vu et al. 2014), CCBC (the connected components based clustering, Bolten et al. 2001), and GC (the greedy clustering, Edgar 2010) using multiple threads. For single threading, they are MLC_ST, CCBC_ST and GC_ST.

-thresholds t: The threshold used to cluster the dataset. For GC and CCBC, it is a value between 0 and 1. For MLC, it is a list of increasing thresholds between 0 and 1. For example, the threshold to cluster the given dataset using MLC can be given as -thresholds 0.95,0.9913.

-fmeasure p: To evaluate the clustering result compared with the classification based on a property given in the titles of the sequences at the position p.

-saveCSM simfilename: To save a complete similarity matrix for the sequences. This option can be run without clustering, i.e. the thresholds argument is not required.

-saveSSM simfilename: To save a sparse similarity matrix based on the clustering result by MLC.

-minsim s: The minimum similarity score to be saved in the simfilename, default value is 0.5.

-K neighs: The K-neighbor number specified for LargeVis, default value is 150. This parameter is provided to save a sparse similarity matrix, in order to improve the accuracy of LargeVis. 

-m minnumber: the minimum number of sequences for applying MLC, otherwise CCBC will be applied. Default value is 100.

-sim simfilename: To specify a similatiry file for visualization. If the arguments -saveSSM simfilename or -saveCSM simfilename are provided, then this argument is not required for visualization.

-visualize d(3D/2D): To visualize the sequences with 2D or 3D.

-predictOpt t1 t2 s n: To predict an optimal threshold that produces the best Fmeasure for clustering, where t1 is the starting threshold, t2 is the end threshold, s is an incremental step of the prediction, and p is the position of the property given in the sequence title to classify the sequences.

Examples:

To cluster:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -title Yeast_CBS_GB_ITS_1seqfor1strain_Species.title -algo MLC -thresholds 0.95,0.9913 -output result.txt

To compute Fmeasure:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -title Yeast_CBS_GB_ITS_1seqfor1strain_Species.title -algo MLC -thresholds 0.95,0.9913 -output result.txt -fmeasure 2

To save a sparse similarity matrix:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -algo MLC -thresholds 0.95,0.9913 -saveSSM yeastSSM.sim

To save a complete similarity matrix:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -saveCSM yeastCSM.sim

To visualize the dataset:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -sim yeastSSM.sim -visualize 3D

To cluster and then visualize:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -algo MLC -thresholds 0.95,0.9913 -saveSSM yeastSSM.sim -visualize 3D

To predict an optimal threshold to cluster the dataset using CCBC:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -algo CCBC -predictOpt 0.98 1 0.001 2

To predict an optimal threshold to cluster the dataset using MLC:

./cluster -input Yeast_CBS_GB_ITS_1seqfor1strain_Species.fas -algo MLC -predictOpt 0.95,0.98 1 0.001 2



