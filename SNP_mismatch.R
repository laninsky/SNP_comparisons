SNP_mismatch <- function(working_dir,input_structure_file) {
  setwd(working_dir)
  library(tidyverse)
  
  # Skipping header line if present in the file (e.g. '# Stacks v2.2;  Structure v2.3; September 05, 2019')
  if (length(unlist(strsplit(readLines(input_structure_file)[1],split = " |\t")))!=length(unlist(strsplit(readLines(input_structure_file)[2],split = " |\t")))) {
    # Checking if locus name header present in file and skipping this if so
    if (length(which(unlist(strsplit(readLines(input_structure_file)[2],split = " |\t"))!="")==TRUE)!=length(unlist(strsplit(readLines(input_structure_file)[3],split = " |\t")))) {
      write.csv(t(read.table(input_structure_file,header=FALSE,skip=2)),"transposed_structure.csv",row.names=FALSE,quote=FALSE)
    } else {
      write.csv(t(read.table(input_structure_file,header=FALSE,skip=1)),"transposed_structure.csv",row.names=FALSE,quote=FALSE)
     }          
  } else { # if no header line present in file...
    # Checking if locus name header present in file and skipping this if so
    if (length(which(unlist(strsplit(readLines(input_structure_file)[1],split = " |\t"))!="")==TRUE)!=length(unlist(strsplit(readLines(input_structure_file)[2],split = " |\t")))) {
    write.csv(t(read.table(input_structure_file,header=FALSE,skip=1)),"transposed_structure.csv",row.names=FALSE,quote=FALSE)
    } else {
      write.csv(t(read.table(input_structure_file,header=FALSE)),"transposed_structure.csv",row.names=FALSE,quote=FALSE)
    }
  }
  
  data <- readr::read_csv("transposed_structure.csv",skip=1)
  
  output <- tibble(sample1 = character(), sample2 = character(), total_SNPs = integer(), mismatched_SNPs = integer(), mismatch_perc = integer(), mismatch_both_hets = integer(), both_hets_perc = integer(), mismatch_both_homs = integer(), both_homs_perc = integer(), mismatch_het_hom = integer(), non_both_hets_perc_het_hom = integer())
  
  # Need to figure out what code is used for missing data. Generally if SNPs are coded as 1-4, missing data will be 0. If coded as 0-3, -9 will be missing data
  # will look for minimum value present in dataframe and set this as misssing data
  missing_data_value <- min(data)
  
  # For every second column
  for (i in (seq(1,(length(names(data))-4),2))) {
    # For every second column following that one
    for (j in seq((i+2),(length(names(data))),2)) {
      # Get the names of the column
      col1 <- names(data)[i]
      col2 <- names(data)[i+1]
      col3 <- names(data)[j]
      col4 <- names(data)[j+1]
      indiv_subset <-  data %>% select(col1,col2,col3,col4)  # subsetting to individuals
      #Look, I'm not happy about the following. I think it looks ugly as shit. HOWEVER, trying to subset based on the col names
      #did not work, so I think this is the best workaround without having to do some kind of crazy enquo/quo thing
      rows_with_data_for_both <- indiv_subset %>% filter(indiv_subset[,1]!=missing_data_value & indiv_subset[,2]!=missing_data_value & indiv_subset[,3]!=missing_data_value & indiv_subset[,4]!=missing_data_value)
      mismatching_SNPs <- rows_with_data_for_both %>% filter(rows_with_data_for_both[,1]!=rows_with_data_for_both[,3] | rows_with_data_for_both[,2]!=rows_with_data_for_both[,4])
      hard_mismatch_both_hets <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]!=mismatching_SNPs[,2] & mismatching_SNPs[,3]!=mismatching_SNPs[,4])
      double_dropout <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]==mismatching_SNPs[,2] & mismatching_SNPs[,3]==mismatching_SNPs[,4])
      single_dropout <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]!=mismatching_SNPs[,2] & mismatching_SNPs[,3]==mismatching_SNPs[,4] | mismatching_SNPs[,1]==mismatching_SNPs[,2] & mismatching_SNPs[,3]!=mismatching_SNPs[,4])
      print(paste("Comparison of ",col1," and ",col3,sep=""))
      print(paste("A total of ",dim(rows_with_data_for_both)[1]," SNPs have data for both samples",sep=""))
      print(paste("Of these ",(dim(mismatching_SNPs)[1])," (",round(((dim(mismatching_SNPs)[1])/(dim(rows_with_data_for_both)[1])*100),2),"%) show mismatches",sep=""))
      print(paste("Of these mismatches ",dim(hard_mismatch_both_hets)[1]," (",round(((dim(hard_mismatch_both_hets)[1])/(dim(mismatching_SNPs)[1])*100),2),"% of mismatching loci) are heterozygous for both samples",sep=""))
      print(paste("Of remaining mismatches ",dim(double_dropout)[1]," (",round(((dim(double_dropout)[1])/((dim(mismatching_SNPs)[1])-(dim(hard_mismatch_both_hets)[1]))*100),2),"% of mismatching loci) are homozygous for both samples",sep=""))
      print(paste("And... ",dim(single_dropout)[1]," (",round(((dim(single_dropout)[1])/((dim(mismatching_SNPs)[1])-(dim(hard_mismatch_both_hets)[1]))*100),2),"% of mismatching loci) are homozygous in one sample and heterozygous in the other",sep=""))
      output <- add_row(output,sample1=col1, sample2=col3,total_SNPs=dim(rows_with_data_for_both)[1],mismatched_SNPs=dim(mismatching_SNPs)[1],mismatch_perc=round(((dim(mismatching_SNPs)[1])/(dim(rows_with_data_for_both)[1])*100),2),mismatch_both_hets=dim(hard_mismatch_both_hets)[1],both_hets_perc=round(((dim(hard_mismatch_both_hets)[1])/(dim(mismatching_SNPs)[1])*100),2),mismatch_both_homs=dim(double_dropout)[1],both_homs_perc=round(((dim(double_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2),mismatch_het_hom=dim(single_dropout)[1],non_both_hets_perc_het_hom=round(((dim(single_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2))
    }
  }
output <- output %>% arrange(mismatch_perc)

plot_midpoint=((max(output$non_both_hets_perc_het_hom,na.rm=TRUE))+(min(output$non_both_hets_perc_het_hom,na.rm=TRUE)))/2

ggplot(output,aes(x = total_SNPs, y = mismatch_perc, color=non_both_hets_perc_het_hom, size=mismatch_both_hets)) +
  geom_point() +
  scale_colour_gradient2(low="blue",mid="purple",midpoint=plot_midpoint,high="red")+
  labs(x = "Total number of SNPs in pairwise comparison", y = "% of SNPs showing mismatch", colour = "% of non het/het\n mismatches that\n were het/hom", size = "# of total\n mismatches that\n were het/het" )

ggsave(filename = "plot_of_mismatches.pdf",plot = last_plot(), device = "pdf")
  
write.table(output,"pairwise_SNP_matching.txt",quote=FALSE,row.names=FALSE)
}
