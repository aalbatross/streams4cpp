## User Guide
### Introduction
stream4cpp library contains 2 types of streams implementation. 
1. Batch Processing based implementation is called Stream.
Batch Processing based implementation applies the transforms and reduction operation parallely on the entire dataset in lazy way. Hence at any time there are more than 1 copy of data/ its transform available in memory when the computation is performed. This type of streams can be useful when we want to do reductions/aggregation on the entire dataset.
2. Stream Processing based implementation is called UBStream.
Stream Processing based application process the tranform and reduction in streaming way where one element from source is transformed one by one in a lazy way. This type of Stream is useful when we want to work on continuos stream of infinite data where aggregation on all data is difficult or impossible, However working on small windows of data for reduction and aggregations will be useful.

Each of the above implementations provide some common set of operations categorised as:
1. Transformation: These operation transforms the incoming data without aggregating it. This step where the source input can be enriched with variety of common transforms like, map, flatmap, filter, skip, limit, sorted, distinct, sliding, fixed etc.
2. Reduction: These operation reduces data to results, they are also terminal operations in the stream processing which concludes processing by yielding aggregate of data, some common operations are toVector, groupingBy, reduce, collect, joining, partitioningBy etc.

