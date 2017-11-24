#!/usr/bin/env python
import sys
import os

tool = sys.argv[1] # the fasta file
fastafilename = sys.argv[2] # the fasta file
namepos = int(sys.argv[3]) # the position of the feature in the header of the sequences used for comparisons
threshold = float(sys.argv[4])
endthreshold = 0
if len(sys.argv) > 5:
	endthreshold = float(sys.argv[5])
step = 0.01
if len(sys.argv) > 6:
	step = float(sys.argv[6])

def GetClasses(fastafilename):
	classes = []
	speciesnames = []
	#read the fasta file
	fastafile= open(fastafilename)
	for line in fastafile:
		if line.startswith('>'):
			words = line.split('|')
			seqid = int(words[0].strip('>'))
			speciesname = words[namepos]
			if speciesname in speciesnames:
				i=speciesnames.index(speciesname)
				refclass = classes[i] 
				refclass.append(seqid)
			else:
				speciesnames.append(speciesname)
				refclass=[]
				refclass.append(seqid)
				classes.append(refclass)
				
	return classes

def GetClustersByCDHIT(resultfilename):
	clusters = []
	#read the clustering result file
	cluster = []
	i=0
	resultfile = open(resultfilename)
	for line in resultfile:
		if line.startswith('>'):
			cluster = []
			clusters.append(cluster)
		else:
			words = line.split('>')
			seqid = int(words[1].split('|')[0])
			cluster.append(seqid)
	return clusters

def GetClustersByUCLUST(resultfilename):
	clusters = []
	#read the clustering result file
	cluster = []
	clusterindexes = []
	i=0
	resultfile = open(resultfilename)
	for line in resultfile:
		words = line.split('\t')
		if len(words) > 1:
			clusterindex = int(words[1])
			seqid = int(words[8].split('|')[0])
			if clusterindex in clusterindexes:
				i = clusterindexes.index(clusterindex)
				clusters[i].append(seqid)
			else:
				cluster = []
				cluster.append(seqid)
				clusters.append(cluster)
				clusterindexes.append(clusterindex)
				
	return clusters

def GetClustersByMMSEQ(resultfilename):
	clusters = []
	#read the clustering result file
	cluster = []
	clusternames = []
	resultfile = open(resultfilename)
	i=-1
	currentclustername = ""
	for line in resultfile:
		words = line.split('\t')
		clustername = words[0]		
		seqid = int(words[1].split('|')[0])
		if clustername == currentclustername:
			clusters[i].append(seqid)
		else:
			currentclustername = clustername
			i = i + 1
			cluster = []
			cluster.append(seqid)
			clusters.append(cluster)
			
	return clusters

def ComputeFmeasure(classes,clusters):
	#compute F-measure
	f=0
	n=0
	for group in classes:
		m = 0
		for cluster in clusters:
			i = len(set(group) & set(cluster))
			v = float(2*i)/float((len(group) + len(cluster)))
			if m < v:
				m=v
		
		n = n + len(group)
		f = f +	(len(group)*m)	
	return float(f)/float(n) 

#convert the fasta file to the right format or create a db based on the fasta file
if tool == "uclust":
	path = "/home/dvu/tools/uclust/"
	command = path + "./uclust --sort " + fastafilename  + " --output seqs_sorted.fasta"
elif (tool == "linclust" or tool == "kclust"):
	os.system("rm DB*")
	os.system("rm -r tmp")
	os.system("mkdir tmp")
	path = "/home/duong/tools/mmseqs2/bin/"
	command = path + "mmseqs createdb " + fastafilename + " DB"
os.system(command)
if endthreshold < threshold:
	
	#cluster and compute F-measure
	if tool == "cdhit":
		command = "cd-hit-est -i " + fastafilename + " -n 10 -o result -c " + str(threshold)
		os.system(command)
		clusters = GetClustersByCDHIT("result.clstr")
	elif tool == "uclust":
		command = path + "./uclust --input seqs_sorted.fasta --uc result.uc --id " + str(threshold)
		os.system(command)
		clusters = GetClustersByUCLUST("result.uc")
	elif tool == "kclust":
		command = path + "mmseqs cluster DB DB_clu tmp --min-seq-id " + str(threshold)
		os.system(command)
		command = path + "mmseqs createtsv DB DB DB_clu DB_clu.tsv"
		os.system(command)
		clusters = GetClustersByMMSEQ("DB_clu.tsv")
	elif tool == "linclust":
		command = path + "mmseqs linclust DB DB_clu tmp --min-seq-id " + str(threshold)
		os.system(command)
		command = path + "mmseqs createtsv DB DB DB_clu DB_clu.tsv"
		os.system(command)
		clusters = GetClustersByMMSEQ("DB_clu.tsv")
	classes = GetClasses(fastafilename)
	f = ComputeFmeasure(classes,clusters)
	print("F-measure: " + str(f))
else:
	os.system("rm " + tool + "opt.txt")
	resultfile = open(tool + "opt.txt","a")
	classes = GetClasses(fastafilename)
	
	#predict optimal threshold
	t = threshold
	while t <= endthreshold:
		if tool == "cdhit":
			command = "cd-hit-est -i " + fastafilename + " -n 10 -o result -c " + str(t)
			os.system(command)
			clusters = GetClustersByCDHIT("result.clstr")		
		elif tool == "uclust":
			command = path + "./uclust --input seqs_sorted.fasta --uc result.uc --id " + str(t)
			os.system(command)
			clusters = GetClustersByUCLUST("result.uc")
		elif tool == "kclust":			
			os.system("rm DB_clu*")
			command = path + "mmseqs cluster DB DB_clu tmp --min-seq-id " + str(t)
			os.system(command)
			command = path + "mmseqs createtsv DB DB DB_clu DB_clu.tsv"
			os.system(command)
			clusters = GetClustersByMMSEQ("DB_clu.tsv")
		elif tool == "linclust":			
			os.system("rm DB_clu*")
			command = path + "mmseqs linclust DB DB_clu tmp --min-seq-id " + str(t)
			os.system(command)
			command = path + "mmseqs createtsv DB DB DB_clu DB_clu.tsv"
			os.system(command)
			clusters = GetClustersByMMSEQ("DB_clu.tsv")
		
		f = ComputeFmeasure(classes,clusters)
		resultfile.write(str(t) + '\t' + str(f) + '\n')
		t = t + step
	resultfile.close()
	
	#os.system("rm result*")
		
