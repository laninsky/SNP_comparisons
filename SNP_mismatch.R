SNP_mismatch <- function(working_dir,input_structure_file) {
setwd(working_dir)
#dplyr to look at the "best" version of individual 3
library(tidyverse)
write.csv(t(read.table(input_structure_file,header=FALSE)),"transposed_structure.csv",col.names=FALSE,row.names=FALSE,quote=FALSE)
data <- readr::read_csv("transposed_structure.csv",skip=1)

output <- tibble(sample1 = character(), sample2 = character(), total_SNPs = integer(), mismatched_SNPs = integer(), mismatch_perc = integer(), mismatch_both_hets = integer(), both_hets_perc = integer(), mismatch_both_homs = integer(), both_homs_perc = integer(), mismatch_het_hom = integer(), perc_het_hom = integer())

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
    rows_with_data_for_both <- indiv_subset %>% filter(indiv_subset[,1]!=-9 & indiv_subset[,2]!=-9 & indiv_subset[,3]!=-9 & indiv_subset[,4]!=-9)
    mismatching_SNPs <- rows_with_data_for_both %>% filter(rows_with_data_for_both[,1]!=rows_with_data_for_both[,3] | rows_with_data_for_both[,2]!=rows_with_data_for_both[,4])
    hard_mismatch_both_hets <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]!=mismatching_SNPs[,2] & mismatching_SNPs[,3]!=mismatching_SNPs[,4])
    double_dropout <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]==mismatching_SNPs[,2] & mismatching_SNPs[,3]==mismatching_SNPs[,4])
    single_dropout <- mismatching_SNPs %>% filter(mismatching_SNPs[,1]!=mismatching_SNPs[,2] & mismatching_SNPs[,3]==mismatching_SNPs[,4] | mismatching_SNPs[,1]==mismatching_SNPs[,2] & mismatching_SNPs[,3]!=mismatching_SNPs[,4])
    print(paste("Comparison of ",col1," and ",col3,sep=""))
    print(paste("A total of ",dim(rows_with_data_for_both)[1]," SNPs have data for both samples",sep=""))
    print(paste("Of these ",(dim(mismatching_SNPs)[1])," (",round(((dim(mismatching_SNPs)[1])/(dim(rows_with_data_for_both)[1])*100),2),"%) show mismatches",sep=""))
    print(paste("Of these mismatches ",dim(hard_mismatch_both_hets)[1]," (",round(((dim(hard_mismatch_both_hets)[1])/(dim(mismatching_SNPs)[1])*100),2),"% of mismatching loci) are heterozygous for both samples",sep=""))
    print(paste("Of these mismatches ",dim(double_dropout)[1]," (",round(((dim(double_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2),"% of mismatching loci) are homozygous for both samples",sep=""))
    print(paste("Of these mismatches ",dim(single_dropout)[1]," (",round(((dim(single_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2),"% of mismatching loci) are homozygous in one sample and heterozygous in the other",sep=""))
    output <- add_row(output,sample1=col1, sample2=col3,total_SNPs=dim(rows_with_data_for_both)[1],mismatched_SNPs=dim(mismatching_SNPs)[1],mismatch_perc=round(((dim(mismatching_SNPs)[1])/(dim(rows_with_data_for_both)[1])*100),2),mismatch_both_hets=dim(hard_mismatch_both_hets)[1],both_hets_perc=round(((dim(hard_mismatch_both_hets)[1])/(dim(mismatching_SNPs)[1])*100),2),mismatch_both_homs=dim(double_dropout)[1],both_homs_perc=round(((dim(double_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2),mismatch_het_hom=dim(single_dropout)[1],perc_het_hom=round(((dim(single_dropout)[1])/(dim(mismatching_SNPs)[1])*100),2))
  }
}
}
