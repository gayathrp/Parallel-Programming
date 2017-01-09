import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class InvertedIndexes {
	public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, Text> {
		JobConf conf;
		private final static Text word = new Text();
		private final static Text location = new Text();

		public void configure(JobConf job) {
			this.conf = job;
		}

		public void map(LongWritable docId, Text value, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {
			// retrieve # keywords from JobConf
			int argc = Integer.parseInt(conf.get("argc"));
			int count = 1;
			// get the current file name
			FileSplit fileSplit = (FileSplit) reporter.getInputSplit();
			String filename = "" + fileSplit.getPath().getName();
			location.set(filename);
			HashMap<String, Integer> hmap = new HashMap<>();
			String line = value.toString();
			StringTokenizer itr = new StringTokenizer(line);
			while (itr.hasMoreTokens()) {
				String str = itr.nextToken();
				for (int i = 0; i < argc; i++) {
					if (conf.get("keyword" + i).equals(str)) {
						if (hmap.containsKey(str)) {
							int count1 = hmap.get(str) + 1;
							hmap.put(str, count1);
						} else {
							hmap.put(str, count);
						}
					}
				}
			}
			for (java.util.Map.Entry<String, Integer> set : hmap.entrySet() ) {
				String word = set.getKey();
				String file = filename + "-" + set.getValue();
				output.collect(new Text(word), new Text(file));
			}
		}
	}
	public static class Reduce extends MapReduceBase implements Reducer<Text, Text, Text, Text> {
		JobConf conf;
		public void configure(JobConf job) {
			this.conf = job;
		}
		public void reduce(Text key, Iterator<Text> values, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {
			// actual computation is here.
			// finally, print it out.
			int count = 0;
			HashMap<String, Integer> hmap = new HashMap<>();
			while (values.hasNext()) {
				String[] parts = values.next().toString().split("-");
				String str = parts[0];
				if(hmap.get(str) == null)
				{
					count = Integer.parseInt(parts[1]);
					hmap.put(str, count);
				}
				else
				{
					count = hmap.get(str) + Integer.parseInt(parts[1]);
                                        hmap.put(str, count);
				}
			}
			Set set = hmap.entrySet();                                             
			Iterator iterator = set.iterator();                                    
			while (iterator.hasNext()) {                                           
				java.util.Map.Entry me = (java.util.Map.Entry) iterator.next();    
			}                                                                      
			java.util.Map<String, Integer> map1 = sortByValues(hmap);              
			Set set2 = map1.entrySet();                                            
			Iterator iterator2 = set2.iterator();                                  
			while (iterator2.hasNext()) {                                          
				java.util.Map.Entry me2 = (java.util.Map.Entry) iterator2.next();  
			}                                                                      	
			boolean bool = true;
			StringBuilder value = new StringBuilder();
			for (java.util.Map.Entry<String, Integer> set1 : map1.entrySet() ) {
				if (!bool) {
                    			value.append("  ");
                		}
                		bool = false;
				String file = set1.getKey();
				value.append(file + "-" + set1.getValue());
			}
			output.collect(key, new Text(value.toString()));
		}
		private static HashMap sortByValues(java.util.Map<String, Integer> map2) {
			List list = new LinkedList(map2.entrySet());
			// Defined Custom Comparator here
			Collections.sort(list, new Comparator() {
				public int compare(Object o1, Object o2) {
					return -((Comparable) ((java.util.Map.Entry) (o1)).getValue()).compareTo(((java.util.Map.Entry) (o2)).getValue());
				}
			});
			// Here I am copying the sorted list in HashMap
			// using LinkedHashMap to preserve the insertion order
			HashMap sortedHashMap = new LinkedHashMap();
			for (Iterator it = list.iterator(); it.hasNext();) {
				java.util.Map.Entry entry = (java.util.Map.Entry) it.next();
				sortedHashMap.put(entry.getKey(), entry.getValue());
			}
			return sortedHashMap;

		};
	}

	public static void main(String[] args) throws Exception {
		// input format:
		// hadoop jar invertedindexes.jar InvertedIndexes input output keyword1 keyword2 ...
		JobConf conf = new JobConf(InvertedIndexes.class); // AAAAA is this program’s file name
		conf.setJobName("InvertedIndexes"); // BBBBB is a job name, whatever you like conf.setOutputKeyClass(Text.class);
		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(Text.class);
		conf.setMapperClass(InvertedIndexes.Map.class);
		conf.setCombinerClass(InvertedIndexes.Reduce.class);
		conf.setReducerClass(InvertedIndexes.Reduce.class);
		conf.setInputFormat(TextInputFormat.class);
		conf.setOutputFormat(TextOutputFormat.class);
		FileInputFormat.setInputPaths(conf, new Path(args[0])); // input directory name
		FileOutputFormat.setOutputPath(conf, new Path(args[1])); // output directory name
		conf.set("argc", String.valueOf(args.length - 2)); // argc maintains #keywords
		for (int i = 0; i < args.length - 2; i++)
			conf.set("keyword" + i, args[i + 2]); // keyword1, keyword2, ...
		long time1 = System.currentTimeMillis();
        	JobClient.runJob(conf);
        	long time2 = System.currentTimeMillis();
        	long time = time2 - time1;
        	System.out.println("Elapsed Time:" + time);		
	}

}
