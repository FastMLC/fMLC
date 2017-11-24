#This file is to visualize the dataset in R. 
#set the working directory
setwd("C:/Users/Duong/Documents/Doc/CBSPapers/MLC_Visualization_eScience/R program")

#We first need to cluster the dataset. After that:
#Load the clustering result by clicking on the Save result of the main windows of fMLC in output file Yeast_CBS_GB_ITS_1seqfor1strain_Species.txt. 
output <- read.delim("Yeast_CBS_GB_ITS_1seqfor1strain_Species.txt", header = TRUE)
myoutput <-data.frame(index=output$Sequence.id, clusterindex = output$Reference.cluster.index)
y <-myoutput[order(myoutput$index),]
colorindexes <- y$clusterindex

#Load the coordinates obtained by LargeVis by clicking on Visualize button on the main windows of fMLC
x <- read.table("Yeast_CBS_GB_ITS_1seqfor1strain_Species.outLargeVis", skip = 1, sep= " ") 
mydata <- data.frame(index=x[,1],coor1=x[,2],coor2=x[,3],coor3 =x[,4]) #3D
x <-mydata[order(mydata$index),]

#count how many times a color appear for a reference cluster
freq<-sapply(1:length(colorindexes),function(x)sum(colorindexes[1:x]==colorindexes[x])) 
mydata <- data.frame(index=x[,1],coor1=x[,2],coor2=x[,3], coor3=x[,4], colorindexes=colorindexes, freq=freq)
ordereddata <-mydata[order(mydata$freq, decreasing=TRUE), ]

#get distinct colors for different groups
library(randomcoloR)
n <-length(unique(colorindexes))
palette <- distinctColorPalette(n)
palette[1]=3
palette[2]=2
palette[3]=4
palette[4]=6
palette[5]=5
palette[6]=1
palette[7]=7
palette[8]=8
#give color for each data point
newcolorindexes = c()
mylist = c()
index = 0
for (colorindex in ordereddata$colorindexes){
  #print(colorindex)
  if (length(mylist) >0) {
    index = match(colorindex,mylist)
  }
  if (is.na(index) || index == 0) {
    index = length(mylist)+1
    mylist=append(mylist,colorindex)
  } 
  newcolorindexes=append(newcolorindexes,palette[index])
}

#visualize the data
rgl.open()
rgl.points(ordereddata$coor1,ordereddata$coor2,ordereddata$coor3, color = newcolorindexes) 
rgl.bg(color = "white")
rgl.snapshot(filename = "visualization.png")
rgl.postscript("plot.pdf",fmt="pdf")
rgl.close() 