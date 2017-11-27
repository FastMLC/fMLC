[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.926820.svg)](https://doi.org/10.5281/zenodo.926820)

Please cite the tool with its DOI if you are using it in your scientific publication. 

# fMLC

fMLC is the official implementation of the MultiLevel Clustering (MLC) algorithm decribed in [Vu D. et al. 2014](https://www.nature.com/articles/srep06837) , used to cluster massive DNA sequences. fMLC was initially implemented by Szaniszlo Szoke and further developed by Duong Vu. It is written in C++ and supports multi-threaded parallelism. fMLC is also integrated with an interactive web-based tool called [DIVE](https://github.com/NLeSC/DiVE) to visualize the resulting DNA sequences based embeddings in 2D or 3D. The work is financially supported by the Westerdijk Fungal Biodiversity Institute and the Netherlands eScience Center.

## Install

[Windows](https://github.com/FastMLC/fMLC/tree/master/Windows)

[Linux](https://github.com/FastMLC/fMLC/tree/master/Linux)

## Data

[Download](http://www.westerdijkinstitute.nl/Download/SmallDatasetOf4KYeastITSSequences.zip) a small demo dataset (circa 4K Yeast ITS Sequences).

[Download](http://www.westerdijkinstitute.nl/Download/LargeDatasetOf350KITSSequences.zip) a large demo dataset (circa 350K IT sequences). 

## Results

After clustering the DNA sequences by fMLC, the grouping of the sequences can be saved as output of fMLC. A sparse (or complete) similarity matrix can also be saved in the folder where the dataset is given, to capture the similarity structure of the sequences. Based on this similarity matrix, the coordiates of the sequences can be computed using LargeVis and saved in the folder LargeVis/. Finally, a json file containing the coordinates and metadata of the sequences is resided in the folder DiVE/data folder as an input of DiVE to visualize the data. This json file can be used for visualization by external applications as well.The clustering and visualization results of the two datasets can be found at https://github.com/FastMLC/fMLC/tree/master/data.

## Contact person 

Duong Vu (d.vu@westerdijkinstitute.nl)


## References

Bolten, E., Schliep, A., Schneckener, S., Schomburg D. & Schrader, R (2001). Clustering protein sequences- structure prediction by transitive homology. Bioinformatics 17, 935-941.

Edgar, R.C (2010). Search and clustering orders of magnitude faster than BLAST. Bioinformatics 26, 2460-2461.
Paccanaro, P., Casbon, J.A. & Saqi, M.A (2006). Spectral clustering of proteins sequences.  Nucleic Acids Res 34, 1571.

Vu D. et al. (2014). Massive fungal biodiversity data re-annotation with multi-level clustering. Scientific Reports 4: 6837.


