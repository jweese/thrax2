# thrax2
scfg extractor for machine translation

This is intended to replace `jweese/thrax`.[1]

### Building

`mkdir build && cd build && cmake .. && make`

Note: `thrax2` requires a C++17-compliant compiler.

### Running

`thrax2 --config configfile --threads N <corpus | sort | uniq -c`
* see `examples/` for configuration files;
* see `scripts/` for support scripts to calculate feature scores.

### Motivation

The original Thrax, in the name of speed, used Apache hadoop for both rule extraction and rule scoring. The rule collections that were intermediate results were too big to be held in memory, so hadoop's map-reduce-like behavior was used to sort and stream the rules in order to calculate their scores.

In 2018, that kind of chicanery is not necessary. In fact, it's probably openly harmful, given the significant overhead involved in writing intermediate files to disk and shuffling data across the network to different hadoop nodes. Machines are big and fast enough to do things locall now.[2]

When you factor in the human costs of setting up and maintaining a hadoop cluster, tuning the JVM of various child processes, and so on, the advantage of Thrax (reproducibility) is not worth its expense.

-----

1. Weese et al., 2"Joshua 3.0: Syntax-based Machine Translation with the Thrax Grammar Extractor." WMT2011.  
2. See, for example, Drake's "Command-line Tools can be 235x Faster than your Hadoop Cluster" (https://adamdrake.com/command-line-tools-can-be-235x-faster-than-your-hadoop-cluster.html)
