# Processing MRFs

In this example we will process anthem's 2023-03-01 MRFs. 

You can use json buffet to extract `in_network` files using the following piped commands.

```
curl -o - https://antm-pt-prod-dataz-nogbd-nophi-us-east1.s3.amazonaws.com/anthem/2023-03-01_anthem_index.json.gz  | gunzip | ./build/json-buffet reporting_structure//in_network_files//
```

You will see output such as: 

```...
Offset: 3669400
Length: 548
{"description": "BCBS Tennessee, Inc. : Network P", "location": "https://anthembcca.mrf.bcbs.com/2023-03_890_58B0_in-network-rates_51_of_60.json.gz?&Expires=1681999217&Signature=IWCMvF8p6yVN~5iQNbQ-kUh8fZx6uJRZuCkUSVG24odcI2JYnFnBSWsyXBDpuELlJbW5fUHEyBs4ayqdyKmtagycZEBvYZcurTeYZVPgRPeH7uzP2Zt6a3OBOPaLFnnEeG0X8N4YPDak7TpKg~aSS-KY3j0gK8XoTVOtO5AZfPgFRzPJ-Tlo~mh-W78S~IZMDyi9tB6DLAC89qrk5la-hhYfYLBSiTd0z0Q6T0ECNbWO-GyAtLxzrA6ew8c9qS0cxxMrPev6iaBNELYtXZiJT5nFNPz1rtwPvdWLB3EnuDYLwDeirFS9OB5nqBTw7Vk0eAav0b9svmwYfAXgfmBQnQ__&Key-Pair-Id=K27TQMT39R1C8A"}
```


## Indexing the in_network files

One step you may want to do is index the json file as it streams then look up those values using AWS's range filter. 

**Currently the json-indexer command is hard coded to only work on finding NPIs in `in_network` files. We plan to generalize this tool in the future

```
curl -o - https://anthembcca.mrf.bcbs.com/2023-03_890_58B0_in-network-rates_51_of_60.json.gz\?\&Expires\=1681999217\&Signature\=IWCMvF8p6yVN\~5iQNbQ-kUh8fZx6uJRZuCkUSVG24odcI2JYnFnBSWsyXBDpuELlJbW5fUHEyBs4ayqdyKmtagycZEBvYZcurTeYZVPgRPeH7uzP2Zt6a3OBOPaLFnnEeG0X8N4YPDak7TpKg\~aSS-KY3j0gK8XoTVOtO5AZfPgFRzPJ-Tlo\~mh-W78S\~IZMDyi9tB6DLAC89qrk5la-hhYfYLBSiTd0z0Q6T0ECNbWO-GyAtLxzrA6ew8c9qS0cxxMrPev6iaBNELYtXZiJT5nFNPz1rtwPvdWLB3EnuDYLwDeirFS9OB5nqBTw7Vk0eAav0b9svmwYfAXgfmBQnQ__\&Key-Pair-Id\=K27TQMT39R1C8A | gunzip | ./build/json-indexer > npis.csv
```


The npis.csv will hold 

| NPI        	| Start     	| Length 	|
|------------	|-----------	|--------	|
| 1710951728 	|123421220 	    |18754 	    |
| 1710951728 	|602606138 	    |19625  	|
| 1710951728 	|178235393  	|18728   	|
| 1710951728 	|109851201 	    |18980 	    |
| 1710951728 	|226910139  	|121347 	|
| 1710951728 	|103038852  	|18823  	|
| 1710951728 	|560191795  	|17869  	|
| 1710951728 	|156503892 	    |235658 	|


This will create a csv file of the NPI locations in the format of: npi | start | length in the file.



