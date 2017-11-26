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

When installing and running fMLC together with DiVE, a .json file containing the 3D coordinates and metadata of the original data will reside in the 'DiVE/data' folder. This file can be used for visualization by external applications as well. 


## Contact person 

Duong Vu (d.vu@westerdijkinstitute.nl)


## References

Bolten, E., Schliep, A., Schneckener, S., Schomburg D. & Schrader, R (2001). Clustering protein sequences- structure prediction by transitive homology. Bioinformatics 17, 935-941.

Edgar, R.C (2010). Search and clustering orders of magnitude faster than BLAST. Bioinformatics 26, 2460-2461.
Paccanaro, P., Casbon, J.A. & Saqi, M.A (2006). Spectral clustering of proteins sequences.  Nucleic Acids Res 34, 1571.

Vu D. et al. (2014). Massive fungal biodiversity data re-annotation with multi-level clustering. Scientific Reports 4: 6837.


