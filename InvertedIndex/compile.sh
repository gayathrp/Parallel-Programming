/CSSDIV/students/css534/gayathrp/hadoop-2.7.3/bin/hdfs dfs -rm -r /user/gayathrp/output_g3

javac -cp `/CSSDIV/students/css534/gayathrp/hadoop-2.7.3/bin/hadoop classpath`:. InvertedIndexes.java

jar -cvf InvertedIndex_gayathrp.jar *.class
