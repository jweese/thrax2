# thrax2
scfg extractor for machine translation

This is intended to replace `jweese/thrax`.[1]

### Building

First, in your build directory, use `ccmake` or an environment variable to set `CMAKE_BUILD_TYPE=Release`. Then generate makefile with `cmake`, then type `make`.

Note: `thrax2` requires a C++17-compliant compiler.

### Running

The binaries `src/{hiero,samt}` generate Hiero and SAMT grammars, respectively. They will read an aligned parallel corpus from stdin and produce rules on stdout. The rules are not unique. Typically we use the many `scripts/filter_*` scripts to reduce the rules of interest. The stream of rules should then be piped to `scripts/score` to produce feature scores for a unique set of rules.

* `scripts/default_{hiero,samt}` will run extraction, filtering, and scoring for the typical Hiero or SAMT setup. Those scripts are easy to modify for your purposes.

### Motivation

The original Thrax, in the name of speed, used Apache hadoop for both rule extraction and rule scoring. The rule collections that were intermediate results were too big to be held in memory, so hadoop's map-reduce-like behavior was used to sort and stream the rules in order to calculate their scores.

In 2018, that kind of chicanery is not necessary. In fact, it's probably openly harmful, given the significant overhead involved in writing intermediate files to disk and shuffling data across the network to different hadoop nodes. Machines are big and fast enough to do things locall now.[2]

When you factor in the human costs of setting up and maintaining a hadoop cluster, tuning the JVM of various child processes, and so on, the advantage of Thrax (reproducibility) is not worth its expense.

-----

1. Weese et al., 2"Joshua 3.0: Syntax-based Machine Translation with the Thrax Grammar Extractor." WMT2011.  
2. See, for example, Drake's "Command-line Tools can be 235x Faster than your Hadoop Cluster" (https://adamdrake.com/command-line-tools-can-be-235x-faster-than-your-hadoop-cluster.html)
