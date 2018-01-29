
# fMLC

fMLC is the official implementation of the MultiLevel Clustering (MLC) algorithm decribed in [Vu D. et al. 2014](https://www.nature.com/articles/srep06837) , used to cluster massive DNA sequences. fMLC was initially implemented by Szaniszlo Szoke and further developed by Duong Vu. It is written in C++ and supports multi-threaded parallelism. fMLC is also integrated with an interactive web-based tool called [DIVE](https://github.com/NLeSC/DiVE) to visualize the resulting DNA sequences based embeddings in 2D or 3D. The work is financially supported by the Westerdijk Fungal Biodiversity Institute and the Netherlands eScience Center.

# Citation

Please cite the following paper if you are using fMLC:

D Vu, S Georgievska, S Szoke, A Kuzniar, V Robert. fMLC: Fast Multi-Level Clustering and Visualization of Large Molecular Datasets, Bioinformatics, btx810, https://doi.org/10.1093/bioinformatics/btx810 

[Pdf verion](https://academic.oup.com/bioinformatics/advance-article/doi/10.1093/bioinformatics/btx810/4747887?guestAccessKey=da7a1811-354a-4445-8084-cae44ccafd6f)

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.926820.svg)](https://doi.org/10.5281/zenodo.926820)

## Install

[Windows](https://github.com/FastMLC/fMLC/tree/master/Windows)

[Linux](https://github.com/FastMLC/fMLC/tree/master/Linux)

## Data
There are two datasets available as inputs for fMLC. The "small" dataset contains ~4000 ITS yeast sequences, checked and validated by the specialists at the Westerdijk Fungal Biodiversity Institute. This dataset were analyzed and released in [Vu D. et al. 2016](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5192050/). The "large" dataset contains ~350K ITS fungal sequences downloaded from GenBank (https://www.ncbi.nlm.nih.gov/) which was used in [Vu D. et al. 2014](https://www.nature.com/articles/srep06837) to evaluate the speed of MLC.

[Download](http://www.westerdijkinstitute.nl/Download/SmallDatasetOf4KYeastITSSequences.zip) the small demo dataset. 

[Download](http://www.westerdijkinstitute.nl/Download/LargeDatasetOf350KITSSequences.zip) the large demo dataset. 

## Results

After clustering the DNA sequences by fMLC, the groupings of the sequences can be saved as output of fMLC. A sparse (or complete) similarity matrix (in .sim format) can be saved in the folder where the dataset is given, to capture the similarity structure of the sequences. Based on this similarity matrix, the coordiates of the sequences can be computed and saved (in .outLargeVis format) using LargeVis. Finally, a json file containing the coordinates and metadata of the sequences is resided in the folder DiVE/data folder as an input of DiVE to visualize the data. This json file can be used for visualization by external applications as well.The clustering and visualization results of the two datasets can be found at https://github.com/FastMLC/fMLC/tree/master/data.

## Contact person 

Duong Vu (d.vu@westerdijkinstitute.nl)


## References

Bolten, E., Schliep, A., Schneckener, S., Schomburg D. & Schrader, R (2001). Clustering protein sequences- structure prediction by transitive homology. Bioinformatics 17, 935-941.

Edgar, R.C (2010). Search and clustering orders of magnitude faster than BLAST. Bioinformatics 26, 2460-2461.
Paccanaro, P., Casbon, J.A. & Saqi, M.A (2006). Spectral clustering of proteins sequences.  Nucleic Acids Res 34, 1571.

Vu D. et al. (2014). Massive fungal biodiversity data re-annotation with multi-level clustering. Scientific Reports 4: 6837.


